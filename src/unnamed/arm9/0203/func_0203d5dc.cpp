//cpp
/* Distance between two Vector2s (Vector2.h). */
#include "cstd.h"
#include "types.h"
#include "Vector2.h"
#include "Vector2Math.h"

fx32 func_0203d5dc(const Vector2 *a, const Vector2 *b) {
    fx32 dx = a->x - b->x;
    fx32 dy = a->y - b->y;
    long long sq = (long long)dx * dx;
    sq += (long long)dy * dy;
    return cstd::sqrt((u64)sq);
}
