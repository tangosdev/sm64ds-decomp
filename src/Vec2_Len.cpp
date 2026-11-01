//cpp
// Vec2_Len: length of a 2D vector (its X and Y components)
#include "common.h"
#include "cstd.h"

extern "C" {

/* Not the shared Vector3: the ROM function reads two words, and its callers
   pass 2-vectors (including pointers into the middle of int arrays). */
typedef struct {
    Fix12i x, y;
} Vec2;


Fix12i Vec2_Len(const Vec2 *v) {
    long long sq = (long long)v->x * v->x;
    sq += (long long)v->y * v->y;
    return cstd::sqrt((u64)sq);
}
}

/* Zero 2D origin. Retail __sinit_020750ec stores both words and registers
   NullDestructor_0203d47c, the empty ~Vector2. */
#ifndef SM64DS_PLATFORM_PC
struct ZeroVector2 {
    int x;
    int y;

    ZeroVector2() : x(0), y(0) {}
    ~ZeroVector2();
};

ZeroVector2 data_020a0ec8;
#endif
