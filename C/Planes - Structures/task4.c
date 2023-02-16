#include "./utils.h"
// functia care verifica daca un obstacol se afla in calea avionului
int checkHit(plane planeInfo, int xCoord, int yCoord) {
    if (planeInfo.type == '1') {
        if (planeInfo.direction == 'N') {
            if (xCoord <= planeInfo.line + 1 && yCoord >= planeInfo.column - 2 && yCoord <= planeInfo.column + 2) {
                return 1;
            } else if ((xCoord == planeInfo.line + 2 || xCoord == planeInfo.line + 3)
                        && yCoord >= planeInfo.column - 1 && yCoord <= planeInfo.column + 1) {
                return 1;
            }
        } else if (planeInfo.direction == 'W') {
            if (yCoord <= planeInfo.column + 1 && xCoord >= planeInfo.line - 2 && xCoord <= planeInfo.line + 2) {
                return 1;
            } else if ((yCoord == planeInfo.column + 2 || yCoord == planeInfo.column + 3)
                        && xCoord >= planeInfo.line - 1 && xCoord <= planeInfo.line + 1) {
                return 1;
            }
        } else if (planeInfo.direction == 'S') {
            if (xCoord >= planeInfo.line - 1 && yCoord >= planeInfo.column - 2 && yCoord <= planeInfo.column + 2) {
                return 1;
            } else if ((xCoord == planeInfo.line - 2 || xCoord == planeInfo.line - 3)
                        && yCoord >= planeInfo.column - 1 && yCoord <= planeInfo.column + 1) {
                return 1;
            }
        } else if (planeInfo.direction == 'E') {
            if (yCoord >= planeInfo.column - 1 && xCoord >= planeInfo.line - 2 && xCoord <= planeInfo.line + 2) {
                return 1;
            } else if ((yCoord == planeInfo.column - 2 || yCoord == planeInfo.column - 3)
                        && xCoord >= planeInfo.line - 1 && xCoord <= planeInfo.line + 1) {
                return 1;
            }
        }
    } else {
        if (planeInfo.direction == 'N') {
            if (xCoord <= planeInfo.line + 2 && yCoord >= planeInfo.column - 3 && yCoord <= planeInfo.column + 3) {
                return 1;
            } else if ((xCoord == planeInfo.line + 3 || xCoord == planeInfo.line + 4)
                        && yCoord >= planeInfo.column - 2 && yCoord <= planeInfo.column + 2) {
                return 1;
            }
        } else if (planeInfo.direction == 'W') {
            if (yCoord <= planeInfo.column + 2 && xCoord >= planeInfo.line - 3 && xCoord <= planeInfo.line + 3) {
                return 1;
            } else if ((yCoord == planeInfo.column + 3 || yCoord == planeInfo.column + 4)
                        && xCoord >= planeInfo.line - 2 && xCoord <= planeInfo.line + 2) {
                return 1;
            }
        } else if (planeInfo.direction == 'S') {
            if (xCoord >= planeInfo.line - 2 && yCoord >= planeInfo.column - 3 && yCoord <= planeInfo.column + 3) {
                return 1;
            } else if ((xCoord == planeInfo.line - 3 || xCoord == planeInfo.line - 4)
                        && yCoord >= planeInfo.column - 2 && yCoord <= planeInfo.column + 2) {
                return 1;
            }
        } else if (planeInfo.direction == 'E') {
            if (yCoord >= planeInfo.column - 2 && xCoord >= planeInfo.line - 3 && xCoord <= planeInfo.line + 3) {
                return 1;
            } else if ((yCoord == planeInfo.column - 3 || yCoord == planeInfo.column - 4)
                        && xCoord >= planeInfo.line - 2 && xCoord <= planeInfo.line + 2) {
                return 1;
            }
        }
    }
    return 0;
}
void SolveTask4(void* info, int nrPlanes, int nrObstacles, int* x, int* y, int N) {
    int* notHit = (int*) malloc(nrPlanes * sizeof(int));
    if (notHit == NULL) {
        printf("Alocare nereusita");
    }
    for (int i = 0; i < nrPlanes; i++) {
        notHit[i] = 1;
    }
    for (int k = 0; k < nrObstacles; k++) {
        for (int j = 0; j < nrPlanes; j++) {
            // pentru fiecare avion se verifica daca
            // obstacolul x[k], y[k] se afla in calea sa
            plane* inter = (plane*) info + j;
            if (checkHit(*inter, *(x + k), *(y + k)))
                notHit[j] = 0;
        }
    }
    // numaram pentru cate pozitii notHit[poz] == 1
    // aceste pozitii sunt corespunzatoare acelor avioane
    // care pot trece fara a se lovi de obstacole
    int count = 0;
    for (int i = 0; i < nrPlanes; i++) {
        if (notHit[i] == 1) {
            count++;
        }
    }
    free(notHit);
    printf("%d", count);
}
