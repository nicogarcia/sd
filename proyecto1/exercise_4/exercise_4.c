#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/config.h"

int main() {
	pid_t pid;
	char str[255];
	machine* server;
	machine* client;

	config_server();

	printf("Elija el servidor:\n");
	server = choose_machine();

	printf("Elija el cliente:\n");
	client = choose_machine();

	printf("Iniciando servidor en %s (%s)...\n", server->name, server->ip);
	sprintf(str, "./exercise_4_server %s", server->port);
	deploy(server, str);

	printf("\nIniciando cliente en %s (%s)...\n", client->name, client->ip);
	sprintf(str, "./exercise_4_client %s %s", server->ip, server->port);
	deploy(client, str);

	return 0;
}
