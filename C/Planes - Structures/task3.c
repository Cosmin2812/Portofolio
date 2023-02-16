#include "./utils.h"
int comp(const void* p1, const void* p2) {
    plane copy1 = *((const plane*) p1);
    plane copy2 = *((const plane*) p2);
    if (copy1.type - copy2.type) {
        return (copy1.type - copy2.type);
    } else if (copy1.code1 - copy2.code1) {
        return -(copy1.code1 - copy2.code1);
    } else if (copy1.code2 - copy2.code2) {
        return -(copy1.code2 - copy2.code2);
    } else if (copy1.code3 - copy2.code3) {
        return -(copy1.code3 - copy2.code3);
    } else if (copy1.speed - copy2.speed) {
        return (copy1.speed - copy2.speed);
    }
    return 0;
}
void SolveTask3(void* info, int nrPlanes) {
    qsort(info, nrPlanes, sizeof(plane), comp);
    SolveTask1(info, nrPlanes);
}
