#include "common.h"

/* Texture-mapped triangle-strip emitter for the ov007 water/cloth grid: one
 * strip per grid row, alternating direction each row. The per-column texcoord
 * offsets tu/tv are refreshed from the next row's normals as the row is walked,
 * so each vertex is emitted with its own row's perturbation. Sibling of
 * func_ov007_020ca86c (the untextured emitter). */

#define REG_BEGIN (*(vu32 *)0x4000500)
#define REG_END   (*(vu32 *)0x4000504)
#define REG_NRM   (*(vu32 *)0x4000484)
#define REG_TEX   (*(vu32 *)0x4000488)
#define REG_VTX   (*(vu32 *)0x400048C)

#define FX_MUL(a, b) ((s32)(((s64)(a) * (s64)(b) + 0x800) >> 12))

static inline void G3_Vtx(s16 x, s16 y, s16 z)
{
    REG_VTX = (u16)x | ((u16)y << 16);
    REG_VTX = (u16)z;
}

static inline void G3_Tex(s32 s, s32 t)
{
    REG_TEX = (u16)s | ((u32)(u16)t << 16);
}

#define SEND_VTX(p) G3_Vtx((s16)((p)->x >> shift), (s16)((p)->y >> shift), (s16)((p)->z >> shift))
#define SEND_NRM(q) \
    REG_NRM = ((((q)->x) >> 3) & 0x3FF) | (((((q)->y) >> 3) & 0x3FF) << 10) | (((((q)->z) >> 3) & 0x3FF) << 20)
#define SEND_TEX(s, t) G3_Tex(((s) << 8) >> 16, ((t) << 8) >> 16)

void func_ov007_020caeac(Vector3 *vtx, Vector3_16 *nrm, s32 *tu, s32 *tv, s32 w, s32 h,
                         s32 u0, s32 v0, s32 du, s32 dv, s32 su, s32 sv, s32 shift)
{
    s32 i;
    s32 dir;
    s32 w1;
    s32 v;
    s32 ua;
    s32 ub;
    s32 rowbase;
    s32 half;
    s64 su64;
    s64 sv64;
    s32 vstride;
    s32 voff;
    Vector3_16 *n2;
    s64 nx;
    s64 ny;
    s32 round;
    s32 nstride;
    s32 k;
    Vector3_16 *n;

    w1 = w + 1;
    k = 0;
    v = v0;
    dir = 1;
    if (w1 > 0) {
        n = nrm;
        do {
            tu[k] = FX_MUL(n->x, su);
            tv[k] = FX_MUL(n->y, sv);
            k++;
            n++;
        } while (k < w1);
    }
    i = 0;
    if (h > 0) {
        nstride = w1 * sizeof(Vector3_16);
        rowbase = i;
        half = du / 2;
        su64 = su;
        sv64 = sv;
        vstride = w1 * sizeof(Vector3);
        do {
            s32 j;
            s32 oa;
            s32 ob;
            oa = (dir == 1) ? 0 : half;
            ua = u0 + oa;
            ob = (dir == 1) ? half : 0;
            ub = u0 + ob;
            REG_BEGIN = 2;
            for (j = 0; j < w1; j++) {
                s32 idx;
                s32 vidx;
                Vector3 *p;
                Vector3_16 *q;
                Vector3 *p2;
                s32 t;
                s32 t2;
                idx = (dir == 1) ? j : (w1 - 1) - j;
                vidx = idx + rowbase;
                voff = vidx * sizeof(Vector3);
                p = (Vector3 *)((u8 *)vtx + voff);
                q = (Vector3_16 *)((u8 *)nrm + vidx * sizeof(Vector3_16));
                n2 = (Vector3_16 *)((u8 *)q + nstride);
                t = tv[idx];
                SEND_TEX(ua + (tu[idx] + du * idx), t + v);
                SEND_NRM(q);
                SEND_VTX(p);
                round = 0x800;
                nx = n2->x;
                tu[idx] = (s32)((nx * su64 + round) >> 12);
                ny = n2->y;
                tv[idx] = (s32)((ny * sv64 + round) >> 12);
                t2 = tv[idx];
                SEND_TEX(ub + (tu[idx] + du * idx), dv + (t2 + v));
                SEND_NRM(n2);
                p2 = (Vector3 *)(vstride + (s32)p);
                SEND_VTX(p2);
            }
            REG_END = 0;
            v += dv;
            dir = -dir;
            rowbase += w1;
            i++;
        } while (i < h);
    }
}
