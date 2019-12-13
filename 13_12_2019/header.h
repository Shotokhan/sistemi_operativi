typedef struct{
	int operando[5];
	int testa;
	int coda;
	int numPieni;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
} MonitorOperandi;

typedef struct{
	int risultato[5];
	int testa;
	int coda;
	int numPieni;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
} MonitorRisultati;

typedef struct{
	MonitorOperandi* op;
	MonitorRisultati* r;
	int thread_id;
} dataOpRis;

void inserisci_operando(MonitorOperandi* op, int operando);

int preleva_operando(MonitorOperandi* op);

void inserisci_risultato(MonitorRisultati* r, int risultato);

int preleva_risultato(MonitorRisultati* r);

void inizializza(MonitorOperandi* op, MonitorRisultati* r);

void rimuovi(MonitorOperandi* op, MonitorRisultati* r);

void checkError(int retVal, char* msg);

void* Prod(void* data);

void* Cons_Prod(void* data);

void* Cons(void* data);


