CC=gcc
CFLAGS=-g -o
TFLAGS=-g -lrt -o
DEST = ../build/fedora
LIBS=../lib/config.h ../lib/config.c

all: project_tester.o

project_tester.o: project_tester.c
	cp project_config $(DEST)/
	cp deployment_script_ssh $(DEST)/
	cp deployment_script_sftp $(DEST)/
	$(CC) $(TFLAGS) $(DEST)/$(basename $@) $^