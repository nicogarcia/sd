CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -o
DEST = ../build/fedora

all: exercise_2.o exercise_2_image.o 

exercise_2.o: exercise_2.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^
	
exercise_2_image.o: exercise_2_image.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^

clean:
	rm -rf *o build/*