#define MUTEXL 0
#define SYNCH 1
#define MUTEXS 2
#define MUTEX 3

typedef long msg;

typedef struct{
	int numLettori;
	int numScrittori;
	msg messaggio;
} Buffer;

void Wait_Sem(int, int);
void Signal_Sem(int, int);
void Lettore(int, Buffer*);
void Scrittore(int, Buffer*);

