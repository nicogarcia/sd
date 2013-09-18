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


#define MARGIN "    "

void help_screen();

int temp;
pthread_t server_t;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/**
 * Inicializa el socket del proceso.
 */
void pair_init(char *port)
{
	struct sockaddr_in socket_addr;
	struct utsname name;
	struct hostent *node = NULL; /* Servidor obtenido por el hostname */

	uname(&name);
	strcpy(mach.port, port);
	strcpy(mach.name, name.nodename);

	node = gethostbyname(mach.name); /* Toma un nombre como argumento y retorna un puntero a estructura hostent que contiene informacion de ese host. */
	inet_ntop(AF_INET, (char *)node->h_addr, mach.ip, 50);

	buddy_connected = 0;

	printf("Inicializando sockets ...\n");
	//Creo un socket del tipo stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR abriendo el socket");

	bzero((char *) &socket_addr, sizeof(socket_addr)); /* Seteo todos los valores de la estructura a cero. */

	//Seteo todos los parametros de la direccion de internet del servidor
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(atoi(mach.port)); /* Convierto a orden de la red */


	if (bind(sockfd, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0) /* Intento vincular el puerto */
		error("ERROR vinculando el socket");
	printf("Socket vinculado ...\n");

	listen(sockfd, BACKLOG); /* Permito escuchar en ese puerto */
}



void *peer_recv(void *ptr)
{
	message_t msg;
	char buffer[256],
		*token;

	FILE *fd;
	int	new_sockfd = *((int *)ptr),
		i,
	 	close_connection = 0;

	peers_connected++;

	while(!close_connection)
	{
		/* Leo los datos enviados por el cliente */
		if (recv(new_sockfd, (void *) &msg, sizeof(message_t), 0) < 0)
			error("ERROR leyendo del socket");

		switch(msg.type)
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
						strcpy(msg.users[i++].name, buffer);
					}
				}

				msg.lenght = i;
				fclose(fd);
				break;
			}
			case GET_TIME:
			{
				gettimeofday(&msg.time, NULL);
				break;
			}
			case GET_TEMP:
			{
				msg.temp = temp + (double)(rand() / RAND_MAX);
				break;
			}
			case GET_UNAME:
			{
				uname(&msg.uname);
				break;
			}
			case CLOSE_CONN:
			{
				close_connection = 1;
				break;
			}
		}
		send(new_sockfd, (void *) &msg, sizeof(message_t), 0);
	}

	close(new_sockfd);
	peers_connected--;

	pthread_exit(0);
}


void peer_send(enum msg_t msg_type)
{
	message_t msg;
	int i;

	msg.type = msg_type;

	//Trato de enviar el mensaje, si falla entonces se cayo la conexion
	if (send(buddy_sockfd, (void *) &msg, sizeof(message_t), 0) < 0){
		printf("ERROR escribiendo en el socket\n");
		buddy_connected = 0;
		return;
	}

	recv(buddy_sockfd, (void *) &msg, sizeof(message_t), 0);

	switch(msg.type)
	{
		case CLOSE_CONN:
		{
			printf("Conexión cerrada\n");
			buddy_connected = 0;
			break;
		}
		case GET_USERS:
		{
			printf("Los usuarios loggeados en mi compañero son: \n");
			i = 0;

			while(i < msg.lenght)
				printf("%s\n", msg.users[i++].name);

			break;
		}
		case GET_TIME:
		{
			time_t nowtime;
			struct tm *nowtm;
			char tmbuf[64], buf[64];

			nowtime = msg.time.tv_sec;
			nowtm = localtime(&nowtime);
			strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
			snprintf(buf, sizeof buf, "%s.%06d", tmbuf, msg.time.tv_usec);

			printf("El tiempo en mi compañero es: %s\n", buf);
			break;
		}
		case GET_UNAME:
		{
			printf("Mi compañero es: %s\n", msg.uname.nodename);
			printf("Arquitectura: %s\n", msg.uname.machine);
			printf("Sistema operativo: %s %s\n", msg.uname.sysname, msg.uname.version);
			break;
		}
		case GET_TEMP:
		{
			printf("La temperatura en la localidad de mi compañero es: %.2f°C\n", msg.temp);
			break;
		}
	}
}


