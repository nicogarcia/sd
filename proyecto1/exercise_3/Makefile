CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -o
DEST = ../build/fedora
LIBS=../lib/table_library.h ../lib/config.h ../lib/table_library.c ../lib/config.c

all: exercise_3.o

exercise_3.o: exercise_3.c
	$(CC) $(TFLAGS) $(DEST)/$@  $(LIBS) $^ 

clean:
	rm -rf *o build/*