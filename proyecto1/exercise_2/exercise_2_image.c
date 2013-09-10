#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int i;
	FILE *fp;

	printf("\tRealizando 1000 iteraciones...\n");
	for (i = 0; i < 1000; i++)
		i = i + 0 + 0;

	fp = fopen("salida.txt", "w+");
	printf("\tEscribiendo en salida.txt...\n");
	fprintf(fp, "Soy la imagen ejecutable cuyo pid es: \n");

	printf("\tEscribiendo process ID...\n");
	fprintf(fp, "%i\n", getpid());

	return 0;
}
