#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>

#include "procedure.h"

void* Richiedente(void* data){
	MonitorElaborazioni* m = (MonitorElaborazioni*) data;
	for(int i = 0; i < 3; i++){
		produzione(m);
	}
	pthread_exit(NULL);
}

void* Elaboratore(void* data){
	MonitorElaborazioni* m = (MonitorElaborazioni*) data;
	for(int i = 0; i < 6; i++){
		consumo(m);
	}
	pthread_exit(NULL);
}

void produzione(MonitorElaborazioni* m){
	pthread_mutex_lock(&(m->mutex));
	while(m->num_liberi == 0){
		pthread_cond_wait(&(m->prod), &(m->mutex));
	}
	int i = 0;
	while(m->stato[i] != LIBERO && i < MAX_ELAB){
		i++;
	}
	m->stato[i] = IN_USO;
	m->num_liberi -= 1;
	pthread_mutex_unlock(&(m->mutex));

	int n_op = rand() % (MAX_OPERANDI - 1) + 2;	
	m->elaborazioni[i].tot_operandi = n_op;
	for(int j=0; j < n_op; j++){
		m->elaborazioni[i].operandi[j] = rand() % 11;
	}
	printf("Scritti %d operandi\n", n_op);

	pthread_mutex_lock(&(m->mutex));
	m->stato[i] = OCCUPATO;
	m->num_occupati += 1;
	pthread_cond_signal(&(m->cons));
	pthread_mutex_unlock(&(m->mutex));
}

void consumo(MonitorElaborazioni* m){
	int minOp = MAX_OPERANDI + 1;
	int indMinOp = MAX_ELAB + 1;

	pthread_mutex_lock(&(m->mutex));
	while(m->num_occupati == 0){
		pthread_cond_wait(&(m->cons), &(m->mutex));
	}
	int i = 0;
	while(i < MAX_ELAB){
		if(m->stato[i] == OCCUPATO){
			if(m->elaborazioni[i].tot_operandi < minOp){
				minOp = m->elaborazioni[i].tot_operandi;
				indMinOp = i;
				if(minOp == 2)
					break;
			}
		}
		i++;
	}
	if(indMinOp == MAX_ELAB + 1){
		printf("Errore di sincronizzazione\n"); return;
	}
	m->stato[indMinOp] = IN_USO;
	m->num_occupati -= 1;
	pthread_mutex_unlock(&(m->mutex));
	
	sleep(minOp);
	int somma = 0;
	int j;
	char printBuf[22 + minOp * 3 + 12];
	j = sprintf(printBuf, "%s", "Elaborati ");
	j+= sprintf(printBuf + j, "%d", minOp);
	j+= sprintf(printBuf + j, "%s", " operandi:");
	for(int i = 0; i < minOp; i++){
		somma += m->elaborazioni[indMinOp].operandi[i];
		j += sprintf(printBuf + j, "%s", " ");
		j += sprintf(printBuf + j, "%d", m->elaborazioni[indMinOp].operandi[i]);
	}
	j += sprintf(printBuf + j, "%s", ", somma: ");
	j += sprintf(printBuf + j, "%d\n", somma);
	printf("%s", printBuf);

	pthread_mutex_lock(&(m->mutex));
	m->stato[indMinOp] = LIBERO;
	m->num_liberi += 1;
	pthread_cond_signal(&(m->prod));
	pthread_mutex_unlock(&(m->mutex));
}

void init_monitorElab(MonitorElaborazioni* m){
	int r = pthread_mutex_init(&(m->mutex), NULL);
	if(r!=0){perror("Errore creazione mutex\n"); pthread_exit((void*) -1);}
	r = pthread_cond_init(&(m->prod), NULL);
	if(r!=0){perror("Errore creazione prod\n"); pthread_exit((void*) -1);}
	r = pthread_cond_init(&(m->cons), NULL);
	if(r!=0){perror("Errore creazione cons\n"); pthread_exit((void*) -1);}
	m->num_occupati = 0;
	m->num_liberi = MAX_ELAB;
	for(int i = 0; i < MAX_ELAB; i++){
		m->stato[i] = LIBERO;
		m->elaborazioni[i].tot_operandi = 0;
		for(int j = 0; j < MAX_OPERANDI; j++){
			m->elaborazioni[i].operandi[j] = 0;
		}
	}
}

void destroy_monitorElab(MonitorElaborazioni* m){
	pthread_mutex_destroy(&(m->mutex));
	pthread_cond_destroy(&(m->prod));
	pthread_cond_destroy(&(m->cons));
	free(m);
}
