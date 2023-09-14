CC = gcc
CFLAGS = -Wall -Wextra
BINDIR = ./bin
SRCDIR = ./src
TARGET = $(BINDIR)/main

all: directories $(TARGET)

directories: $(BINDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $< -o $(TARGET)

clean:
	rm -rf $(BINDIR)


.PHONY: all directories clean
