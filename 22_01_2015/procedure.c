#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#include "header.h"

void enterMonitor(MonitorRisorsa* m){
	waitSem(m->mutex, 0);
}

void leaveMonitor(MonitorRisorsa* m){
	signalSem(m->mutex, 0);
}

void waitSem(int semid, int semnum){
	struct sembuf s;
	s.sem_op = -1;
	s.sem_flg = 0;
	s.sem_num = semnum;
	int r = semop(semid, &s, 1);
	checkError(r, "Errore wait sem\n");
}

void signalSem(int semid, int semnum){
	struct sembuf s;
	s.sem_op = 1;
	s.sem_flg = 0;
	s.sem_num = semnum;
	int r = semop(semid, &s, 1);
	checkError(r, "Errore signal sem\n");
}

void initMonitor(MonitorRisorsa* m){
	m->mutex = semget(ftok(KEY_PATH, KEY_CHAR_MONITOR), 1, IPC_CREAT|IPC_EXCL|0664);
	checkError(m->mutex, "Errore creazione mutex\n");
	int r = semctl(m->mutex, 0, SETVAL, 1);
	checkError(1, "Errore semctl\n");
}

void removeMonitor(MonitorRisorsa* m){
	int r = semctl(m->mutex, 0, IPC_RMID);
	checkError(r, "Errore rimozione mutex\n");
}

void checkError(int retVal, char* msg){
	if(retVal < 0){
		perror(msg);
		_exit(1);
	}
}
