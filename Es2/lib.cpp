#include <iostream>
#include "lib.h"

void leggi_vettore(int vect[], int n){
	for(int i=0; i<n; i++){
		std::cin >> vect[i];
	}
}

void stampa_vettore(const int vect[], int n){
	for(int i=0; i<n; i++){
		std::cout << vect[i] << std::endl;
	}
}

void somma_vettori(const int vect1[], const int vect2[], int vect3[], int n){
	for(int i=0; i<n; i++){
		vect3[i] = vect1[i] + vect2[i];
	}
}
