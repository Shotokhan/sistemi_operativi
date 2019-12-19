#define DIM_CODA 10

#define KEY_CHAR_MONITOR 'm'
#define KEY_CHAR_QUEUE 'q'

#define UTENTE 0
#define SCHEDULER 1

#define N_REQ 5
#define N_UTENTI 5
#define MEM 20

typedef struct{
	unsigned int posizione;
	pid_t processo;
} richiesta;

typedef struct{
	richiesta req[DIM_CODA];
	unsigned int testa;
	unsigned int coda;
	unsigned int numOccupati;
	monitorHoare* m;
	int id_monitor;
} codaCircolare;

void Schedulatore(codaCircolare* q);

void Utente(codaCircolare* q);

void initCoda(codaCircolare* q);

void removeCoda(codaCircolare* q);

void enqueue(codaCircolare* q, richiesta r);

void dequeue(codaCircolare* q, richiesta* r);

void waitProcesses();
