typedef int Elem;

typedef struct{
	Elem* dati;
	int dim;
	pthread_mutex_t mutex;
	pthread_cond_t push;
	pthread_cond_t pop;
	int headPtr;
	int init; // uno stack non può essere inizializzato 2 volte
} Stack;

void StackInit(Stack* s, int dim);

void StackRemove(Stack* s);

void StackPush(Stack* s, Elem e);

Elem StackPop(Stack* s);

int StackSize(Stack* s);

void checkError(int ret, char* msg);
