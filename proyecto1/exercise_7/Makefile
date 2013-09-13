CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lpthread -lrt -o
DEST = ../build

all: server.o client.o parallel_server.o

server.o: server.c
	$(CC) $(CFLAGS) $(DEST)/$@ $^

client.o: client.c timer.c
	$(CC) $(CFLAGS) $(DEST)/$@ $^

parallel_server.o: parallel_server.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^

clean:
	rm -rf *o build/*