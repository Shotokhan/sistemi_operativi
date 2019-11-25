#define DIM 3

#define LIBERO 0
#define IN_USO 1
#define OCCUPATO 2

typedef struct{
	int id;
	int quota;
} Volo;

typedef struct{
	Volo vettore_voli[DIM];
	int stato[DIM];
	int num_liberi;
	pthread_mutex_t mutex;
	pthread_cond_t prod;
} GestioneVoli;

GestioneVoli* g;

void* ThreadLifeline(void* heap);

void InserisciVolo (GestioneVoli* g, int id);

int AggiornaVolo (GestioneVoli* g, int id, int quota);

int RimuoviVolo (GestioneVoli* g, int id);

void init_GestioneVoli(GestioneVoli* g);

void remove_GestioneVoli(GestioneVoli* g);

