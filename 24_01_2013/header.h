#define DISPONIBILE 0
#define AFFITTATO 1

#define N_FILM 3
#define N_COPIE 2

typedef struct{
	int id_film;
	int id_copia;
	int stato;
} DVD;

typedef struct{
	DVD dvd[N_FILM * N_COPIE];	
	int numCopie[N_FILM];
	pthread_mutex_t mutex;
	pthread_cond_t affitto[N_FILM];
} Monitor;

void* cliente(void* monitor);

void* controllo(void* monitor);

int affitta(Monitor* m, int id_film);

void restituisci(Monitor* m, int id_film, int id_copia);

void stampa(Monitor* m);

void init_monitor(Monitor* m);

void remove_monitor(Monitor* m);

void check_error(int ret, char* msg);

// questa ricerca si potrebbe implementare in modo più efficiente
// allocando un albero binario sull'heap invece di un vettore
// questa funzione ritorna l'indice in cui il DVD cercato si trova nel vettore
// se si passa id_copia <= 0, cerca un DVD disponibile
int ricercaDVD(int id_film, int id_copia, DVD* vettore, int dim);
