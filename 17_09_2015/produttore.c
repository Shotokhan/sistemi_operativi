#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>

#include "procedure.h"

int main(){
	srand(time(NULL)*getpid());
	key_t semkey = ftok("./", 'b');
	key_t shmkey = ftok("./", 'c');
	int shm_id = shmget(shmkey, sizeof(BufferCircolare), 0);
	BufferCircolare* buf = shmat(shm_id, 0, 0);
	int sem_id = semget(semkey, 0, 0);

	produci_elemento(sem_id, buf);

	_exit(0);
}
