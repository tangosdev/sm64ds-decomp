#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
};

struct dCamera_c {
    char pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    char pad_08c[0x170 - 0x8c];
    s32 mScriptParam; /* 0x170 */
};

extern struct dActor_c *func_0200e55c(unsigned int ownerID);

int func_020080b0(struct dCamera_c *c) {
    struct dActor_c *obj = func_0200e55c(0x13);
    Vector3 *base = &obj->mPos;

    int z = base->z;
    int y = base->y + c->mScriptParam;
    int x = base->x;

    c->lookAt.x = x;
    c->lookAt.y = y;
    c->lookAt.z = z;
    return 1;
}
