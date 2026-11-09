#include "types.h"
#include "Ov007Sprite.h"
#include "Ov007Texture.h"
#include "nitro/fx/fx.h"
extern void func_ov007_020c1448(Ov007Texture *self, u32 texGen, u32 repeat, u32 flip, u32 color0);
extern void Texture_BindPalette(Ov007Texture *tex);
extern void func_ov007_020c49bc(int a0, int a1, int a2, int a3, int a4, int a5, u16 *a6, int a7);
extern int data_ov007_0210325c;
#define reg_G3_COLOR        (*(volatile u32 *)0x04000480)
#define reg_G3_POLYGON_ATTR (*(volatile u32 *)0x040004a4)
#define reg_G3_MTX_PUSH     (*(volatile u32 *)0x04000444)
#define reg_G3_MTX_POP      (*(volatile u32 *)0x04000448)
#define reg_G3_MTX_SCALE    (*(volatile u32 *)0x0400046c)
#define reg_G3_MTX_TRANS    (*(volatile u32 *)0x04000470)
#define reg_G3_TEXCOORD     (*(volatile u32 *)0x04000488)
#define reg_G3_VTX_16       (*(volatile u32 *)0x0400048c)
#define reg_G3_VTX_XZ       (*(volatile u32 *)0x04000498)
#define reg_G3_VTX_YZ       (*(volatile u32 *)0x0400049c)
#define reg_G3_BEGIN_VTXS   (*(volatile u32 *)0x04000500)
#define reg_G3_END_VTXS     (*(volatile u32 *)0x04000504)

/* NitroSDK G3 immediate-mode commands (g3imm.h). Their arguments are
 * evaluated right to left, which is the load order the ROM shows for the
 * texcoord and translate operands. File-local because adding them to
 * include/nitro/gx/g3imm.h renumbers ov075/d_s_entry_entry's pinned @NNN. */
static inline void G3_Color(u16 rgb) {
    reg_G3_COLOR = rgb;
}
static inline void G3_PushMtx(void) {
    reg_G3_MTX_PUSH = 0;
}
static inline void G3_PopMtx(int num) {
    reg_G3_MTX_POP = num;
}
static inline void G3_Translate(fx32 x, fx32 y, fx32 z) {
    reg_G3_MTX_TRANS = x;
    reg_G3_MTX_TRANS = y;
    reg_G3_MTX_TRANS = z;
}
static inline void G3_Scale(fx32 x, fx32 y, fx32 z) {
    reg_G3_MTX_SCALE = x;
    reg_G3_MTX_SCALE = y;
    reg_G3_MTX_SCALE = z;
}
static inline void G3_Begin(int primitive) {
    reg_G3_BEGIN_VTXS = primitive;
}
static inline void G3_End(void) {
    reg_G3_END_VTXS = 0;
}
static inline void G3_TexCoord(fx32 s, fx32 t) {
    reg_G3_TEXCOORD = (u16)(fx16)(s >> 8) | ((u16)(fx16)(t >> 8) << 16);
}
static inline void G3_Vtx(fx16 x, fx16 y, fx16 z) {
    reg_G3_VTX_16 = (u16)x | ((u16)y << 16);
    reg_G3_VTX_16 = (u16)z;
}
static inline void G3_VtxXZ(fx16 x, fx16 z) {
    reg_G3_VTX_XZ = (u16)x | ((u16)z << 16);
}
static inline void G3_VtxYZ(fx16 y, fx16 z) {
    reg_G3_VTX_YZ = (u16)y | ((u16)z << 16);
}

/* Draws the sprite's textured quad in `color` at `alpha` (0-31) with
 * polygon id `polygonId`, offset by `vec`; `flags` is the sprite's
 * drawFlags for the plain pass (func_ov007_020c421c passes 0 for its
 * shadow pass). */
void func_ov007_020c4684(Ov007Sprite *s, u16 color, int alpha, int polygonId, int flags, s16 *vec) {
    int tw = 0;
    u16 *tilemap;
    u16 repeatU = s->repeatU;
    u16 repeatV = s->repeatV;
    int s0 = s->texS << 12;
    int th = 0;
    int t0 = s->texT << 12;
    Ov007Texture *tex;
    int texW;
    int w;
    int h;
    int useColors;

    if (flags & 8) {
        tilemap = s->tilemap;
    } else {
        tilemap = 0;
    }
    if (alpha == 0) {
        return;
    }
    tex = s->frames[s->frame];
    G3_Color(color);
    if (tex != 0) {
        tw = 8 << tex->sizeS;
        th = 8 << tex->sizeT;
    }
    func_ov007_020c1448(tex, 1, s->texRepeat, s->texFlip, s->texColor0);
    Texture_BindPalette(tex);
    texW = tw;
    reg_G3_POLYGON_ATTR = s->polygonAttr | 0x80 | (polygonId << 24) | (alpha << 16);
    if (tex != 0) {
        if (repeatU == 1) {
            tw = s->width;
        }
        if (repeatV == 1) {
            th = s->height;
        }
    }
    G3_PushMtx();
    w = s->worldWidth;
    h = s->worldHeight;
    G3_Translate(vec[0] + -w / 2, vec[1] + -h / 2, vec[2]);
    G3_Scale(FX_MUL(w, data_ov007_0210325c), FX_MUL(h, data_ov007_0210325c), 0x1000);
    if (((flags & 1) == 0) && ((flags & 2) == 0)) {
        func_ov007_020c49bc(tw, th, s0, t0, repeatU, repeatV, tilemap, texW);
    } else {
        useColors = s->drawFlags & 2;
        G3_Begin(1);
        if (useColors) {
            G3_Color(s->cornerColors[0]);
        }
        G3_TexCoord(s0, t0);
        G3_Vtx(0, 0x1000, 0);
        if (useColors) {
            G3_Color(s->cornerColors[1]);
        }
        G3_TexCoord(s0, (th << 12) * repeatV + t0);
        G3_VtxYZ(0, 0);
        if (useColors) {
            G3_Color(s->cornerColors[2]);
        }
        G3_TexCoord((tw << 12) * repeatU + s0, (th << 12) * repeatV + t0);
        G3_VtxXZ(0x1000, 0);
        if (useColors) {
            G3_Color(s->cornerColors[3]);
        }
        G3_TexCoord((tw << 12) * repeatU + s0, t0);
        G3_VtxYZ(0x1000, 0);
        G3_End();
    }
    G3_PopMtx(1);
}
