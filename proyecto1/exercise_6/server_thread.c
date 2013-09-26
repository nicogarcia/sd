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

#define VERSION "NAME\n   Peer implementado en C\nAUTHORS\n   Trippel Nagel Juan Manuel\n   Garcia Nicolas\n"

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

void *peer_recv(void *ptr)
{
	int request_t;
	int temp = abs((rand() * 100) % 27);

	char buffer[256],
		*token,
		 response[256];
	char tmbuf[64];

	struct timeval time;
	time_t nowtime;
	struct tm *nowtm;
	struct utsname myname;

	FILE *fd;
	int	new_sockfd = *((int *)ptr),
		i,
	 	close_connection = 0;

	peers_connected++;

	while(!close_connection)
	{
		/* Leo los datos enviados por el cliente */
		if ((i = recv(new_sockfd, (void *) &request_t, sizeof(int), 0)) < 0)
			error("ERROR leyendo del socket");

		request_t = ntohl(request_t);
		bzero(response, sizeof(response));

		switch(request_t)
		{
			case GET_USERS:
			{
				fd = fopen("/etc/passwd", "r");
				i = 0;
				while(fgets(buffer, 255, fd) != NULL){
					strcpy(buffer, strtok(buffer, ":")); /* El primer token es el nombre de usuario */
					token = strtok(NULL, ":"); /* El segundo son los privilegios del usuario */
					token = strtok(NULL, ":"); /* El tercero es el numero del usuario */

					if(atoi(token) >= 1000){
						strcat(response, buffer);
						strcat(response, "\n");
					}
				}

				fclose(fd);
				break;
			}
			case GET_TIME:
			{
				gettimeofday(&time, NULL);
				nowtime = time.tv_sec;
				nowtm = localtime(&nowtime);
				strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
				snprintf(response, sizeof response, "%s.%06d\n", tmbuf, time.tv_usec);
				break;
			}
			case GET_TEMP:
			{
				temp+= (double)(rand() / RAND_MAX);
				snprintf(response,
						 sizeof response,
						 "Temperatura: %.2f°C\n",
						 &temp);
				break;
			}
			case GET_UNAME:
			{
				uname(&myname);
				snprintf(response,
						 sizeof response,
						 "Nodo: %s\nArquitectura: %s\nSistema operativo: %s %s\n",
						 myname.nodename,
						 myname.machine,
						 myname.sysname,
						 myname.version);

				break;
			}
			case GET_SERV_VERSION:
			{
				strcpy(response, VERSION);
				break;
			}
			case CLOSE_CONN:
			{
				strcpy(response, "Conexión cerrada con exito\n");
				close_connection = 1;
				break;
			}

		}
		send(new_sockfd, (void *) &response, sizeof(response), 0);
	}
	close(new_sockfd);
	peers_connected--;
	pthread_exit(0);
}
