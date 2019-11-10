#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>

#define SPAZIO_DSP 0
#define MSG_DSP 1
#define MUTEX_P 2
#define MUTEX_C 3
#define MUTEX_COL 4
#define MUTEX_SUMS 5
#define PARALLEL_SUM 6

#define DIM_BUFFER 3
#define NUM_PRODUTTORI 1
#define NUM_CONSUMATORI 1

#define DIM_ROW 40
#define DIM_COL 18

#define BUFFER_VUOTO 0
#define BUFFER_PIENO 1
#define BUFFER_INUSO 2
//semnum

// TODO: implementare un altro tipo di produttore ed un altro
// tipo di consumatore, in modo che la matrice venga passata e letta
// nel buffer; quindi il produttore che chiama computeSum è il consumatore
// del produttore che passa la matrice nel buffer

struct data{
	int mat[DIM_COL][DIM_ROW];
	int sums[DIM_ROW];
	int colIndex;
	int sumsIndex;
	int finalSum;
};

struct prodcons{
        int buffer[DIM_BUFFER];
        int stato[DIM_BUFFER];
};

struct childrenType{
        char* name;
        int* pidArray;
        int numChildren;
};

void Wait_Sem(int semid, int semnum){
	struct sembuf s;

	s.sem_flg = 0;
	s.sem_num = semnum;
	s.sem_op = -1;

	int r = semop(semid, &s, 1);
	if(r<0) {perror("Errore wait\n");}
}

void Sem_op_N(int semid, int semnum, int N){
	struct sembuf s;

	s.sem_flg = 0;
	s.sem_num = semnum;
	s.sem_op = N;

	int r = semop(semid, &s, 1);
	if(r<0) {perror("Errore wait\n");}
}

void Signal_Sem(int semid, int semnum){
        struct sembuf s;

        s.sem_flg = 0;
        s.sem_num = semnum;
        s.sem_op = 1;

        int r = semop(semid, &s, 1);
        if(r<0) {perror("Errore signal\n");}
}

void Wait_for_zero_Sem(int semid, int semnum){
	int r;
	
	printf("Valore semaforo %d: %d\n", semnum, semctl(semid, semnum, GETVAL));

	struct sembuf s;
		
	s.sem_flg = 0;
	s.sem_num = semnum;
	s.sem_op = 0;

	r = semop(semid, &s, 1);
	if(r<0){perror("Errore wait_for_zero\n");}
}

int computeSum(struct data* data_ptr, int ds_sem);


void Produttore(struct prodcons * shm_ptr, int ds_sem, struct data* data_ptr){
	int index = 0;
	Wait_Sem(ds_sem, SPAZIO_DSP);
	Wait_Sem(ds_sem, MUTEX_P);
	while(index <= DIM_BUFFER && shm_ptr->stato[index] != BUFFER_VUOTO){
		index++;
	}
	shm_ptr->stato[index] = BUFFER_INUSO;
	Signal_Sem(ds_sem, MUTEX_P);

	shm_ptr->buffer[index] = computeSum(data_ptr, ds_sem);
	shm_ptr->stato[index] = BUFFER_PIENO;
	Signal_Sem(ds_sem, MSG_DSP);
}

void Consumatore(struct prodcons * shm_ptr, int ds_sem){
	int index = 0;
	Wait_Sem(ds_sem, MSG_DSP);
	Wait_Sem(ds_sem, MUTEX_C);
	while(index <= DIM_BUFFER && shm_ptr->stato[index] != BUFFER_PIENO){
		index++;
	}
	shm_ptr->stato[index] = BUFFER_INUSO;
	Signal_Sem(ds_sem, MUTEX_C);

	printf("Valore letto nel buffer: %d\n", shm_ptr->buffer[index]);
	shm_ptr->stato[index] = BUFFER_VUOTO;
	Signal_Sem(ds_sem, SPAZIO_DSP);	
}

int whichChild(struct childrenType *childType, int numTypes, int pid);

void waitProcesses(struct childrenType *childType, int numTypes, int numProcesses);

