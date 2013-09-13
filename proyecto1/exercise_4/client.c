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

#define DEFAULT_BUFFER_SIZE 2048

typedef struct{
	double *tests; /* Guardo los tiempos de respuesta a cada pedido */
	int *buffer_sizes;
	double conn_time;
	int n_tests;
}test_t;


test_t tests_res;

int sockfd,/* Descriptor del socket del cliente */
	server_sockfd, /* Descriptor del socket del servidor */
	portno, /* Numero de puerto */
	server_portno; /* Numero de puerto del servidor */

char *server_hostname;

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
	char *buffer;
	int test,
		buffsize;
	struct timeval t1, t2;
	double et;

	for(test = 0; test < tests_res.n_tests; test++)
	{
		buffsize = tests_res.buffer_sizes[test];
		buffer = calloc(buffsize, sizeof(char)); /* Creo el nuevo buffer */

		bzero(buffer, buffsize);
		//Lleno el buffer
		int i = 0;
		for(i = 0; i < buffsize; i++)
			buffer[i] = (i % 54) + 65;
	    
		gettimeofday(&t1, NULL);

		if (send(server_sockfd, buffer, buffsize - 1, 0) < 0) /* Intento enviar el pedido al servidor */
			error("ERROR escribiendo en el socket");
	    	  
		if (recv(server_sockfd, buffer, buffsize - 1, 0) < 0)
			error("ERROR leyendo del socket");

		gettimeofday(&t2, NULL);

		et = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		et += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

		tests_res.tests[test] = et;
	}
	    
}

void show_results()
{
	int test;
	system("clear");

	printf("Información de la conexión\n");
	printf("-----------------------------------------\n");
	printf("Tiempo de conexión: %f ms \n", tests_res.conn_time);

	for(test = 0; test < tests_res.n_tests; test++)
	{
		printf("Tamaño del buffer: %i bytes \n", tests_res.buffer_sizes[test]);
		printf("Tiempo trabajo servidor: %f ms \n", tests_res.tests[test]);

	}
}

int main(int argc, char *argv[])
{   
	int tests;
	double et;
	struct timeval t1, t2;

	//Verifico si estan todos los argumentos requeridos
	if (argc < 3) {
		fprintf(stderr,"usage: %s hostname port [-b buffer_lenght[,...]]\n", argv[0]);
		exit(0);
	}	
	//Obtengo el tamaño del buffer, si no se paso se usa el tamaño por defecto
	if(argc >= 3)
	{
		if(!strcmp(argv[3], "-b")){
			tests = argc - 4; /* Obtengo la cantidad de buffers a probar */
			tests_res.n_tests = tests;
			tests_res.buffer_sizes = calloc(tests_res.n_tests, sizeof(char));

			while(tests > 0)
			{
				tests_res.buffer_sizes[tests_res.n_tests - tests] = atoi(argv[tests + 3]);
				tests--;
			}
		}
		else{
			fprintf(stderr,"usage: %s hostname port [-b buffer_lenght[,...]]\n", argv[0]);
			exit(0);
		}
	}
	else
	{
		tests_res.n_tests = 1;
		tests_res.buffer_sizes = calloc(1, sizeof(char));
		tests_res.buffer_sizes[0] = DEFAULT_BUFFER_SIZE;
	}

	tests_res.tests = calloc(tests_res.n_tests, sizeof(double));

	client_init(atoi(argv[2]), argv[1]);

	gettimeofday(&t1, NULL);
	client_connect(); /* Solicito tarea al servidor */
	gettimeofday(&t2, NULL);

	et = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
	et += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

	tests_res.conn_time = et;

	client_request();


   
	show_results();
    close(sockfd); /* Cierro la conexion */

    return 0;
}
