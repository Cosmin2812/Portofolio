#include "headers.h"
int main(int argc, char* argv[]) {
    // cerinta 1
    // se citeste imaginea in "imagine"
    // se creeaza arborele cuaternar corespunzator
    // se calculeaza si se afiseaza informatiile cerute despre arbore
    if(strcmp(argv[1], "-c1") == 0) {
        int factor = atoi(argv[2]);
        FILE* f = fopen(argv[3], "rb");
        FILE* g = fopen(argv[4], "w");
        imag* imagine = CitireImagine(f);
        TArb a = CreeazaArbore(NULL, imagine, factor, 0, 0, 0, imagine->dim);
        int nr_niveluri = nrNiveluri(a, 1);
        int nr_blocuri = nrFrunze(a);
        unsigned int nivel_min = nivelMin(a, 0);
        unsigned int zona_max = 0;
        if(nivel_min != 0) {
            zona_max = imagine->dim / (pow(2, nivel_min));
        } else {
            zona_max = imagine->dim;
        }
        fprintf(g, "%d\n%d\n%d\n", nr_niveluri, nr_blocuri, zona_max);
        DezalocaImagine(&imagine);
        DistrugeArbore(&a);
        fclose(f);
        fclose(g);
    }
    // cerinta 2
    // se scriu datele din arbore in fisierul de compresie parcurgandu-l prin BFS
    if(strcmp(argv[1], "-c2") == 0) {
        int factor = atoi(argv[2]);
        FILE* f = fopen(argv[3], "rb");
        FILE* g = fopen(argv[4], "wb");
        imag* imagine = CitireImagine(f);
        TArb a = CreeazaArbore(NULL, imagine, factor, 0, 0, 0, imagine->dim);
        fwrite(&(imagine->dim), sizeof(unsigned int), 1, g);
        TCoada* queue = InitQ();
        BFS(a, g, queue);
        DezalocaCoada(&queue);
        DezalocaImagine(&imagine);
        DistrugeArbore(&a);
        fclose(g);
        fclose(f);
    }
    // cerinta 3
    // se extrage arborele din fisierul de compresie
    // se creeaza variabila imag_rec in care este stocata matricea de pixeli
    // se introduc datele din matrice in fisierul de output
    if(strcmp(argv[1], "-d") == 0) {
        FILE* f = fopen(argv[2], "rb");
        FILE* g = fopen(argv[3], "wb");
        unsigned int dim_imag = 0;
        TArb ResultedTree = GetBinaryTree(f, &(dim_imag));
        imag* imag_rec = RecuperareImagine(ResultedTree, dim_imag);
        InsertInFile(g, imag_rec);
        DistrugeArbore(&ResultedTree);
        DezalocaImagine(&imag_rec);
        fclose(f);
        fclose(g);
    }
    return 0;
}
