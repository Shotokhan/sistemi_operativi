typedef struct{
	key_t msqid;
	int threadId;
} buffer;

void* serverWorker(void* buf);

void initBuffer(buffer buf[], int N, int msqid);
