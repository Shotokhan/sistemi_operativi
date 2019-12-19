#define KEY_PATH "./"
#define KEY_CHAR_MUTEX 'a'
#define KEY_CHAR_COND 'b'
#define KEY_CHAR_URGENT 'c'
#define KEY_CHAR_SHM 'd'

typedef struct{
	int mutex;
	int n_cond;
	int* cond_count;
	int condsem;
	int urgent_count;
	int urgent_sem;
	int shmid;
} monitorHoare;

void initMonitor(monitorHoare* m, int num_var_cond);

void removeMonitor(monitorHoare* m);

void checkError(int ret, char* msg);

void enterMonitor(monitorHoare* m);

void leaveMonitor(monitorHoare* m);

void waitCondition(monitorHoare* m, unsigned int cond);

void signalCondition(monitorHoare* m, unsigned int cond);

void Wait_Sem(int semid, int semnum);

void Signal_Sem(int semid, int semnum);
