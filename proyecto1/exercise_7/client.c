/* Un cliente en el dominio de internet que se se comunica utilizando TCP
   El numero de puerto y el nombre del host son pasados como argumento */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#define BUFFER_SIZE 2048

int sockfd,/* Descriptor del socket del cliente */
	server_sockfd, /* Descriptor del socket del servidor */
	portno, /* Numero de puerto */
	server_portno; /* Numero de puerto del servidor */

char *server_hostname;
struct timeval serv_time;

void client_init(); /* Inica el socket del cliente */
void client_connect(); /* Realiza la peticion al servidor */
void client_request(); /* Realiza la peticion al servidor */


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void client_init(int port, char *hostname)
{
	server_hostname = hostname;
	server_portno = port; /* Asigno el puerto */

	sockfd = socket(AF_INET, SOCK_STREAM, 0); /* Creo el socket */
	if (sockfd < 0)
		error("ERROR abriendo el socket");

}

void client_connect()
{
	struct hostent *server; /* Servidor obtenido por el hostname */
	struct sockaddr_in serv_addr; /* Direccion del socket del servidor */

	server = gethostbyname(server_hostname); /* Toma un nombre como argumento y retorna un puntero a una estructura hostent que contiene informacion de ese host. */
	if (server == NULL)
		error("ERROR no existe el host\n");

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */
	serv_addr.sin_family = AF_INET;

	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */
	serv_addr.sin_port = htons(server_portno); /* Convierte el numero de puerto en orden de bytes de la red */

	 /* Intento conectar con el servidor */
	while (connect(server_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		sleep(0.5);

}

void client_request()
{
	char buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	start_timer();

	if (send(server_sockfd, buffer, BUFFER_SIZE - 1, 0) < 0) /* Intento enviar el pedido al servidor */
		error("ERROR escribiendo en el socket");

	if (recv(server_sockfd, &serv_time, sizeof(struct timeval), 0) < 0)
		error("ERROR leyendo del socket");

	stop_timer();

}

void show_results()
{
	int test;
	system("clear");

	printf("Información de la conexión\n");
	printf("-----------------------------------------\n");
	printf("Tiempo de conexión: %f ms \n", timediff());
	printf("Tiempo en el servidor : %f ms \n", serv_time.tv_sec);

}

int main(int argc, char *argv[])
{
	int tests;
	double et;
	struct timeval t1, t2;

	//Verifico si estan todos los argumentos requeridos
	if (argc < 3) {
		fprintf(stderr,"usage: %s hostname port\n", argv[0]);
		exit(0);
	}

	client_init(atoi(argv[2]), argv[1]);
	client_connect(); /* Solicito tarea al servidor */
	client_request();

	show_results();
    close(sockfd); /* Cierro la conexion */

    return 0;
}
