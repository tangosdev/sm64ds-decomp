#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/g3imm.h"

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

/* Bind a particle texture record: set its image and palette (texture
   coordinates from the texcoord command, texGen 1), then load a texture
   matrix that scales by its width and height. func_0204a5c8 binds the system's
   texture with it, and each particle's own when the texture animates. */
void func_0204af3c(struct Tex *self)
{
    struct TexParam p = self->param;
    G3_TexImageParam(p.fmt, GX_TEXGEN_TEXCOORD, p.sizeS, p.sizeT, p.repeat, p.flip, p.color0,
                     self->texAddr);
    G3_TexPlttBase(self->pltAddr, p.fmt);
    G3_MtxMode(3);
    G3_Identity();
    G3_Scale(self->width << 12, self->height << 12, 0);
    G3_MtxMode(1);
}
