#define SYNCH 0
#define MUTEX_P 1
#define MUTEX_L 2

typedef struct{
	char prox_appello[11];
	unsigned int num_prenotati;
	unsigned int num_lettori;
} buffer;

void doc_aggiorna(int ds_sem, buffer* buf, char* foo);

void doc_leggi(int ds_sem, buffer* buf, int setZero);

void stud_prenota(int ds_sem, buffer* buf);

void stud_leggi(int ds_sem, buffer* buf);

void inizioLettura(int ds_sem, buffer* buf);

void fineLettura(int ds_sem, buffer* buf);

void Wait_Sem(int ds_sem, int semnum);

void Signal_Sem(int ds_sem, int semnum);
