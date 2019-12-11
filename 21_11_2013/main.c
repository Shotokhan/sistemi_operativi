#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "header.h"

int main(int argc, char* argv[]){
	if(argc != 3 || strlen(argv[1]) != 1 || strlen(argv[2]) != 1){
		printf("Errore; utilizzo: ./chat <char coda 1> <char coda 2>\n");
		return 0;
	}
	int pidMittente, pidRicevente, pid, st, q1, q2;
	char c1 = argv[1][0];
	char c2 = argv[2][0];

	initQueues(&q1, &q2, c1, c2);

	pid = fork();
	if(pid == 0){
		mittente(q1, q2);
	} else{
		checkError(pid, "Errore creazione processo\n");
	}
	pidMittente = pid;

	pid = fork();
	if(pid == 0){
		ricevente(q2);
	} else{
		checkError(pid, "Errore creazione processo\n");
	}
	pidRicevente = pid;

	while((pid=wait(&st))>0){
		if(pid == pidMittente){
			printf("Il processo mittente %d ha terminato ", pid);
		} else if(pid == pidRicevente){
			printf("Il processo ricevente %d ha terminato ", pid);
		} else{
			// il main non vedra' la terminazione del daemon
			printf("Il daemon %d ha terminato ", pid);
		}
		if(WIFEXITED(st)){
			printf("regolarmente, con stato %d\n", WEXITSTATUS(st));
		} else if(WIFSIGNALED(st)){
			printf("involontariamente, a causa del segnale %d\n", WTERMSIG(st));
		}
	}
}
