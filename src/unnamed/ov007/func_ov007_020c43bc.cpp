//cpp
#include "types.h"
#include "nitro/fx/fx.h"
namespace cstd {
int fdiv(s32, s32);
}
extern s16 data_02082214[];

struct A {
    char pad[0xc];
    u16 fc;
    u16 fe;
    s32 f10;
    char pad2[0xc];
    s32 f20;
    s32 f24;
};
struct B { char pad[0xd4]; u16 fd4; };

extern "C" void func_ov007_020c43bc(A *a, B *b)
{
    int x = a->f10;
    int i = (b->fd4 >> 4) << 1;
    int j = i + 1;
    s16 s = data_02082214[j];
    if (x < 0) x = -x;
    s16 c = data_02082214[i];
    a->f20 = cstd::fdiv(FX_MUL((a->fc << 12), FX_MUL(c, x)), s * 0x60);
    x = a->f10;
    i = (b->fd4 >> 4) << 1;
    j = i + 1;
    s = data_02082214[j];
    if (x < 0) x = -x;
    c = data_02082214[i];
    a->f24 = cstd::fdiv(FX_MUL((a->fe << 12), FX_MUL(c, x)), s * 0x60);
}
