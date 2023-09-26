#include "headers.h"
// functie de initializare a unui nod din arbore
TArb InitializeNod() {
    TArb nod = (TArb) malloc(sizeof(TNod));
    if(nod == NULL) {
        return NULL;
    }
    nod->color.R = 0;
    nod->color.G = 0;
    nod->color.B = 0;
    nod->dr_jos = NULL;
    nod->st_jos = NULL;
    nod->dr_sus = NULL;
    nod->st_sus = NULL;
    return nod;
}
void Afiseaza(TArb a) {
    if(a == NULL) return;
    printf("%d %d %d ", a->color.R, a->color.G, a->color.B);
    printf("(");
    Afiseaza(a->st_sus);
    Afiseaza(a->dr_sus);
    Afiseaza(a->dr_jos);
    Afiseaza(a->st_jos);
    printf(")");
}
// functie pentru calcularea numarului de nivele din arbore
int nrNiveluri(TArb a, int lvl) {
    if(a == NULL) {
        return lvl - 1;
    }
    int max_n = -1;
    int ns_s = nrNiveluri(a->st_sus, lvl + 1);
    if(max_n < ns_s) {
        max_n = ns_s;
    }
    int nd_s = nrNiveluri(a->dr_sus, lvl + 1);
    if(max_n < nd_s) {
        max_n = nd_s;
    }
    int ns_j = nrNiveluri(a->st_jos, lvl + 1);
    if(max_n < ns_j) {
        max_n = ns_j;
    }
    int nd_j = nrNiveluri(a->dr_jos, lvl + 1);
    if(max_n < nd_j) {
        max_n = nd_j;
    }
    return max_n;
}
// functie pentru a calcula numarul de frunze din arbore
int nrFrunze(TArb a) {
	if(a == NULL) return 0;
	if(!(a->st_sus) && !(a->dr_sus) && !(a->st_jos) && !(a->dr_jos)) {
		return 1;
	} else {
		return nrFrunze(a->st_sus) + nrFrunze(a->dr_sus) + nrFrunze(a->st_jos) + nrFrunze(a->dr_jos);
	}
}
// functie pentru a calcula nivelul minim pe care se afla o frunza
// se foloseste pentru a determina dimensiunea blocului cel mai mare, care a ramas nedivizat
int nivelMin(TArb a, int lvl) {
    int min;
    // ne bazam pe faptul ca daca arborele arborele are copilul st_sus, atunci ii are si pe ceilalti 3 
    // daca arborele nu are copilul st_sus, atunci nu are niciunul
    if(a->st_sus) {
	    min = nivelMin(a->st_sus, lvl + 1);
    }
    if(a->dr_sus) {
	    int nd_s = nivelMin(a->dr_sus, lvl + 1);
        if(min > nd_s) {
            min = nd_s;
        }
    }
    if(a->st_jos) {
        int ns_j = nivelMin(a->st_jos, lvl + 1);
        if(min > ns_j) {
            min = ns_j;
        }
    }
    if(a->dr_jos) {
        int nd_j = nivelMin(a->dr_jos, lvl + 1);
        if(min > nd_j) {
            min = nd_j;
        }
    }
    if(!(a->st_sus) && !(a->dr_sus) && !(a->st_jos) && !(a->dr_jos)) {
        return lvl;
    }
	return min;
}
// functii pentru dezalocarea arborelui
void DezalocaArbore(TArb a) {
    if(a->st_sus && a->dr_sus && a->st_jos && a->dr_jos) {
        DezalocaArbore(a->st_sus);
        DezalocaArbore(a->dr_sus);
        DezalocaArbore(a->st_jos);
        DezalocaArbore(a->dr_jos);
    }
    free(a);
}
void DistrugeArbore(TArb* a) {
    if(*a == NULL) return;
    DezalocaArbore(*a);
    *a = NULL;
}