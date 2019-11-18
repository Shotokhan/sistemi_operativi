#define N_PROD 3
#define N_CONS 3

typedef int msg;

typedef struct{
	msg buffer;
	int buffer_pieno;
	int buffer_vuoto;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
} buff;

void* Produttore(void* heap);

void* Consumatore(void* heap);

void Produci(buff* b, msg mx);

void Consuma(buff* b);

void iniziaConsumo(buff* b);
void fineConsumo(buff* b);
void iniziaProduzione(buff* b);
void fineProduzione(buff* b);

void init_buffer(buff* b);

void free_buffer(buff* b);
