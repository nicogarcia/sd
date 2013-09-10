/*
 * exercise_2.c
 *
 *  Created on: 10/09/2013
 *      Author: nico
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

int main() {
	pid_t pid;
	struct timespec start;
	struct timespec end;
	int max_proc_count = 0;
	long execv_time;

	// Load image with execv()
	printf("Cargando imagen ejecutable mediante execv()...\n");
	clock_gettime(CLOCK_REALTIME, &start);
	pid = fork();
	if (pid == 0){
		execv("./exercise_2_image", NULL);
		printf("*** Error: Could not load the image\n");
	}
	waitpid(pid, 0, 0);
	clock_gettime(CLOCK_REALTIME, &end);

	execv_time = end.tv_nsec - start.tv_nsec;

	// Load image with system()
	printf("\nCargando imagen ejecutable mediante system()...\n");
	clock_gettime(CLOCK_REALTIME, &start);
	system("./exercise_2_image");
	clock_gettime(CLOCK_REALTIME, &end);

	if (pid != -1 && pid != 0) {
		printf("\nTiempo en cargar imagen ejecutable con execv(): %li ns (%li us)\n",
				execv_time, execv_time / 1000);
		printf("Tiempo en cargar imagen ejecutable con system(): %li ns (%li us)\n",
				end.tv_nsec - start.tv_nsec,
				(end.tv_nsec - start.tv_nsec) / 1000);
	}

	return 0;
}
