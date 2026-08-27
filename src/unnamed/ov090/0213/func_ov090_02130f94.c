// @symbol func_ov090_02130f94
/* recovered: shared common types */
#include "common.h"
#pragma opt_strength_reduction off
#pragma opt_common_subs off


extern void MulMat4x3Mat4x3(struct Matrix4x3* out, struct Matrix4x3* a, struct Matrix4x3* b);
extern void Vec3_Lsl(struct Vector3* out, const struct Vector3* in, int n);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6);
extern struct Matrix4x3 data_020a0e68;

void func_ov090_02130f94(char* c_)
{
    int zero;
    int sh;
    char* c;
    int i;
    struct Matrix4x3* src;
    int idx;
    int b;
    unsigned int id;

    c = c_;
    b = (int)((*(int*)(c + 0xb0) & 8) != 0);
    if (b != 0) return;

    src = (struct Matrix4x3*)(c + 0x328);
    i = 0;
    idx = 6;
    zero = 0;
    sh = 3;
    id = 0xea;
    for (; i < 4; ++i) {
        struct Vector3 v;
        struct Vector3 r;

        data_020a0e68 = *src;
        MulMat4x3Mat4x3((struct Matrix4x3*)*(void**)(c + 0x320) + idx, &data_020a0e68, &data_020a0e68);
        v.x = *(int*)((char*)&data_020a0e68 + 0x24);
        v.y = *(int*)((char*)&data_020a0e68 + 0x28);
        v.z = *(int*)((char*)&data_020a0e68 + 0x2c);
        Vec3_Lsl(&r, &v, sh);
        v.x = r.x;
        v.y = r.y;
        v.z = r.z;
        v.y = *(int*)(c + 0x3ac);
        *(unsigned int*)(c + i * 4 + 0x380) =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned int*)(c + i * 4 + 0x380), id, v.x, v.y, r.z, zero, zero);
        idx += 3;
    }
}
