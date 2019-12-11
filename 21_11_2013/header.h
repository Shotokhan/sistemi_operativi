#define EXIT_MSG "exit"

#define EXIT_TYPE 2

#define CNT_TYPE 3

#define REMOVE_Q 4

#define MAX_DIM 21

#define KEY_PATH "./"

typedef struct{
	long tipo;
	char data[11];
	int end;
} msg;

typedef struct{
	long tipo;
	int cnt;
} msgCnt;

void mittente(int q1, int q2);

void ricevente(int q2);

void inviaMsg(int q1, int q2, int counter, int daemon);

void checkError(int retVal, char* msg);

void initQueues(int* q1, int* q2, char c1, char c2);

void removeQueues(int q1, int q2);
