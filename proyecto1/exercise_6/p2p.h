#include "../lib/config.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/utsname.h>


#ifndef p2p_h
#define p2p_h

#define MAX_ATTEMPS 5
#define BACKLOG 5
#define PAYLOAD_SIZE 2048


enum msg_t { GET_USERS, GET_TIME, GET_UNAME, GET_TEMP, CLOSE_CONN};
int buddy_connected,
	buddy_sockfd,
	sockfd,
	peers_connected;

typedef struct{
	char name[50];
} user_t;

machine buddy_mach, mach;

typedef struct{
	enum msg_t type;
	int lenght;
	union{
		user_t users[256];
		struct timeval time;
		double temp;
		struct utsname uname;
	};
}message_t;

void peer_connect();
void *p2p_server(void *ptr);
void *peer_recv(void *ptr);
void peer_send(enum msg_t msg_type);



#endif
