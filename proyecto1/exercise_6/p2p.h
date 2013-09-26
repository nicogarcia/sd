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


enum msg_t { GET_USERS, GET_TIME, GET_UNAME, GET_TEMP, GET_SERV_VERSION, CLOSE_CONN};

int buddy_connected,
	buddy_sockfd,
	sockfd,
	peers_connected;


machine buddy_mach, mach;


void peer_connect();
void *p2p_server(void *ptr);
void *peer_recv(void *ptr);
void peer_send(enum msg_t msg_type);
void peer_exit();



#endif
