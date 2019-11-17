#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"

int main(){
	int qid = open_queue(ftok(FTOK_PATH_Q, FTOK_CHAR_Q));
	if(qid==-1) _exit(1);
	char* args1[] = {"./client1", NULL};
	char* args2[] = {"./client2", NULL};
	char* args3[] = {"./server", NULL};
	int pid, st;
	pid = fork();
	if(pid == 0){
		execvp(args1[0], args1);
	}
	pid = fork();
	if(pid == 0){
		execvp(args2[0], args2);
	}
	pid = fork();
	if(pid == 0){
		execvp(args3[0], args3);
	}

	while((pid=wait(&st))>0){
		printf("Il processo %d ha terminato ", pid);
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, con stato %d\n", WTERMSIG(st));
		}
	}

	msgctl(qid, IPC_RMID, 0);
return 0;
}