int main(){
	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(int), IPC_CREAT|IPC_EXCL|0664);
	struct prodcons* p = (struct prodcons*)shmat(id_shm, NULL, 0);
	
	int key_mat = ftok(".", 'Z');
	int id_data = shmget(key_mat, sizeof(int), IPC_CREAT|IPC_EXCL|0664);
	struct data* data_ptr = (struct data*)shmat(id_data, NULL, 0);

	int key_sem = IPC_PRIVATE;
	int id_sem = semget(key_sem, 7, IPC_CREAT|IPC_EXCL|0664);
	
	for(int i=0; i<DIM_BUFFER; i++){
		p->buffer[i]=0;
		p->stato[i]=BUFFER_VUOTO;
	}
	
	srand(time(NULL));
	for(int i=0; i<DIM_COL; i++){
		for(int j=0; j<DIM_ROW; j++){
			data_ptr->mat[i][j] = rand() % 10;
			printf("%d ", data_ptr->mat[i][j]);
		}
		printf("\n");
	}

	semctl(id_sem, SPAZIO_DSP, SETVAL, DIM_BUFFER);
	semctl(id_sem, MSG_DSP, SETVAL, 0);
	semctl(id_sem, MUTEX_P, SETVAL, 1);
	semctl(id_sem, MUTEX_C, SETVAL, 1);	
	semctl(id_sem, MUTEX_COL, SETVAL, 1);
	semctl(id_sem, MUTEX_SUMS, SETVAL, 1);
	// semctl(id_sem, PARALLEL_SUM, SETVAL, DIM_ROW);
	semctl(id_sem, PARALLEL_SUM, SETVAL, 0);

	int pid = fork();
	if(pid == 0){
		printf("Sono il produttore\n");
		// qui ci sarebbe anche il passaggio di una nuova matrice
		Produttore(p, id_sem, data_ptr);
		_exit(0);
	}
	pid = fork();
	if(pid == 0){
		printf("Sono il consumatore\n");
		Consumatore(p, id_sem);
		_exit(0);
	}

	semctl(key_sem, IPC_RMID, 0);
	shmctl(id_shm, IPC_RMID, 0);
	shmctl(id_data, IPC_RMID, 0);
return 0;
}

int whichChild(struct childrenType *childType, int numTypes, int pid){
	for(int i=0; i<numTypes; i++){
		struct childrenType* pnt = &(childType[i]);
		for(int j=0; j < (pnt->numChildren); j++){
			if( (pnt->pidArray[j]) == pid ){
				return i;
			}
		}
	}
	return -1;
}

void waitProcesses(struct childrenType *childType, int numTypes, int numProcesses){
	int i = 0;
	int pid, st;
	while(i < numProcesses){
		pid = wait(&st);
		int type = whichChild(childType, numTypes, pid);
		if(type != -1){
			printf("Il figlio %s con pid %d ha terminato l'esecuzione: ", childType[type].name, pid);
		} else {
			printf("Il figlo %d di tipo sconosciuto ha terminato l'esecuzione: ", pid);
		}
		if(WIFEXITED(st)){
                	printf("regolarmente, con stato %d\n", st);
                } else if(WIFSIGNALED(st)){
                        printf("a causa del segnale %d\n", WTERMSIG(st));
                }
		i++;
	}
}

int computeSum(struct data* data_ptr, int ds_sem){
	// Preconditions:
	// Semval of MUTEX_COL and MUTEX_SUMS is equal to 1
	// Semval of PARALLEL_SUM is equal to DIM_ROW (Number of columns) (or 0 for the 2nd version)
	// Mat of data struct is populated
	data_ptr->colIndex = 0;
	data_ptr->sumsIndex = 0;
	data_ptr->finalSum = 0;	

	int* pidToWait = (int*) malloc(sizeof(int));
	struct childrenType* childrenGroup = (struct childrenType*) malloc(2 * sizeof(struct childrenType));
	char nome1[13] = "somma_finale";
	char nome2[14] = "somma_colonna";
	int pids[DIM_ROW];
	childrenGroup[0].name = nome1;
	childrenGroup[0].pidArray = pidToWait;
	childrenGroup[0].numChildren = 1;
	childrenGroup[1].name = nome2;
	childrenGroup[1].pidArray = pids;
	childrenGroup[1].numChildren = DIM_ROW;
	
	int pid = fork();
	if(pid == 0){
		// Wait_for_zero_Sem(ds_sem, PARALLEL_SUM);
		int x = (-1) * DIM_ROW;
		Sem_op_N(ds_sem, PARALLEL_SUM, x);
		printf("Sono il processo che fa la somma finale\n");
		for(int i=0; i<DIM_ROW; i++){
			data_ptr->finalSum += data_ptr->sums[i];
		}
		_exit(0);
	}
	*pidToWait = pid;
	for(int i=0; i<DIM_ROW; i++){
		pid = fork();
		if(pid == 0){
			int j, sum=0;
			printf("Sono un processo che somma sulle colonne\n");
			// Wait_Sem(ds_sem, PARALLEL_SUM);
			Wait_Sem(ds_sem, MUTEX_COL);
			j = data_ptr->colIndex;
			data_ptr->colIndex += 1;
			Signal_Sem(ds_sem, MUTEX_COL);

			for(int i=0; i<DIM_COL; i++){
				sum += data_ptr->mat[i][j];
			}

			Wait_Sem(ds_sem, MUTEX_SUMS);
			data_ptr->sums[data_ptr->sumsIndex] = sum;
			data_ptr->sumsIndex += 1;
			Signal_Sem(ds_sem, MUTEX_SUMS);
			
			// Wait_for_zero_Sem(ds_sem, PARALLEL_SUM);
			Signal_Sem(ds_sem, PARALLEL_SUM);
			_exit(0);
		}
		pids[i] = pid;
	}
	//waitpid(*pidToWait, NULL, 0);	
	waitProcesses(childrenGroup, 2, DIM_ROW + 1);
	return data_ptr->finalSum;
}
