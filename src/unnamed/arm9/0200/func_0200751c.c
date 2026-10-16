#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x8e];
    s16 mAngleY; /* 0x08e */
};

struct dCamera_c {
    char pad_000[0xb0];
    Vector3 savedLookAt; /* 0x0b0 */
    char pad_0bc[0x110 - 0xbc];
    struct dActor_c *mTargetPlayer; /* 0x110 */
    char pad_114[0x186 - 0x114];
    s16 mTargetAngleY; /* 0x186 */
};

int func_0200751c(struct dCamera_c *o) {
    int tmp = o->mTargetPlayer->mAngleY;
    tmp += 0x8000;
    o->mTargetAngleY = (short)tmp;
    o->savedLookAt.x = 0;
    o->savedLookAt.y = 0;
    o->savedLookAt.z = 0;
    return 1;
}
