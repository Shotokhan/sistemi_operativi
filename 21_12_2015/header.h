#define DIM 10

#define LIBERO 0
#define IN_USO 1
#define OCCUPATO 2

typedef struct{
	int indirizzo;
	int dato;
} Buffer;

typedef struct{
	Buffer vettore[DIM];
	int stato[DIM];
	pthread_mutex_t mutex;
	pthread_cond_t prod;
	int numOccupati;
	int numLiberi;
} GestioneIO;

typedef struct{
	GestioneIO* g;
	int threadId;
} Data;

void Inizializza(GestioneIO* g);

void Produci(GestioneIO* g, Buffer* b);

int Consuma(GestioneIO* g, Buffer* b);

void* Produttore(void* data);

void* Consumatore(void* data);

void Rimuovi(GestioneIO* g);

void checkError(int retVal, char* msg);
