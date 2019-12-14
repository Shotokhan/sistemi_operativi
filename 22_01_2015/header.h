#define KEY_PATH "./"
#define KEY_CHAR_QUEUE 'a'
#define KEY_CHAR_MONITOR 'b'
#define KEY_CHAR_RISORSA 'c'

// questo monitor non ha variabili condition
// perché è acceduto solo dai server
// quindi è sufficiente la mutua esclusione
typedef struct{
	int mutex;
} MonitorRisorsa;

typedef struct{
	int a;
	int b;
	MonitorRisorsa m;
} Risorsa;

typedef struct{
	long tipo;
	int a;
	int b;
} Messaggio;

void enterMonitor(MonitorRisorsa* m);

void leaveMonitor(MonitorRisorsa* m);

void waitSem(int semid, int semnum);

void signalSem(int semid, int semnum);

void initMonitor(MonitorRisorsa* m);

void removeMonitor(MonitorRisorsa* m);

void checkError(int retVal, char* msg);
