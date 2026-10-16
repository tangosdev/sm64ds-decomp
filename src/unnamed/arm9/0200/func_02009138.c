// @symbol func_02009138
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern int Math_Function_0203b14c(int *p, int a, int b, int c, int d);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, short ang);
extern void MulVec3Mat4x3(struct Vector3 *v, struct Matrix4x3 *m, struct Vector3 *res);
extern struct Matrix4x3 data_020a0e68;

/* The fields used here, named as in include/dCamera_c.h. */
struct dCamera_c {
    char pad_000[0x80];
    struct Vector3 lookAt; /* 0x080 */
    struct Vector3 pos;    /* 0x08c */
    char pad_098[0x17c - 0x98];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
};

int func_02009138(int *thiz, int arg)
{
    struct dCamera_c *cam = (struct dCamera_c *)thiz;
    struct Vector3 v;
    int len;
    int r;
    Vec3_Sub(&v, &cam->pos, &cam->lookAt);
    len = LenVec3(&v);
    r = Math_Function_0203b14c(&len, arg, 0x300, 0x60000, 0x200);
    v.x = 0;
    v.y = 0;
    v.z = len;
    Matrix4x3_FromTranslation(&data_020a0e68, cam->lookAt.x, cam->lookAt.y, cam->lookAt.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, cam->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, cam->mAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &cam->pos);
    return (r == 0) ? 1 : 0;
}
