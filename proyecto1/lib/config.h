#ifndef CONFIG_H
#define CONFIG_H

// Config structures
typedef struct mq {
	char name[50];
	char ip[50];
	char port[50];
	char user[50];
	char pass[50];
} machine;

machine alpha, default_alpha;
machine sun,default_sun;
machine fedora,default_fedora;


void load_config(char* filename);

void print_config(machine* m);

void print_all_configs();

void change_config();

#endif
