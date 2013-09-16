/*
 * project_tester.c
 *
 *  Created on: 09/09/2013
 *      Author: nico
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

/** To add exercises modify this 3 parameters */
const int exercises_qty = 9;
char* exercises[] = { "./exercise_1", "./exercise_2", "./exercise_3",
		"./exercise_4", "", "./exercise_6", "./exercise_7", "", "" };
char* exercises_titles[] = { "Ejercicio 1: Creacion de procesos",
		"Ejercicio 2: Carga de imagen ejecutable",
		"Ejercicio 3: Llamadas locales y remotas", "Ejercicio 4: Sockets TCP",
		"*Ejercicio 5: Minicalculadora Local y RPC",
		"*Ejercicio 6: Comunicacion pares con sockets",
		"*Ejercicio 7: Sincronizacion de relojes", "*Ejercicio 8: Explicacion",
		"*Ejercicio 12: Sockets Java" };
/** ------------------------------------------ **/

void launch_excercise(int i);

int main() {
	int i;
	char input[3];
	int ex_selector;

	while (1) {

		system("clear");

		printf("=============================================\n");
		printf("  Bienvenido al asistente de prueba del\n");
		printf("  Proyecto N°1 - Sistemas Distribuidos 2013\n");
		printf("=============================================\n");
		printf("Elija un ejercicio para ejecutar:\n\n");

		for (i = 0; i < exercises_qty; i++) {
			printf("%i. %s\n", i + 1, exercises_titles[i]);
		}

		printf("\nOpcion [ enter para salir ]: ");
		fgets(input, 3, stdin);
		if (input != NULL && *input == '\n') {
			printf("\nHasta luego!\n");
			break;
		}
		ex_selector = strtol(input, NULL, 10);
		fflush(stdin);

		if (ex_selector > 0 && ex_selector <= exercises_qty) {
			system("clear");
			printf("%s\n", exercises_titles[ex_selector - 1]);
			printf("----------------------------------------\n");
			launch_excercise(ex_selector - 1);
		} else {
			printf("El ejercicio elegido no existe.\n");
			getchar();
			getchar();
		}
	}

	return 0;
}

void launch_excercise(int i) {
	pid_t pid;

	pid = fork();

	if (pid == 0) {
		execv(exercises[i], NULL);
		printf("*** Error: Could not launch the exercise. ***\n\n");
		getchar();
		getchar();
	}
	waitpid(pid, 0, 0);

	printf("\n*** Ejecucion finalizada, presione enter para continuar...\n");
	getchar();
	getchar();
}
