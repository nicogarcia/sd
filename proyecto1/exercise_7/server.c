/* Un servidor en el dominio de internet que se se comunica utilizando TCP
   El numero de puerto es pasado como argumento */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include<signal.h>

#define BACKLOG 5 /* Cantidad de clientes que pueden ser atendidos al simultaneo */
#define BUFFER_SIZE 2048

int sockfd, /* File descriptor del socket del servidor */
	portno; /* Numero de puerto donde el servidor acepta conexiones */


int served_clients; /* Cantidad de clientes servidos */

void server_exit(); /* Cierra el servidor */
void server_init(); /* Inicializa el socket y otros parametros del servidor */
void server_listen(); /* Escucha el socket por pedidos de clientes */
void *server_task(int *newsockfd); /* Atiende a un cliente, recibe el file descriptor del socket cliente */

void sig_handler(int signo)
{
  if (signo == SIGINT){
    printf("\nreceived SIGINT\n");
    server_exit();
    exit(0);
  }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void server_listen()
{
	double et;
	struct timeval t1, t2;
	int newsockfd;
	struct sockaddr_in cli_addr; /* Direccion del socket del cliente */
	socklen_t clilen = sizeof(cli_addr);
	pthread_t worker;
	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */
		if (newsockfd < 0) error("server> ERROR accepting\n");
		//Realizo la tarea de servicio al cliente en un nuevo hilo
		pthread_create(&worker, NULL, server_task, &newsockfd);
	}
}


void *server_task(int *newsockfd)
{
    struct timeval time; /* Creo el buffer para recibir el mensaje */
    char buffer;

   	if (recv(*newsockfd, &buffer, sizeof(char), 0) < 0) /* Leo los datos enviados por el cliente */
   		error("server> ERROR reading socket");

   	gettimeofday(&time, NULL);

   	if (send(*newsockfd, &time, sizeof(struct timeval), 0) < 0) /* Envio la respuesta al cliente */
   		error("server> ERROR writing socket");

    close(*newsockfd); /* Termino la conexion con el cliente */
    pthread_exit(0);
}

void server_init(int port)
{
	struct sockaddr_in serv_addr; /* sockaddr_in es una estructura que contiene una direccion de internet */
	portno = port;

	//Creo un socket del tipo stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("server> ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo todos los valores de la estructura a cero. */
	//Seteo todos los parametros de la direccion de internet del servidor
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); /* Convierto a orden de la red */

	printf("server> Socket startup ...\n");
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) /* Intento vincular el puerto */
		error("server> ERROR binding socket");

	listen(sockfd, BACKLOG); /* Permito escuchar en ese puerto */
}


void server_exit() {
	close(sockfd); /* Cierro el socket del servidor */
	printf("server> Connection end\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"usage: %s port\n", argv[0]);
		exit(1);
	}
	signal(SIGINT, sig_handler); /* Registro el handler de señales */
	server_init(atoi(argv[1])); /* Inicializo el servidor */
	server_listen(); /* Escucho por peticiones */
	server_exit(); /* Cierro el servidor */
    return 0;
}


