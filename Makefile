# Makefile for CASH!

# Variables
CC = gcc
CFLAGS = -std=c11 -Wall
DEBUG_FLAGS = -ggdb3
TARGET = cash
SRC = main.c cash.c lexer.c parser.c interpreter.c environment.c error.c
OBJ = main.o cash.o lexer.o parser.o interpreter.o environment.o error.o 

.PHONY: all clean run debug memleak
all: $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)
cash.o: $(SRC)
	$(CC) -c $(SRC)

clean:
	rm $(OBJ)
	rm $(TARGET)

run: $(TARGET)
	./$(TARGET)
debug: $(SRC)
	$(CC) -o $(TARGET) $(CFLAGS) $(DEBUG_FLAGS) $(SRC)
run-memleak: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(TARGET)
