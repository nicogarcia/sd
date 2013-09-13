#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../lib/table_library.h"

typedef struct {
	char str[2048];
} long_string;

typedef struct {
	char str[1024];
} half_long_string;

int local_call_4(int num) {
	num = (num * 2 + num - 4) / num;
	return num;
}

long_string local_call_2048(half_long_string str1, half_long_string str2) {
	long_string l;
	int n;

	n = n + n - n / n;

	strcpy(l.str, str1.str);
	strcpy(l.str + 1024, str2.str);

	return l;
}

int main() {
	int i, j;
	pid_t pid;
	int max_proc_count = 0;
	struct timespec start, end, diff[8];
	table* t;
	int widths[] = { 20, 20, 20 };

	// 1028 bytes arg
	half_long_string hl_str[2];
	for (j = 0; j < 2; j++)
		for (i = 0; i < 1024; i++)
			hl_str[j].str[i] = 65 + i % 26;

	// Local call 4 bytes
	clock_gettime(CLOCK_REALTIME, &start);
	local_call_4(1000);
	clock_gettime(CLOCK_REALTIME, &end);
	diff[0].tv_nsec = end.tv_nsec - start.tv_nsec;
	diff[0].tv_sec = end.tv_sec - start.tv_sec;

	// Local call 2048 bytes
	clock_gettime(CLOCK_REALTIME, &start);
	local_call_2048(hl_str[0], hl_str[1]);
	clock_gettime(CLOCK_REALTIME, &end);
	diff[1].tv_nsec = end.tv_nsec - start.tv_nsec;
	diff[1].tv_sec = end.tv_sec - start.tv_sec;

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
	table_add_data(t, 1, 1, "%lis %lins", diff[0].tv_sec, diff[0].tv_nsec);
	table_add_data(t, 1, 2, "%lis %lins", diff[1].tv_sec, diff[1].tv_nsec);

	// Write row data
	table_add_row(t);
	table_add_data(t, 2, 0, "RPC (local)");
	table_add_data(t, 2, 1, "%lis %lins", diff[0].tv_sec, diff[0].tv_nsec);
	table_add_data(t, 2, 2, "%lis %lins", diff[1].tv_sec, diff[1].tv_nsec);

	// Write row data
	table_add_row(t);
	table_add_data(t, 3, 0, "RPC (remota)");
	table_add_data(t, 3, 1, "%lis %lins", diff[0].tv_sec, diff[0].tv_nsec);
	table_add_data(t, 3, 2, "%lis %lins", diff[1].tv_sec, diff[1].tv_nsec);

	// Write row data
	table_add_row(t);
	table_add_data(t, 4, 0, "RPC (hetero)");
	table_add_data(t, 4, 1, "%lis %lins", diff[0].tv_sec, diff[0].tv_nsec);
	table_add_data(t, 4, 2, "%lis %lins", diff[1].tv_sec, diff[1].tv_nsec);

	// Print table
	table_print(t);

	return 0;
}
