//cpp
/* Distance between two Fix12 (x, y) pairs. Only two words are read. */
#include "cstd.h"

extern "C" {

typedef int Fix12i;

typedef unsigned long long u64;


Fix12i func_0203d5dc(const int *a, const int *b) {
    Fix12i dx = a[0] - b[0];
    Fix12i dy = a[1] - b[1];
    long long sq = (long long)dx * dx;
    sq += (long long)dy * dy;
    return cstd::sqrt((u64)sq);
}
}
