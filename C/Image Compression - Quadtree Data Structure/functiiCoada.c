#include "headers.h"
// functie de initializare a cozii 
TCoada* InitQ() {
    TCoada* queue = (TCoada*) malloc(sizeof(TCoada));
    if(queue == NULL) return NULL;
    queue->inc = NULL;
    queue->sf = NULL;
    return queue;
}
// functie de adaugare in coada
int PushQ(TCoada* queue, TArb arbore) {
    TLista aux = (TLista) malloc(sizeof(TCelula));
    aux->nod = arbore;
    aux->urm = NULL;
    if(aux == NULL) return 0;
    if(queue->inc == NULL) {
        queue->inc = aux;
        queue->sf = aux;
    } else {
        queue->sf->urm = aux;
        queue->sf = aux;
    }
    return 1;
}
// functie de eliminare a unui element din coada
TArb PopQ(TCoada* queue) {
    if(queue->inc == NULL) return NULL;
    TArb arb = queue->inc->nod;
    TLista aux = queue->inc;
    queue->inc = queue->inc->urm;
    if(queue->inc == NULL) {
        queue->sf = NULL;
    }
    free(aux);
    return arb;
}
// functie de dezalocare a cozii
void DezalocaCoada(TCoada** queue) {
    free(*queue);
    *queue = NULL;
}