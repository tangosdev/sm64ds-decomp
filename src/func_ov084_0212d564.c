#include "types.h"
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

struct Locals {
    s16 acc[3];
    int tmp[3];
};

extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* d, void* a, void* b);
extern void Vec3_LslInPlace(void* v, int sh);
extern void SubVec3(void* a, void* b, void* c);

extern u8 data_ov084_02130294[];
extern s32 data_020a0e68[];
extern s16 data_02082214[];

void func_ov084_0212d564(char* c)
{
    struct Locals locals;

    Matrix4x3_FromRotationY(c + 0x12c, *(s16*)(c + 0x8e));
    *(s32*)(c + 0x150) = *(s32*)(c + 0x5c) >> 3;
    *(s32*)(c + 0x154) = *(s32*)(c + 0x60) >> 3;
    *(s32*)(c + 0x158) = *(s32*)(c + 0x64) >> 3;

    if ((u32)(*(s32*)(c + 0x1ec) - 2) > 1) return;
    if (*(s32*)(c + 0x204) != *(s32*)(c + 0x210)) return;

    locals.acc[0] = 0;
    locals.acc[1] = 0;
    locals.acc[2] = 0;
    {
        char* base = *(char**)(c + 0x120);
        int i;
        u8* p = data_ov084_02130294;
        for (i = 0; i < 5; i++) {
            locals.acc[0] += *(s16*)(base + *p * 0x34 + 0x1a);
            locals.acc[1] += *(s16*)(base + *p * 0x34 + 0x1c);
            locals.acc[2] += *(s16*)(base + *p++ * 0x34 + 0x1e);
        }
    }

    *(s32*)(c + 0x1f8) = 0;
    *(s32*)(c + 0x1fc) = 0;
    *(s32*)(c + 0x200) = 0;

    Vec3_Asr(locals.tmp, c + 0x5c, 3);
    Matrix4x3_FromTranslation(
        data_020a0e68,
        locals.tmp[0],
        locals.tmp[1],
        locals.tmp[2]
    );
    MulMat4x3Mat4x3(
        *(char**)(c + 0x124) + 0x120,
        data_020a0e68,
        data_020a0e68
    );
    *(s32*)(c + 0x1f8) = data_020a0e68[0x24 / 4];
    *(s32*)(c + 0x1fc) = data_020a0e68[0x28 / 4];
    *(s32*)(c + 0x200) = data_020a0e68[0x2c / 4];
    Vec3_LslInPlace(c + 0x1f8, 3);
    SubVec3(c + 0x1f8, c + 0x5c, c + 0x1f8);

    {
        int sx = data_02082214[((u16)locals.acc[0] >> 4) * 2] * 0x32;
        int sy = data_02082214[((u16)locals.acc[1] >> 4) * 2];
        int n;

        *(s32*)AT(c, 0x1f8) += (int)(((s64)sx * sy + 0x800) >> 12);

        n = 0;
        if (*(u16*)(c + 0xc) == 0xfb)
            n = 1;
        if (n != 0) {
            int v = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
            *(s32*)AT(c, 0x1fc) -= 0x19000 - v;
        } else {
            int v = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
            *(s32*)AT(c, 0x1fc) -= 0x32000 - v;
        }

        {
            int sz = data_02082214[((u16)locals.acc[1] >> 4) * 2 + 1];
            *(s32*)AT(c, 0x200) += (int)(((s64)sx * sz + 0x800) >> 12);
        }

        *(s32*)(c + 0x1f8) =
            (int)(((s64)*(s32*)(c + 0x1f8) * *(s32*)(c + 0x204) + 0x800) >> 12);
        *(s32*)(c + 0x1fc) =
            (int)(((s64)*(s32*)(c + 0x1fc) * *(s32*)(c + 0x204) + 0x800) >> 12);
        *(s32*)(c + 0x200) =
            (int)(((s64)*(s32*)(c + 0x200) * *(s32*)(c + 0x204) + 0x800) >> 12);
    }
}
