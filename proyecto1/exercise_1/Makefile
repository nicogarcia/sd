CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -o
DEST = ../build/fedora

all: exercise_1.o 

exercise_1.o: exercise_1.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^

clean:
	rm -rf *o build/*