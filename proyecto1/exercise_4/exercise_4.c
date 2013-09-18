#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/config.h"
#include <string.h>
#include <ctype.h>

#define DEFAULT_PORT "12500"
#define DEFAULT_HOST "localhost"
#define DEFAULT_CONFIG "-b 2048 4"


int isnumber(char *str){
	int i,
		valid = 1;

	while(i < strlen(str) && valid)
		valid = isdigit(str[i++]);

	return valid;
}

int main() {

	char command[256],
		 buffer_command[256] = DEFAULT_CONFIG,
		 serv_port[50] = DEFAULT_PORT,
		 serv_host[50] = DEFAULT_HOST,
		 buffer[256];

	int n_tests = 0,
		loop = 1;

	pid_t cli_pid, serv_pid;

	printf("Desea realizar un experimento personalizado? (s/n): ");
	while(loop){
		scanf("%s", buffer);
		if(!strcmp(buffer, "s")){

			strcpy(buffer_command, "-b");
			system("clear");
			printf("Configuración del experimento\n");

			printf("Puerto del servidor: ");
			scanf("%s", serv_port);

			printf("Hostname del servidor: ");
			scanf("%s", serv_host);

			do{
				printf("Prueba de buffer %i [ ( q para salir )]: ", n_tests + 1);
				fflush(0);
				scanf("%s", buffer);
				if(isnumber(buffer))
				{
					strcat(buffer_command, " ");
					strcat(buffer_command, buffer);
					n_tests++;
				}
				else
					printf("%s no es un numero valido\n", buffer);
			}while(strcmp(buffer, "q"));

			loop = 0;
		}
		else{
			if(!strcmp(buffer, "n"))
				loop = 0;
			else
				printf("Debe ingresar s/n: ");
		}
	}

	cli_pid = fork();
	system("clear");
	if(cli_pid == 0)
	{
		strcpy(command, "xterm -hold -fn 9x18 -geometry  150x50-0+0 -e ./exercise_4_server ");
		strcat(command, serv_port);
		strcat(command, " ");
		strcat(command, buffer_command);
		if(system(command) < 0)
			perror("ERROR ejecutando servidor\n");
	}
	else
	{
		serv_pid = fork();
		if(serv_pid == 0){
			strcpy(command, "xterm -hold -fs 18 -geometry 150x50-0+0 -e ./exercise_4_client ");
			strcat(command, serv_host);
			strcat(command," ");
			strcat(command, serv_port);
			strcat(command, " ");
			strcat(command, buffer_command);
			if(system(command) < 0)
				perror("ERROR ejecutando cliente\n");
		}
	}

	waitpid(cli_pid, NULL, 0);
	waitpid(serv_pid, NULL, 0);


	return 0;
}
