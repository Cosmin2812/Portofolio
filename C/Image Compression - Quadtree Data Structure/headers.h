#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_TYPE 7
typedef struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} pixel;
typedef struct {
    char* type;
    unsigned int dim;
    int maxDimPixels;
    pixel** pixels;
} imag;
typedef struct nod {
    struct nod* st_sus, *dr_sus, *st_jos, *dr_jos;
    pixel color;
} TNod, *TArb;
typedef struct celula {
    struct celula *urm;
    TArb nod;
} TCelula, *TLista;
typedef struct coada {
    TLista inc, sf;
} TCoada;

TCoada* InitQ();
int PushQ(TCoada* queue, TArb arbore);
TArb PopQ(TCoada* queue);
void DezalocaCoada(TCoada** queue);

imag* CitireImagine(FILE* f);
void DezalocaImagine(imag** imagine);
unsigned long long getColorMean(unsigned int x, unsigned int y, imag* imagine, unsigned int size, unsigned char* redRem, unsigned char* greenRem, unsigned char* blueRem);
TArb CreeazaArbore(TArb radacina, imag* imagine, unsigned long long prag, int directie, unsigned int x, unsigned int y, unsigned int size);
void DezalocaArbore(TArb a);
void DistrugeArbore(TArb* a);

void BFS(TArb arbore, FILE* g, TCoada* queue);
void GetNode(FILE* f, TArb a, TCoada* queue);
TArb GetBinaryTree(FILE* f, unsigned int* dim_imag);
void ConstruiesteImagine(TArb a, imag* imagine, unsigned int x, unsigned int y, unsigned int size);
imag* RecuperareImagine(TArb a, unsigned int dim_imag);
void InsertInFile(FILE* h, imag* imagine);

int nrFrunze(TArb a);
int nrNiveluri(TArb a, int lvl);
void Afiseaza(TArb a);
int nivelMin(TArb a, int lvl);
TArb InitializeNod();

