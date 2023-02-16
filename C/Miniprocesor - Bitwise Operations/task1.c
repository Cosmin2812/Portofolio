#include <stdio.h>
#include <stdlib.h>
#define BYTE_DIM 8
// functie de adaugare a bit-ului in secventa
void add_bit(int* x, int dim_seq, unsigned int bit) {
    if (bit == 1) {
        *x = *x | (1 << dim_seq);
    }
}
// functie de extragere a unei secvente de biti de dimensiunea dim din nr
int extract_sequence(unsigned int nr, int dim) {
    int temp = 0;
    unsigned int bit = 0;
    int nrBitesInt = sizeof(unsigned int) * BYTE_DIM;
    for (int i = nrBitesInt - 1; i >= nrBitesInt - dim; i--) {
        bit = (nr >> i) & 1;
        add_bit(&temp, dim - (nrBitesInt - i), bit);
    }
    return temp;
}
int main() {
    unsigned int seq = 0;
    scanf("%u", &seq);
    // N - numarul de operatii
    int N = extract_sequence(seq, 3) + 1;
    seq = seq << 3;
    char* operators = malloc(N * sizeof(char));
    if (!operators) {
        printf("Alocarea a esuat");
        return 0;
    }
    for (int i = 0; i < N; i++) {
        int op = extract_sequence(seq, 2);
        switch (op) {
            case 0:
            operators[i] = '+';
            break;
            case 1:
            operators[i] = '-';
            break;
            case 2:
            operators[i] = '*';
            break;
            case 3:
            operators[i] = '/';
            break;
        }
        seq = seq << 2;
    }
    int dim_operators = extract_sequence(seq, 4) + 1;
    printf("%d ", N);
    for (int i = 0; i < N; i++) {
        printf("%c ", operators[i]);
    }
    printf("%d", dim_operators);
    printf("\n");
    free(operators);
    return 0;
}
