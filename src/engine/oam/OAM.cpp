//cpp
/* arm9/OAM -- the sprite/OAM manager. 0x02020820..0x02021a04, 13 functions.
 *
 * OAM is not polymorphic (no RTTI, no vtable, no ctor/dtor -- include/OAM.h):
 * every member is static and the two OAM staging buffers are file-scope bss,
 * so the key-function/vtable partition rules never engage. The run is
 * contiguous and exhaustive: bounded below by func_02020768 and above by
 * Particle::SysTracker::Contents::Unlink. The OamAttr field initializer
 * func_02020820 opens the run under its own address name.
 *
 * Two Render overloads stay `extern "C"` free functions -- the by-value
 * Fix12<int> scale/rotation parameters are on the by-value-class wall
 * (OAM.h's note: mwccarm homes r0-r3 to the stack for them, +0x14).
 *
 * Source order is ROM-ascending under `#pragma defer_codegen off` -- the
 * deferred default would read optimization pragmas last-wins at end of file
 * and these members need positional brackets: the 10-arg Render keeps its
 * opt_common_subs/opt_propagation/opt_loop_invariants offs, LoadAffineParams
 * keeps opt_strength_reduction/opt_common_subs off, and both brackets restore
 * `on` so the rest of the file gets defaults.
 *
 * Four local decompositions of the same 8-byte attribute entry survive
 * because each function's bit reads were matched against its own view:
 * OamAttrEntry (the hardware-entry bitfields Render writes), OamAttrTpl (the
 * sprite-template bitfields the 7-arg Render walks), OamAttrRaw (the
 * a01/a2/a3 word view the 10-arg Render extracts with shifts) and OAMEntry
 * (the a/b pair Reset stores). Shared OamAttr (4 x u16) stays the parameter
 * type on the member signatures; each body binds its view in one cast.
 *
 * deslop leftovers:
 * - The two Fix12 Render overloads keep mangled `extern "C"` spellings
 *   (wall above); RenderSub's call sites keep them as one-declared
 *   callee.
 * - data_020755a0/data_020755ac stay `unsigned char[]` table externs --
 *   GetObjWidth/GetObjHeight read single bytes out of stride-4 slots and
 *   the double cast is load-bearing.
 */

#pragma defer_codegen off

#include "types.h"
#include "OAM.h"
#include "OamAttr.h"
#include "OamAnimation.h"
#include "CP15.h"
#include "cstd.h"

struct Matrix2x2 { s32 m00, m01, m10, m11; };

/* The hardware entry as Render(Matrix2x2*) decomposes it. */
struct OamAttrEntry {
    u32 yb : 8, objMode : 2, mode : 2, mosaic : 1, dep : 1, shape : 2, xc : 9, aff : 5, size : 2;
    u32 tile : 10, prio : 2, pal : 4, hi2 : 16;
};
#define ATTR01(o) (*(u32 *)(o))

/* The sprite-template entry as the 7-arg Render decomposes it. */
struct OamAttrTpl {
    u32 yb : 8;
    u32 rsFlags : 2;
    u32 objMode : 2;
    u32 mosaic : 1;
    u32 colorDepth : 1;
    u32 shape : 2;
    u32 xc : 9;
    u32 hi : 7;
    u32 tile : 10;
    u32 prio : 2;
    u32 pal : 4;
    u32 hi2 : 16;
};

/* The raw-word entry as the 10-arg Render walks it (a3 == 0xffff terminates
 * the template list). OamEntryRaw is the same shape for the staging buffers. */
struct OamAttrRaw { u32 a01; u16 a2; u16 a3; };
struct OamEntryRaw { u32 a01; u16 a2; u16 pad; };

/* The (attr0, attr1) pair as Reset stores it. */
struct OAMEntry { u32 a; u16 b; u16 pad; };

