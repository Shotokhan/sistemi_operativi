#include <iostream>
#include <dlfcn.h>
// #include "lib.h"

#define NMAX 30

using namespace std;

typedef void (*PFN_leggiV)(int[], int);
typedef void (*PFN_stampaV)(const int[], int);
typedef void (*PFN_sommaV)(const int[], const int[], int[], int);

int main(){
	void* handle = dlopen("./liblib.so", RTLD_LAZY);
	if (handle == NULL) {
		return 1;
	}
	PFN_leggiV leggi_vettore = reinterpret_cast<PFN_leggiV>(dlsym(handle, "leggi_vettore"));
	PFN_stampaV stampa_vettore = reinterpret_cast<PFN_stampaV>(dlsym(handle, "stampa_vettore"));
	PFN_sommaV somma_vettori = reinterpret_cast<PFN_sommaV>(dlsym(handle, "somma_vettori"));	
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

	dlclose(handle);
	return 0;
}
