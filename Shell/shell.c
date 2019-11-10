#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

int main(){
	unsigned int size = 256;
        unsigned int s_argc = 21;
	char* buffer = (char*) malloc(sizeof(char)*size*s_argc);
        char** s_argv = (char**) malloc(sizeof(char*)*(s_argc - 1));
        for(unsigned int i=0; i < s_argc; i++){
	        s_argv[i] = (char *) malloc(sizeof(char)*size);
        }
	
	//unsigned int jMax=0;

	while(1){
		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL){		
		} else {
			strcpy(cwd, "/");
		}
		int pid, st;
		printf("%s -$ ", cwd);
		char* flag = fgets(buffer, size*s_argc, stdin);
		if(flag != buffer){
			break;
		}
		//printf("\nBuffer: %s\n", buffer);
		unsigned int i=0;
		unsigned int j=0;
		unsigned int offset=0;
		while(buffer[i] != '\n'){
			//printf("Buffer[%d]: %c\n", i, buffer[i]);
			if(buffer[i] != ' '){
				if(i-offset >= size - 1){
					s_argv[j][i-offset] = '\0';
					break;
				}
				s_argv[j][i-offset] = buffer[i];
				//printf("s_argv[%d][%d]: %c\n", j, i-offset, s_argv[j][i-offset]);
			} else{
				s_argv[j][i-offset] = '\0';
				j++;
				offset=i+1;
				if(j >= s_argc - 1){
					break;
				}
			}
			i++;
		}
		s_argv[j][i-offset] = '\0';
		char* tmp = NULL;
		if(j < s_argc - 1){
			tmp = s_argv[j+1];
			s_argv[j+1] = NULL;
		}
		/*
		if(j > jMax){
			jMax=j;
		}
		for(unsigned int k=0; k<=jMax; k++){
			printf("argv[%d]: %s\n", k, s_argv[k]);
		}
		printf("------------------\n");
		*/
		if(strcmp(s_argv[0], "cd") == 0){
			chdir(s_argv[1]);
		} else{
			pid = fork();
			if (pid < 0){
		                perror("Fork fallita\n");
	       		} else if (pid == 0){
        	        	execvp(s_argv[0], s_argv);
		                perror("Exec fallita\n");
        		        _exit(1);
		        } else{
		                wait(&st);
				if(st != 0){
        			        printf("Il figlio %d  ha terminato l'esecuzione ", pid);
			                if (WIFEXITED(st)){
                			        printf("con stato: %d\n", WEXITSTATUS(st));
        			        } else {
			                        if (WIFSIGNALED(st)){
                			                printf(" con terminazione involontaria per segnale %d\n", WTERMSIG(st));
        			                }
			                }
				}
				if(tmp != NULL){
					s_argv[j+1] = tmp;
				}
	        	}
		}
	}

        for(unsigned int i=0; i < s_argc - 1; i++){
	        free(s_argv[i]);
        }
        free(s_argv);
	free(buffer);
return 0;
}
