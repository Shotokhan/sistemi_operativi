#define LIBERO 0
#define OCCUPATO 1
#define IN_USO 2

#define PROD 0
#define CONS 1

#define MAX_PRODOTTI 100

typedef struct{
	unsigned int id_fornitore;
	unsigned int stato;
} scaffale;

typedef struct{
	scaffale buffer[MAX_PRODOTTI];
	int livello_scorte;
} magazzino;

void Fornitore(monitor* M, magazzino* shm);

void Cliente(monitor* M, magazzino* shm);

int inizioProduzione(monitor* M, magazzino* shm);

void fineProduzione(monitor* M, magazzino* shm, int i);

int inizioConsumo(monitor* M, magazzino* shm);

void fineConsumo(monitor* M, magazzino* shm, int i);

void initMagazzino(magazzino* shm);
