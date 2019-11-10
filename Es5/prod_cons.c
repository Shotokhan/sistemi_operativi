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
//semnum

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

void Produttore(int* shm_ptr, int ds_sem){
	Wait_Sem(ds_sem, SPAZIO_DSP);
	*shm_ptr = rand() % 100;
	Signal_Sem(ds_sem, MSG_DSP);
}

void Consumatore(int* shm_ptr, int ds_sem){
	Wait_Sem(ds_sem, MSG_DSP);
	printf("Valore shared memory: %d\n", *shm_ptr);
	Signal_Sem(ds_sem, SPAZIO_DSP);
}

int main(){
	int key_t = ftok(".", 'B');
	int id_shm = shmget(key_t, sizeof(int), IPC_CREAT|IPC_EXCL|0664);
	int* p = (int*)shmat(id_shm, NULL, 0);

	int key_sem = IPC_PRIVATE;
	int id_sem = semget(key_sem, 2, IPC_CREAT|IPC_EXCL|0664);
	
	*p = 0;
	
	semctl(id_sem, SPAZIO_DSP, SETVAL, 1);
	semctl(id_sem, MSG_DSP, SETVAL, 0);

//	int fatherPid = getpid();
	int pid;

	pid = fork();
	if(pid == 0){
		srand(time(NULL));
		printf("Sono il produttore\n");
		Produttore(p, id_sem);
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
return 0;
}
