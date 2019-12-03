#define DIM 5

typedef struct{
	int a;
	int b;
} elemento;

typedef struct{
	elemento vettore[DIM];
	int testa;
	int coda;
	int numLiberi;
	int numOccupati;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	pthread_cond_t cons;
} monitorVettore;

typedef struct{
	elemento buffer;
	pthread_mutex_t mutex;
	int numLettori;
	int numScrittori;
	pthread_cond_t lettore;
	pthread_cond_t scrittore;
} monitorBuffer;

typedef struct{
	monitorVettore* v;
	int threadId;
} bufGeneratore;

typedef struct{
	monitorVettore* v;
	monitorBuffer* b;
	int threadId;
} bufAggiornatore;

typedef struct{
	monitorBuffer* b;
	int threadId;
} bufDestinatario;

void genera(monitorVettore* v, elemento e);

elemento preleva(monitorVettore* v);

void aggiorna(monitorBuffer* b, elemento e);

void consulta(monitorBuffer* b);

void initMonitorVettore(monitorVettore* v);

void initMonitorBuffer(monitorBuffer* v);

void freeMonitorVettore(monitorVettore* v);

void freeMonitorBuffer(monitorBuffer* v);

void* generatore(void* buf);

void* aggiornatore(void* buf);

void* destinatario(void* buf);
