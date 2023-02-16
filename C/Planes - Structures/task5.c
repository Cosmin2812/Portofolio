#include "./utils.h"
// functia care verifica daca coordonatele punctului
// se afla in interiorul matricei
int verification(int line, int column, int N) {
    if (line < 0 || line > N - 1) {
        return 0;
    }
    if (column < 0 || column > N - 1) {
        return 0;
    }
    return 1;
}
// functia care "deseneaza" sau "sterge" avionul de tip '1'
// la pozitia corespunzatoare momentului t
void drawPlane5_1(char** mat, plane* info, int N, char k, int t) {
    int lineHead = info -> line;
    int columnHead = info -> column;
    char direction = info -> direction;
    char type = info -> type;
    int v = info -> speed;
    if (direction == 'N') {
        lineHead = lineHead - v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            if (verification(lineHead + 1, i, N)) {
                *(*(mat + lineHead + 1) + i) = k;
            }
        }
        if (verification(lineHead + 2, columnHead, N)) {
            *(*(mat + lineHead + 2) + columnHead) = k;
        }
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            if (verification(lineHead + 3, i, N)) {
                *(*(mat + lineHead + 3) + i) = k;
            }
        }
    } else if (direction == 'S') {
        lineHead = lineHead + v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            if (verification(lineHead - 1, i, N)) {
                *(*(mat + lineHead - 1) + i) = k;
            }
        }
        if (verification(lineHead - 2, columnHead, N)) {
            *(*(mat + lineHead - 2) + columnHead) = k;
        }
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            if (verification(lineHead - 3, i, N)) {
                *(*(mat + lineHead - 3) + i) = k;
            }
        }
    } else if (direction == 'E') {
        columnHead = columnHead + v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            if (verification(i, columnHead - 1, N)) {
                *(*(mat + i) + columnHead - 1) = k;
            }
        }
        if (verification(lineHead, columnHead - 2, N)) {
            *(*(mat + lineHead) + columnHead - 2) = k;
        }
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            if (verification(i, columnHead - 3, N)) {
                *(*(mat + i) + columnHead - 3) = k;
            }
        }
    } else if (direction == 'W') {
        columnHead = columnHead - v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            if (verification(i, columnHead + 1, N)) {
                *(*(mat + i) + columnHead + 1) = k;
            }
        }
        if (verification(lineHead, columnHead + 2, N)) {
            *(*(mat + lineHead) + columnHead + 2) = k;
        }
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            if (verification(i, columnHead + 3, N)) {
                *(*(mat + i) + columnHead + 3) = k;
            }
        }
    }
}
// functia care "deseneaza" sau "sterge" avionul de tip '1'
// la pozitia corespunzatoare momentului t
void drawPlane5_2(char** mat, plane* info, int N, char k, int t) {
    int lineHead = info -> line;
    int columnHead = info -> column;
    char direction = info -> direction;
    char type = info -> type;
    int v = info -> speed;
    if (direction == 'N') {
        lineHead = lineHead - v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            if (verification(lineHead + 1, i, N)) {
                *(*(mat + lineHead + 1) + i) = k;
            }
        }
        for (int i = columnHead - 3; i <= columnHead + 3; i++) {
            if (verification(lineHead + 2, i, N)) {
                *(*(mat + lineHead + 2) + i) = k;
            }
        }
        if (verification(lineHead + 3, columnHead, N)) {
            *(*(mat + lineHead + 3) + columnHead) = k;
        }
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            if (verification(lineHead + 4, i, N)) {
                *(*(mat + lineHead + 4) + i) = k;
            }
        }
    } else if (direction == 'S') {
        lineHead = lineHead + v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = columnHead - 1; i <= columnHead + 1; i++) {
            if (verification(lineHead - 1, i, N)) {
                *(*(mat + lineHead - 1) + i) = k;
            }
        }
        for (int i = columnHead - 3; i <= columnHead + 3; i++) {
            if (verification(lineHead - 2, i, N)) {
                *(*(mat + lineHead - 2) + i) = k;
            }
        }
        if (verification(lineHead - 3, columnHead, N)) {
            *(*(mat + lineHead - 3) + columnHead) = k;
        }
        for (int i = columnHead - 2; i <= columnHead + 2; i++) {
            if (verification(lineHead - 4, i, N)) {
                *(*(mat + lineHead - 4) + i) = k;
            }
        }
    } else if (direction == 'E') {
        columnHead = columnHead + v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            if (verification(i, columnHead - 1, N)) {
                *(*(mat + i) + columnHead - 1) = k;
            }
        }
        for (int i = lineHead - 3; i <= lineHead + 3; i++) {
            if (verification(i, columnHead - 2, N)) {
                *(*(mat + i) + columnHead - 2) = k;
            }
        }
        if (verification(lineHead, columnHead - 3, N)) {
            *(*(mat + lineHead) + columnHead - 3) = k;
        }
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            if (verification(i, columnHead - 4, N)) {
                *(*(mat + i) + columnHead - 4) = k;
            }
        }
    } else if (direction == 'W') {
        columnHead = columnHead - v * t;
        if (verification(lineHead, columnHead, N)) {
            *(*(mat + lineHead) + columnHead) = k;
        }
        for (int i = lineHead - 1; i <= lineHead + 1; i++) {
            if (verification(i, columnHead + 1, N)) {
                *(*(mat + i) + columnHead + 1) = k;
            }
        }
        for (int i = lineHead - 3; i <= lineHead + 3; i++) {
            if (verification(i, columnHead + 2, N)) {
                *(*(mat + i) + columnHead + 2) = k;
            }
        }
        if (verification(lineHead, columnHead + 3, N)) {
            *(*(mat + lineHead) + columnHead + 3) = k;
        }
        for (int i = lineHead - 2; i <= lineHead + 2; i++) {
            if (verification(i, columnHead + 4, N)) {
                *(*(mat + i) + columnHead + 4) = k;
            }
        }
    }
}
void SolveTask5(void* info, int nrPlanes, int T, int nrPoints, int* x, int* y, int N) {
    char** mat = init_mat(N);
    int* time = malloc((T + 1) * sizeof(int));
    for (int t = 0; t <= T; t++) {
        time[t] = 0;
    }
    for (int k = 0; k < nrPlanes; k++) {
        // se ia pe rand fiecare avion
        plane* inter = (plane*) info + k;
        for (int t = 0; t <= T; t++) {
            // se deseneaza avionul la momentul t
            if (inter -> type == FIRST_TYPE_PLANE) {
                drawPlane5_1(mat, inter, N, DRAWING_ELEMENT, t);
            } else {
                drawPlane5_2(mat, inter, N, DRAWING_ELEMENT, t);
            }
            for (int i = 0; i < nrPoints; i++) {
                // se verifica pentru toate punctele
                // daca unul dintre ele se afla "in" avion
                int v = inter -> speed;
                int xCoord = *(x + i);
                int yCoord = *(y + i);
                if (mat[xCoord][yCoord] == DRAWING_ELEMENT) {
                    time[t]++;
                    break;
                }
            }
            // se "sterge" avionul
            if (inter -> type == '1') {
                drawPlane5_1(mat, inter, N, BACKROUND_ELEMENT, t);
            } else {
                drawPlane5_2(mat, inter, N, BACKROUND_ELEMENT, t);
            }
        }
    }
    // se afiseaza pentru fiecare moment de timp
    // cate avioane se aflau in interiorul punctelor
    for (int t = 0; t <= T; t++) {
        printf("%d: %d\n", t, time[t]);
    }
    free(time);
    free_mat(mat, N);
}
