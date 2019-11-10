#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "waitProc.h"

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
			printf("Il figlio %d di tipo sconosciuto ha terminato l'esecuzione: ", pid);
		}
		if(WIFEXITED(st)){
                	printf("regolarmente, con stato %d\n", st);
                } else if(WIFSIGNALED(st)){
                        printf("a causa del segnale %d\n", WTERMSIG(st));
                }
		i++;
	}
}
