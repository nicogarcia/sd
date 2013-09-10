/*
 * exercise_1.c
 *
 *  Created on: 10/09/2013
 *      Author: nico
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int main(){
	pid_t pid;
	struct timespec start;
	struct timespec end;
	int max_proc_count = 0;

	clock_gettime(CLOCK_REALTIME, &start);
	pid = fork();
	clock_gettime(CLOCK_REALTIME, &end);

	if(pid == 0)
		return 0;

	if(pid != -1 && pid != 0){
		printf("Tiempo en crear un proceso: %li ns (%li us)\n",
				end.tv_nsec - start.tv_nsec,
				(end.tv_nsec - start.tv_nsec) / 1000);
	}

	do{
		pid = fork();
		if(pid == 0 || pid == -1)
			break;
		max_proc_count++;
	}while(pid != -1);

	if(pid != 0)
		printf("Maxima cantidad de procesos: %i\n", max_proc_count);

	return 0;
}
