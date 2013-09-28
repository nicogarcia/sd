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

#define VERSION "NAME\n   Peer implemented with C\nAUTHORS\n   Trippel Nagel Juan Manuel\n   Garcia Nicolas\n"

void *p2p_server(void *ptr)
{
	struct sockaddr_in cli_addr; /* Direccion del socket del cliente */
	socklen_t clilen = sizeof(cli_addr);
	struct peer_socketd psfd;
	int new_sockfd, i;
	pthread_t recv_t;
	temp = abs((rand() * 100) % 27);

	while(1)
	{
		new_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */
		psfd.sockfd = new_sockfd;
		i = 0;
		while(i < BACKLOG){
			if(peers_socketsfd[i] == FREE_PEER_DESCRIPTOR){
				psfd.peerd = i;
				break;
			}
			i++;
		}
		psfd.peerd = peers_connected;
		//Cuando llega un pedido de una cliente se establece la conexion
		peers_socketsfd[psfd.peerd] = new_sockfd;
		pthread_create(&recv_t, NULL, peer_recv, &psfd);
	}
	pthread_exit(0);
}

/**
 * Recibe mensajes de un peer conectado.
 *  */
void *peer_recv(void *ptr)
{
	int request_t;

	char buffer[256],
		*token,
		 response[256];
	char tmbuf[64];

	struct timeval time;
	time_t nowtime;
	struct tm *nowtm;
	struct utsname myname;
	double mytemp = temp;

	FILE *fd;
	int	new_sockfd = ((struct peer_socketd *)ptr)->sockfd,
		peerd = ((struct peer_socketd *)ptr)->peerd,
		i,
	 	close_connection = 0;

	peers_connected++;

	while(!close_connection)
	{
		/* Leo los datos enviados por el cliente */
		if ((i = recv(new_sockfd, (void *) &request_t, sizeof(int), 0)) < 0)
			close_connection = 1;
		else{
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
					mytemp = temp + (rand() * 100) % 5;
					snprintf(response,
							 sizeof response,
							 "Temperature: %.2f°C\n",
							 mytemp);
					break;
				}
				case GET_UNAME:
				{
					uname(&myname);
					snprintf(response,
							 sizeof response,
							 "Node: %s\nArch: %s\nOperating system: %s %s\n",
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
					strcpy(response, "Connection closed\n");
					close_connection = 1;
					break;
				}

			}
			send(new_sockfd, (void *) &response, sizeof(response), 0);
		}
	}
	close(new_sockfd);

	peers_connected--;
	peers_socketsfd[peerd] = FREE_PEER_DESCRIPTOR;
	pthread_exit(0);
}
