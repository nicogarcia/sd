/* Un servidor en el dominio de internet que se se comunica utilizando TCP
 El numero de puerto es pasado como argumento */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>

#define BACKLOG 5 /* Cantidad de clientes que pueden ser atendidos al simultaneo */
#define DEFAULT_BUFFER_SIZE 2048

int sockfd, /* File descriptor del socket del servidor */
	portno; /* Numero de puerto donde el servidor acepta conexiones */

int *buffer_sizes;
int n_tests = 1;

int served_clients; /* Cantidad de clientes servidos */

void server_exit(); /* Cierra el servidor */
void server_init(); /* Inicializa el socket y otros parametros del servidor */
void server_listen(); /* Escucha el socket por pedidos de clientes */
void *server_task(int *ptr); /* Atiende a un cliente, recibe el file descriptor del socket cliente */

void sig_handler(int signo)
{
  if (signo == SIGINT){
    printf("\nreceived SIGINT\n");
    server_exit();
    exit(0);
  }
}

void error(const char *msg) {
	perror(msg);
	exit(1);
}

void server_listen() {
	int newsockfd;
	struct sockaddr_in cli_addr; /* Direccion del socket del cliente */
	socklen_t clilen = sizeof(cli_addr);
	pthread_t task;

	printf("(server) Listening ...\n");
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */
	if (newsockfd < 0)	error("(server) ERROR accepting request\n");
	//Realizo la tarea de servicio al cliente
	pthread_create(&task, NULL, (void *)server_task, (void *)&newsockfd);
	pthread_join(task, NULL);
}

void *server_task(int *newsockfd) {
	char *buffer; /* Creo el buffer para recibir el mensaje */
	int test, buffsize;

	printf("(server) Starting tests ...\n");
	for (test = 0; test < n_tests; test++) {
		printf("(server) Buffer test: %i bytes\n", buffer_sizes[test]);

		buffsize = buffer_sizes[test];
		buffer = calloc(buffsize, sizeof(char)); /* Creo el nuevo buffer */

		bzero(buffer, buffsize); /* Seteo todos los valores del buffer a cero */
		/* Leo los datos enviados por el cliente */
		if (recv(*newsockfd, buffer, buffsize - 1, 0) < 0)
			error("(server) ERROR reading socket");

		if (send(*newsockfd, buffer, buffsize - 1, 0) < 0) /* Envio la respuesta al cliente */
			error("(server) ERROR  writing socket");
		printf("(server) Successful test\n");
	}
	printf("(server) Tests end\n");
	close(*newsockfd); /* Termino la conexion con el cliente */
	pthread_exit(0);
}

void server_init(int port) {
	struct sockaddr_in serv_addr; /* sockaddr_in es una estructura que contiene una direccion de internet */
	portno = port;
	//Creo un socket del tipo stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)	error("(server) ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo todos los valores de la estructura a cero. */
	//Seteo todos los parametros de la direccion de internet del servidor
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); /* Convierto a orden de la red */

	printf("(server)Socket startup ...\n");
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) /* Intento vincular el puerto */
		error("(server) ERROR binding socket");

	listen(sockfd, BACKLOG); /* Permito escuchar en ese puerto */
}

void server_exit() {
	close(sockfd); /* Cierro el socket del servidor */
	printf("(server) Connection closed\n");
}

int main(int argc, char *argv[]) {
	int tests;

	if (argc < 2) {
		fprintf(stderr, "usage: %s port[-b buffer_lenght[,...]]\n", argv[0]);
		exit(1);
	}

	//Obtengo el tamaño del buffer, si no se paso se usa el tamaño por defecto
	if (argc > 2) {
		if (!strcmp(argv[2], "-b")) {
			tests = argc - 3; /* Obtengo la cantidad de buffers a probar */
			n_tests = tests;

			buffer_sizes = calloc(n_tests, sizeof(int));
			while (tests > 0) {
				buffer_sizes[tests - 1] = atoi(argv[tests + 2]);
				tests--;
			}
		} else {
			fprintf(stderr, "usage: %s port [-b buffer_lenght[,...]]\n",
					argv[0]);
			exit(0);
		}
	} else {
		n_tests = 1;
		buffer_sizes = calloc(1, sizeof(int));
		buffer_sizes[0] = DEFAULT_BUFFER_SIZE;
	}

	signal(SIGINT, sig_handler); /* Registro el handler de señales */
	server_init(atoi(argv[1])); /* Inicializo el servidor */
	server_listen(); /* Escucho por peticiones */
	server_exit(); /* Cierro el servidor */

	return 0;
}

