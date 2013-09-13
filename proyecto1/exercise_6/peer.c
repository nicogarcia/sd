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
#include "p2p.h"

const int msg_types_qty = 4;
char* message_types[] = {
		"Usuarios: Muestra los usuarios logueados en el sistema par.",
		"Hora: Muestra la hora del sistema par.",
		"Nombre: Muestra el nombre de la maquina par.",
		"Temperatura: Muestra la temperatura actual de la maquina par."
};

pthread_t server_t, client_t;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void pair_init(int port, char *hostn, int budport)
{
	struct sockaddr_in socket_addr;
	portno = port; /* Asigno el numero de puerto */
	buddy_portno = budport; /* Asigno numero de puerto del proceso par */
	buddy_hostname = hostn; /* Asigno el nombre del host del proceso par */

	//Creo un socket del tipo stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR abriendo el socket");

	bzero((char *) &socket_addr, sizeof(socket_addr)); /* Seteo todos los valores de la estructura a cero. */

	//Seteo todos los parametros de la direccion de internet del servidor
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(portno); /* Convierto a orden de la red */

	if (bind(sockfd, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0) /* Intento vincular el puerto */
		error("ERROR vinculando el socket");

	listen(sockfd, BACKLOG); /* Permito escuchar en ese puerto */
}

void connected()
{
	pthread_t send_t,
			  recv_t;
	int op;

	pthread_create(&recv_t, NULL, peer_recv, NULL);
	while(1)
	{
		printf("(peer) ");
		scanf("%i", &op);
		peer_send((enum msg_t)op);
	}
}

void *peer_recv(void *ptr)
{
	message_t msg;
	while(1)
	{
		/* Leo los datos enviados por el cliente */
		if (recv(buddy_sockfd, (void *) &msg, sizeof(message_t), 0) < 0)
			error("ERROR leyendo del socket");

		switch(msg.type)
		{
			case GET_USERS:
			{
				break;
			}
			case GET_TIME:
			{
				gettimeofday(&msg.time, NULL);
				break;
			}
			case GET_UNAME:
			{
				uname(&msg.uname);
				break;
			}
		}
		send(buddy_sockfd, (void *) &msg, sizeof(message_t), 0);
	}
	pthread_exit(0);
}


void peer_send(enum msg_t msg_type)
{
	message_t msg;
	msg.type = msg_type;

	if (send(buddy_sockfd, (void *) &msg, sizeof(message_t), 0) < 0)
		error("ERROR escribiendo en el socket\n");

	recv(buddy_sockfd, (void *) &msg, sizeof(message_t), 0);

	switch(msg.type)
	{
		case GET_TIME:
		{
			printf("El tiempo en mi compañero es: %i\n", msg.time);
			break;
		}
		case GET_UNAME:
		{
			printf("El nombre de la maquina de mi compañero es: %s\n", msg.uname.nodename);
			break;
		}
	}
}
void update_screen()
{
	int i;
	system("clear");
	printf("Bienvenido al asistente de prueba del\n");
	printf("Ejercicio N°7 - Sistemas Distribuidos 2013\n");
	printf("-----------------------------------------\n");
	printf("Elija el tipo de mensaje a enviar:\n\n");

	for (i = 0; i < msg_types_qty; i++) {
		printf("%i. %s\n", i + 1, message_types[i]);
	}

	printf("\n");
}



int main(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr,"usage: %s port buddy_hostname buddy_port\n", argv[0]);
		exit(1);
	}
	pair_init(atoi(argv[1]), argv[2], atoi(argv[3])); /* Inicializo el socket */

	update_screen();
	pthread_create(&server_t, NULL, (void *) p2p_server, (void *) NULL);
	pthread_create(&client_t, NULL, (void *) p2p_client, (void *) NULL);
	pthread_join(client_t, NULL);
	pthread_join(server_t, NULL);
	return 0;
}
