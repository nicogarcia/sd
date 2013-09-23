CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -lpthread -lnsl  -o #-lsocket
DEST = ../build/fedora
LIBS=../lib/table_library.h ../lib/table_library.c 

all: exercise_4.o exercise_4_server.o exercise_4_client.o

exercise_4.o: $(LIBS) exercise_4.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@)  $^
	
exercise_4_server.o: server.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@)  $^

exercise_4_client.o: client.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $(LIBS)$^