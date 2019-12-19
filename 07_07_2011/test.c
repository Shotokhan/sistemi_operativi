#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include "test.h"

void* FooPush(void* stack){
	Stack* s = (Stack*) stack;
	Elem val;
	for(int i=0; i<4; i++){
		val = rand() % 11;
		StackPush(s, val);
		printf("\tPushed %d\n", val);
		sleep(1);
	}
	pthread_exit(0);
}

void* BarPop(void* stack){
	Stack* s = (Stack*) stack;
	Elem op1, op2, sum;
	for(int i=0; i<10; i++){
		op1 = StackPop(s);
		op2 = StackPop(s);
		sum = op1 + op2;
		printf("Due pop: %d + %d = %d\n", op1, op2, sum);
		sleep(1);
	}
	pthread_exit(0);
}
