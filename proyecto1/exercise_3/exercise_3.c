#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../lib/table_library.h"
#include "../lib/config.h"
#include "rpc_times.h"

int local_call_4(int num) {
	num = (num * 2 + num - 4) / num;
	return num;
}

long_string local_call_2048(half_long_string str1) {
	long_string l;
	int n = 2;

	n = n + n - n / n;

	strcpy(l.str, str1.str1);
	strcpy(l.str + 1024, str1.str2);

	return l;
}

CLIENT* client_create(char* host) {
	CLIENT *clnt;

	clnt = clnt_create(host, RPC_TIMES, RPC_TIMES_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}
	return clnt;
}

void rpc_call_2048(char *host, half_long_string hl1) {
	CLIENT *clnt;
	long_string *result_2;

	clnt = client_create(host);

	result_2 = rpc_call_2048_1(&hl1, clnt);
	if (result_2 == (long_string *) NULL) {
		clnt_perror(clnt, "call failed");
	}

	clnt_destroy(clnt);
}

void rpc_call_4(char *host, int arg_4bytes) {
	CLIENT *clnt;
	int *result_1;
	int rpc_call_4_1_arg;

	clnt = client_create(host);

	result_1 = rpc_call_4_1(&rpc_call_4_1_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror(clnt, "call failed");
	}

	clnt_destroy(clnt);
}

int main() {
	int i;
	struct timespec start, end, diff[8];
	table* t;
	int widths[] = { 20, 20, 20 };
	machine* server;
	half_long_string hl_str;

	config_server();

	deploy(machines[0], "./rpc_times_server");
	deploy(machines[2], "./rpc_times_server");
	deploy(machines[3], "./rpc_times_server");

	sleep(5);

	// 1028 bytes arg
	for (i = 0; i < 1024; i++) {
		hl_str.str1[i] = 65 + i % 26;
		hl_str.str2[i] = 65 + i % 26;
	}

	{
		// Local call 4 bytes
		printf("Calling Local (no RPC): 4 bytes");
		fflush(stdout);
		clock_gettime(CLOCK_REALTIME, &start);
		local_call_4(1000);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[0].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[0].tv_sec = end.tv_sec - start.tv_sec;

		// Local call 2048 bytes
		printf(", 2048 bytes\t\tOK\n");
		clock_gettime(CLOCK_REALTIME, &start);
		local_call_2048(hl_str);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[1].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[1].tv_sec = end.tv_sec - start.tv_sec;
	}
	{
		server = machines[4];
		// Remote call 4 bytes
		printf("Calling Local RPC: 4 bytes");
		clock_gettime(CLOCK_REALTIME, &start);
		rpc_call_4(server->ip, 1000);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[2].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[2].tv_sec = end.tv_sec - start.tv_sec;

		// Remote call 2048 bytes
		printf(", 2048 bytes\t\tOK\n");
		clock_gettime(CLOCK_REALTIME, &start);
		rpc_call_2048(server->ip, hl_str);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[3].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[3].tv_sec = end.tv_sec - start.tv_sec;
	}
	{
		server = machines[2];
		// Remote call 4 bytes (Remote: Fedora)
		printf("Calling Remote Homogeneous: 4 bytes");
		clock_gettime(CLOCK_REALTIME, &start);
		rpc_call_4(server->ip, 1000);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[4].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[4].tv_sec = end.tv_sec - start.tv_sec;

		// Remote call 2048 bytes (Remote: Fedora)
		printf(", 2048 bytes\t\tOK\n");
		clock_gettime(CLOCK_REALTIME, &start);
		rpc_call_2048(server->ip, hl_str);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[5].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[5].tv_sec = end.tv_sec - start.tv_sec;
	}
	{
		server = machines[2];
		// Remote call 4 bytes (Remote: Alpha)
		printf("Calling Remote heterogeneous: 4 bytes");
		clock_gettime(CLOCK_REALTIME, &start);
		rpc_call_4(server->ip, 1000);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[6].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[6].tv_sec = end.tv_sec - start.tv_sec;

		// Remote call 2048 bytes (Remote: Alpha)
		printf(", 2048 bytes\t\tOK\n");
		clock_gettime(CLOCK_REALTIME, &start);
		rpc_call_2048(server->ip, hl_str);
		clock_gettime(CLOCK_REALTIME, &end);
		diff[7].tv_nsec = end.tv_nsec - start.tv_nsec;
		diff[7].tv_sec = end.tv_sec - start.tv_sec;
	}

	printf("\nResultados\n");
	printf("==========\n");

	// Initialize table
	t = table_initialize(3, widths);
	// Write row data
	table_add_row(t);
	table_add_data(t, 0, 0, "");
	table_add_data(t, 0, 1, "4b in, 4b out");
	table_add_data(t, 0, 2, "2x1024 in, 2048 out");

	// Write row data
	table_add_row(t);
	table_add_data(t, 1, 0, "Local call");
	table_add_data(t, 1, 1, "%lis %09li ns", diff[0].tv_sec, diff[0].tv_nsec);
	table_add_data(t, 1, 2, "%lis %09li ns", diff[1].tv_sec, diff[1].tv_nsec);

	// Write row data
	table_add_row(t);
	table_add_data(t, 2, 0, "RPC (local)");
	table_add_data(t, 2, 1, "%lis %09li ns", diff[2].tv_sec, diff[2].tv_nsec);
	table_add_data(t, 2, 2, "%lis %09li ns", diff[3].tv_sec, diff[3].tv_nsec);

	// Write row data
	table_add_row(t);
	table_add_data(t, 3, 0, "RPC (remota)");
	table_add_data(t, 3, 1, "%lis %09li ns", diff[4].tv_sec, diff[4].tv_nsec);
	table_add_data(t, 3, 2, "%lis %09li ns", diff[5].tv_sec, diff[5].tv_nsec);

	// Write row data
	table_add_row(t);
	table_add_data(t, 4, 0, "RPC (hetero)");
	table_add_data(t, 4, 1, "%lis %09li ns", diff[6].tv_sec, diff[6].tv_nsec);
	table_add_data(t, 4, 2, "%lis %09li ns", diff[7].tv_sec, diff[7].tv_nsec);

	// Print table
	table_print(t);

	return 0;
}
