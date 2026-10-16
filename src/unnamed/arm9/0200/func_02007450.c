#include "types.h"

/* The fields used here, named as in include/dCamera_c.h. */
struct dCamera_c {
    char pad_000[0x8c];
    Vector3 pos; /* 0x08c */
    char pad_098[0xb0 - 0x98];
    Vector3 savedLookAt; /* 0x0b0 */
    char pad_0bc[0x186 - 0xbc];
    s16 mTargetAngleY; /* 0x186 */
};

extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, void *unk);

int func_02007450(struct dCamera_c *cam)
{
    short angle = cam->mTargetAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->pos, angle, &cam->savedLookAt);
    return 1;
}
