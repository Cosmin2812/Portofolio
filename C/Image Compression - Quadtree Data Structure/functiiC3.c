#include "headers.h"
// functie de extragere a arborelui cuaternar din fisierul f
// pentru a resepcta ordinea aparitiei informatiilor din fisierul f, atunci cand formam arborele folosim o coada
TArb GetBinaryTree(FILE* f, unsigned int* dim_imag) {
    *dim_imag = 0;
    fread(dim_imag, sizeof(unsigned int), 1, f);
    TCoada* queue = InitQ();
    if(queue == NULL) return NULL;
    TArb arbore = InitializeNod();
    if(arbore == NULL) {
        DezalocaCoada(&queue);
        return NULL;
    }
    TArb aux = arbore;
    do {
        GetNode(f, aux, queue);
        aux = PopQ(queue);
    } while(aux != NULL);
    DezalocaCoada(&queue);
    return arbore;
}
// functie auxiliara
// daca tipul citit din f este 0, atunci se adauga nodurile copii in coada
// daca tipul citit din f este 1, atunci se scriu valorile RGB in celula
void GetNode(FILE* f, TArb a, TCoada* queue) {
    unsigned char type = -1;
    fread(&type, sizeof(unsigned char), 1, f);
    if(type == 0) {
        a->st_sus = InitializeNod();
        a->dr_sus = InitializeNod();
        a->st_jos = InitializeNod();
        a->dr_jos = InitializeNod();
        PushQ(queue, a->st_sus);
        PushQ(queue, a->dr_sus);
        PushQ(queue, a->dr_jos);
        PushQ(queue, a->st_jos);
        // printf("sal");
    } else {
        fread(&(a->color.R), sizeof(unsigned char), 1, f);
        fread(&(a->color.G), sizeof(unsigned char), 1, f);
        fread(&(a->color.B), sizeof(unsigned char), 1, f);
        //printf("%d %d %d\n", a->color.R, a->color.G, a->color.B);
    }
}
// functie de constructie a imaginii pe baza arborelui cuaternar
void ConstruiesteImagine(TArb a, imag* imagine, unsigned int x, unsigned int y, unsigned int size) {
    unsigned int i, j;
    if(a->st_sus || a->dr_sus || a->st_jos || a->dr_jos) {
        ConstruiesteImagine(a->st_sus, imagine, x, y, size / 2);
        ConstruiesteImagine(a->dr_sus, imagine, x, y + size / 2, size / 2);
        ConstruiesteImagine(a->st_jos, imagine, x + size / 2, y, size / 2);
        ConstruiesteImagine(a->dr_jos, imagine, x + size / 2, y + size / 2, size / 2);
    } else {
        for(i = 0; i < size; i++) {
            for(j = 0; j < size; j++) {
                imagine->pixels[x + i][y + j].R = a->color.R;
                imagine->pixels[x + i][y + j].G = a->color.G;
                imagine->pixels[x + i][y + j].B = a->color.B;
            }
        }
    }
}
// functie care aloca un element de tip imag
// se foloseste de functia anterioara pentru a pune valorile din arbore in matricea pixels**
imag* RecuperareImagine(TArb a, unsigned int dim_imag) {
    unsigned int i, j;
    imag* imagine = (imag*) malloc(sizeof(imag));
    imagine->dim = dim_imag;
    imagine->maxDimPixels = 255;
    imagine->type = malloc(MAX_TYPE * sizeof(char));
    strcpy(imagine->type, "P6");
    imagine->pixels = malloc(imagine->dim * sizeof(pixel*));
    if(imagine->pixels == NULL) {
        free(imagine->type);
        free(imagine);
        return NULL;
    }
    for(i = 0; i < imagine->dim; i++) {
        imagine->pixels[i] = malloc(imagine->dim * sizeof(pixel));
        if(imagine->pixels[i] == NULL) {
            for(j = 0; j < i; j++) free(imagine->pixels[j]);
            free(imagine->type);
            free(imagine->pixels);
            free(imagine);
            return NULL;
        }
    }
    ConstruiesteImagine(a, imagine, 0, 0, dim_imag);
    return imagine;
}
// functie care insereaza in fiserul h informatiile din structura de tip imag, conform formatului cerut
void InsertInFile(FILE* h, imag* imagine) {
    unsigned int i, j;
    fprintf(h, "%s\n", imagine->type);
    fprintf(h, "%u %u\n", imagine->dim, imagine->dim);
    fprintf(h, "%d\n", imagine->maxDimPixels);
    for(i = 0; i < imagine->dim; i++) {
        for(j = 0; j < imagine->dim; j++) {
            fwrite(&(imagine->pixels[i][j].R), sizeof(unsigned char), 1, h);
            fwrite(&(imagine->pixels[i][j].G), sizeof(unsigned char), 1, h);
            fwrite(&(imagine->pixels[i][j].B), sizeof(unsigned char), 1, h);
        }
    }
}