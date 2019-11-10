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

#define DIM_BUFFER 3
#define NUM_PRODUTTORI 5
#define NUM_CONSUMATORI 5

#define BUFFER_VUOTO 0
#define BUFFER_PIENO 1
#define BUFFER_INUSO 2
//semnum

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

void Signal_Sem(int semid, int semnum){
        struct sembuf s;

        s.sem_flg = 0;
        s.sem_num = semnum;
        s.sem_op = 1;

        int r = semop(semid, &s, 1);
        if(r<0) {perror("Errore signal\n");}
}

void Produttore(struct prodcons * shm_ptr, int ds_sem){
	int index = 0;
	Wait_Sem(ds_sem, SPAZIO_DSP);
	Wait_Sem(ds_sem, MUTEX_P);
	while(index <= DIM_BUFFER && shm_ptr->stato[index] != BUFFER_VUOTO){
		index++;
	}
	shm_ptr->stato[index] = BUFFER_INUSO;
	Signal_Sem(ds_sem, MUTEX_P);

	shm_ptr->buffer[index] = rand() % 100;
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

int main(){
	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(int), IPC_CREAT|IPC_EXCL|0664);
	struct prodcons* p = (struct prodcons*)shmat(id_shm, NULL, 0);

	int key_sem = IPC_PRIVATE;
	int id_sem = semget(key_sem, 4, IPC_CREAT|IPC_EXCL|0664);
	
	for(int i=0; i<DIM_BUFFER; i++){
		p->buffer[i]=0;
		p->stato[i]=BUFFER_VUOTO;
	}
	
	semctl(id_sem, SPAZIO_DSP, SETVAL, DIM_BUFFER);
	semctl(id_sem, MSG_DSP, SETVAL, 0);
	semctl(id_sem, MUTEX_P, SETVAL, 1);
	semctl(id_sem, MUTEX_C, SETVAL, 1);	

	struct childrenType* childrenGroup = (struct childrenType*) malloc(2 * sizeof(struct childrenType));

	int prod[NUM_PRODUTTORI];
	char name_prod[11] = "produttore";
	childrenGroup[0].name = name_prod;
	childrenGroup[0].pidArray = prod;
	childrenGroup[0].numChildren = NUM_PRODUTTORI;

	int cons[NUM_CONSUMATORI];
	char name_cons[12] = "consumatore";
	childrenGroup[1].name = name_cons;
	childrenGroup[1].pidArray = cons;
	childrenGroup[1].numChildren = NUM_CONSUMATORI;

	int pid, st;
	int i = 0;
	while(i < NUM_PRODUTTORI){
		pid = fork();
		if(pid == 0){
			srand(getpid()*time(NULL));
			printf("Sono il produttore\n");
			Produttore(p, id_sem);
			_exit(0);
		}
		prod[i] = pid;
		i++;
	}
	i = 0;
	while(i < NUM_CONSUMATORI){
		pid = fork();
		if(pid == 0){
			printf("Sono il consumatore\n");
			Consumatore(p, id_sem);
			_exit(0);
		}
		cons[i] = pid;
		i++;
	}
	i = 0;
	while(i < NUM_PRODUTTORI + NUM_CONSUMATORI){
		pid = wait(&st);
		int type = whichChild(childrenGroup, 2, pid);
		if(type != -1){
			printf("Il figlio %s con pid %d ha terminato l'esecuzione: ", childrenGroup[type].name, pid);
			if(WIFEXITED(st)){
				printf("regolarmente, con stato %d\n", st);
			} else if(WIFSIGNALED(st)){
				printf("a causa del segnale %d\n", WTERMSIG(st));
			}
		}
		i++;
	}
	semctl(key_sem, IPC_RMID, 0);
	shmctl(id_shm, IPC_RMID, 0);
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
