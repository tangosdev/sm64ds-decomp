#include "types.h"
#include "private/Ov080Mat.h"

// 6f: keep constant live / flip coloring
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

static inline void G3_TexPlttBase(u32 addr, u32 texFmt)
{
    reg_G3_TEXPLTT_BASE = addr >> (4 - (texFmt == 2 ? 1 : 0));
}

void func_ov080_02125460(char *c)
{
    struct Ov080Mat *mat = *(struct Ov080Mat **)(c + 0x1a8);
    unsigned char mode = (*(u32 *)(c + 8) >> 0xd) & 3;
    u32 difAmb = mat->difAmb;
    if (mode >= 2 || *(unsigned short *)(c + 0x1b6) == 0)
        difAmb |= 0x7fff0000;
    reg_G3_DIF_AMB = difAmb;
    reg_G3_SPE_EMI = (*(struct Ov080Mat **)(c + 0x1a8))->speEmi;
    {
        struct Ov080Mat *m2 = *(struct Ov080Mat **)(c + 0x1a8);
        G3_TexImageParam(m2->texAddr, (m2->param >> 0x1a) & 7, 0, (m2->param >> 0x14) & 7,
                         (m2->param >> 0x17) & 7, 0, 0, (m2->param >> 0x1d) & 1);
    }
    {
        struct Ov080Mat *m3 = *(struct Ov080Mat **)(c + 0x1a8);
        G3_TexPlttBase(m3->pltAddr, (m3->param >> 0x1a) & 7);
    }
    reg_G3_POLYGON_ATTR = 0x11f0088;
}
