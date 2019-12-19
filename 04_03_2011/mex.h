#define KEY_PATH "./"
#define KEY_CHAR_REQ 'r'
#define KEY_CHAR_ANS 'a'

typedef struct{
	long tipo;
	int val[2];
	int pid;
} client_msg;

typedef struct{
	long tipo;
	int result;
} server_msg;
