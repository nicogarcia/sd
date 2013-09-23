#ifndef CONFIG_H
#define CONFIG_H

// Config structures
typedef struct mq {
	char name[50];
	char ip[50];
	char port[50];
	char user[50];
} machine;

machine alpha, default_alpha;
machine sun_m, default_sun;
machine fedora, default_fedora;
machine local, default_local;

extern machine* machines[];
extern machine* default_machines[];

machine* choose_machine();

void load_config(char* filename);

void print_config(machine* m);

void print_all_configs();

void change_config();

void config_server();

void deploy(machine* m, char* execute_command);

#endif
