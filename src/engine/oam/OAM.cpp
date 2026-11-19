//cpp
/* arm9/OAM -- the sprite/OAM manager. 0x02020820..0x02021a04, 13 functions.
 *
 * OAM is not polymorphic (no RTTI, no vtable, no ctor/dtor -- include/OAM.h):
 * every member is static and the two OAM staging buffers are file-scope bss,
 * so the key-function/vtable partition rules never engage. The run is
 * contiguous and exhaustive: bounded below by func_02020768 and above by
 * dPa_c::level_c::cutTable. The OamAttr field initializer
 * func_02020820 opens the run under its own address name.
 *
 * Source order is ROM-ascending under `#pragma defer_codegen off` -- the
 * deferred default would read optimization pragmas last-wins at end of file
 * and this member needs a positional bracket: the 10-arg Render keeps its
 * opt_propagation off, and the bracket restores `on` so the rest of the
 * file gets the default. opt_loop_invariants is off for the whole file.
 *
 * The bodies read and write OAM records as NitroSDK's GXOamAttr: the
 * attribute bitfields for the fields they extract, attr01 and attr2 for the
 * words they compose, and the two staging buffers are GXOamAttr arrays.
 * Shared OamAttr (4 x u16) stays the parameter type on the member
 * signatures, which the mangled names fix; each body binds GXOamAttr in one
 * cast.
 *
 * deslop leftovers:
 * - GetObjWidth/GetObjHeight read single bytes out of the stride-4
 *   slots in data_020755a0/data_020755ac, and the double cast is
 *   load-bearing.
 */

#pragma defer_codegen off
/* Render needs loop-invariant motion off (see there); every other member
 * compiles the same either way, so it is a file-wide setting. */
#pragma opt_loop_invariants off

#include "types.h"
#include "OAM.h"
#include "OamAttr.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/gxcommon.h"
#include "nitro/gx/gx.h"
#include "OamAnimation.h"
#include "nitro/os/cache.h"

#include "Matrix2x2.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/mi/memory.h"
#include "nitro/gx/gx_load.h"

extern u8  data_0209e660;    /* sub-OAM suppress flag / single-buffer select */
extern int data_0209e664;    /* main-buffer fill counter */
extern int data_0209e668;    /* main-buffer affine-slot counter */
extern int data_0209e66c;    /* sub-buffer affine-slot counter */
extern int data_0209e670;    /* sub-buffer fill counter */
extern GXOamAttr data_0209e674[]; /* main OAM staging buffer: 128 x 8B */
extern int data_0209e67c[];
extern int data_0209e694[];
extern GXOamAttr data_0209ea74[];             /* sub OAM staging buffer: 128 x 8B */
extern const unsigned char data_020755a0[12]; /* OBJ width, stride 4 */
extern const unsigned char data_020755ac[12]; /* OBJ height, stride 4 */

extern "C" {
int func_0202043c(OamAnimation *anim);
}

/* -------------------------------------------------------------------------- */

