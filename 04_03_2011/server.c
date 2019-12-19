#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "header.h"

int main(){	
	workerData* W = (workerData*) malloc(sizeof(workerData));
	initData(W);

	Manager(W);	
	
	removeData(W);
	free(W);

	_exit(0);
}
