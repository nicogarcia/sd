CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lpthread -lrt -o
DEST = ../build/fedora

all: server.o client.o parallel_server.o

server.o: server.c
	$(CC) $(CFLAGS) $(DEST)/$(basename $@) $^

client.o: client.c timer.c
	$(CC) $(CFLAGS) $(DEST)/$(basename $@) $^

parallel_server.o: parallel_server.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^

clean:
	rm -rf *o build/*