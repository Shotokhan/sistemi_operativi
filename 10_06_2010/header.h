typedef struct{
	int mutex;
	int num_var_cond;
	int id_cond;
	int* cond;
	int id_shared;
} monitor;

void init_monitor(monitor* M, int n_cond);

void enter_monitor(monitor* M);

void leave_monitor(monitor* M);

void remove_monitor(monitor* M);

void wait_condition(monitor* M, int _cond);

void signal_condition(monitor* M, int _cond);

void Wait_Sem(int id_sem, int semnum);

void Signal_Sem(int id_sem, int semnum);
