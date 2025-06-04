PROJECT = 24011937

CC = gcc
CFLAGS = -Wall -g -std=c2x -Wno-discarded-qualifiers -Wno-overflow
LFLAGS = -lm 

SOURCES = main.c

build:
	$(CC) $(CFLAGS) $(SOURCES) -o $(PROJECT) $(LFLAGS)

run: build 
	./$(PROJECT)
