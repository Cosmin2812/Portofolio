#include "./utils.h"
// se afiseaza datele avioanelor folosind tipul struct plane
void SolveTask1(void* info, int nrPlanes) {
    for (int i = 0; i < nrPlanes; i++) {
        plane* inter = (plane*) info + i;
        printf("(%hi, %hi)\n", inter -> line, inter -> column);
        printf("%c\n", (inter -> direction));
        printf("%c%c%c%c\n", inter -> type, inter -> code1, inter -> code2, inter -> code3);
        printf("%d\n\n", inter -> speed);
    }
}
