#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "header.h"

#define N_CLIENTS 3

int main(){
	key_t keyval = ftok(KEY_PATH, KEY_CHAR);
	int msqid;
	if((msqid = openQueue(keyval)) == -1){
		_exit(1);
	}
	char* client[] = {"./Client", NULL};
	char* server[] = {"./Server", NULL};
	int pid, st;
	for(int i=0; i<N_CLIENTS; i++){
		pid = fork();
		if(pid == 0){
			execvp(client[0], client);
			perror("Exec fallita\n");
			_exit(1);
		}
	}
	pid = fork();
	if(pid == 0){
		execvp(server[0], server);
		perror("Exec fallita\n");
		_exit(1);
	}
	int thread = pid;
	while((pid=wait(&st))>0){
		if(pid == thread){
			printf("Il server %d ha terminato con stato %d\n", thread, WEXITSTATUS(st));
		} else{
			printf("Il client %d ha terminato: ", pid);
			if(WIFEXITED(st)){
				printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
			} else if (WIFSIGNALED(st)){
				printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
			}
		}
	}
	removeQueue(msqid);
	return 0;
}