extern u8  data_0209e660;    /* sub-OAM suppress flag / single-buffer select */
extern int data_0209e664;    /* main-buffer fill counter */
extern int data_0209e668;    /* main-buffer affine-slot counter */
extern int data_0209e66c;    /* sub-buffer affine-slot counter */
extern int data_0209e670;    /* sub-buffer fill counter */
extern OamAttr data_0209e674[];  /* main OAM staging buffer: 128 x 8B */
extern int data_0209e67c[];
extern int data_0209e694[];
extern OamAttr data_0209ea74[];  /* sub OAM staging buffer: 128 x 8B */
extern s16 data_02082214[];      /* sin/cos table, indexed by (angle >> 4) * 2 */
extern unsigned char data_020755a0[];  /* OBJ width table, stride 4 */
extern unsigned char data_020755ac[];  /* OBJ height table, stride 4 */

extern "C" {
void func_020566dc(const void *src, unsigned int offset, unsigned int count);
void func_02056674(const void *src, unsigned int offset, unsigned int count);
void MultiCopy_Int(int *dst, int *src, int len);
void MultiCopy32Bytes(int *src, int *dst, int len);

int func_0202043c(int thiz);
}

/* -------------------------------------------------------------------------- */
extern "C" {

/* @symbol func_02020820 */
void func_02020820(char *self, int *src, int arg2, int arg3, u8 arg5, u8 arg6)
{
    OamAnimation *anim = (OamAnimation *)self;
    anim->frames = (const void *)src[0];
    anim->frameCount = src[1];
    func_0202043c((int)self);
    anim->palette = -1;
    anim->priority = -1;
    anim->scaleX = 0x1000;
    anim->scaleY = 0x1000;
    anim->rotation = 0;
    anim->loops = (u8)arg3;
    anim->enabled = (u8)arg5;
    anim->affine = (u8)arg6;
    /* 0x28: no reader in the tree yet, so its role is unproven. */
    *(int *)(self + 0x28) = arg2;
}

}

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2
/* ROM ordinal 1 -- OAM::LoadAffineParams(OamAttr *, int *, Matrix2x2 *)
 * 0x02020884  size 0xe8 */
#pragma opt_strength_reduction off
#pragma opt_common_subs off
int OAM::LoadAffineParams(OamAttr *oam, int *count, Matrix2x2 *mtx)
{
    /* Find an existing affine slot holding this matrix, or append one, and
     * return its index. The stride of 4 is not arbitrary: an affine matrix's
     * four coefficients are interleaved across four CONSECUTIVE entries,
     * which is exactly why this function strides `i += 4` and touches only
     * attr3. */
    int *m = (int *)mtx;
    unsigned int k0 = (unsigned short)((unsigned int)m[0] >> 4);
    unsigned int k1 = (unsigned short)((unsigned int)m[1] >> 4);
    unsigned int k2 = (unsigned short)((unsigned int)m[2] >> 4);
    unsigned int k3 = (unsigned short)((unsigned int)m[3] >> 4);
    int n = *count;
    int i;

    for (i = 0; i < n; i += 4) {
        OamAttr *e = &oam[i];
        if (k0 == e[0].attr3 && k1 == e[1].attr3 &&
            k2 == e[2].attr3 && k3 == e[3].attr3)
            return i >> 2;
    }
    if (n >= 0x80)
        return -1;
    {
        OamAttr *e = &oam[n];
        e[0].attr3 = m[0] >> 4;
        e[1].attr3 = m[1] >> 4;
        e[2].attr3 = m[2] >> 4;
        e[3].attr3 = m[3] >> 4;
    }
    {
        int t = *count;
        *count = t + 4;
        return t >> 2;
    }
}
#pragma opt_strength_reduction on
#pragma opt_common_subs on

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM12GetObjHeightEii
/* ROM ordinal 2 -- OAM::GetObjHeight(int, int)
 * 0x0202096c  size 0x14 */
