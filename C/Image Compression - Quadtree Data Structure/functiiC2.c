#include "headers.h"
// functie de parcurgere a arborelui de tipul breadth first search
// totodata scrie valorile RGB ale celulelor de tip frunza in fisierul g asa cum este descris in cerinta 2
void BFS(TArb arbore, FILE* g, TCoada* queue) {
    do {
        if(!(arbore->st_sus) && !(arbore->dr_sus) && !(arbore->st_jos) && !(arbore->dr_jos)) {
            unsigned char type = 1;
            fwrite(&type, sizeof(unsigned char), 1, g);
            fwrite(&(arbore->color.R), sizeof(unsigned char), 1, g);
            fwrite(&(arbore->color.G), sizeof(unsigned char), 1, g);
            fwrite(&(arbore->color.B), sizeof(unsigned char), 1, g);
        } else {
            unsigned char type = 0;
            fwrite(&type, sizeof(unsigned char), 1, g);
            PushQ(queue, arbore->st_sus);
            PushQ(queue, arbore->dr_sus);
            PushQ(queue, arbore->dr_jos);
            PushQ(queue, arbore->st_jos);
        }
    arbore = PopQ(queue);
    } while(arbore != NULL);
}