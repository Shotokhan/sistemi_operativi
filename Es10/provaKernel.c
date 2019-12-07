#include <linux/kernel.h>
// serve per il messaggio kernel info
#include <linux/module.h>
// serve per il modulo

// le funzioni da implementare devono avere proprio le seguenti firme:
// il SO le vede come punto d'ingresso e punto d'uscita

// questa è una versione base: si possono utilizzare anche delle macro
// libreria <linux/init.h>

// si possono anche utilizzare altre interfacce da implementare

// si possono modificare delle info riguardo al modulo del kernel,
// andando a definire delle costanti

// per creare un driver, il modulo del kernel deve creare una entry nella cartella /dev che rappresenta il dispositivo
// e poi deve implementare la logica del driver
// l'entry in dev si crea con il comando "mknod"

int init_module(void){
	printk(KERN_INFO "Hello world 1 ");
	return 0;
}

void cleanup_module(void){
	printk(KERN_INFO "Hello world 1 ");
}
