CFLAGS=-std=c99 -Wall -Wextra -g

all: main.c parser.c scanner.c token.c stack.c parser.h scanner.h token.h stack.h errors.h
	gcc -o compiler main.c parser.c scanner.c token.c stack.c
