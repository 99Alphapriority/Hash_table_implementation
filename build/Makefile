#! /bin/bash
SRC = ../src
INC = ../include
BIN = ../bin
OBJ = ../obj
CC = gcc

OBJS = $(OBJ)/hash_table.o $(OBJ)/prime.o $(OBJ)/hash_table_test.o

$(BIN)/test: $(OBJS)
	gcc -o $@ $(OBJS)

$(OBJS): $(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ) $(BIN)
	gcc -c -w $< -o $@ -I $(INC)

clean:
	rm -rf $(BIN) $(OBJS) $(OBJ)
