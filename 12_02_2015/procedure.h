#define MAX_OPERANDI 4
#define MAX_ELAB 5

#define LIBERO 0
#define IN_USO 1
#define OCCUPATO 2

typedef struct{
	int operandi[MAX_OPERANDI];
	int tot_operandi;
} buffer;

typedef struct{
	buffer elaborazioni[MAX_ELAB];
	int stato[MAX_ELAB];
	int num_occupati;
	int num_liberi;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
} MonitorElaborazioni;

void* Richiedente(void* data);

void* Elaboratore(void* data);

void produzione(MonitorElaborazioni* m);

void consumo(MonitorElaborazioni* m);

void init_monitorElab(MonitorElaborazioni* m);

void destroy_monitorElab(MonitorElaborazioni* m);




