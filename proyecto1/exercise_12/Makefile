CC=javac
CFLAGS=-g
DEST = ../build/fedora

all: exercise_12.o

exercise_12.o: Message.java ClientThread.java TaskThread.java ServerThread.java Peer.java
	$(CC) $(CFLAGS) -d $(DEST) $^

clean:
	rm -rf *o build/*