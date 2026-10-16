#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x8e];
    s16 mAngleY; /* 0x08e */
};

struct dCamera_c {
    char pad_000[0x8c];
    Vector3 pos;        /* 0x08c */
    Vector3 mTargetPos; /* 0x098 */
    char pad_0a4[0x110 - 0xa4];
    struct dActor_c *mTargetPlayer; /* 0x110 */
};

extern void func_0200cb58(void *obj, int index);
extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, const int *ptr);
extern int data_02086eb4[];

int func_020094e4(struct dCamera_c *cam) {
    int buf[3];
    func_0200cb58(cam, 0x1f);
    buf[0] = data_02086eb4[0];
    buf[1] = data_02086eb4[1];
    buf[2] = data_02086eb4[2];
    short angle = cam->mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->mTargetPos, angle, buf);
    return 1;
}
