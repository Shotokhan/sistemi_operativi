#define COND_S 0
#define COND_L 1

typedef struct{
	int temperatura;
	unsigned int umidita;
	unsigned short pioggia;
	unsigned int numScrittori;
	unsigned int numLettori;
} meteo;

meteo* init_meteo(int* id_meteo_ptr);

void remove_meteo(int id_meteo);

void scrittore(monitor* M, meteo* ds_shm);

void lettore(monitor* M, meteo* ds_shm);

void inizioScrittura(monitor* M, meteo* ds_shm);

void fineScrittura(monitor* M, meteo* ds_shm);

void inizioLettura(monitor* M, meteo* ds_shm);

void fineLettura(monitor* M, meteo* ds_shm);
