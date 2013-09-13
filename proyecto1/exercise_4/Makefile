CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -o
DEST = ../build/fedora
LIBS=../lib/config.h ../lib/config.c

all: exercise_4.o exercise_4_server.o exercise_4_client.o

exercise_4.o: $(LIBS) exercise_4.c 
	$(CC) $(TFLAGS) $(DEST)/$@ $^
	
exercise_4_server.o: server.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^

exercise_4_client.o: client.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^