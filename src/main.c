#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#define TAPE_SIZE 30000
#define MAX_COMMAND_SIZE 1024

char tape[TAPE_SIZE] = {0};
char *ptr = tape;

volatile sig_atomic_t break_requested = 0;

void display_tape() {
    // TODO: Display first 10 cells for now (should diff based on ptr)
    for (int i = 0; i < 10; i++) {
        printf("%d ", tape[i]);
    }
    printf("... \n");
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void interpret(char *commands) {
    char *command_ptr = commands;
    while (*command_ptr) {
        switch (*command_ptr) {
            case '>':
                ++ptr;
                break;
            case '<':
                --ptr;
                break;
            case '+':
                *ptr = (*ptr + 1) % 256;
                break;
            case '-':
                *ptr = (*ptr + 255) % 256;
                break;
            case '.':
                putchar(*ptr);
                break;
            case ',':
                *ptr = getchar();
                break;
            case '[':
                if (!*ptr) {
                    int loop_count = 1;
                    while (loop_count) {
                        ++command_ptr;
                        if (*command_ptr == '[') loop_count++;
                        if (*command_ptr == ']') loop_count--;
                    }
                }
                break;
            case ']':
                if (*ptr) {
                    int loop_count = 1;
                    while (loop_count) {
                        --command_ptr;
                        if (*command_ptr == '[') loop_count--;
                        if (*command_ptr == ']') loop_count++;
                    }
                }
                break;
            default:
                // Ignore other chars
                break;
        }
        ++command_ptr;
    }
}

void interpret_step_by_step() {
    printf("\nStep-through: Input code, then 'enter' to step, 'q' to quit");
    printf("\n~ ");

    clear_stdin();

    char commands[MAX_COMMAND_SIZE];
    if (!fgets(commands, MAX_COMMAND_SIZE, stdin)) {
        return;
    }
    if (commands[0] == '\n') {
        printf("\nEnter code or 'ctrl+c' to break\n");
        return;
    }

    size_t len = strlen(commands);
    if (len > 0 && commands[len - 1] == '\n') {
        commands[len - 1] = '\0';
    }

    printf("\n");

    char *command_ptr = commands;
    while (*command_ptr) {
        printf("Executing '%c'...\n", *command_ptr);

        char tempCommand[2] = {*command_ptr, '\0'};
        interpret(tempCommand);

        display_tape();

        printf("Press 'enter' to continue, 'q' to quit...\n");

        char input = 0;
        while (input != '\n' && input != 'q') {
            input = getchar();
        }

        if (input == 'q') {
            break;
        }
        
        ++command_ptr;
    }

    printf("\nDone! Enter code or 'ctrl+c' to break\n");
}

jmp_buf jump_buffer;

void handle_sigint(int sig) {
    (void) sig;
    break_requested = 1;
    longjmp(jump_buffer, 1);
}

void handle_break() {
    printf("\n\nBreak: (e)xit, (c)ontinue, (s)tep\n");
    
    char c;
    while ((c = getchar()) != 'e' && c != 'E' && c != 's' && c != 'S' && c != 'c' && c != 'C') {
        while (getchar() != '\n');  // Clear any additional input
    }
    
    switch (c) {
        case 'e':
        case 'E':
            exit(0);
            break;
        case 's':
        case 'S':
            interpret_step_by_step();
            break;
        default:
            // For 'c' or 'C', simply continue the main loop.
            break;
    }
}

int main() {
    char commands[MAX_COMMAND_SIZE];

    signal(SIGINT, handle_sigint);

    printf("Brainfuck REPL\n");
    printf("Enter code or 'ctrl+c' to break\n");

    while (1) {
        printf("\r~ ");
        fflush(stdout);

        if (setjmp(jump_buffer) == 0) {
            fgets(commands, MAX_COMMAND_SIZE, stdin);
        }

        if (break_requested) {
            handle_break();
            break_requested = 0;
        } else {
            interpret(commands);
        }
    }

    return 0;
}
