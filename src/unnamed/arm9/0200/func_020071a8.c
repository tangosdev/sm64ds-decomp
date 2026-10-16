#include "types.h"

/* The fields used here, named as in include/dCamera_c.h. */
struct dCamera_c {
    char pad_000[0x8c];
    struct Vector3 pos;        /* 0x08c */
    struct Vector3 mTargetPos; /* 0x098 */
    char pad_0a4[0x19a - 0xa4];
    short mOrbitTargetYaw; /* 0x19a */
};

extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern short AngleDiff(short a, short b);

void func_020071a8(struct dCamera_c *cam, short ang, int threshold)
{
    short diff = AngleDiff(Vec3_HorzAngle(&cam->mTargetPos, &cam->pos), ang);
    if (diff > threshold)
        cam->mOrbitTargetYaw = ang;
}
