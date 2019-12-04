#define N_CLIENT 8
#define N_BALANCE 2
#define N_SERVER 3

#define KEY_PATH "./"
#define KEY_CHAR_1 'c'
#define KEY_CHAR_2 'b'
#define KEY_CHAR_3 's'

typedef struct{
	long tipo;
	int data;
} Msg;

void client(int qidClient);

// utilizzo una coda in più per i soli balancer:
// in questo modo, nel caso ci fossero più balancer,
// possono gestire in mutua esclusione una variabile 
// di conteggio per il balancing circolare dei server

void balancer(int qidClient, int qidBalance, int qidServer[]);

void server(int qidServer);

void initQueues(int* qidClient, int* qidBalance, int qidServer[]);

void removeQueues(int qidClient, int qidBalance, int qidServer[]);

void checkError(int retVal, char* msg);
