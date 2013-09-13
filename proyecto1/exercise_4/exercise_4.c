#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/config.h"

int main() {
	pid_t pid;
	char answer;

	load_config("project_config");

	print_all_configs();
	printf("\nEsta es la configuracion actual, desea cambiarla? [y/n] ");
	answer = getchar();
	fflush(0);

	while (answer == 'y') {
		change_config();
		print_all_configs();
		printf("\nEsta es la configuracion actual, desea cambiarla? [y/n] ");
		answer = getchar();
		fflush(0);
	}

	pid = fork();
	if (pid == 0) {
		printf("Iniciando servidor...\n\n");
		//deploy_server(machines[0]);
		system("xterm -hold -e ./exercise_4_server 3000");
	} else {
		sleep(1);
		printf("Iniciando cliente...\n\n");
		//deploy_server(machines[1]);
		system("xterm -hold -e ./exercise_4_client localhost 3000");
	}

	return 0;
}
