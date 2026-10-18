// Vec2_Len: length of a 2D vector (its X and Y components)
#include "common.h"

/* Not the shared Vector3: the ROM function reads two words, and its callers
   pass 2-vectors (including pointers into the middle of int arrays). */
typedef struct {
    Fix12i x, y;
} Vec2;

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i Vec2_Len(const Vec2 *v) {
    long long sq = (long long)v->x * v->x;
    sq += (long long)v->y * v->y;
    return _ZN4cstd4sqrtEy((u64)sq);
}
