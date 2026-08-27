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

int func_02009138(int *thiz, int arg)
{
    struct Vector3 v;
    int len;
    int r;
    Vec3_Sub(&v, (struct Vector3*)((char*)thiz + 0x8c), (struct Vector3*)((char*)thiz + 0x80));
    len = LenVec3(&v);
    r = Math_Function_0203b14c(&len, arg, 0x300, 0x60000, 0x200);
    v.x = 0;
    v.y = 0;
    v.z = len;
    Matrix4x3_FromTranslation(&data_020a0e68, thiz[0x20], thiz[0x21], thiz[0x22]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)((char*)thiz + 0x17c));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)((char*)thiz + 0x17e));
    MulVec3Mat4x3(&v, &data_020a0e68, (struct Vector3*)((char*)thiz + 0x8c));
    return (r == 0) ? 1 : 0;
}
