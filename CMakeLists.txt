cmake_minimum_required(VERSION 3.13)
project(Parser C)

set(CMAKE_C_STANDARD 99)

add_executable(Parser main.c parser.c scanner.c stack.c token.c psa.c symtable.c code_gen.c code_gen.h)