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

#define BUFFER_SIZE 2048 /* Tamaño del mensaje  */
#define BACKLOG 5 /* Cantidad de clientes que pueden ser atendidos al simultaneo */


int sockfd, /* File descriptor del socket del servidor */
	portno; /* Numero de puerto donde el servidor acepta conexiones */
     
struct sockaddr_in serv_addr; /* sockaddr_in es una estructura que contiene una direccion de internet */  

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
	
	struct sockaddr_in cli_addr; /* Direccion del socket del cliente */
	socklen_t clilen = sizeof(cli_addr);	
	int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* Espero por pedidos de clientes */	
	
	//Comienza el timer
	gettimeofday(&t1, NULL);
	
	//Realizo la tarea de servicio al cliente
	server_task(newsockfd);
	
	//Termina el timer
	gettimeofday(&t2, NULL);
	
	// compute and print the elapsed time in millisec
	et = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
	et += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
	
	printf("Tamaño del buffer %i bytes \n", BUFFER_SIZE);
	printf("Tiempo de servicio %f ms \n", et);
}


void server_task(int newsockfd)
{	
	if (newsockfd < 0) 
		error("ERROR aceptando la petición"); 	
	
    char input[BUFFER_SIZE]; /* Creo el buffer para recibir el mensaje */
    char output[BUFFER_SIZE]; /* Creo el buffer para el mensaje que se enviara al cliente */
    
    bzero(input, BUFFER_SIZE); /* Seteo todos los valores del buffer a cero */
    bzero(output, BUFFER_SIZE); /* Seteo todos los valores del buffer a cero */
	
	/* Leo los datos enviados por el cliente */ 
	if (recv(newsockfd, input, BUFFER_SIZE - 1, 0) < 0) 
		error("ERROR leyendo del socket");
	    
	
	bcopy((char *)input, (char *)output, BUFFER_SIZE); /* Copia lenght bytes del primer parametro al segundo */
	
	if (send(newsockfd, output, BUFFER_SIZE - 1, 0) < 0) /* Envio la respuesta al cliente */ 
		error("ERROR escribiendo en el socket");	

    close(newsockfd); /* Termino la conexion con el cliente */
}

void server_init(int portno)
{ 	
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
		fprintf(stderr,"usage %s port\n", argv[0]);
		exit(1);
	}
	
	server_init(atoi(argv[1])); /* Inicializo el servidor */
	server_listen(); /* Escucho por peticiones */
	server_exit(); /* Cierro el servidor */
    return 0; 
}


