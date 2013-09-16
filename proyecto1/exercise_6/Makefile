CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lpthread -lrt -o
DEST = ../build/fedora

all: peer.o 

peer.o: peer.c server_thread.c client_thread.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^

clean:
	rm -rf *o build/*