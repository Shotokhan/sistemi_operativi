#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"

int open_queue(key_t keyval){
	int qid;
	if((qid = msgget(keyval, IPC_CREAT|0666))==-1){
		return -1;
	}
	return qid;
}
