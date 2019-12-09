#define N_TRENI 4

#define N_VIAGGIATORI 10

typedef struct{
	int stazione;
	pthread_mutex_t mutex;
	pthread_cond_t capotreno;
	pthread_cond_t viaggiatore;
	int num_viaggiatori;
	int num_capitreno_wait;
	int num_capitreno;
} monitor_treno;

typedef struct{
	monitor_treno* m;
	int capotreno_id;
	int treno;
} capotreno_data;

typedef struct{
	monitor_treno* m[N_TRENI];
	int viaggiatore_id;
} viaggiatore_data;

void inizializza(monitor_treno* m);

void rimuovi(monitor_treno* m);

int leggi_stazione(monitor_treno* m);

void scrivi_stazione(monitor_treno* m, int stazione);

void inizio_lettura(monitor_treno* m);

void fine_lettura(monitor_treno* m);

void inizio_scrittura(monitor_treno* m);

void fine_scrittura(monitor_treno* m);

void* capotreno(void* data);

void* viaggiatore(void* data);

void check_error(int retval, char* msg);
