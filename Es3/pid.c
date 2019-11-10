#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int pid, st;
	
	pid = fork();
	if (pid == -1){
		fprintf(stderr, "generazione processo fallita");
		return -1;
	}
	if (pid == 0){
		printf("Sono il processo figlio\n");
		printf("Mio PID : %d\n\n", getpid());
		printf("PID di mio padre: %d\n\n", getppid());
		_exit(4);
	}
	else if (pid > 0){
		printf("Sono il processo padre\n");
		printf("Mio PID : %d\n\n", getpid());
		printf("PID di mio padre: %d\n\n", getppid());
		sleep(2);
		wait(&st);
		printf("Il figlio %d ha terminato l'esecuzione ", pid);
		if((char)st==0)
			printf("con stato: %d\n\n", st>>8);
		else
			printf("involontariamente!\n\n");
			
	}
	return 0;
}	
