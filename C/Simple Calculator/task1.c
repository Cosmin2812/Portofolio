#include <stdio.h>
#define MAX_NUMS 100
#define MAX_PRIORITY 1
#define SUM_PRIORITY 1
#define MULTIPLICATION_PRIORITY 1
// functie care citeste numarul N si N numere intr-un vector float si operatori intr-un vector char
void citire(float *numbers, int *length, char *operators) {
    scanf("%d", length);
    for (int i = 0; i < (*length); i++) {
        scanf("%f", &numbers[i]);
    }
    getchar();
    for (int i = 0; i < (*length) - 1; i++) {
        scanf("%c", &operators[i]);
    }
}
// functie care stabileste prioritatile fiecarei operatii
int establish_priority(char c) {
    if (c == '+' || c == '-') {
        return SUM_PRIORITY;
    } else if (c =='*' || c == '/') {
        return MULTIPLICATION_PRIORITY;
    }
    return -1;
}
// functie care face calculul dintre doua numere din vector cu operatia corespunzatoare
// returneaza -1 pentru ca numarul returnat sa nu fie egal cu nicio pozitie din vectorul de numere
void make_calculation(float *numbers, int *mask, int length, int numberPosition, char operation) {
    int numberPosition2 = 0;
    for (int i = numberPosition + 1; i < length; i++) {
        if (mask[i] == 1) {
            numberPosition2 = i;
            break;
        }
    }
    if (operation == '*') {
        numbers[numberPosition] = numbers[numberPosition] * numbers[numberPosition2];
        mask[numberPosition2] = 0;
    } else if (operation == '/') {
        numbers[numberPosition] = numbers[numberPosition] / numbers[numberPosition2];
        mask[numberPosition2] = 0;
    } else if (operation == '+') {
        numbers[numberPosition] = numbers[numberPosition] + numbers[numberPosition2];
        mask[numberPosition2] = 0;
    } else if (operation== '-') {
        numbers[numberPosition] = numbers[numberPosition] - numbers[numberPosition2];
        mask[numberPosition2] = 0;
    }
}
int main() {
    float numbers[MAX_NUMS];
    int N = 0;
    int mask[MAX_NUMS];
    char operators[MAX_NUMS-1];
    citire(numbers, &N, operators);
    // cand N == 1 programul afiseaza automat primul element din vectorul de numere
    if (N == 1) {
        printf("%f\n", numbers[0]);
        return 0;
    }
    // cand N == 0 programul afiseaza automat numarul 0 ca float
    if (N == 0) {
        printf("%f\n", 0.0);
        return 0;
    }
    // vector mask in care stocam 0 (daca numarul a fost parcurs) sau 1 (daca numarul nu a fost parcurs)
    for (int i = 0; i < N; i++) {
        mask[i] = 1;
    }
    for (int currentPriority = 1; currentPriority <= MAX_PRIORITY; currentPriority++) {
        // numbersIndex retine index-ul in vectorul de numere
        // numbersIndexHelp ne ajuta sa spunem cand index-ul trebuie sa se miste din pozitia curenta si cand nu
        int numbersIndex = -1, numbersIndexHelp = -1;
        for (int operatorsIndex = 0; operatorsIndex < N - 1; operatorsIndex++) {
            //  conditie ajutatoare care sa ne spuna cand index-ul ramane la locul lui si cand nu
            if (numbersIndexHelp == numbersIndex) {
                // cautam index-ul numarului care nu a fost parcurs
                for (int i = numbersIndex + 1; i < N; i++) {
                    if (mask[i] == 1) {
                        numbersIndex = i;
                        break;
                    }
                }
            }
            int opPriority = establish_priority(operators[operatorsIndex]);
            // daca prioritatea operatiei corespunzatoare index-ului este aceeasi cu cea care ne trebuie, facem operatia
            // folosim numbersIndexHelp pentru a stoca -1
            // astfel, stim ca la urmatoarea iteratie, index-ul in numbers sa ramana pe aceeasi pozitie
            if (currentPriority == opPriority) {
                make_calculation(numbers, mask, N, numbersIndex, operators[operatorsIndex]);
                numbersIndexHelp = -1;
            // daca prioritatea operatiei corespunzatoare index-ului este mai mica decat cea care ne trebuie,
            // schimbam pozitia index-ului la urmatoarea iteratie in vectorul de numere
            // dandu-i variabilei ajutatoare valoarea index-ului
            } else if (currentPriority < opPriority) {
                numbersIndexHelp = numbersIndex;
            }
        }
    }
    // rezultatul este stocat pe pozitia 0 a vectorului de numere
    printf("%f\n", numbers[0]);
    return 0;
}
