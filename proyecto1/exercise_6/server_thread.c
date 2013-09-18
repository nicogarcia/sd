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
	int new_sockfd;
	pthread_t recv_t;

	while(1)
	{
		new_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */
		//Cuando llega un pedido de una cliente se establece la conexion
		pthread_create(&recv_t, NULL, peer_recv, (void *) &new_sockfd);
	}
	pthread_exit(0);
}
