#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/ipc.h>
#include <string.h>

#define DIM 10

typedef int (*func_type)(char*, int);

char* shared_memory(int n_argc, char* n_argv[], int* id);

void* create_shm(char* path, char id, unsigned int size, int* id_shm);

int validInput(char c);

int init_shm(char* p);

int read_shm(char* p, int id);
int write_shm(char* p, int id);
int changeMode_shm(char* p, int id);
int attachProcess_shm(char* p, int id);
int delete_shm(int id);

static func_type func_array[4] = {&read_shm, &write_shm, &changeMode_shm, &attachProcess_shm};

int main(int argc, char* argv[]){
	int st, id;
	char *p = shared_memory(argc, argv, &id);
	
	init_shm(p);

	while(p != 0){
		printf("Cosa vuoi fare con la shared memory?\n");
		printf("[R]ead, [W]rite, [C]hange mode, [D]elete and quit, [A]ttach a process\n");
		char c;
		int index, flag;
		do{
			c = getchar();
			index = validInput(c);
			while((getchar()) != '\n');
		} while(index==0);
		switch(index){
			case 5:
				flag = delete_shm(id);
				if(flag==0){
					p = 0;
				}
				break;
			default:
				func_array[index - 1](p, id);
				break;
		}
		
	}

return 0;
}

char* shared_memory(int n_argc, char* n_argv[], int* id){
	if(n_argc < 2){
		printf("Shared memory non passata; la creo.\n");
		char* p = (char*) create_shm(".", 'B', sizeof(char)*DIM, id);
		return p;
	} else {
		int num_id;
		sscanf(n_argv[1], "%d", &num_id);
		char *p = (char*)shmat(num_id, NULL, 0);
		if(p==(void*)-1){
			printf("La shared memory passata non è valida; ne creo una valida.\n");
			p = (char*) create_shm(".", 'B', sizeof(char), id);
			return p;
		} else{
			*id = num_id;
		}
		return p;
	}
}

void* create_shm(char* path, char id, unsigned int size, int* id_shm){
	key_t chiave = ftok(path, id);
        printf("Chiave: %d\n", chiave);
        *id_shm = shmget(chiave, size, IPC_CREAT|IPC_EXCL|0664);
        printf("Id shm: %d\n", id_shm);
        if(*id_shm < 0){
        	perror("Errore shmget\n"); exit(1);
        }
        char *p = (char*) shmat(*id_shm, NULL, 0);
        if(p==(void*)-1){
        	perror("Errore shmat\n"); exit(1);
        }
        return p;
}

int validInput(char c){
	unsigned int b = 0;
	switch(c){
		case 'R':
			b = 1;
			break;
		case 'W':
                        b = 2;
                        break;
		case 'C':
                        b = 3;
                        break;
		case 'A':
                        b = 4;
                        break;
		case 'D':
                        b = 5;
                        break;
	}
	return b;
}

int init_shm(char* p){
	unsigned int i = 0;
	while(i < DIM){
		p[i] = '\0';
		i++;
	}
	return 0;
}

int read_shm(char* p, int id){
	unsigned int i = 0;
	while(i < DIM && p[i] != '\0'){
		printf("%c", p[i]);
		i++;
	}
	printf("\n");
	return 0;
}

int write_shm(char* p, int id){
	printf("Inserire una stringa da scrivere nella shared memory:\n");
	char* flag = fgets(p, DIM, stdin);
	if(flag != p){
		printf("Errore di scrittura\n");
		return 1;
	}
	printf("Scrittura effettuata\n");
	return 0;
}

int changeMode_shm(char* p, int id){
	struct shmid_ds shmid_ds, *buf;
	buf = &shmid_ds;
	int rtrn, mode;
	rtrn = shmctl(id, IPC_STAT, buf);
	if(rtrn == -1){
		printf("Errore nel caricamento del descrittore della shared memory\n");
		return 1;
	}
	printf("Permessi attuali: 0%o \n", buf->shm_perm.mode);
	printf("Inserire i nuovi permessi in formato ottale:\n");
	scanf("%o", &mode);
	buf->shm_perm.mode = mode;
	printf("Nuovi permessi: 0%o \n", buf->shm_perm.mode);
	int flag = shmctl(id, IPC_SET, buf);
	if(flag == -1){
		printf("Errore durante il cambio dei permessi\n");
		return 1;
	}
	return 0;
}

int attachProcess_shm(char *p, int id){
	int pid = fork();
	if(pid < 0){
		printf("Errore creazione processo figlio\n");
		return 1;
	} else if(pid == 0){
		printf("Sono il processo figlio\n");
		printf("Scrivo qualcosa nella shared memory\n");
		// printf("%d\n", p);
		strcpy(p, "foo");
		// printf("%d\n", p);
		printf("Termino\n");
		_exit(0);
	} else{
		printf("Sono il processo padre\n");
		wait(NULL);
		printf("Contenuto shm dopo la terminazione del figlio: ");
		read_shm(p, id);
	}
	return 0;
}

int delete_shm(int id){
	int flag = shmctl(id, IPC_RMID, 0);
	if(flag == -1){
		printf("Errore durante la cancellazione della shared memory\n");
		return 1;
	}
	return 0;
}

