CC=gcc
BIN=bin
OBJ=obj

CFLAGS=`curl-config --cflags` -Wall

all: check_dirs 
	$(CC) -c -o $(OBJ)/conn.o conn/conn.c $(CFLAGS)
	$(CC) -o $(BIN)/conn $(OBJ)/conn.o `curl-config --libs` -lpthread

check_dirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)
