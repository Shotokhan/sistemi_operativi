#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/ipc.h>

int main(){
	key_t chiave = ftok(".", 'B');
	int st;
	printf("Chiave : %d\n", chiave);

	// int id_shm = shmget(chiave, sizeof(int), 0);
	// int id_shm = shmget(chiave, sizeof(int), IPC_CREAT|0664);
	// int id_shm = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0664);
	int id_shm = shmget(chiave, sizeof(int), IPC_CREAT|IPC_EXCL|0664);
	printf("id_shm: %d\n", id_shm);
	if(id_shm < 0){
		perror("Errore shmget"); exit(1);
	}

	int *p = (int*)shmat(id_shm, NULL, 0);
	// int *p = (int*)shmat(id_shm, NULL, SHM_READONLY);
	if(p==(void*)-1){
		perror("errore shmat");
	}
	int pid = fork();
	if(pid < 0){
		perror("Fork fallita"); _exit(1);
	} else if(pid == 0){
		printf("Sono il processo figlio\n");
		*p = 123;
		printf("Termino\n");
		// kill(getppid(), 9);
		sleep(3);
		_exit(0);
	} else{
		printf("Sono il processo padre\n");
		wait(NULL);
		printf("Contenuto area di memoria: %d\n", *p);
	}
	shmctl(id_shm, IPC_RMID, 0);
return 0;
}
