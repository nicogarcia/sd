CC=gcc
CFLAGS=-g -Wall
TFLAGS=-lrt -o
DEST = ../build/fedora

all: exercise_1.o 

exercise_1.o: exercise_1.c
	$(CC) $(CFLAGS) $(TFLAGS) $(DEST)/$(basename $@) $^

clean:
	rm -rf *o build/*