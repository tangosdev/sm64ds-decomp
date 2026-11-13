//cpp
// Vec2_Len: length of a 2D vector (its X and Y components)
#include "common.h"
#include "cstd.h"
#include "Vector2Math.h"

Fix12i Vec2_Len(const Vector2 *v) {
    const Fix12i *xy = (const Fix12i *)v;
    long long sq = (long long)xy[0] * xy[0];
    sq += (long long)xy[1] * xy[1];
    return cstd::sqrt(sq);
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
