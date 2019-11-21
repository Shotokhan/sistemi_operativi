typedef struct{
	int mutex;
	int n_cond;
	int condsem;
	int* cond;
	int id_shm;
} monitor;

void enter_monitor(monitor* m);

void leave_monitor(monitor* m);

void signal_cond(monitor* m, int id_cond);

void wait_cond(monitor* m, int id_cond);

void init_monitor(monitor* m, int n_cond);

void remove_monitor(monitor* m);

void Wait_Sem(int id_sem, int semnum);

void Signal_Sem(int id_sem, int semnum);
