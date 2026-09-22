// @symbol _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii
/* recovered: OAM::Render(bool, OamAttr *, int, int, int, int, Fix12<int>, Fix12<int>, int, int)
 * @ 0x02020994 (arm9, size 0x690).
 *
 * Walks a chain of OamAttr entries (terminated by a3 == 0xffff) and writes each
 * one into whichever OAM buffer is being filled this frame, clipping against the
 * screen; a non-unit scale or a rotation goes through an affine slot from
 * LoadAffineParams. palette / priority == -1 keep the entry's own bits, mode > 0
 * overrides the entry's object mode.
 *
 * C with the mangled name, the shape of the 7-parameter sibling
 * (_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi.c): the two Fix12<int> parameters are
 * plain ints here and are used DIRECTLY, never copied into locals. That is what
 * reproduces the ROM's prologue (notes 6xx, run link100 lane W12-5): a scalar
 * parameter used in the loop is register-homed in PARAMETER ORDER, so its ldr sb /
 * ldr fp sit before the mode self-home store in the scheduler's input; a
 * class-typed by-value parameter, or a local copy of a scalar one, is a body load
 * that follows every homing store and the last store wins the tie.
 *
 * The three pragma offs are unscoped and load-bearing; folding this file into a
 * multi-function TU needs scoped restores, which change these bytes (notes 6ay).
 */
#include "types.h"

#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off

struct OamAttr { u32 a01; u16 a2; u16 a3; };
struct OamEntry { u32 a01; u16 a2; u16 pad; };
struct Matrix2x2 { int a, b, c, d; };

extern u8 data_0209e660;
extern int data_0209e664;
extern int data_0209e668;
extern int data_0209e66c;
extern int data_0209e670;
extern struct OamEntry data_0209e674[];
extern struct OamEntry data_0209ea74[];
extern s16 data_02082214[];

extern unsigned char _ZN3OAM11GetObjWidthEii(int shape, int size);
extern unsigned char _ZN3OAM12GetObjHeightEii(int shape, int size);
extern int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(struct OamAttr *attr, int *affCnt, struct Matrix2x2 *m);
extern int _ZN4cstd4fdivEii(int num, int den);


void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, struct OamAttr *attr, int xOff, int yOff, int palette, int priority,
                 int scaleX, int scaleY, int rotation, int mode)
{
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
    struct OamEntry *dst;
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
            w = _ZN3OAM11GetObjWidthEii((int)((a << 16) >> 30), (int)(a >> 30));
            }
        {
        int h;
        {
            u32 a = attr->a01;
            h = _ZN3OAM12GetObjHeightEii((int)((a << 16) >> 30), (int)(a >> 30));
        }
        y = u;

        if (scaleX != 0x1000 || scaleY != 0x1000 || rotation != 0) {
            x = (x + (w >> 1)) << 12;
            y = (y + (h >> 1)) << 12;
            halfW = w >> 1;
            halfH = h >> 1;
            if (scaleX != 0x1000)
                x = _ZN4cstd4fdivEii(x, scaleX);
            if (scaleY != 0x1000)
                y = _ZN4cstd4fdivEii(y, scaleY);
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
                m.a = (c2 * scaleX + 0x800) >> 12;
                {
                s16 s2 = data_02082214[mtxIdx];
                m.b = (s2 * scaleX + 0x800) >> 12;
                m.c = -((s2 * scaleY + 0x800) >> 12);
                }
                m.d = (c2 * scaleY + 0x800) >> 12;
                if (attr->a01 & 0x10000000) {
                    m.a = -m.a;
                    m.b = -m.b;
                }
                if (attr->a01 & 0x20000000) {
                    m.c = -m.c;
                    m.d = -m.d;
                }
                aff = _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2((struct OamAttr *)(dst - *pCount), pAff, &m);
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
