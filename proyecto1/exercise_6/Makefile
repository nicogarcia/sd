CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lpthread -lrt -o
DEST = ../build

all: peer.o 

peer.o: peer.c server_thread.c client_thread.c
	$(CC) $(TFLAGS) $(DEST)/$@ $^

clean:
	rm -rf *o build/*