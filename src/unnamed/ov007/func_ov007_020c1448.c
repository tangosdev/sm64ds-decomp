#include "types.h"
#pragma opt_propagation off
#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)

static inline void G3_TexImageParam(u32 addr, u32 texFmt, u32 texGen, u32 sizeS, u32 sizeT,
                                    u32 repeat, u32 flip, u32 pltt0)
{
    reg_G3_TEXIMAGE_PARAM = (u32)((addr >> 3) | (texFmt << 26) | (texGen << 30)
        | (sizeS << 20) | (sizeT << 23) | (repeat << 16) | (flip << 18) | (pltt0 << 29));
}

struct Tex
{
    int f0;
    u32 fmt;
    u32 f8;
    u32 fc;
    u32 texAddr;
    u32 sizeS;
    u32 sizeT;
};

void func_ov007_020c1448(struct Tex *self, u32 texGen, u32 repeat, u32 flip, u32 color0)
{
    if (self != 0)
    {
        G3_TexImageParam(self->texAddr, self->fmt, texGen, self->sizeS, self->sizeT, repeat, flip,
                         color0);
    }
    else
    {
        reg_G3_TEXIMAGE_PARAM = 0x40000000;
    }
}
