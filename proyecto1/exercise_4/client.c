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
#include "../lib/table_library.h"

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
	printf("(cliente) Inicializando sockets ...\n");
	server_hostname = hostname;
	server_portno = port; /* Asigno el puerto */
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); /* Creo el socket */
	if (sockfd < 0)
		error("(cliente) ERROR abriendo el socket");

}

void client_connect()
{	

	struct hostent *server; /* Servidor obtenido por el hostname */
	struct sockaddr_in serv_addr; /* Direccion del socket del servidor */

	server = gethostbyname(server_hostname); /* Toma un nombre como argumento y retorna un puntero a una estructura hostent que contiene informacion de ese host. */
	if (server == NULL)
		error("(cliente) ERROR no existe el host\n");

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */
	serv_addr.sin_family = AF_INET;

	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */
	serv_addr.sin_port = htons(server_portno); /* Convierte el numero de puerto en orden de bytes de la red */

	printf("(cliente) Estableciendo conexión con el servidor ...");
	 /* Intento conectar con el servidor */
	while (connect(server_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		sleep(0.5);
		printf(".");
	}
	printf("\n");
}

void client_request()
{
	char *buffer;
	int test, buffsize;
	struct timeval t1, t2;
	double et;

	printf("(cliente) Iniciando pruebas ...\n");
	//Realizo las pruebas de buffer para cada tamaño dado
	for(test = 0; test < tests_res.n_tests; test++)
	{
		printf("(cliente) Realizando prueba de buffer: %i bytes\n", tests_res.buffer_sizes[test]);
		buffsize = tests_res.buffer_sizes[test];
		buffer = calloc(buffsize, sizeof(char)); /* Creo el nuevo buffer */

		bzero(buffer, buffsize);
		//Lleno el buffer
		int i = 0;
		for(i = 0; i < buffsize; i++)
			buffer[i] = (i % 54) + 65;
	    
		gettimeofday(&t1, NULL);

		if (send(server_sockfd, buffer, buffsize - 1, 0) < 0) /* Intento enviar el pedido al servidor */
			error("(cliente) ERROR escribiendo en el socket");
	    	  
		if (recv(server_sockfd, buffer, buffsize - 1, 0) < 0)
			error("(cliente) ERROR leyendo del socket");

		gettimeofday(&t2, NULL);

		et = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		et += (t2.tv_usec - t1.tv_usec);   // us to ms

		tests_res.tests[test] = et;

		printf("(cliente) Prueba exitosa\n");
	}
	    
}

void show_results()
{
	int test;
	int widths[] = { 20, 25, 50, 20 };
	table* t;

	printf("(cliente) Resultados del experimento\n");

	// Initialize table
	t = table_initialize(4, widths);

	// Write row data
	table_add_row(t);
	table_add_data(t, 0, 0, "Tamaño del buffer");
	table_add_data(t, 0, 1, "Tiempo de conexión");
	table_add_data(t, 0, 2, "Tiempo trabajo servidor ( incluye tiempo de transmición )");
	table_add_data(t, 0, 3, "Tiempo de espera");


	for(test = 0; test < tests_res.n_tests; test++)
	{
		// Write row data
		table_add_row(t);
		table_add_data(t, test + 1, 0, "%li bytes", tests_res.buffer_sizes[test]);
		table_add_data(t, test + 1, 1, "%.2f ms", tests_res.conn_time);
		table_add_data(t, test + 1, 2, "%.2f ms", tests_res.tests[test]);
		table_add_data(t, test + 1, 3, "%.2f ms", tests_res.tests[test] + tests_res.conn_time);

	}

	// Print table
	table_print(t);
}

int main(int argc, char *argv[])
{   
	int tests;
	double et;
	struct timeval t1, t2;

	//Verifico si estan todos los argumentos requeridos
	if (argc < 2) {
		fprintf(stderr,"usage: %s server_hostname server_port [-b buffer_lenght[,...]]\n", argv[0]);
		exit(0);
	}	
	//Obtengo el tamaño del buffer, si no se paso se usa el tamaño por defecto
	if(argc > 2)
	{
		if(!strcmp(argv[3], "-b")){
			tests = argc - 4; /* Obtengo la cantidad de buffers a probar */
			tests_res.n_tests = tests;
			tests_res.buffer_sizes = calloc(tests_res.n_tests, sizeof(int));

			while(tests > 0)
			{
				tests_res.buffer_sizes[tests - 1] = atoi(argv[tests + 3]);
				tests--;
			}
		}
		else{
			fprintf(stderr,"usage: %s server_hostname server_port [-b buffer_lenght[,...]]\n", argv[0]);
			exit(0);
		}
	}
	else
	{
		tests_res.n_tests = 1;
		tests_res.buffer_sizes = calloc(1, sizeof(int));
		tests_res.buffer_sizes[0] = DEFAULT_BUFFER_SIZE;
	}

	tests_res.tests = calloc(tests_res.n_tests, sizeof(double));


	client_init(atoi(argv[2]), argv[1]);

	gettimeofday(&t1, NULL);
	client_connect(); /* Solicito tarea al servidor */
	gettimeofday(&t2, NULL);

	et = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
	et += (t2.tv_usec - t1.tv_usec);   // us to ms

	tests_res.conn_time = et;

	client_request();
   
	show_results();
    close(sockfd); /* Cierro la conexion */

    return 0;
}
