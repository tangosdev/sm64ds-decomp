//cpp
/* Distance between two fx32 (x, y) pairs. Only two words are read. */
#include "cstd.h"
#include "types.h"

fx32 func_0203d5dc(const int *a, const int *b) {
    fx32 dx = a[0] - b[0];
    fx32 dy = a[1] - b[1];
    long long sq = (long long)dx * dx;
    sq += (long long)dy * dy;
    return cstd::sqrt((u64)sq);
}
