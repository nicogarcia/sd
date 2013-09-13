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

#define BUFFER_SIZE 2048 /* Tamaño del mensaje */

int sockfd, /* Descriptor del socket del cliente */
	portno; /* NUmero de puerto */

struct sockaddr_in serv_addr; /* Direccion del socket del servidor */

void client_init(); /* Inica el socket del cliente */
void client_request(); /* Realiza la peticion al servidor */


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void client_init(int port, char *hostname)
{
	struct hostent *server; /* Servidor obtenido por el hostname */
	portno = port; /* Asigno el puerto */
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); /* Creo el socket */
	if (sockfd < 0)
		error("ERROR abriendo el socket");
		
	server = gethostbyname(hostname); /* Toma un nombre como argumento y retorna un puntero a una estructura hostent que contiene informacion de ese host. */
	if (server == NULL) 
		error("ERROR no existe el host\n");
	

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */
	serv_addr.sin_family = AF_INET;
	
	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */
	serv_addr.sin_port = htons(portno); /* Convierte el numero de puerto en orden de bytes de la red */
	
}

void client_request()
{	
	char input[BUFFER_SIZE];
	char output[BUFFER_SIZE];
	
	bzero(input, BUFFER_SIZE);
	bzero(output, BUFFER_SIZE);
	
	//Lleno el buffer
	int i = 0;
	for(i = 0; i < BUFFER_SIZE; i++)
		input[i] = (i % 54) + 65;
	    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) /* Intento conectar con el servidor */
		error("ERROR conectando");
	    
	if (send(sockfd, input, BUFFER_SIZE - 1, 0) < 0) /* Intento enviar el pedido al servidor */
	    error("ERROR escribiendo en el socket");
	    	  
	if (recv(sockfd, output, BUFFER_SIZE - 1, 0) < 0) 
	    error("ERROR leyendo del socket");
	    
	//printf("%s\n", buffer);
}


int main(int argc, char *argv[])
{   
	//Verifico si estan todos los argumentos requeridos
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}	
		
	double et;
	struct timeval t1, t2;

	//Comienza el timer
	gettimeofday(&t1, NULL);

	client_init(atoi(argv[2]), argv[1]);
	client_request(); /* Solicito tarea al servidor */

	//Termina el timer
	gettimeofday(&t2, NULL);

	// compute and print the elapsed time in millisec
	et = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
	et += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

	printf("Tamaño del buffer %i bytes \n", BUFFER_SIZE);
	printf("TIempo de espera %f ms \n", et);
   
    close(sockfd); /* Cierro la conexion */
    return 0;
}
