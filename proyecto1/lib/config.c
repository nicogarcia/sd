#include <stdlib.h>
#include <stdio.h>
#include "../lib/config.h"
#include <string.h>

const int MACHINES_QTY = 4; // ALPHA, SUN, FEDORA, LOCALHOST
machine* machines[] = { &alpha, &sun_m, &fedora, &local };
machine* default_machines[] = { &default_alpha, &default_sun, &default_fedora,
		&default_local };
int launched_times = 0;

void config_server() {
	char answer;

	load_config("project_config");

	print_all_configs();
	printf("\nEsta es la configuracion actual, desea mantenerla? [y/n] ");
	answer = getchar();
	fflush(stdin);

	while (answer == 'n') {
		fflush(stdin);
		getchar();
		change_config();
		print_all_configs();
		printf("\nEsta es la configuracion actual, desea mantenerla? [y/n] ");
		answer = getchar();
		fflush(stdin);
	}
}

machine* choose_machine() {
	int i;
	int chosen;
	char input[3];

	for (i = 0; i < MACHINES_QTY; i++)
		printf("%d - %s\n", i + 1, machines[i]->name);
	printf("Opcion: ");
	fgets(input, 3, stdin);
	while (input != NULL && *input == '\n') {
		printf("Opcion: ");
		fgets(input, 3, stdin);
	}
	chosen = strtol(input, NULL, 10);
	fflush(stdin);

	while (!(chosen > 0 && chosen <= MACHINES_QTY)) {
		printf("Opcion invalida. Ingrese nuevamente.\nOpcion: ");
		chosen = getchar() - '0';
		getchar();
	}
	return machines[chosen - 1];
}

void load_config(char* filename) {
	FILE* fp = fopen(filename, "r");
	int i;

	if (fp == NULL) {
		printf("*** ERROR: Archivo invalido\n");
		return;
	}

	// Read config
	for (i = 0; i < MACHINES_QTY; i++) {
		fscanf(fp, "%s", default_machines[i]->name);
		fscanf(fp, "%s", default_machines[i]->ip);
		fscanf(fp, "%s", default_machines[i]->port);
		fscanf(fp, "%s", default_machines[i]->user);
		*machines[i] = *default_machines[i];
	}
	fclose(fp);
}

void print_config(machine* m) {
	printf("%s:\t%s\tPort: %s\t", m->name, m->ip, m->port);
	printf("User: %s\n", m->user);
}

void print_all_configs() {
	int i;
	for (i = 0; i < MACHINES_QTY; i++)
		print_config(machines[i]);
}

void change_config() {
	int i;
	char* res;
	char* pos;
	for (i = 0; i < MACHINES_QTY; i++) {
		printf("%s IP [%s]: ", machines[i]->name, default_machines[i]->ip);
		res = fgets(machines[i]->ip, 50, stdin);
		if ((pos = strchr(res, '\n')) != NULL)
			*pos = 0;
		if (*res == 0)
			strcpy(machines[i]->ip, default_machines[i]->ip);

		printf("%s Port [%s]: ", machines[i]->name, default_machines[i]->port);
		res = fgets(machines[i]->port, 50, stdin);
		if ((pos = strchr(res, '\n')) != NULL)
			*pos = 0;
		if (*res == 0)
			strcpy(machines[i]->port, default_machines[i]->port);

		printf("%s User [%s]: ", machines[i]->name, default_machines[i]->user);
		res = fgets(machines[i]->user, 50, stdin);
		if ((pos = strchr(res, '\n')) != NULL)
			*pos = 0;
		if (*res == 0)
			strcpy(machines[i]->user, default_machines[i]->user);
	}
	fflush(0);
}

void deploy(machine* m, char* execute_command) {
	char str[2500];

	printf("\t====== %s ======\n", m->name);
	// File uploading
	sprintf(str,
			"xterm -geometry 100x24-0+0 -T \"%s Project Upload - %s \" -e ",
			m->name, m->ip);
	sprintf(str + strlen(str), "\"sftp %s@%s < deployment_script_sftp  \"",
			m->user, m->ip);
	printf("\tEnviando script de deployment...\n");
	system(str);

	// Project compilation
	sprintf(str, "xterm -geometry 100x24-0+0 -T \"%s Compilation - %s \" -e ",
			m->name, m->ip);
	sprintf(str + strlen(str),
			"\"ssh %s@%s \\\"/bin/bash < deployment_script_ssh \\\" \"",
			m->user, m->ip);
	printf("\tCompilando...\n");
	system(str);

	// Project execution
	if (launched_times % 2)
		sprintf(str,
				"(xterm -hold -fg white -bg black -geometry 100x24-0+0  -T \"%s Project execution - %s \" -e ",
				m->name, m->ip);
	else
		sprintf(str,
				"(xterm -hold -fg white -bg blue -geometry 100x24-0-0  -T \"%s Project execution- %s \" -e ",
				m->name, m->ip);
	sprintf(str + strlen(str),
			"\"ssh -t %s@%s \\\"/bin/bash <(echo ~/project1_src/build/fedora/%s) \\\" \""
					") &", m->user, m->ip, execute_command);
	printf("\tEjecutando servidor...\n");
	system(str);

	launched_times++;
}
