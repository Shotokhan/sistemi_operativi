#define DIM 5

typedef struct{
	int vettore[DIM];
	int testa, coda;
	int produttori_in_attesa;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
	int num_occupati;
} MonitorCoda;

int produzione(MonitorCoda* m, int valore);

int consumazione(MonitorCoda* m);

void init_monitor(MonitorCoda* m);

void remove_monitor(MonitorCoda* m);

void check_error(int ret, char* msg);

void* produttore(void* monitor);

void* consumatore(void* monitor);
