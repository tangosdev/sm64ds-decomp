#include "types.h"
#include "Ov007Sprite.h"
#include "Ov007Texture.h"
#include "nitro/fx/fx.h"
extern void func_ov007_020c1448(Ov007Texture *self, u32 texGen, u32 repeat, u32 flip, u32 color0);
extern void Texture_BindPalette(Ov007Texture *tex);
extern void func_ov007_020c49bc(int width, int height, fx32 s0, fx32 t0, int repeatU, int repeatV,
                                u16 *tilemap, int texWidth);
extern int data_ov007_0210325c;
#include "nitro/gx/g3imm_cmd.h"

/* The NitroSDK immediate-mode commands evaluate their arguments right to
 * left, which is the load order the ROM shows for the texcoord and translate
 * operands. */

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
    REG_POLYGON_ATTR = s->polygonAttr | 0x80 | (polygonId << 24) | (alpha << 16);
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
        G3_Begin(GX_BEGIN_QUADS);
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
