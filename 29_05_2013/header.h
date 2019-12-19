#include "mex.h"

#define DIM 10

#define LIBERO 0
#define IN_USO 1
#define OCCUPATO 2

#define N_WORKERS 2

typedef struct{
	pthread_mutex_t mutex;
	pthread_cond_t manager;
	pthread_cond_t worker;
	client_msg buf[DIM];
	int stato[DIM];
	int numOccupati;
	int numLiberi;
} buffer;

typedef struct{
	buffer* b;
	pthread_t workers[N_WORKERS];
} managerData;

void initBuffer(buffer* b);

void removeBuffer(buffer* b);

void manager_produzione(buffer* b, client_msg m);

void worker_consumo(buffer* b, client_msg* m);

void* Manager(void* data);

void* Worker(void* data);

void checkError(int ret, char* msg);
