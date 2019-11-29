#define N 5

#define MUTEX 0
#define PROD 1
#define CONS 2

typedef struct{
	int elementi[N];
	int testa;
	int coda;
} BufferCircolare;

// cons inizializzato a N

void produci_elemento(int sem_id, BufferCircolare* buf);

void consuma_elementi(int sem_id, BufferCircolare* buf);

void waitSem(int semid, int semnum, int n);

void signalSem(int semid, int semnum, int n);

void initBuffer(BufferCircolare* buf);
