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
// acceasi functie de extragere, dar pentru tipul short
int extract_sequence2(unsigned short nr, int dim) {
    int temp = 0;
    unsigned int bit = 0;
    int nrBitesShort = sizeof(unsigned short) * BYTE_DIM;
    for (int i = nrBitesShort - 1; i >= nrBitesShort - dim; i--) {
        bit = (nr >> i) & 1;
        add_bit(&temp, dim - (nrBitesShort - i), bit);
    }
    return temp;
}
int main() {
    unsigned int seq = 0;
    scanf("%u", &seq);
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
    int nrBitesShort = BYTE_DIM * sizeof(unsigned short);
    // nrOperands - totalul numerelor citite de tip short
    int nrOperands = ((N + 1) * dim_operators) / nrBitesShort;
    if ((N + 1) * dim_operators > (BYTE_DIM * sizeof(unsigned short) * nrOperands)) {
        nrOperands = nrOperands + 1;
    }
    // stocam numerele citite de la tastatura in vectorul operands
    unsigned short* operands = (unsigned short*) malloc(nrOperands * sizeof(unsigned short));
    if (!operands) {
        printf("Alocarea a esuat");
        free(operators);
        return 0;
    }
    for (int i = 0; i < nrOperands; i++) {
        scanf("%hu", operands + i);
    }
    int* numbers = (int*) malloc((N + 1) * sizeof(int));
    if (!numbers) {
        printf("Alocarea a esuat");
        free(operators);
        free(operands);
        return 0;
    }
    // extragem din vectorul operands, numerele necesare in vectorul numbers
    // dupa extragerea numarului, shiftam la stanga cu dim_operators pentru a putea extrage urmatorul numar
    // pentru fiecare element din operands sunt nrOperandsPerShort numere
    // index este index-ul numarului din vectorul operands din care trebuie sa luam bit-ul
    for (int i = 0; i < N + 1; i++) {
        int nrOperandsPerShort = nrBitesShort / dim_operators;
        int index = i / nrOperandsPerShort;
        numbers[i] = extract_sequence2(operands[index], dim_operators);
        operands[index] = operands[index] << dim_operators;
    }
    // efectuam operatiile fara a tine cont de precedenta operatiilor
    int result = numbers[0];
    for (int i = 0; i < N; i++) {
        switch (operators[i]) {
        case '+':
        result = result + numbers[i + 1];
        break;
        case '-':
        result = result - numbers[i + 1];
        break;
        case '*':
        result = result * numbers[i + 1];
        break;
        case '/':
        result = result / numbers[i + 1];
        break;
        }
    }
    printf("%d", result);
    printf("\n");
    free(operators);
    free(numbers);
    free(operands);
    return 0;
}
