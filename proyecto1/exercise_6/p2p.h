#ifndef p2p_h
#define p2p_h

#define BACKLOG 5
#define PAYLOAD_SIZE 2048
#define MAX_CONNECTION_TRIALS 5

enum msg_t { GET_USERS, GET_TIME, GET_UNAME, GET_TEMP};

typedef struct{
	enum msg_t type;
	union{
		struct timeval time;
		int temp;
		struct utsname uname;
	};
}message_t;

void *p2p_client(void *ptr);
void *p2p_server(void *ptr);
void *peer_recv(void *ptr);
void connected();
void peer_send(enum msg_t msg_type);



char *buddy_hostname;

int portno,
	buddy_portno,
	sockfd,
	buddy_sockfd;

#endif
