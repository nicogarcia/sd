#include "rpc_times.h"

int *
rpc_call_4_1_svc(int *argp, struct svc_req *rqstp) {
	static int result;

	/*
	 * insert server code here
	 */

	return &result;
}

long_string *
rpc_call_2048_1_svc(half_long_string *argp, struct svc_req *rqstp) {
	static long_string result;

	/*
	 * insert server code here
	 */

	return &result;
}
