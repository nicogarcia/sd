typedef struct ls long_string;
typedef struct hls half_long_string;

struct ls {
	char str[2048];
};

struct hls {
	char str1[1024];
	char str2[1024];
};

program RPC_TIMES {
	version RPC_TIMES_VERSION {
		int RPC_CALL_4(int) = 1;
		long_string RPC_CALL_2048(half_long_string) = 2;
	} = 1;
} = 1;