u8 OAM::GetObjHeight(int shape, int sizeBits)
{
    /* The twin over the height table, 12 bytes past the width table -- one
     * row of four entries further on. */
    return *(unsigned char *)&((int *)((char *)data_020755ac + sizeBits))[shape];
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM11GetObjWidthEii
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
// @symbol _ZN3OAM6RenderEbP7OamAttriiiiiiii
/* ROM ordinal 4 -- OAM::Render(bool, OamAttr *, int, int, int, int, int, int, int, int)
 * 0x02020994  size 0x690 */
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off

void OAM::Render(bool sub, struct OamAttr *attr_, int xOff, int yOff, int palette, int priority,
                 int scaleX, int scaleY, int rotation, int mode) {
    /* The 10-parameter form: walks a chain of template entries (terminated by
     * a3 == 0xffff) and writes each into whichever OAM buffer is being filled
     * this frame, clipping against the screen; a non-unit scale or a rotation
     * goes through an affine slot from LoadAffineParams. palette / priority
     * == -1 keep the entry's own bits, mode > 0 overrides the entry's object
     * mode. scaleX/scaleY are Fix12<int> by value in the real signature --
     * the literal-name spelling is the by-value-class wall again.
     *
     * The three pragma offs above are load-bearing: the scalar parameters
     * are used DIRECTLY, never copied into locals, which is what reproduces
     * the ROM's prologue register-homing order. `attr_` is spelled OamAttr*
     * for the call-site declarations; the body views it through OamAttrRaw. */
    struct OamAttrRaw *attr = (struct OamAttrRaw *)attr_;
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
    int c100;
    int c300;
    int zero;
    struct Matrix2x2 m;
    int y;
    int w;
    struct OamEntryRaw *dst;
    int x;
    int om;


    if (!sub || data_0209e660 == 1) {
        pCount = &data_0209e664;
        dst = (struct OamEntryRaw *)data_0209e674 + *pCount;
        pAff = &data_0209e668;
    } else {
        pCount = &data_0209e670;
        dst = (struct OamEntryRaw *)data_0209ea74 + *pCount;
        pAff = &data_0209e66c;
    }

    sinIdx = ((u16)(s16)rotation >> 4) * 2;
    mtxIdx = (rotation >> 4) * 2;
    cosIdx = sinIdx + 1;
    zero = 0;
    c300 = 0x300;
    c100 = 0x100;

    while (1) {
        if (*pCount >= 0x80)
            return;

        {
            int t = (int)((attr->a01 << 7) >> 23);
            s8 u;
            if (t >= 0x100)
                t -= 0x200;
            x = t;
            u = *(s8 *)attr;
            {
            u32 a = attr->a01;
            w = GetObjWidth((int)((a << 16) >> 30), (int)(a >> 30));
            }
        {
        int h;
        {
            u32 a = attr->a01;
            h = GetObjHeight((int)((a << 16) >> 30), (int)(a >> 30));
        }
        y = u;

        if (scaleX != 0x1000 || scaleY != 0x1000 || rotation != 0) {
            x = (x + (w >> 1)) << 12;
            y = (y + (h >> 1)) << 12;
            halfW = w >> 1;
            halfH = h >> 1;
            if (scaleX != 0x1000)
                x = cstd::fdiv(x, scaleX);
            if (scaleY != 0x1000)
                y = cstd::fdiv(y, scaleY);
            if (rotation != 0) {
                s16 s, c;
                s = data_02082214[sinIdx];
                c = data_02082214[cosIdx];
                tcy = (s64)c * y;
                tcx = (s64)c * x;
                tsy = (s64)s * y;
                tsx = (s64)s * x;
                x = (((int)((tcx + 0x800) >> 12) - (int)((tsy + 0x800) >> 12)) >> 12) - halfW;
                y = (((int)((tcy + 0x800) >> 12) + (int)((tsx + 0x800) >> 12)) >> 12) - halfH;
            } else {
                x = (x >> 12) - halfW;
                y = (y >> 12) - halfH;
            }
            if (((attr->a01 << 22) >> 30) != 1) {
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
                if (attr->a3 == 0xffff) return;
                attr++;
                continue;
            }
            if (y + w < 0 || y > 0xc0) {
                if (attr->a3 == 0xffff) return;
                attr++;
                continue;
            }
        } else {
            if (x + w < 0 || x > 0x100) {
                if (attr->a3 == 0xffff) return;
                attr++;
                continue;
            }
            if (y + h < 0 || y > 0xc0) {
                if (attr->a3 == 0xffff) return;
                attr++;
                continue;
            }
        }

        }
        }
        if (palette == -1)
            palVal = (int)((*(u32 *)&attr->a2 << 16) >> 28);
        else
            palVal = palette;
        if (priority == -1)
            priVal = (int)((*(u32 *)&attr->a2 << 20) >> 30);
        else
            priVal = priority;
        if (mode > 0)
            om = mode;
        else
            om = (int)((attr->a01 << 20) >> 30);

        {
            int aff;
            u32 ipVal;
            if (scaleX != 0x1000 || scaleY != 0x1000 || rotation != 0) {
                s16 c2 = data_02082214[mtxIdx + 1];
                m.m00 = (c2 * scaleX + 0x800) >> 12;
                {
                s16 s2 = data_02082214[mtxIdx];
                m.m01 = (s2 * scaleX + 0x800) >> 12;
                m.m10 = -((s2 * scaleY + 0x800) >> 12);
                }
                m.m11 = (c2 * scaleY + 0x800) >> 12;
                if (attr->a01 & 0x10000000) {
                    m.m00 = -m.m00;
                    m.m01 = -m.m01;
                }
                if (attr->a01 & 0x20000000) {
                    m.m10 = -m.m10;
                    m.m11 = -m.m11;
                }
                aff = LoadAffineParams((struct OamAttr *)(dst - *pCount), pAff, &m);
                if (aff == -1) {
                    if (attr->a3 == 0xffff) return;
                    attr++;
                    continue;
                }
                ipVal = (((attr->a01 << 22) >> 30) == 1) ? *(int *)&c100 : *(int *)&c300;
            } else {
                ipVal = attr->a01 & 0x30000000;
                aff = *(int *)&zero;
            }
            {
                u32 a23 = *(u32 *)&attr->a2;
                u32 a01 = attr->a01;
                u32 tile, bit13, maskbits;
                tile = a23 << 22;
                tile = tile >> 22;
                w = (int)((a01 << 18) >> 31);
                maskbits = a01 & 0xc000c000;
                bit13 = (a01 << 19) >> 31;
                if (ipVal == 0x100 || ipVal == 0x300) {
                    if (om == 3)
                        dst->a01 = ipVal | ((maskbits | ((y & 0xff) | (aff << 25) | (om << 10) | (bit13 << 12))) | ((x & 0x1ff) << 16));
                    else
                        dst->a01 = ipVal | ((maskbits | ((y & 0xff) | ((aff << 25) | (w << 13)) | (om << 10) | (bit13 << 12))) | ((x & 0x1ff) << 16));
                } else if (om == 3)
                    dst->a01 = ipVal | ((maskbits | ((y & 0xff) | (om << 10) | (bit13 << 12))) | ((x & 0x1ff) << 16));
                else
                    dst->a01 = ipVal | ((maskbits | ((y & 0xff) | (w << 13) | (om << 10) | (bit13 << 12))) | ((x & 0x1ff) << 16));
                dst->a2 = (u16)((tile | (priVal << 10)) | (palVal << 12));
            }
        }
        dst++;
        *pCount = *pCount + 1;
        if (attr->a3 == 0xffff) return;
        attr++;
    }
}
#pragma cplusplus on
#pragma opt_common_subs on
#pragma opt_propagation on
#pragma opt_loop_invariants on

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM6RenderEbP7OamAttriiiiii
/* 0x02021024  size 0x480. One object, one scale, one rotation.
 * Scale and rotation are ints. `OamAttrTpl` is this body's view of the template entry. */
OamAttr *OAM::Render(bool draw, OamAttr *obj_, int px, int py, int pal, int prio, int scale0,
                     int rot0) {
    OamAttrTpl *obj = (OamAttrTpl *)obj_;
    int *counter;
    int *affctr;
    int cnt;
    int h;
    int idx;
    int scale;
    int rot;
    int w;
    OamAttrTpl *res;
    int x;
    int y;
    OamAttrTpl *new_var;
    Matrix2x2 mtx;
    scale = scale0;
    rot = rot0;
    if ((!draw) || (data_0209e660 == 1))
    {
      counter = &data_0209e664;
      cnt = *counter;
      if (cnt >= 0x80)
      {
        return 0;
      }
      res = (OamAttrTpl *)&data_0209e674[cnt];
      affctr = &data_0209e668;
    }
    else
    {
      counter = &data_0209e670;
      cnt = *counter;
      if (cnt >= 0x80)
      {
        return 0;
      }
      res = (OamAttrTpl *)&data_0209ea74[cnt];
      affctr = &data_0209e66c;
    }
    x = obj->xc;
    if (x >= 0x100)
    {
      x -= 0x200;
    }
    y = *((s8 *) obj);
    w = OAM::GetObjWidth(((*((u32 *) obj)) << 16) >> 30, (*((u32 *) obj)) >> 30);
    h = OAM::GetObjHeight(((*((u32 *) obj)) << 16) >> 30, (*((u32 *) obj)) >> 30);
    if (rot != 0)
    {
      int sy;
      int sx;
      int c;
      int s;
      int i;
      i = (((u16) ((s16) rot)) >> 4) * 2;
      s = data_02082214[i];
      c = data_02082214[i + 1];
      sy = y + (h >> 1);
      sx = x + (w >> 1);
      x = (((sx * c) - (sy * s)) >> 12) - (w >> 1);
      y = (((sy * c) + (sx * s)) >> 12) - (h >> 1);
    }
    if ((scale != 0x1000) || (rot != 0))
    {
      if (obj->rsFlags != 1)
      {
        x -= w >> 1;
        y -= h >> 1;
        w <<= 1;
        h <<= 1;
      }
    }
    x += px;
    y += py;
    if (rot != 0)
    {
      if (w < h)
      {
        w = h;
      }
      if (((x + w) < 0) || (x > 0x100))
      {
        return 0;
      }
      if (((y + w) < 0) || (y > 0xc0))
      {
        return 0;
      }
    }
    else
    {
      if (((x + w) < 0) || (x > 0x100))
      {
        return 0;
      }
      if (((y + h) < 0) || (y > 0xc0))
      {
        return 0;
      }
    }
    if (pal == (-1))
    {
      pal = obj->pal;
    }
    if (prio == (-1))
    {
      prio = obj->prio;
    }
    {
      u32 flags;
      if ((scale != 0x1000) || (rot != 0))
      {
        int i = (rot >> 4) * 2;
        int s = data_02082214[i];
        int c = data_02082214[i + 1];
        int cs = ((c * scale) + 0x800) >> 12;
        int ss = ((s * scale) + 0x800) >> 12;
        mtx.m00 = cs;
        mtx.m10 = -ss;
        mtx.m11 = cs;
        mtx.m01 = ss;
        if ((*((u32 *) obj)) & 0x10000000)
        {
          mtx.m01 = -ss;
          mtx.m00 = -cs;
        }
        if ((*((u32 *) obj)) & 0x20000000)
        {
          mtx.m10 = -mtx.m10;
          mtx.m11 = -mtx.m11;
        }
        new_var = res - (*counter);
        idx = OAM::LoadAffineParams((OamAttr *)new_var, affctr, &mtx);
        if (idx == (-1))
        {
          return 0;
        }
        if (obj->rsFlags == 1)
        {
          flags = 0x100;
        }
        else
        {
          flags = 0x300;
        }
      }
      else
      {
        idx = 0;
        flags = (*((u32 *) obj)) & 0x30000000;
      }
      {
        u32 mode;
        u32 mosaic;
        u32 mask;
        u32 dep;
        u32 tiles;
        tiles = obj->tile;
        {
          dep = obj->colorDepth;
          mask = (*((u32 *) obj)) & 0xc000c000;
          mosaic = obj->mosaic;
          mode = obj->objMode;
        }
        if ((flags == 0x100) || (flags == 0x300))
        {
          if (mode == 3)
          {
            *((u32 *) res) = flags | ((mask | ((((y & 0xff) | (idx << 25)) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
          }
          else
          {
            *((u32 *) res) = flags | ((mask | ((((y & 0xff) | ((idx << 25) | (dep << 13))) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
          }
        }
        else
          if (mode == 3)
        {
          *((u32 *) res) = flags | ((mask | (((y & 0xff) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
        }
        else
        {
          *((u32 *) res) = flags | ((mask | (((y & 0xff) | (dep << 13) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
        }
        ((OamAttr *)res)->attr2 = (tiles | (prio << 10)) | (pal << 12);
      }
    }
    *counter += 1;
    return (OamAttr *)res;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2
/* ROM ordinal 6 -- OAM::Render(bool, OamAttr *, int, int, int, int, Matrix2x2 *)
 * 0x020214a4  size 0x2e4 */
void OAM::Render(bool draw, OamAttr *obj_, int px, int py, int pal, int prio, Matrix2x2 *mtx)
{
    /* Place one object into whichever OAM buffer is being written this frame,
     * clipping it against the screen first. `draw' is bool because the
     * mangled name says so (`Eb'); the ROM only ever tests it. */
    OamAttrEntry *obj = (OamAttrEntry *)obj_;
    OamAttrEntry *res;
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
        if (cnt >= 0x80) return;
        res = (OamAttrEntry *)&data_0209e674[cnt];
        affctr = &data_0209e668;
    } else {
        counter = &data_0209e670;
        cnt = *counter;
        if (cnt >= 0x80) return;
        res = (OamAttrEntry *)&data_0209ea74[cnt];
        affctr = &data_0209e66c;
    }

    x = obj->xc;
    if (x >= 0x100) x -= 0x200;
    y = *(s8 *)obj;
    x += px;
    y += py;
    w = GetObjWidth((ATTR01(obj) << 16) >> 30, ATTR01(obj) >> 30);
    h = GetObjHeight((ATTR01(obj) << 16) >> 30, ATTR01(obj) >> 30);

    if (mtx != 0) {
        if (obj->objMode != 1) {
            x -= w >> 1;
            y -= h >> 1;
            w <<= 1;
            h <<= 1;
        }
    }

    if (x + w < 0) return;
    if (x > 0x100) return;
    if (y + h < 0) return;
    if (y > 0xc0) return;

    if (pal == -1)
        pal = obj->pal;
    if (prio == -1)
        prio = obj->prio;

    {
    u32 flags;
    if (mtx != 0) {
        idx = LoadAffineParams((OamAttr *)(res - *counter), affctr, mtx);
        if (idx == -1) return;
        if (obj->objMode == 1)
            flags = 0x100;
        else
            flags = 0x300;
    } else {
        idx = 0;
        flags = ATTR01(obj) & 0x30000000;
    }

    {
        u32 mode, mosaic, mask, dep, tiles;
        tiles = obj->tile;
        dep = obj->dep;
        mask = ATTR01(obj) & 0xc000c000;
        mosaic = obj->mosaic;
        mode = obj->mode;

        if (flags == 0x100 || flags == 0x300) {
            if (mode == 3)
                ATTR01(res) = flags | (mask | ((y & 0xff) | (idx << 25) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
            else
                ATTR01(res) = flags | (mask | ((y & 0xff) | ((idx << 25) | (dep << 13)) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
        } else {
            if (mode == 3)
                ATTR01(res) = flags | (mask | ((y & 0xff) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
            else
                ATTR01(res) = flags | (mask | ((y & 0xff) | (dep << 13) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
        }
        ((OamAttr *)res)->attr2 = tiles | (prio << 10) | (pal << 12);
    }
    }
    *counter += 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM5ResetEv
/* ROM ordinal 7 -- OAM::Reset()
 * 0x02021788  size 0xdc */
void OAM::Reset()
{
    int i;
    if (data_0209e660 != 0) {
        for (i = 0; i < 0x80; i++) {
            ((OAMEntry *)data_0209e674)[i].a = 0xc0;
            ((OAMEntry *)data_0209e674)[i].b = 0;
        }
    } else {
        ((OAMEntry *)data_0209e674)[0].a = 0xc0;
        ((OAMEntry *)data_0209e674)[0].b = 0;
        MultiCopy_Int((int*)data_0209e674, data_0209e67c, 0x18);
        MultiCopy32Bytes((int*)data_0209e674, data_0209e694, 0x3e0);
        MultiCopy32Bytes((int*)data_0209e674, (int*)data_0209ea74, 0x400);
        data_0209e670 = 0;
        data_0209e66c = 0;
    }
    data_0209e664 = 0;
    data_0209e668 = 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM5FlushEv
/* ROM ordinal 8 -- OAM::Flush()
 * 0x02021864  size 0x34 */
void OAM::Flush()
{
    /* Flush the two 0x400-byte staging buffers out of the data cache so the
     * DMA that follows reads what the CPU just wrote. 0x400 is 128 entries
     * of 8 bytes, the full hardware OAM. */
    CP15::FlushAndInvalidateDataCache((u32)data_0209e674, 0x400);
    CP15::FlushAndInvalidateDataCache((u32)data_0209ea74, 0x400);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM4LoadEv
/* ROM ordinal 9 -- OAM::Load()
 * 0x02021898  size 0xb4 */
void OAM::Load()
{
    /* Push the assembled OAM buffers to the hardware. The single-buffer case
     * (data_0209e660 set) sends one buffer to the main engine and stops;
     * otherwise the two buffers swap roles per frame, selected by bit 15 of
     * DISPCAPCNT at 0x04000304, so the one being displayed is never the one
     * being written. */
    if (data_0209e660) {
        func_020566dc(data_0209e674, 0, 0x400);
        return;
    }
    if (((*(unsigned short *)0x4000304 & 0x8000) >> 15) == 1) {
        func_020566dc(data_0209e674, 0, 0x400);
        func_02056674(data_0209ea74, 0, 0x400);
    } else {
        func_020566dc(data_0209ea74, 0, 0x400);
        func_02056674(data_0209e674, 0, 0x400);
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN3OAM9RenderSubEP7OamAttrii
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
// @symbol _ZN3OAM9RenderSubEP7OamAttriiii
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
// @symbol _ZN3OAM12EnableSubOAMEP6OamTmp
/* ROM ordinal 12 -- OAM::EnableSubOAM(OamTmp *)
 * 0x020219f0  size 0x14 */
u32 OAM::EnableSubOAM(OamTmp *tmp) {
    /* Despite the name it only writes zero -- the flag is "sub OAM
     * suppressed", so enabling is clearing it. tmp is the caller's stack
     * slot; this body does not read it. The u32 return is a fiction
     * the ROM does not honour: the function falls off its end without
     * setting r0. The `(long) 0` cast is preserved verbatim. */
    data_0209e660 = (long)0;
}

