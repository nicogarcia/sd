CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -o
DEST = ../build/fedora

all: exercise_2.o exercise_2_image.o 

exercise_2.o: exercise_2.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^
	
exercise_2_image.o: exercise_2_image.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^

clean:
	rm -rf *o build/*