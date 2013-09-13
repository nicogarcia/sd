#include <stdlib.h>
#include <stdio.h>
#include "../lib/config.h"
#include <string.h>

machine* machines[] = { &alpha, &sun, &fedora };
machine* default_machines[] = { &default_alpha, &default_sun, &default_fedora };

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
	for (i = 0; i < 3; i++) {
		printf("%s IP [%s]: ", machines[i]->name, default_machines[i]->ip);
		fflush(0);
		res = fgets(machines[i]->ip, 50, stdin);
		if (res == NULL)
			strcpy(machines[i]->ip, default_machines[i]->ip);

		printf("%s Port [%s]: ", machines[i]->name, default_machines[i]->port);
		res = fgets(machines[i]->port, 50, stdin);
		if (res == NULL)
			strcpy(machines[i]->port, default_machines[i]->port);

		printf("%s User [%s]: ", machines[i]->name, default_machines[i]->user);
		res = fgets(machines[i]->user, 50, stdin);
		if (res == NULL)
			strcpy(machines[i]->user, default_machines[i]->user);

		printf("%s Pass [%s]: ", machines[i]->name, default_machines[i]->pass);
		res = fgets(machines[i]->pass, 50, stdin);
		if (res == NULL)
			strcpy(machines[i]->pass, default_machines[i]->pass);

	}
	fflush(0);
}

void deploy_server(machine* m) {
	char str[255] = "xterm -hold -e scp ";

	printf("Enviando script de deployment...\n");
	sprintf(str + strlen(str), "deployment_script_%s %s@%s", m->name, m->user);

	system(str);
}
