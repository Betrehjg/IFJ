CFLAGS=-std=c99 -Wall -Wextra -g

all: main.c parser.c scanner.c token.c stack.c parser.h scanner.h token.h stack.h errors.h psa.h psa.c code_gen.c code_gen.h
	gcc -o compiler main.c parser.c scanner.c token.c stack.c psa.c code_gen.c
