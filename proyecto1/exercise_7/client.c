/* Un cliente en el dominio de internet que se se comunica utilizando TCP
   El numero de puerto y el nombre del host son pasados como argumento */
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
#include <signal.h>
#define MAX_ATTEMPS 4

int server_sockfd; /* Descriptor del socket del servidor */

typedef struct result{
	double travel_time_u,
		   travel_time_s;

	time_t serv_time_u,
		   serv_time_s,
		   my_time_u,
		   my_time_s,
		   time_dev;
}result_t;

typedef struct{
	result_t *results;
	char **hostnames;
	int *ports;
	int n_tests;
}test_t;

test_t tests_res;

struct timeval serv_time;

void client_init(); /* Inica el socket del cliente */
int client_connect(); /* Realiza la peticion al servidor */
void client_request(); /* Realiza la peticion al servidor */


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void client_init()
{
}

int client_connect(int n_test)
{
	int attemps = 0,
		backoff = 1;

	struct hostent *server; /* Servidor obtenido por el hostname */
	struct sockaddr_in serv_addr; /* Direccion del socket del servidor */

	server = gethostbyname(tests_res.hostnames[n_test]); /* Toma un nombre como argumento y retorna un puntero a una estructura hostent que contiene informacion de ese host. */
	if (server == NULL)	error("ERROR host doesn't exist\n");


	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* Seteo a cero todos los campos de la direccion del servidor */
	serv_addr.sin_family = AF_INET;

	//Copio los datos obtenidos del servidor a la direccion del servidor
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* Copia lenght bytes del primer parametro al segundo */
	serv_addr.sin_port = htons(tests_res.ports[n_test]); /* Convierte el numero de puerto en orden de bytes de la red */

	printf("(client) Connecting to server ...");
	/* Intento conectar con el servidor */
	while(attemps < MAX_ATTEMPS && connect(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		sleep(backoff);
		printf(".");
		fflush(0);
		backoff *= 1.5;
		attemps++;
	}

	printf("\n");
	if(attemps >= MAX_ATTEMPS)
		return 0;
	else return 1;
}

void client_request(int n_test)
{
	char buffer;
	result_t *res;

	//Comiezo el cronometraje
	res = &tests_res.results[n_test];

	start_timer();

	if (send(server_sockfd, &buffer, sizeof(char), 0) < 0) /* Intento enviar el pedido al servidor */
		error("ERROR writing socket");

	if (recv(server_sockfd, &serv_time, sizeof(struct timeval), 0) < 0)
		error("ERROR reading socket");

	//Finalizo el cronometraje
	stop_timer();

	//Calculo los resultados del benchmark
	utimediff(&res->travel_time_u);
	timediff(&res->travel_time_s);

	res->travel_time_s = res->travel_time_s * 0.5f;
	res->travel_time_u = res->travel_time_u * 0.5f;

	res->serv_time_u = serv_time.tv_usec + res->travel_time_u;
	res->serv_time_s = serv_time.tv_sec + res->travel_time_s;

	get_start_time_secs(&res->my_time_s);
	get_start_time_usecs(&res->my_time_u);

	res->time_dev = abs((res->serv_time_s - res->my_time_s) * 1000.0);      // sec to ms
	res->time_dev += abs((res->serv_time_u - res->my_time_u));   // us to ms
}

void show_results(int n_test)
{
	struct tm *nowtm;
	result_t res;
	char timef[256], tmbuf[64];

	res = tests_res.results[n_test];

	printf("RESULTS %i\n", n_test);
	printf("Server: %s %i\n", tests_res.hostnames[n_test], tests_res.ports[n_test]);
	printf("Travel time: %.2f ms \n", res.travel_time_s * 1000 + res.travel_time_u);

	nowtm = localtime(&res.serv_time_s);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(timef, sizeof timef, "%s.%06d", tmbuf, res.serv_time_u);
	printf("Server time: %s\n", timef);

	nowtm = localtime(&res.my_time_s);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(timef, sizeof timef, "%s.%06d", tmbuf, res.my_time_u);
	printf("Local time: %s\n", timef);

	printf("Time deviation: %.2d ms\n", res.time_dev);

}

int main(int argc, char *argv[])
{
	int tests = 0,
		currtest = 0,
		test = 0;

	//Verifico si estan todos los argumentos requeridos
	if (argc < 3) {
		fprintf(stderr,"usage: %s hostname port [hostname] [port]\n", argv[0]);
		exit(0);
	}
	tests = (argc - 1) / 2; /* Obtengo la cantidad de buffers a probar */
	tests_res.n_tests = tests;
	tests_res.hostnames = calloc(tests_res.n_tests, sizeof(char **));
	tests_res.ports = calloc(tests_res.n_tests, sizeof(int *));
	tests_res.results = (result_t *) calloc(tests_res.n_tests, sizeof(result_t));

	while(test < tests)
	{
		tests_res.hostnames[test] = argv[test * 2 + 1];
		tests_res.ports[test] = atoi(argv[test * 2 + 2]);
		test += 1;
	}

	test = 0;
	client_init();
	system("clear");
	while(test < tests){
		if(client_connect(test) > 0){
			client_request(test);
			show_results(test);
		}
		test++;
	}
    return 0;
}
