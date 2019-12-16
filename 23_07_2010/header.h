#define KEY_PATH "./"

#define N_CLIENT 5
#define N_MSG 15

// messaggi di tipo Don't Care
#define DC 10

#define DIM 10

typedef struct{
	long tipo;
	int pid;
} msg;

typedef struct{
	msg data[DIM];
	int count;
} buffer;

void client(int msqid);

void server(int msqid_c, int msqid_p, int printer);

void printer(int msqid);

void init_queues(char key1, char key2, int qid[2]);

void remove_queues(int msqid_c, int msqid_p);

void check_error(int ret, char* msg);

void wait_processes();
