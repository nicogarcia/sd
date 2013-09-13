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


void *p2p_server(void *ptr)
{
	struct sockaddr_in cli_addr; /* Direccion del socket del cliente */
	socklen_t clilen = sizeof(cli_addr);
	printf("Escuchando en el puerto %i\n", portno);
	while(1)
	{
		buddy_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */
		//Cuando llega un pedido de una cliente se establece la conexion
		connected();
	}
	pthread_exit(0);
}
