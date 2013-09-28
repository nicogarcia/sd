/*
 * Proceso par que es cliente/servidor.
 * Ejecuta dos hilos uno cliente y otro servidor.
 */
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
#include <signal.h>
#include "p2p.h"

void help_screen();

pthread_t server_t;

void sig_handler(int signo)
{
  if (signo == SIGINT){
	  printf("\nreceived SIGINT\n");
	  peer_exit();
	  exit(0);
  }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void peer_exit()
{
	int i = 0;
	close(sockfd); /* Cierro el socket del servidor */
	if(buddy_connected) /* Cierro el socket del compañero */
		close(buddy_sockfd);

	while(i < BACKLOG){
		if(peers_socketsfd[i] != FREE_PEER_DESCRIPTOR){
			close(peers_socketsfd[i]);
		}
		i++;
	}
	pthread_cancel(server_t);
	printf("Connection closed\n");
}

/**
 * Inicializa el socket del proceso.
 */
void pair_init(char *port)
{
	struct sockaddr_in socket_addr;
	struct utsname name;
	struct hostent *node = NULL; /* Servidor obtenido por el hostname */

	uname(&name);
	strcpy(mach.port, port);
	strcpy(mach.name, name.nodename);

	node = gethostbyname(mach.name); /* Toma un nombre como argumento y retorna un puntero a estructura hostent que contiene informacion de ese host. */
	inet_ntop(AF_INET, (char *)node->h_addr, mach.ip, 50);

	buddy_connected = 0;

	printf("Startup sockets ...\n");
	//Creo un socket del tipo stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR writing socket");

	bzero((char *) &socket_addr, sizeof(socket_addr)); /* Seteo todos los valores de la estructura a cero. */

	//Seteo todos los parametros de la direccion de internet del servidor
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(atoi(mach.port)); /* Convierto a orden de la red */


	if (bind(sockfd, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0) /* Intento vincular el puerto */
		error("ERROR binding socket");

	listen(sockfd, BACKLOG); /* Permito escuchar en ese puerto */
}




void peer_send(enum msg_t msg_type)
{
	int request_t;
	double et;
	struct timeval start, end;
	char response[256];

	if(buddy_connected){
		request_t = htonl(msg_type);
		bzero(response, sizeof response);

		gettimeofday(&start, NULL);
		//Trato de enviar el mensaje, si falla entonces se cayo la conexion
		if (send(buddy_sockfd, (void *) &request_t, sizeof(int), 0) < 0){
			printf("ERROR writing socket\n");
			buddy_connected = 0;
			return;
		}
		recv(buddy_sockfd, (void *) &response, sizeof(response), 0);
		gettimeofday(&end, NULL);
		et = (end.tv_sec - start.tv_sec) * 1000;
		et += (end.tv_usec - start.tv_usec);

		switch(msg_type)
		{
			case CLOSE_CONN:
			{
				printf("%s", response);
				buddy_connected = 0;
				break;
			}
			default:
			{
				printf("%s", response);
				break;
			}
		}

		printf("Waiting time: %.2fus\n", et);
	}
	else printf("ERROR : Need connection.\n");
}


void help_screen()
{
	char *margin = "     ";
	printf("CONNECTION\n");
	printf("connect: connect [buddy_hostname] [buddy_port]\n");
	printf(margin);
	printf("Connect with a process in [buddy_hostname] with port number [buddy_port]\n");

	printf("close: close\n");
	printf(margin);
	printf("Close connection with peer.\n");

	printf("\nAUXILIAR\n");
	printf("status: status\n");
	printf(margin);
	printf("Show actual state.\n");

	printf("clear: clear\n");
	printf(margin);
	printf("Clear terminal screen.\n");

	printf("exit: exit\n");
	printf(margin);
	printf("Finish actual process.\n");

	printf("\nMESSAGES: Need connection. When connected pair~>. \n");

	printf("users: users\n");
	printf(margin);
	printf("Show peer's logged users.\n");

	printf("time: time\n");
	printf(margin);
	printf("Show peer's system time.\n");

	printf("uname: uname\n");
	printf(margin);
	printf("Show peer's machine name.\n");

	printf("version: version\n");
	printf(margin);
	printf("Request peer's version.\n");

	printf("temp: temp\n");
	printf(margin);
	printf("Show peer's temperature.\n");
}


void status_screen()
{

	printf("ME\n");
	printf("  PORT: %s\n", mach.port);
	printf("  HOSTNAME: %s\n", mach.name);
	printf("  IP: %s\n", mach.ip);


	printf("PEER\n");
	printf("  PORT: %s\n", buddy_mach.port);
	printf("  HOSTNAME: %s\n", buddy_mach.name);
	printf("  IP: %s\n", buddy_mach.ip);

	printf("STATE: ");
	if(buddy_connected)
		printf("CONNECTED\n");
	else
		printf("NO CONNECTED\n");

	printf("PEERs CONNECTED: %i\n", peers_connected);
}

void command_selector()
{
	char command[50];
	char *token;
	int exit = 0;

	while(!exit)
	{
		printf("peer");
		if(buddy_connected)
			printf("~");
		printf("> ");

		scanf ("%[^\n]%*c", command);
		token = strtok(command, " ");

		if(!strcmp(token, "help"))
			help_screen();
		else if(!strcmp(token, "status"))
				status_screen();
		else if(!strcmp(token, "clear"))
				system("clear");
		else if(!strcmp(token, "exit"))
				exit = 1;

		//Comandos de conexion
		else if(!strcmp(token, "temp"))
				peer_send(GET_TEMP);
		else if(!strcmp(token, "close"))
				peer_send(CLOSE_CONN);
		else if(!strcmp(token, "users"))
				peer_send(GET_USERS);
		else if(!strcmp(token, "uname"))
				peer_send(GET_UNAME);
		else if(!strcmp(token, "time"))
				peer_send(GET_TIME);
		else if(!strcmp(token, "version"))
				peer_send(GET_SERV_VERSION);


		else if(!strcmp(token, "connect")){
				if((token = strtok(NULL, " ")) != NULL){
					strcpy(buddy_mach.name, token);
					if((token = strtok(NULL, " ")) != NULL){
						strcpy(buddy_mach.port, token);
					}
				}
				peer_connect();
			}
		else
			printf("The command in not valid\n");
	}
}

void peer_connect()
{
	struct sockaddr_in serv_addr; /* Direccion del socket del servidor */
	struct hostent *server = NULL; /* Servidor obtenido por el hostname */
	int attemps = 0,
		backoff = 1;

	while(server == NULL)
	{
		server = gethostbyname(buddy_mach.name); /* Toma un nombre como argumento y retorna un puntero a estructura hostent que contiene informacion de ese host. */
		sleep(0.5);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */

	buddy_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (buddy_sockfd < 0)
		error("\nERROR opening socket.");

	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */

	inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, buddy_mach.ip, 50);
	serv_addr.sin_port = htons(atoi(buddy_mach.port)); /* Convierte el numero de puerto en orden de bytes de la red */
	serv_addr.sin_family = AF_INET;


	printf("Connecting ...");

	//Intento contectar, sino espero
	while(attemps < MAX_ATTEMPS && connect(buddy_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		sleep(backoff);
		printf(".");
		fflush(0);
		backoff *= 1.5;
		attemps++;
	}

	printf("\n");

	if(attemps < MAX_ATTEMPS){
		buddy_connected = 1; /* Se establecio conexion con el par */
	}
	else printf("ERROR : Failed to connect peer\n");
}


int main(int argc, char *argv[])
{
	int i;
	if (argc < 2) {
		fprintf(stderr,"usage: %s port [-s {buddy_hostname | buddy_ip} buddy_port]\n", argv[0]);
		exit(1);
	}

	signal(SIGINT, sig_handler); /* Registro el handler de señales */

	pair_init(argv[1]); /* Inicializo el socket */

	peers_connected = 0;
	for(i = 0; i < BACKLOG; i++)
		peers_socketsfd[i] = -1;

	pthread_create(&server_t, NULL, (void *) p2p_server, (void *) NULL);

	if(argc > 2){
		if(!strcmp(argv[2], "-s")){
			strcpy(buddy_mach.port, argv[4]);
			strcpy(buddy_mach.name, argv[3]);
			peer_connect();
		}
		else{
			fprintf(stderr,"usage: %s port [-s {buddy_hostname | buddy_ip} buddy_port]\n", argv[0]);
			exit(1);
		}
	}

	command_selector();
	peer_exit();
	return 0;
}