void help_screen()
{
	printf("connect: connect [buddy_hostname] [buddy_port]\n");
	printf(MARGIN);
	printf("Intenta establecer una conexión con el proceso situado en [buddy_hostname] con número de puerto [buddy_port]\n");

	printf("users: users\n");
	printf(MARGIN);
	printf("Muestra los usuarios logueados en el sistema par.\n");

	printf("time: time\n");
	printf(MARGIN);
	printf("Muestra la hora del sistema par.\n");

	printf("uname: uname\n");
	printf(MARGIN);
	printf("Muestra el nombre de la maquina par.\n");

	printf("temp: temp\n");
	printf(MARGIN);
	printf("Muestra la temperatura actual de la localidad de la maquina par.\n");

	printf("close: close\n");
	printf(MARGIN);
	printf("Cierra la conexión con el par.\n");

	printf("status: status\n");
	printf(MARGIN);
	printf("Muestra el estado actual.\n");

	printf("clear: clear\n");
	printf(MARGIN);
	printf("Limpia la pantalla de la consola.\n");

	printf("exit: exit\n");
	printf(MARGIN);
	printf("Termina la ejecuion actual.\n");

}


void status_screen()
{
	printf("Puerto propio: %s\n", mach.port);
	printf("Nombre propio: %s\n", mach.name);
	printf("IP propia: %s\n", mach.ip);


	printf("Puerto compañero: %s\n", buddy_mach.port);
	printf("Nombre compañero: %s\n", buddy_mach.name);
	printf("IP compañero: %s\n", buddy_mach.ip);

	printf("Estado: ");
	if(buddy_connected)
		printf("Conectado\n");
	else
		printf("No conectado\n");

	printf("Pares conectados: %i\n", peers_connected);


}

void command_selector()
{
	char command[50];
	char *token;
	enum msg_t op;
	int exit = 0;

	while(!exit)
	{
		printf("(peer");
		if(buddy_connected)
			printf("~");
		printf(") ");

		scanf ("%[^\n]%*c", command);

		if(!strcmp(command, "help"))
				help_screen();
		else if(!strcmp(command, "status"))
				status_screen();
			else if(!strcmp(command, "clear"))
					system("clear");
			else if(!strcmp(command, "exit"))
					exit = 1;
			else{
				if(buddy_connected){
					if(!strcmp(command, "temp"))
							op = GET_TEMP;
						else if(!strcmp(command, "close"))
								op = CLOSE_CONN;
							else if(!strcmp(command, "users"))
									op = GET_USERS;
								else if(!strcmp(command, "uname"))
										op = GET_UNAME;
									else if(!strcmp(command, "time"))
											op = GET_TIME;
					peer_send(op);
				}
				else{
					token = strtok(command, " ");
					if(!strcmp(token, "connect"))
					{
						if((token = strtok(NULL, " ")) != NULL){
							strcpy(buddy_mach.name, token);
							if((token = strtok(NULL, " ")) != NULL){
								strcpy(buddy_mach.port, token);
							}
						}
						peer_connect();
					}
				}
			}
	}

	//Cierro los sockets
	close(sockfd);
	close(buddy_sockfd);
}

void peer_connect()
{
	struct sockaddr_in serv_addr; /* Direccion del socket del servidor */
	struct hostent *server = NULL; /* Servidor obtenido por el hostname */
	int attemps = 0;

	while(server == NULL)
	{
		server = gethostbyname(buddy_mach.name); /* Toma un nombre como argumento y retorna un puntero a estructura hostent que contiene informacion de ese host. */
		sleep(0.5);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */

	buddy_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (buddy_sockfd < 0)
		error("\nERROR abriendo socket.");

	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */

	inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, buddy_mach.ip, 50);
	serv_addr.sin_port = htons(atoi(buddy_mach.port)); /* Convierte el numero de puerto en orden de bytes de la red */
	serv_addr.sin_family = AF_INET;


	printf("Intentando conectar con el par ...");

	//Intento contectar, sino espero
	while(attemps < MAX_ATTEMPS && connect(buddy_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		sleep(1);
		printf(".");
		fflush(0);
		attemps++;
	}

	printf("\n");

	if(attemps < MAX_ATTEMPS){
		buddy_connected = 1; /* Se establecio conexion con el par */
	}
	else
		printf("Fallo al intentar conectar con el par\n");
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"usage: %s port [-s buddy_hostname buddy_port]\n", argv[0]);
		exit(1);
	}
	pair_init(argv[1]); /* Inicializo el socket */

	temp = abs((rand() * 100) % 27);
	peers_connected = 0;

	pthread_create(&server_t, NULL, (void *) p2p_server, (void *) NULL);

	if(argc > 2){
		if(!strcmp(argv[2], "-s")){
			strcpy(buddy_mach.port, argv[4]);
			strcpy(buddy_mach.name, argv[3]);
			peer_connect();
		}
		else{
			fprintf(stderr,"usage: %s port [-s buddy_hostname buddy_port]\n", argv[0]);
			exit(1);
		}
	}

	command_selector();
	pthread_cancel(server_t);
	return 0;
}
