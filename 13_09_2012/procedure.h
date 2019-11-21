#define DIM 3

#define NUM_VAR_COND 3

#define CONS 0
#define H_PROD 1
#define L_PROD 2

typedef struct{
	int buffer[DIM];
	int testa; int coda;
	int numOccupati;
	monitor m;
} PriorityProdCons;

void inizializza_prod_cons(PriorityProdCons *p);

void produci_alta_prio(PriorityProdCons *p);

void produci_bassa_prio(PriorityProdCons *p);

void consuma(PriorityProdCons *p);

void rimuovi_prod_cons(PriorityProdCons *p);
