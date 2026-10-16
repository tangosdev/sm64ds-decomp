#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x60];
    s32 mPosY; /* 0x060 */
};

struct dCamera_c {
    char pad_000[0x8c];
    Vector3 pos; /* 0x08c */
};

extern struct dActor_c *func_0200e55c(unsigned int ownerID);

int func_02008080(struct dCamera_c *cam) {
    struct dActor_c *a;
    int *dst;

    a = func_0200e55c(0x13);
    dst = &cam->pos.y;
    *dst = *dst + a->mPosY;
    return 1;
}
