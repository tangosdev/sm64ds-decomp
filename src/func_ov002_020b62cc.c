#include "types.h"
// @symbol func_ov002_020b62cc
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern struct Matrix4x3 data_020a0e68;
extern void InvMat4x3(struct Matrix4x3 *d, struct Matrix4x3 *s);
extern void MulVec3Mat4x3(struct Vector3 *v, struct Matrix4x3 *m, struct Vector3 *out);

void func_ov002_020b62cc(unsigned char *self, unsigned char *arg)
{
    struct Vector3 v;
    struct Vector3 axis;
    *(u8 *)(self + 0x34c) = 1;
    data_020a0e68 = *(struct Matrix4x3 *)(self + 0x2ec);
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    MulVec3Mat4x3((struct Vector3 *)(arg + 0x5c), &data_020a0e68, &v);
    v.y = v.y << 1;
    axis.x = 0;
    axis.z = 0;
    axis.y = 0x1000;
    Quaternion_FromVector3((int *)(self + 0x330), &axis, &v);
    Quaternion_Normalize((int *)(self + 0x330));
    *(u8 *)(self + 0x34d) = 0xa;
}
