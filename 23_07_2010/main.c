#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#include "header.h"

int main(){
	int pid, pid_printer;
	char key1 = 'c';
	char key2 = 'h';
	int qid[2];
	init_queues(key1, key2, qid);

	pid = fork();
	if(pid == 0){
		printer(qid[1]);
		_exit(0);
	} else{
		check_error(pid, "Errore creazione processo\n");
	}
	pid_printer = pid;

	pid = fork();
	if(pid == 0){
		server(qid[0], qid[1], pid_printer);
		_exit(0);
	} else{
		check_error(pid, "Errore creazione processo\n");
	}

	for(int i=0; i<N_CLIENT; i++){
		pid = fork();
		if(pid == 0){
			client(qid[0]);
			_exit(0);
		} else{
			check_error(pid, "Errore creazione processo\n");
		}
	}

	wait_processes();

	remove_queues(qid[0], qid[1]);

	return 0;
}
