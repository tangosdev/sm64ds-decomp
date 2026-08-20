#include "types.h"

/* 6f: pack rank for TEXIMAGE_PARAM (b=r2, fmt=r3, lsr#3 then folded lsl#26). */
#pragma opt_propagation off

#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE   (*(volatile u32 *)0x040004ac)
#define reg_G3_DIF_AMB        (*(volatile u32 *)0x040004c0)
#define reg_G3_SPE_EMI        (*(volatile u32 *)0x040004c4)
#define reg_G3_POLYGON_ATTR   (*(volatile u32 *)0x040004a4)

static inline void G3_TexImageParam(u32 addr, u32 texFmt, u32 texGen, u32 sizeS, u32 sizeT,
                                    u32 repeat, u32 flip, u32 pltt0)
{
    reg_G3_TEXIMAGE_PARAM = (u32)((addr >> 3) | (texFmt << 26) | (texGen << 30)
        | (sizeS << 20) | (sizeT << 23) | (repeat << 16) | (flip << 18) | (pltt0 << 29));
}

void func_ov075_0211a948(char *c)
{
    unsigned int t;
    int e;

    reg_G3_DIF_AMB = *(u32 *)(c + 0x90);
    reg_G3_SPE_EMI = *(u32 *)(c + 0x94);
    G3_TexImageParam(*(u32 *)(c + 0x88), (*(u32 *)(c + 0x98) >> 0x1a) & 7, 0,
                     (*(u32 *)(c + 0x98) >> 0x14) & 7, (*(u32 *)(c + 0x98) >> 0x17) & 7,
                     0, 0, (*(u32 *)(c + 0x98) >> 0x1d) & 1);
    t = *(unsigned int *)(c + 0x98);
    e = (((t >> 0x1a) & 7) == 2) ? 1 : 0;
    reg_G3_TEXPLTT_BASE = *(unsigned int *)(c + 0x8c) >> (4 - e);
    reg_G3_POLYGON_ATTR = 0x11f0088;
}
