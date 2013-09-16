#include <stdlib.h>
#include <stdio.h>
#include "../lib/config.h"
#include <string.h>

machine* machines[] = { &alpha, &sun, &fedora };
machine* default_machines[] = { &default_alpha, &default_sun, &default_fedora };
int launched_times = 0;

machine* choose_machine() {
	int i;
	int chosen;
	char input[3];

	for (i = 0; i < 3; i++)
		printf("%d - %s\n", i + 1, machines[i]->name);
	printf("Opcion: ");
	fgets(input, 3, stdin);
	while (input != NULL && *input == '\n') {
		printf("Opcion: ");
		fgets(input, 3, stdin);
	}
	chosen = strtol(input, NULL, 10);
	fflush(stdin);

	while (!(chosen > 0 && chosen <= 3)) {
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
	for (i = 0; i < 3; i++) {
		fscanf(fp, "%s", default_machines[i]->name);
		fscanf(fp, "%s", default_machines[i]->ip);
		fscanf(fp, "%s", default_machines[i]->port);
		fscanf(fp, "%s", default_machines[i]->user);
		fscanf(fp, "%s", default_machines[i]->pass);
		*machines[i] = *default_machines[i];
	}
	fclose(fp);
}

void print_config(machine* m) {
	printf("%s:\t%s\tPort: %s\t", m->name, m->ip, m->port);
	printf("User: %s\tPass: %s\n", m->user, m->pass);
}

void print_all_configs() {
	int i;
	for (i = 0; i < 3; i++)
		print_config(machines[i]);
}

void change_config() {
	int i;
	char* res;
	char* pos;
	for (i = 0; i < 3; i++) {
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

		printf("%s Pass [%s]: ", machines[i]->name, default_machines[i]->pass);
		res = fgets(machines[i]->pass, 50, stdin);
		if ((pos = strchr(res, '\n')) != NULL)
			*pos = 0;
		if (*res == 0)
			strcpy(machines[i]->pass, default_machines[i]->pass);

	}
	fflush(0);
}

void deploy(machine* m, char* execute_command) {
	char str[2500];

	// File uploading
	strcpy(str, "xterm -e ");
	sprintf(str + strlen(str), "\"sftp -b deployment_script_sftp %s@%s \"",
			m->user, m->ip);
	printf("\tEnviando script de deployment...\n");
	system(str);

	// Project compilation
	strcpy(str, "xterm -e ");
	sprintf(str + strlen(str),
			"\"ssh %s@%s \\\"/bin/bash < deployment_script_ssh \\\" \"",
			m->user, m->ip);
	printf("\tCompilando en servidor...\n");
	system(str);

	// Project compilation
	if (launched_times % 2)
		strcpy(str,
				"(xterm -fg white -bg black -geometry 100x24-0+0 -e ");
	else
		strcpy(str,
				"(xterm -fg white -bg blue -geometry 100x24-0-0 -e ");
	sprintf(str + strlen(str),
			"\"ssh -t %s@%s \\\"/bin/bash <(echo ~/project1_src/build/fedora/./%s) \\\" \""
					") &", m->user, m->ip, execute_command);
	printf("\tEjecutando...\n");
	system(str);

	launched_times++;
}
