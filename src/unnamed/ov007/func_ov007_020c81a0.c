#include "nitro/fx/fx.h"
typedef short s16;
typedef long long s64;

struct T {
    int* f0;
    int f4;
    int f8;
    char pad_c[4];
    int f10;
    int f14;
    char pad_18[4];
    s16 f1c;
    char pad_1e[2];
    int f20;
};

void func_ov007_020c81a0(struct T* o, int factor, int shift)
{
    if (!(o->f20 & 1)) {
        if (o->f1c == 0x1000) {
            *(int *)((int)&o->f4) += (o->f10 - FX_MUL(factor, o->f4)) >> shift;
            *(int *)((int)&o->f8) += (o->f14 - FX_MUL(factor, o->f8)) >> shift;
        } else {
            *(int *)((int)&o->f10) -= FX_MUL(factor, o->f4);
            *(int *)((int)&o->f14) -= FX_MUL(factor, o->f8);
            *(int *)((int)&o->f4) += FX_MUL(o->f10, (o->f1c >> shift));
            *(int *)((int)&o->f8) += FX_MUL(o->f14, (o->f1c >> shift));
        }
        o->f0[0] += o->f4 >> shift;
        *(int *)((int)&o->f0[1]) += o->f8 >> shift;
    }

    o->f14 = 0;
    o->f10 = o->f14;
}
