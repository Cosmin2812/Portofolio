#include "./utils.h"
// functia care "deseneaza" in matrice avion de tip '1'
void drawPlane1(char** mat, plane* planeInfo, int N) {
    int lineHead = planeInfo -> line;
    int columnHead = planeInfo -> column;
    char direction = planeInfo -> direction;
    char type = planeInfo -> type;
    int v = planeInfo -> speed;
    if (direction == 'N') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            *(*(mat + lineHead + 1) + i) = '*';
        }
        *(*(mat + lineHead + 2) + columnHead) = '*';
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            *(*(mat + lineHead + 3) + i) = '*';
        }
    } else if (direction == 'S') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            *(*(mat + lineHead - 1) + i) = '*';
        }
        *(*(mat + lineHead - 2) + columnHead) = '*';
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            *(*(mat + lineHead - 3) + i) = '*';
        }
    } else if (direction == 'E') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            *(*(mat + i) + columnHead - 1) = '*';
        }
        *(*(mat + lineHead) + columnHead - 2) = '*';
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            *(*(mat + i) + columnHead - 3) = '*';
        }
    } else if (direction == 'W') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            *(*(mat + i) + columnHead + 1) = '*';
        }
        *(*(mat + lineHead) + columnHead + 2) = '*';
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            *(*(mat + i) + columnHead + 3) = '*';
        }
    }
}
// functia care "deseneaza" in matrice avion de tip '2'
void drawPlane2(char** mat, plane* planeInfo, int N) {
    int lineHead = planeInfo -> line;
    int columnHead = planeInfo -> column;
    char direction = planeInfo -> direction;
    char type = planeInfo -> type;
    int v = planeInfo -> speed;
    if (direction == 'N') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            *(*(mat + lineHead + 1) + i) = '*';
        }
        for (int i = columnHead - 3; i <= columnHead + 3; i++) {
            *(*(mat + lineHead + 2) + i) = '*';
        }
        *(*(mat + lineHead + 3) + columnHead) = '*';
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            *(*(mat + lineHead + 4) + i) = '*';
        }
    } else if (direction == 'S') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            *(*(mat + lineHead - 1) + i) = '*';
        }
        for (int i = columnHead - 3; i <= columnHead + 3; i++) {
            *(*(mat + lineHead - 2) + i) = '*';
        }
        *(*(mat + lineHead - 3) + columnHead) = '*';
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            *(*(mat + lineHead - 4) + i) = '*';
        }
    } else if (direction == 'E') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            *(*(mat + i) + columnHead - 1) = '*';
        }
        for (int i = lineHead - 3; i <= lineHead + 3; i++) {
            *(*(mat + i) + columnHead - 2) = '*';
        }
        *(*(mat + lineHead) + columnHead - 3) = '*';
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            *(*(mat + i) + columnHead - 4) = '*';
        }
    } else if (direction == 'W') {
        *(*(mat + lineHead) + columnHead) = '*';
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            *(*(mat + i) + columnHead + 1) = '*';
        }
        for (int i = lineHead - 3; i <= lineHead + 3; i++) {
            *(*(mat + i) + columnHead + 2) = '*';
        }
        *(*(mat + lineHead) + columnHead + 3) = '*';
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            *(*(mat + i) + columnHead + 4) = '*';
        }
    }
}
void SolveTask2(void* info, int nrPlanes, int N, char** mat) {
    for (int i = 0; i < nrPlanes; i++) {
        // se ia pe rand fiecare avion
        plane* inter = (plane*) info + i;
        if (inter -> type == '1') {
            drawPlane1(mat, inter, N);
        } else {
            drawPlane2(mat, inter, N);
        }
    }
    // se afiseaza matricea
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%c", *(*(mat + i) + j));
        }
        if (i < N - 1) {
            printf("\n");
        }
    }
}
