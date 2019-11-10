#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main(){
	int pid, st;

	pid = fork();
	if (pid < 0){
		perror("Fork fallita\n");
	} else if (pid == 0){
		execl("/bin/ls","ls","-l",NULL);
		perror("Exec fallita\n");
		_exit(1);
	} else{
	//	kill(pid, 9);
		wait(&st);
		printf("Il figlio %d  ha terminato l'esecuzione ", pid);
		if (WIFEXITED(st)){
			printf("con stato: %d\n", WEXITSTATUS(st));
		} else {
			if (WIFSIGNALED(st)){
				printf(" con terminazione involontaria per segnale %d\n", WTERMSIG(st));
			}
		}
	}
	return 0;
}
