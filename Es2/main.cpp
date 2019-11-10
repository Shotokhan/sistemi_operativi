#include <iostream>
#include "lib.h"

#define NMAX 30

using namespace std;

int main(){
	int a[NMAX], b[NMAX], c[NMAX];
	int n;
	do{
		cout<<"\nInserire il numero degli elementi : ";
		cin >> n;
	} while (n > NMAX);

	leggi_vettore(a, n);
	leggi_vettore(b, n);

	somma_vettori(a, b, c, n);

	stampa_vettore(c, n);
	return 0;
}
