CC=gcc
TFLAGS=-g -lpthread -lrt -o
DEST = ../build/fedora
LIBS = ../lib/config.h ../lib/config.c p2p.h

all: peer.o 

peer.o: peer.c server_thread.c 
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $(LIBS) $^

clean:
	rm -rf *o build/*