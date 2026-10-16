#include "types.h"

/* The fields used here, named as in include/dCamera_c.h. */
struct dCamera_c {
    char pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    Vector3 pos;    /* 0x08c */
    char pad_098[0x17c - 0x98];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
};

extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern short Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);

void func_0200928c(struct dCamera_c *cam)
{
    short h = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleY = h;

    short v = Vec3_VertAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = v;
}
