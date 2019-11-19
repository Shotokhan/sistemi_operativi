typedef struct{
	int mutex;
	int num_var_cond;
	int id_cond;
	int* cond;
	int id_shm;
} monitor;

void init_monitor(monitor* M, int ncond);

void Wait_Sem(int sem, int semnum);

void Signal_Sem(int sem, int semnum);

void enter_monitor(monitor* M);

void leave_monitor(monitor* M);

void signal_cond(monitor* M, int condInd);

void wait_cond(monitor* M, int condInd);

void signal_all(monitor* M, int condInd);

void delete_monitor(monitor* M);


