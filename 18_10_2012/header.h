#define KEY_PATH "./"

#define KEY_CHAR '0'

#define OP 1
#define R0 2

#define MAX 4

typedef struct{
	long tipo;
	int operandi[MAX];
	int numOperandi;
} msgCalc;

void initQueues(int* qid, int nQueues);

void removeQueues(int* qid, int nQueues);

void checkError(int retVal, char* msg);

void waitProcesses();

void msgGen(msgCalc* msg, int qid, int nOps, char* opName, int* count);

void opSend(msgCalc* msg, int qid, int nOps, int* ops);
