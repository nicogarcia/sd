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
#include "p2p.h"

void *p2p_client(void *ptr)
{
	struct sockaddr_in serv_addr; /* Direccion del socket del servidor */
	struct hostent *server = NULL; /* Servidor obtenido por el hostname */

	fflush(0);

	while(server == NULL)
	{
		server = gethostbyname(buddy_hostname); /* Toma un nombre como argumento y retorna un puntero a estructura hostent que contiene informacion de ese host. */
		sleep(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */

	buddy_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (buddy_sockfd < 0)
		error("\nERROR abriendo socket.");

	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */
	serv_addr.sin_port = htons(buddy_portno); /* Convierte el numero de puerto en orden de bytes de la red */
	serv_addr.sin_family = AF_INET;


	//Intento contectar, sino espero 2 segundos
	while(connect(buddy_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		sleep(2);
	}

	connected();
	pthread_exit(0);
}
