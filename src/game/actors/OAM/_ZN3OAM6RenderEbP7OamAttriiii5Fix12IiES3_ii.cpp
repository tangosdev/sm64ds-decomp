//cpp
// NONMATCHING (TERMINAL-FLOOR): functionally-verified C at the proven compiler floor.
// OAM::Render(bool, OamAttr*, int, int, int, int, Fix12<int>, Fix12<int>, int, int)
// @ 0x02020994 (arm9, size 0x690). 2 words diverge: one prologue filler transposition
// (ldr fp,[sp,#0x8c] / str r0,[sp,#0x94] swapped at +0x38). A pre-body, parameter-count-
// driven filler-zip tie-break, immune to all body-level source (notes 6ay); all owned
// builds agree against the ROM. Order-only delta: functionally identical.
// TU NOTE: the pragma offs below are unscoped; folding this file into a multi-function
// TU requires scoped restores which change these bytes (6ay trap).
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off

typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;
typedef long long s64;

template <typename T> struct Fix12 { T val; };

struct OamAttr { u32 a01; u16 a2; u16 a3; };
struct OamEntry { u32 a01; u16 a2; u16 pad; };
struct Matrix2x2 { int a, b, c, d; };

extern "C" {
extern u8 data_0209e660;
extern int data_0209e664;
extern int data_0209e668;
extern int data_0209e66c;
extern int data_0209e670;
extern OamEntry data_0209e674[];
extern OamEntry data_0209ea74[];
extern s16 data_02082214[];
}

struct OAM {
    static void Render(bool sub, OamAttr *attr, int xOff, int yOff, int palette, int priority,
                       Fix12<int> scaleX, Fix12<int> scaleY, int rotation, int mode);
    static u8 GetObjWidth(int shape, int size);
    static u8 GetObjHeight(int shape, int size);
    static int LoadAffineParams(OamAttr *attr, int *affCnt, Matrix2x2 *m);
};

namespace cstd { int fdiv(int num, int den); }

void OAM::Render(bool sub, OamAttr *attr, int xOff, int yOff, int palette, int priority,
                 Fix12<int> scaleX, Fix12<int> scaleY, int rotation, int mode)
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
    Matrix2x2 m;
    int y;
    int sxv;
    int w;
    int syv;
    OamEntry *dst;
    int x;
    int om;

    sxv = scaleX.val;
    syv = scaleY.val;

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
            w = GetObjWidth((int)((a << 16) >> 30), (int)(a >> 30));
            }
        {
        int h;
        {
            u32 a = attr->a01;
            h = GetObjHeight((int)((a << 16) >> 30), (int)(a >> 30));
        }
        y = u;

        if (sxv != 0x1000 || syv != 0x1000 || rotation != 0) {
            x = (x + (w >> 1)) << 12;
            y = (y + (h >> 1)) << 12;
            halfW = w >> 1;
            halfH = h >> 1;
            if (sxv != 0x1000)
                x = cstd::fdiv(x, sxv);
            if (syv != 0x1000)
                y = cstd::fdiv(y, syv);
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
            if (sxv != 0x1000 || syv != 0x1000 || rotation != 0) {
                s16 c2 = data_02082214[mtxIdx + 1];
                m.a = (c2 * sxv + 0x800) >> 12;
                {
                s16 s2 = data_02082214[mtxIdx];
                m.b = (s2 * sxv + 0x800) >> 12;
                m.c = -((s2 * syv + 0x800) >> 12);
                }
                m.d = (c2 * syv + 0x800) >> 12;
                if (attr->a01 & 0x10000000) {
                    m.a = -m.a;
                    m.b = -m.b;
                }
                if (attr->a01 & 0x20000000) {
                    m.c = -m.c;
                    m.d = -m.d;
                }
                aff = LoadAffineParams((OamAttr *)(dst - *pCount), pAff, &m);
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
