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

#define BACKLOG 5 /* Cantidad de clientes que pueden ser atendidos al simultaneo */
#define BUFFER_SIZE 2048

int sockfd, /* File descriptor del socket del servidor */
	portno; /* Numero de puerto donde el servidor acepta conexiones */


int served_clients; /* Cantidad de clientes servidos */

void server_exit(); /* Cierra el servidor */
void server_init(); /* Inicializa el socket y otros parametros del servidor */
void server_listen(); /* Escucha el socket por pedidos de clientes */
void server_task(); /* Atiende a un cliente, recibe el file descriptor del socket cliente */

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
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */

	if (newsockfd < 0)
		error("ERROR aceptando la petición");

	//Realizo la tarea de servicio al cliente
	server_task(newsockfd);
}


void server_task(int newsockfd)
{
    struct timeval time; /* Creo el buffer para recibir el mensaje */
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE); /* Seteo todos los valores del buffer a cero */

    while(1)
    {
    	/* Leo los datos enviados por el cliente */
    	if (recv(newsockfd, buffer, BUFFER_SIZE - 1, 0) < 0)
    		error("ERROR leyendo del socket");

    	gettimeofday(&time, NULL);

    	if (send(newsockfd, &time, sizeof(struct timeval), 0) < 0) /* Envio la respuesta al cliente */
    		error("ERROR escribiendo en el socket");

    }

    close(newsockfd); /* Termino la conexion con el cliente */
}

void server_init(int port)
{
	struct sockaddr_in serv_addr; /* sockaddr_in es una estructura que contiene una direccion de internet */
	portno = port;

	//Creo un socket del tipo stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR abriendo el socket");

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo todos los valores de la estructura a cero. */

	//Seteo todos los parametros de la direccion de internet del servidor
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); /* Convierto a orden de la red */

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) /* Intento vincular el puerto */
		error("ERROR vinculando el socket");

	listen(sockfd, BACKLOG); /* Permito escuchar en ese puerto */
}


void server_exit()
{
	close(sockfd); /* Cierro el socket del servidor */
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"usage: %s port\n", argv[0]);
		exit(1);
	}

	server_init(atoi(argv[1])); /* Inicializo el servidor */
	server_listen(); /* Escucho por peticiones */
	server_exit(); /* Cierro el servidor */

    return 0;
}


