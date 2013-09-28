CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lpthread -lrt -o #-lsocket
DEST = ../build/fedora

all: server.o client.o 

server.o: server.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^

client.o: client.c timer.c ../lib/table_library.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^


clean:
	rm -rf *o build/*