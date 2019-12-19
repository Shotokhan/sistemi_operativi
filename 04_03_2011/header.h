#include "mex.h"

typedef struct{
	int val[2];
	int pid;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
	int consumato;
} workerData;

void Manager(workerData* W);

void* Worker(void* data);

void checkError(int ret, char* msg);

void initData(workerData* W);

void removeData(workerData* W);
