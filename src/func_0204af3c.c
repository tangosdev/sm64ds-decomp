#pragma opt_propagation off

typedef unsigned int u32;
typedef unsigned short u16;
typedef int s32;
typedef s32 fx32;

#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE   (*(volatile u32 *)0x040004ac)
#define reg_G3_MTX_MODE       (*(volatile u32 *)0x04000440)
#define reg_G3_IDENTITY       (*(volatile u32 *)0x04000454)
#define reg_G3_SCALE          (*(volatile u32 *)0x0400046c)

static inline void G3_MtxMode(int mode)
{
    reg_G3_MTX_MODE = (u32)mode;
}

static inline void G3_Identity(void)
{
    reg_G3_IDENTITY = 0;
}

static inline void G3_Scale(fx32 x, fx32 y, fx32 z)
{
    reg_G3_SCALE = (u32)x;
    reg_G3_SCALE = (u32)y;
    reg_G3_SCALE = (u32)z;
}

static inline void G3_TexPlttBase(u32 addr, u32 texFmt)
{
    reg_G3_TEXPLTT_BASE = (u32)(addr >> (4 - (texFmt == 2)));
}

static inline void G3_TexImageParam(u32 addr, u32 texFmt, u32 texGen, u32 sizeS, u32 sizeT,
                                    u32 repeat, u32 flip, u32 pltt0)
{
    reg_G3_TEXIMAGE_PARAM = (u32)((addr >> 3) | (texFmt << 26) | (texGen << 30)
        | (sizeS << 20) | (sizeT << 23) | (repeat << 16) | (flip << 18) | (pltt0 << 29));
}

struct TexParam
{
    u32 fmt : 4;
    u32 sizeS : 4;
    u32 sizeT : 4;
    u32 repeat : 2;
    u32 flip : 2;
    u32 color0 : 1;
};

struct Tex
{
    int f0;
    u32 texAddr;
    u32 pltAddr;
    struct TexParam param;
    u16 width;
    u16 height;
};

void func_0204af3c(struct Tex *self)
{
    struct TexParam p = self->param;
    G3_TexImageParam(self->texAddr, p.fmt, 1, p.sizeS, p.sizeT, p.repeat, p.flip, p.color0);
    G3_TexPlttBase(self->pltAddr, p.fmt);
    G3_MtxMode(3);
    G3_Identity();
    G3_Scale(self->width << 12, self->height << 12, 0);
    G3_MtxMode(1);
}
