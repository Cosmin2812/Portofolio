#include "headers.h"
// functie care calculeaza scorul de similaritate pentru blocul care incepe de la coordonatele x si y si este de dimensiunea size
// functia transmite si prin adresa culoarea medie (RGB) pentru blocul respectiv
unsigned long long getColorMean(unsigned int x, unsigned int y, imag* imagine, unsigned int size, unsigned char* redRem, unsigned char* greenRem, unsigned char* blueRem) {
    unsigned int i, j;
    unsigned long long red = 0;
    unsigned long long green = 0;
    unsigned long long blue = 0;
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            // printf("%d %d %d -- ", imagine->pixels[x + i][y + j].R, imagine->pixels[x + i][y + i].G, imagine->pixels[x + i][y + i].B);
            red = red + imagine->pixels[x + i][y + j].R;
            green = green + imagine->pixels[x + i][y + j].G;
            blue = blue + imagine->pixels[x + i][y + j].B;
        }
        // printf("\n");
    }
    red = red / (size * size);
    green = green / (size * size);
    blue = blue / (size * size);
    *redRem = (unsigned char) red;
    *greenRem = (unsigned char) green;
    *blueRem = (unsigned char) blue;
    unsigned long long mean = 0;
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            mean = mean + (red - (unsigned long long) imagine->pixels[x + i][y + j].R) * (red - (unsigned long long) imagine->pixels[x + i][y + j].R) +
            (green - (unsigned long long) imagine->pixels[x + i][y + j].G) * (green - (unsigned long long) imagine->pixels[x + i][y + j].G) +
            (blue - (unsigned long long) imagine->pixels[x + i][y + j].B) * (blue - (unsigned long long) imagine->pixels[x + i][y + j].B);  
        }
    }
    mean = mean / (3 * size * size);
    return mean;
}
// functie recursiva de creare a arborelui cuaternar
TArb CreeazaArbore(TArb radacina, imag* imagine, unsigned long long prag, int directie, unsigned int x, unsigned int y, unsigned int size) {
    // directie = 1 / 2 / 3 / 4 (stanga sus / dreapta sus / stanga jos / dreapta jos)
    TArb nod = InitializeNod();
    if(nod == NULL) return NULL;
    unsigned long long mean = getColorMean(x, y, imagine, size, &(nod->color.R), &(nod->color.G), &(nod->color.B));
    if(radacina != NULL) {
        if(directie == 1) {
            radacina->st_sus = nod;
        }
        if(directie == 2) {
            radacina->dr_sus = nod;
        }
        if(directie == 3) {
            radacina->st_jos = nod;
        }
        if(directie == 4) {
            radacina->dr_jos = nod;
        }
    }
    if(mean > prag) {
        nod->color.R = 0;
        nod->color.G = 0;
        nod->color.B = 0;
        CreeazaArbore(nod, imagine, prag, 1, x, y, size / 2);
        CreeazaArbore(nod, imagine, prag, 2, x, y + size / 2, size / 2);
        CreeazaArbore(nod, imagine, prag, 3, x + size / 2, y, size / 2);
        CreeazaArbore(nod, imagine, prag, 4, x + size / 2, y + size / 2, size / 2);
    }
    if(radacina == NULL) {
        return nod;
    } else {
        return NULL;
    }
}
// functie care extrage informatiile din imaginea .ppm si le pune intr-o structura de tipul imag in care sunt retinute
// tipul imaginii (P6), dimensiunea imaginii, dimensiunea maxima a pixelilor (255) si o matrice de structuri (pixel**)
// tipul pixel contine valorile RGB pentru fiecare pixel din imagine
imag* CitireImagine(FILE* f) {
    unsigned int i, j;
    imag* imagine = malloc(sizeof(imag));
    if(imagine == NULL) return NULL;
    imagine->type = malloc(MAX_TYPE * sizeof(char));
    if(imagine->type == NULL) {
        free(imagine);
        return NULL;
    }
    fscanf(f, "%s", imagine->type);
    fscanf(f, "%u", &(imagine->dim));
    unsigned int t = 0;
    fscanf(f, "%u", &(t));
    fscanf(f, "%d", &(imagine->maxDimPixels));
    fseek(f, 1, SEEK_CUR);
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
    for(i = 0; i < imagine->dim; i++) {
        for(j = 0; j < imagine->dim; j++) {
            fread(&(imagine->pixels[i][j].R), sizeof(unsigned char), 1, f);
            fread(&(imagine->pixels[i][j].G), sizeof(unsigned char), 1, f);
            fread(&(imagine->pixels[i][j].B), sizeof(unsigned char), 1, f);
        }
    }
    return imagine;
}
// functie de dezalocare a imaginii
void DezalocaImagine(imag** imagine) {
    int i;
    free((*imagine)->type);
    for(i = 0; i < (*imagine)->dim; i++) {
        free((*imagine)->pixels[i]);
    }
    free((*imagine)->pixels);
    free(*imagine);
    *imagine = NULL;
}