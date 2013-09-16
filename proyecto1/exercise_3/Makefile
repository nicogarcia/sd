CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -lnsl -o
DEST = ../build/fedora
LIBS=../lib/table_library.h ../lib/config.h ../lib/table_library.c ../lib/config.c

all: rpc_times.o exercise_3.o

rpc_times.o: 
	make -f Makefile.rpc_times

exercise_3.o: rpc_times_clnt.c rpc_times_xdr.c exercise_3.c
	$(CC) $(TFLAGS) $(DEST)/$(basename $@)  $(LIBS) $^ 

clean:
	#rm -rf rpc* Makefile.rpc_times
	rm -rf *o build/* 