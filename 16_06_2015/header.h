#define REQUEST_TO_SEND 1

#define KEY_PATH "./"
#define KEY_CHAR 'z'

typedef struct{
	long tipoMsg;
	int pid;
} synch;

typedef struct{
	long pid;
	unsigned short int val[2];
	unsigned short int sum;
} data;

void sendSincrona(int msqid, data* D);

void receiveRendezVous(int msqid, data* D);

int openQueue(key_t keyval);

int getQueue(key_t keyval);

void removeQueue(int msqid);

