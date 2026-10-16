#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
    char pad_068[0x8e - 0x68];
    s16 mAngleY; /* 0x08e */
};

struct dCamera_c {
    char pad_000[0xb0];
    Vector3 savedLookAt; /* 0x0b0 */
    char pad_0bc[0x110 - 0xbc];
    struct dActor_c *mTargetPlayer; /* 0x110 */
};

extern int data_02086ec0[];
extern void Vec3_RotateYAndTranslate(void *src, void *dst, short angle, void *unk);

int func_0200762c(struct dCamera_c *self)
{
    int stack[3];
    stack[0] = data_02086ec0[0];
    stack[1] = data_02086ec0[1];
    stack[2] = data_02086ec0[2];

    short angle = self->mTargetPlayer->mAngleY;
    angle = (short)(angle + 0x8000);

    Vec3_RotateYAndTranslate(&self->savedLookAt, &self->mTargetPlayer->mPos, angle, stack);

    self->savedLookAt.y = 0x1c9000;
    return 1;
}