void OamAnimation::func_02020820(int *src, int screen, int loops, u8 enabled, u8 affine) {
    this->frames = (const OamAnimFrame *)src[0];
    this->frameCount = src[1];
    func_0202043c(this);
    this->palette = -1;
    this->priority = -1;
    this->scaleX = 0x1000;
    this->scaleY = 0x1000;
    this->rotation = 0;
    this->loops = loops;
    this->enabled = enabled;
    this->affine = affine;
    this->screen = screen;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- OAM::LoadAffineParams(OamAttr *, int *, Matrix2x2 *)
 * 0x02020884  size 0xe8 */

/* Write a 2x2 matrix into the four consecutive entries an affine slot spans
 * (attr3 of each, in 8.8 fixed point). Inline, not open-coded: the ROM
 * re-reads mtx->m00 here instead of reusing the load the search made, which
 * is what an inlined callee's fresh parameter does. */
static inline void SetAffineParams(OamAttr *oam, const Matrix2x2 *mtx) {
    oam[0].attr3 = mtx->m00 >> 4;
    oam[1].attr3 = mtx->m01 >> 4;
    oam[2].attr3 = mtx->m10 >> 4;
    oam[3].attr3 = mtx->m11 >> 4;
}

int OAM::LoadAffineParams(OamAttr *oam, int *count, Matrix2x2 *mtx)
{
    /* Find an existing affine slot holding this matrix, or append one, and
     * return its index. The stride of 4 is not arbitrary: an affine matrix's
     * four coefficients are interleaved across four CONSECUTIVE entries,
     * which is exactly why this function strides `i += 4` and touches only
     * attr3. */
    u16 pa = mtx->m00 >> 4;
    u16 pb = mtx->m01 >> 4;
    u16 pc = mtx->m10 >> 4;
    u16 pd = mtx->m11 >> 4;
    int i;

    for (i = 0; i < *count; i += 4) {
        if (pa == (oam + i)[0].attr3 && pb == (oam + i)[1].attr3 && pc == (oam + i)[2].attr3 &&
            pd == (oam + i)[3].attr3)
            return i >> 2;
    }
    if (*count >= 128)
        return -1;
    SetAffineParams(&oam[*count], mtx);

    int slot = *count >> 2;
    *count += 4;
    return slot;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- OAM::GetObjHeight(int, int)
 * 0x0202096c  size 0x14 */
u8 OAM::GetObjHeight(int shape, int sizeBits)
{
    /* The twin over the height table, 12 bytes past the width table -- one
     * row of four entries further on. */
    return *(unsigned char *)&((int *)((char *)data_020755ac + sizeBits))[shape];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- OAM::GetObjWidth(int, int)
 * 0x02020980  size 0x14 */
u8 OAM::GetObjWidth(int shape, int sizeBits)
{
    /* OBJ shape (square / wide / tall) and size bits together pick one of
     * twelve widths. `sizeBits` is a BYTE offset added before the `[shape]`
     * index; the entry is then read as a single byte out of a stride-4 slot. */
    return *(unsigned char *)&((int *)((char *)data_020755a0 + sizeBits))[shape];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- OAM::Render(bool, OamAttr *, int, int, int, int, int, int, int, int)
 * 0x02020994  size 0x690 */
/* Kept, measured 2026-11-16 (#821). Render is the only function in the
 * bracket, and it needs both settings: without either it is the right size
 * but 279 words off; propagation off alone 0x698, loop invariants off alone
 * 0x688. With neither, the closest spelling found (tile read as
 * attr->charNo, the sine table through FX_SinIdx/FX_CosIdx, identical
 * either way) is 0x690 with 220 words off, in three places: loop-invariant
 * motion hoists the matrix's `mtxIdx + 1` ahead of the loop, which shifts
 * every later stack slot by 4 and re-colours the rotation block's s64
 * products. Not moved: s64 assignment order and declaration order (5 + 2
 * orders, identical), mtxIdx before / after sinIdx (0x68c / identical),
 * s16 or u16 mtxIdx (0x694), `&FX_SinCosTable_[mtxIdx]` + [1] (0x684),
 * sin read before cos (218 off), Matrix2x2 scoped in the branch and
 * `for (;;)` (identical), `while (*pCount < 128)` (0x6a8). Propagation off
 * is not a TU-wide build flag: file-wide it breaks LoadAffineParams, the
 * 8-parameter Render and Reset (re-measured #821 r105 with loop invariants
 * off file-wide); optimization_level 2 file-wide breaks three functions,
 * level 3 leaves this one at 0x688. */
#pragma opt_propagation off
/* Kept (#821 r79): with loop invariants on, mtxIdx declared in the matrix block is 0x690 but 16
   words off (the sin/cos/mtx spill slots trade places with halfW/halfH); four declaration
   orders and scoped halfW/halfH stay there, all three indices scoped is 0x688, mtxIdx at the
   loop top or inline 0x698. */

void OAM::Render(bool sub, struct OamAttr *attr_, int xOff, int yOff, int palette, int priority,
                 int scaleX, int scaleY, int rotation, int mode) {
    /* The 10-parameter form: walks a chain of template entries (terminated by
     * a3 == 0xffff) and writes each into whichever OAM buffer is being filled
     * this frame, clipping against the screen; a non-unit scale or a rotation
     * goes through an affine slot from LoadAffineParams. palette / priority
     * == -1 keep the entry's own bits, mode > 0 overrides the entry's object
     * mode.
     *
     * The scalar parameters are used DIRECTLY, never copied into locals,
     * which is what reproduces the ROM's prologue register-homing order.
     * The 0x100 / 0x300 / 0 constants the prologue spills are the compiler's
     * own hoist of the bare literals below. Under the two pragmas the sine
     * table stays indexed by the hoisted sinIdx/cosIdx/mtxIdx and the tile
     * number stays a two-step shift: FX_SinIdx/FX_CosIdx in the matrix
     * measure 0x680, attr->charNo 4 words off. `attr_` is spelled OamAttr*
     * for the call-site declarations; the body views it as GXOamAttr. */
    GXOamAttr *attr = (GXOamAttr *)attr_;
    int priVal;
    int palVal;
    int *pCount;
    int *pAff;
    int sinIdx;
    int cosIdx;
    int mtxIdx;
    int halfW;
    int halfH;
    s64 tcy;
    s64 tsx;
    s64 tsy;
    s64 tcx;
    struct Matrix2x2 m;
    int y;
    int w;
    GXOamAttr *dst;
    int x;
    int om;

    if (!sub || data_0209e660 == 1) {
        pCount = &data_0209e664;
        dst = data_0209e674 + *pCount;
        pAff = &data_0209e668;
    } else {
        pCount = &data_0209e670;
        dst = data_0209ea74 + *pCount;
        pAff = &data_0209e66c;
    }

    sinIdx = ((u16)(s16)rotation >> 4) * 2;
    mtxIdx = (rotation >> 4) * 2;
    cosIdx = sinIdx + 1;

    while (1) {
        if (*pCount >= 128)
            return;

        {
            int t = attr->x;
            s8 u;
            if (t >= 0x100)
                t -= 0x200;
            x = t;
            u = attr->y;
            w = GetObjWidth(attr->shape, attr->size);
            {
                int h;
                h = GetObjHeight(attr->shape, attr->size);
                y = u;

                if (scaleX != 0x1000 || scaleY != 0x1000 || rotation != 0) {
                    x = (x + (w >> 1)) << 12;
                    y = (y + (h >> 1)) << 12;
                    halfW = w >> 1;
                    halfH = h >> 1;
                    if (scaleX != 0x1000)
                        x = FX_Div(x, scaleX);
                    if (scaleY != 0x1000)
                        y = FX_Div(y, scaleY);
                    if (rotation != 0) {
                        s16 s, c;
                        s = FX_SinCosTable_[sinIdx];
                        c = FX_SinCosTable_[cosIdx];
                        tcy = (s64)c * y;
                        tcx = (s64)c * x;
                        tsy = (s64)s * y;
                        tsx = (s64)s * x;
                        x = (((int)((tcx + 0x800) >> 12) - (int)((tsy + 0x800) >> 12)) >> 12) -
                            halfW;
                        y = (((int)((tcy + 0x800) >> 12) + (int)((tsx + 0x800) >> 12)) >> 12) -
                            halfH;
                    } else {
                        x = (x >> 12) - halfW;
                        y = (y >> 12) - halfH;
                    }
                    if (attr->rsMode != 1) {
                        w <<= 1;
                        x -= halfW;
                        h <<= 1;
                        y -= halfH;
                    }
                }

                x += xOff;
                y += yOff;

                if (rotation != 0) {
                    if (w < h)
                        w = h;
                    if (x + w < 0 || x > 0x100) {
                        if (attr->attr3 == 0xffff)
                            return;
                        attr++;
                        continue;
                    }
                    if (y + w < 0 || y > 0xc0) {
                        if (attr->attr3 == 0xffff)
                            return;
                        attr++;
                        continue;
                    }
                } else {
                    if (x + w < 0 || x > 0x100) {
                        if (attr->attr3 == 0xffff)
                            return;
                        attr++;
                        continue;
                    }
                    if (y + h < 0 || y > 0xc0) {
                        if (attr->attr3 == 0xffff)
                            return;
                        attr++;
                        continue;
                    }
                }
            }
        }
        if (palette == -1)
            palVal = attr->cParam;
        else
            palVal = palette;
        if (priority == -1)
            priVal = attr->priority;
        else
            priVal = priority;
        if (mode > 0)
            om = mode;
        else
            om = attr->objMode;

        {
            int aff;
            u32 ipVal;
            if (scaleX != 0x1000 || scaleY != 0x1000 || rotation != 0) {
                s16 c2 = FX_SinCosTable_[mtxIdx + 1];
                m.m00 = (c2 * scaleX + 0x800) >> 12;
                {
                    s16 s2 = FX_SinCosTable_[mtxIdx];
                    m.m01 = (s2 * scaleX + 0x800) >> 12;
                    m.m10 = -((s2 * scaleY + 0x800) >> 12);
                }
                m.m11 = (c2 * scaleY + 0x800) >> 12;
                if (attr->attr01 & 0x10000000) {
                    m.m00 = -m.m00;
                    m.m01 = -m.m01;
                }
                if (attr->attr01 & 0x20000000) {
                    m.m10 = -m.m10;
                    m.m11 = -m.m11;
                }
                aff = LoadAffineParams((struct OamAttr *)(dst - *pCount), pAff, &m);
                if (aff == -1) {
                    if (attr->attr3 == 0xffff)
                        return;
                    attr++;
                    continue;
                }
                if (attr->rsMode == 1)
                    ipVal = 0x100;
                else
                    ipVal = 0x300;
            } else {
                ipVal = attr->attr01 & 0x30000000;
                aff = 0;
            }
            {
                /* Kept (#821 r76): genuine; the ROM reads attr2 and attr3 as one word and GXOamAttr
                   has no word member for them. */
                u32 a23 = *(u32 *)&attr->attr2;
                u32 a01 = attr->attr01;
                u32 tile, mosaic, maskbits;
                int color;
                tile = a23 << 22;
                tile = tile >> 22;
                color = ((a01 << 18) >> 31);
                maskbits = a01 & 0xc000c000;
                mosaic = (a01 << 19) >> 31;
                if (ipVal == 0x100 || ipVal == 0x300) {
                    if (om == 3)
                        dst->attr01 =
                            ipVal |
                            ((maskbits | ((y & 0xff) | (aff << 25) | (om << 10) | (mosaic << 12))) |
                             ((x & 0x1ff) << 16));
                    else
                        dst->attr01 =
                            ipVal | ((maskbits | ((y & 0xff) | ((aff << 25) | (color << 13)) |
                                                  (om << 10) | (mosaic << 12))) |
                                     ((x & 0x1ff) << 16));
                } else if (om == 3)
                    dst->attr01 = ipVal | ((maskbits | ((y & 0xff) | (om << 10) | (mosaic << 12))) |
                                           ((x & 0x1ff) << 16));
                else
                    dst->attr01 =
                        ipVal |
                        ((maskbits | ((y & 0xff) | (color << 13) | (om << 10) | (mosaic << 12))) |
                         ((x & 0x1ff) << 16));
                dst->attr2 = (u16)((tile | (priVal << 10)) | (palVal << 12));
            }
        }
        dst++;
        *pCount = *pCount + 1;
        if (attr->attr3 == 0xffff)
            return;
        attr++;
    }
}
#pragma cplusplus on
#pragma opt_propagation on

/* -------------------------------------------------------------------------- */
/* 0x02021024  size 0x480. One object, one scale, one rotation.
 * Scale and rotation are ints. */
OamAttr *OAM::Render(bool draw, OamAttr *obj_, int px, int py, int pal, int prio, int scale0,
                     int rot0) {
    GXOamAttr *obj = (GXOamAttr *)obj_;
    int *counter;
    int *affctr;
    int cnt;
    int h;
    int idx;
    int scale;
    int rot;
    int w;
    GXOamAttr *res;
    int x;
    int y;
    Matrix2x2 mtx;
    scale = scale0;
    rot = rot0;
    if ((!draw) || (data_0209e660 == 1)) {
        counter = &data_0209e664;
        cnt = *counter;
        if (cnt >= 128) {
            return 0;
        }
        res = &data_0209e674[cnt];
        affctr = &data_0209e668;
    } else {
        counter = &data_0209e670;
        cnt = *counter;
        if (cnt >= 128) {
            return 0;
        }
        res = &data_0209ea74[cnt];
        affctr = &data_0209e66c;
    }
    x = obj->x;
    if (x >= 0x100) {
        x -= 0x200;
    }
    y = (s8)obj->y;
    w = OAM::GetObjWidth(obj->shape, obj->size);
    h = OAM::GetObjHeight(obj->shape, obj->size);
    if (rot != 0) {
        int sy;
        int sx;
        int c;
        int s;
        s = FX_SinIdx((u16)(s16)rot >> 4);
        c = FX_CosIdx((u16)(s16)rot >> 4);
        sy = y + (h >> 1);
        sx = x + (w >> 1);
        x = (((sx * c) - (sy * s)) >> 12) - (w >> 1);
        y = (((sy * c) + (sx * s)) >> 12) - (h >> 1);
    }
    if ((scale != 0x1000) || (rot != 0)) {
        if (obj->rsMode != 1) {
            x -= w >> 1;
            y -= h >> 1;
            w <<= 1;
            h <<= 1;
        }
    }
    x += px;
    y += py;
    if (rot != 0) {
        if (w < h) {
            w = h;
        }
        if (((x + w) < 0) || (x > 0x100)) {
            return 0;
        }
        if (((y + w) < 0) || (y > 0xc0)) {
            return 0;
        }
    } else {
        if (((x + w) < 0) || (x > 0x100)) {
            return 0;
        }
        if (((y + h) < 0) || (y > 0xc0)) {
            return 0;
        }
    }
    if (pal == (-1)) {
        pal = obj->cParam;
    }
    if (prio == (-1)) {
        prio = obj->priority;
    }
    {
        u32 flags;
        if ((scale != 0x1000) || (rot != 0)) {
            int s = FX_SinIdx(rot >> 4);
            int c = FX_CosIdx(rot >> 4);
            int cs = ((c * scale) + 0x800) >> 12;
            int ss = ((s * scale) + 0x800) >> 12;
            mtx.m00 = cs;
            mtx.m10 = -ss;
            mtx.m11 = cs;
            mtx.m01 = ss;
            if (obj->attr01 & 0x10000000) {
                mtx.m01 = -ss;
                mtx.m00 = -cs;
            }
            if (obj->attr01 & 0x20000000) {
                mtx.m10 = -mtx.m10;
                mtx.m11 = -mtx.m11;
            }
            idx = OAM::LoadAffineParams((OamAttr *)(res - *counter), affctr, &mtx);
            if (idx == (-1)) {
                return 0;
            }
            if (obj->rsMode == 1) {
                flags = 0x100;
            } else {
                flags = 0x300;
            }
        } else {
            idx = 0;
            flags = obj->attr01 & 0x30000000;
        }
        {
            u32 mode;
            u32 mosaic;
            u32 mask;
            u32 dep;
            u32 tiles;
            tiles = obj->charNo;
            {
                dep = obj->colorMode;
                mask = obj->attr01 & 0xc000c000;
                mosaic = obj->mosaic;
                mode = obj->objMode;
            }
            if ((flags == 0x100) || (flags == 0x300)) {
                if (mode == 3) {
                    res->attr01 =
                        flags |
                        ((mask | ((((y & 0xff) | (idx << 25)) | (mode << 10)) | (mosaic << 12))) |
                         ((x & 0x1ff) << 16));
                } else {
                    res->attr01 =
                        flags |
                        ((mask | ((((y & 0xff) | ((idx << 25) | (dep << 13))) | (mode << 10)) |
                                  (mosaic << 12))) |
                         ((x & 0x1ff) << 16));
                }
            } else if (mode == 3) {
                res->attr01 = flags | ((mask | (((y & 0xff) | (mode << 10)) | (mosaic << 12))) |
                                       ((x & 0x1ff) << 16));
            } else {
                res->attr01 =
                    flags | ((mask | (((y & 0xff) | (dep << 13) | (mode << 10)) | (mosaic << 12))) |
                             ((x & 0x1ff) << 16));
            }
            res->attr2 = (tiles | (prio << 10)) | (pal << 12);
        }
    }
    *counter += 1;
    return (OamAttr *)res;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- OAM::Render(bool, OamAttr *, int, int, int, int, Matrix2x2 *)
 * 0x020214a4  size 0x2e4 */
void OAM::Render(bool draw, OamAttr *obj_, int px, int py, int pal, int prio, Matrix2x2 *mtx) {
    /* Place one object into whichever OAM buffer is being written this frame,
     * clipping it against the screen first. `draw' is bool because the
     * mangled name says so (`Eb'); the ROM only ever tests it. */
    GXOamAttr *obj = (GXOamAttr *)obj_;
    GXOamAttr *res;
    int *counter;
    int *affctr;
    int cnt;
    int h;
    int idx;
    int w;
    int x;
    int y;

    if (!draw || data_0209e660 == 1) {
        counter = &data_0209e664;
        cnt = *counter;
        if (cnt >= 128)
            return;
        res = &data_0209e674[cnt];
        affctr = &data_0209e668;
    } else {
        counter = &data_0209e670;
        cnt = *counter;
        if (cnt >= 128)
            return;
        res = &data_0209ea74[cnt];
        affctr = &data_0209e66c;
    }

    x = obj->x;
    if (x >= 0x100)
        x -= 0x200;
    y = (s8)obj->y;
    x += px;
    y += py;
    w = GetObjWidth(obj->shape, obj->size);
    h = GetObjHeight(obj->shape, obj->size);

    if (mtx != 0) {
        if (obj->rsMode != 1) {
            x -= w >> 1;
            y -= h >> 1;
            w <<= 1;
            h <<= 1;
        }
    }

    if (x + w < 0)
        return;
    if (x > 0x100)
        return;
    if (y + h < 0)
        return;
    if (y > 0xc0)
        return;

    if (pal == -1)
        pal = obj->cParam;
    if (prio == -1)
        prio = obj->priority;

    {
        u32 flags;
        if (mtx != 0) {
            idx = LoadAffineParams((OamAttr *)(res - *counter), affctr, mtx);
            if (idx == -1)
                return;
            if (obj->rsMode == 1)
                flags = 0x100;
            else
                flags = 0x300;
        } else {
            idx = 0;
            flags = obj->attr01 & 0x30000000;
        }

        {
            u32 mode, mosaic, mask, dep, tiles;
            tiles = obj->charNo;
            dep = obj->colorMode;
            mask = obj->attr01 & 0xc000c000;
            mosaic = obj->mosaic;
            mode = obj->objMode;

            if (flags == 0x100 || flags == 0x300) {
                if (mode == 3)
                    res->attr01 =
                        flags | (mask | ((y & 0xff) | (idx << 25) | (mode << 10) | (mosaic << 12)) |
                                 ((x & 0x1ff) << 16));
                else
                    res->attr01 = flags | (mask |
                                           ((y & 0xff) | ((idx << 25) | (dep << 13)) |
                                            (mode << 10) | (mosaic << 12)) |
                                           ((x & 0x1ff) << 16));
            } else {
                if (mode == 3)
                    res->attr01 = flags | (mask | ((y & 0xff) | (mode << 10) | (mosaic << 12)) |
                                           ((x & 0x1ff) << 16));
                else
                    res->attr01 =
                        flags | (mask | ((y & 0xff) | (dep << 13) | (mode << 10) | (mosaic << 12)) |
                                 ((x & 0x1ff) << 16));
            }
            res->attr2 = tiles | (prio << 10) | (pal << 12);
        }
    }
    *counter += 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- OAM::Reset()
 * 0x02021788  size 0xdc */
void OAM::Reset() {
    int i;
    if (data_0209e660 != 0) {
        for (i = 0; i < 128; i++) {
            data_0209e674[i].attr01 = 0xc0;
            data_0209e674[i].attr2 = 0;
        }
    } else {
        data_0209e674[0].attr01 = 0xc0;
        data_0209e674[0].attr2 = 0;
        MI_CpuCopy32(data_0209e674, data_0209e67c, 0x18);
        MI_CpuCopyFast(data_0209e674, data_0209e694, 0x3e0);
        MI_CpuCopyFast(data_0209e674, data_0209ea74, 0x400);
        data_0209e670 = 0;
        data_0209e66c = 0;
    }
    data_0209e664 = 0;
    data_0209e668 = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- OAM::Flush()
 * 0x02021864  size 0x34 */
void OAM::Flush()
{
    /* Flush the two 0x400-byte staging buffers out of the data cache so the
     * DMA that follows reads what the CPU just wrote. 0x400 is 128 entries
     * of 8 bytes, the full hardware OAM. */
    DC_FlushRange(data_0209e674, 0x400);
    DC_FlushRange(data_0209ea74, 0x400);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- OAM::Load()
 * 0x02021898  size 0xb4 */
void OAM::Load()
{
    /* Push the assembled OAM buffers to the hardware. The single-buffer case
     * (data_0209e660 set) sends one buffer to the main engine and stops;
     * otherwise the two buffers swap roles with the display select (POWCNT1
     * bit 15: which engine drives the top screen), so each engine gets the
     * buffer assembled for the screen it is currently on. */
    if (data_0209e660) {
        GX_LoadOAM(data_0209e674, 0, 0x400);
        return;
    }
    if (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB) {
        GX_LoadOAM(data_0209e674, 0, 0x400);
        GXS_LoadOAM(data_0209ea74, 0, 0x400);
    } else {
        GX_LoadOAM(data_0209ea74, 0, 0x400);
        GXS_LoadOAM(data_0209e674, 0, 0x400);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- OAM::RenderSub(OamAttr *, int, int)
 * 0x0202194c  size 0x50 */
void OAM::RenderSub(OamAttr *data, s32 x, s32 y)
{
    /* The short form of the sub-screen sprite call: unscaled, unrotated, with
     * the palette and priority already in the attribute words. 0x1000 is 1.0
     * in 20.12; -1 in palette/priority/mode means "leave the attribute word
     * alone". The type is `OamAttr`, NOT `OamAttri` -- the extra `i` in
     * `P7OamAttrii` belongs to the parameter list. */
    OAM::Render(1, data, x, y, -1, -1, 0x1000, 0x1000, 0, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- OAM::RenderSub(OamAttr *, int, int, int, int)
 * 0x0202199c  size 0x54 */
void OAM::RenderSub(OamAttr *data, s32 x, s32 y, s32 palette, s32 priority)
{
    /* The long form: same as the three-argument overload but with palette and
     * priority given explicitly rather than left as -1. Everything else is
     * still the identity transform -- 0x1000 scale in both axes (1.0 in
     * 20.12), no rotation, mode -1. */
    OAM::Render(1, data, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- OAM::EnableSubOAM(OamTmp *)
 * 0x020219f0  size 0x14 */
u32 OAM::EnableSubOAM(OamTmp *tmp) {
    /* Despite the name it only writes zero -- the flag is "sub OAM
     * suppressed", so enabling is clearing it. tmp is the caller's stack
     * slot; this body does not read it. The u32 return is a fiction
     * the ROM does not honour: the function falls off its end without
     * setting r0. The `(long) 0` cast is preserved verbatim. */
    data_0209e660 = 0;
}

