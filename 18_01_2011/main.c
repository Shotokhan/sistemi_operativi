#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "header.h"

int main(){
	int qidClient, qidBalance, qidServer;
	initQueues(&qidClient, &qidBalance, &qidServer);
	
	int pid, st;
	for(int i=0; i<N_CLIENT; i++){
		pid = fork();
		if(pid == 0){
			printf("Sono il client %d\n", getpid());
			for(int i=0; i<15; i++){
				client(qidClient);
				sleep(1);
			}
			_exit(0);
		} else checkError(pid, "Errore creazione processo\n");
	}

	for(int i=0; i<N_BALANCE; i++){
		pid = fork();
		if(pid == 0){
			printf("Sono il balancer %d\n", getpid());
			int numIterations = N_CLIENT * 15 / N_BALANCE;
			for(int i=0; i < numIterations; i++){
				balancer(qidClient, qidBalance, qidServer);
			}
			_exit(0);
		} else checkError(pid, "Errore creazione processo\n");
	}

	for(int i=0; i<N_SERVER; i++){
		pid = fork();
		if(pid == 0){
			int serverNum = i + 1;
			printf("Sono il server %d, di tipo %d\n", getpid(), serverNum);
			int numIterations = N_CLIENT * 15 / N_SERVER;
			for(int i=0; i < numIterations; i++){
				server(qidServer, serverNum);
			}
			_exit(0);
		} else checkError(pid, "Errore creazione processo\n");
	}

	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if (WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}

	removeQueues(qidClient, qidBalance, qidServer);
	_exit(0);
}
