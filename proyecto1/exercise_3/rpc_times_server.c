#include "rpc_times.h"

int *
rpc_call_4_1_svc(int *argp, struct svc_req *rqstp) {
	static int result;

	result = (*argp * 2 + *argp - 4) / *argp;

	return &result;
}

long_string *
rpc_call_2048_1_svc(half_long_string *argp, struct svc_req *rqstp) {
	static long_string result;

	int n = 2;

	n = n + n - n / n;

	strcpy(result.str, *argp->str1);
	strcpy(result.str + 1024, *argp->str2);

	return &result;
}
