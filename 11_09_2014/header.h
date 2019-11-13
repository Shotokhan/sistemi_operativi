#define MSG_DSP 0
#define SPAZIO_DSP 1
#define MUTEX_C 2
#define MUTEX_P 3

#define DIM 4

#define N_PROD 3
#define N_CONS 3

#define VUOTO 0
#define IN_USO 1
#define PIENO 2

typedef struct{
	int chiave[DIM];
	int valore[DIM];
	int stato[DIM];
} buffer;

void Produttore(int ds_sem, buffer* buf_ptr, int key);

void Consumatore(int ds_sem, buffer* buf_ptr, int key);

void Wait_Sem(int ds_sem, int semnum);

void Signal_Sem(int ds_sem, int semnum);
