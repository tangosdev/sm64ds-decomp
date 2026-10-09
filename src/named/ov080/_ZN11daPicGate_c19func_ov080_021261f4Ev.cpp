//cpp
// NONMATCHING: mwccarm 2004/b56 codegen floor: one 5-insn register-assignment/scheduling tie in the G3_TEXIMAGE_PARAM pack (notes/mwccarm-codegen.md 6cf, 6cz) (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 2004/b56 (see notes/matching-style.md).
// Counts as decompiled, not matched.
#include "types.h"
#pragma defer_codegen off
#include "daPicGate_c.h"
#include "private/Ov080Mat.h"
#include "common.h"
#define G3_MTX_MODE     (*(int *)0x04000440)
#define G3_MTX_PUSH     (*(int *)0x04000444)
#define G3_MTX_POP      (*(int *)0x04000448)
#define G3_MTX_SCALE    (*(int *)0x0400046c)
#define G3_NORMAL       (*(int *)0x04000484)
#define G3_TEXCOORD     (*(int *)0x04000488)
#define G3_VTX_16       (*(int *)0x0400048c)
#define G3_POLYGON_ATTR    (*(int *)0x040004a4)
#define G3_TEXIMAGE_PARAM  (*(int *)0x040004a8)
#define G3_TEXPLTT_BASE    (*(int *)0x040004ac)
#define G3_DIF_AMB         (*(int *)0x040004c0)
#define G3_SPE_EMI         (*(int *)0x040004c4)
#define G3_LIGHT_VECTOR (*(int *)0x040004c8)
#define G3_LIGHT_COLOR  (*(int *)0x040004cc)
#define G3_BEGIN        (*(int *)0x04000500)
#define G3_END          (*(int *)0x04000504)
enum { kMtxPosVec = 2, kMtxPos = 1, kScale = 0x20000, kTriStrip = 2 };
extern "C" {
extern void MulMat4x3Mat4x3(const int *a, const int *b, int *out);
extern Matrix4x3 data_0209b3ec;
extern void func_020553a4(void *mtx);
extern void func_02055388(void *m);
extern void func_02055998(void *m);
extern int data_ov080_0212771c[];
}
/* texgen=NORMAL variant: no per-vertex TEXCOORD, positions at >>7. */
#define EMIT_VTX7(v) \
    G3_NORMAL = (v)->color; \
    vx = (v)->x; \
    vy = (v)->y; \
    vz = (v)->z; \
    s0 = (s16)(vx >> 7); \
    s1 = (s16)(vy >> 7); \
    s2 = (s16)(vz >> 7); \
    G3_VTX_16 = (u16)s0 | ((u16)s1 << 16); \
    G3_VTX_16 = (u16)s2

void daPicGate_c::func_ov080_021261f4()
{
    int tmp[12];
    int i, j, n;
    int z = 0;

    MulMat4x3Mat4x3(mMtx, data_0209b3ec.m, tmp);
    G3_MTX_PUSH = z;
    func_02055998(data_ov080_0212771c);
    func_02055388(tmp);
    G3_MTX_MODE = kMtxPosVec;
    func_020553a4(tmp);
    G3_TEXCOORD = 0x04000400;
    G3_LIGHT_VECTOR = 0xe0000000;
    G3_LIGHT_COLOR = 0xc0007fff;
    {
        struct Ov080Mat *m2 = (struct Ov080Mat *)mTexRecord;
        G3_DIF_AMB = m2->difAmb | 0x7fff0000;
    }
    G3_SPE_EMI = ((struct Ov080Mat *)mTexRecord)->speEmi;
    {
        struct Ov080Mat *m2 = (struct Ov080Mat *)mTexRecord;
        G3_TEXIMAGE_PARAM = (int)((m2->texAddr >> 3) | (((m2->param >> 0x1a) & 7) << 26)
            | 0x80000000 | (((m2->param >> 0x14) & 7) << 20)
            | (((m2->param >> 0x17) & 7) << 23) | (((m2->param >> 0x1d) & 1) << 29));
    }
    {
        struct Ov080Mat *m3 = (struct Ov080Mat *)mTexRecord;
        G3_TEXPLTT_BASE = m3->pltAddr >> (4 - (((m3->param >> 0x1a) & 7) == 2 ? 1 : 0));
    }
    G3_POLYGON_ATTR = 0x01000088 | ((((u8)((param1 >> 8) & 0x1f)) == 7) ? 0x14 : 0x1f) << 16;
    G3_MTX_SCALE = 0x10000;
    G3_MTX_SCALE = 0x10000;
    G3_MTX_SCALE = 0x10000;

    n = (int)mCols;
    i = z;
    n = n - 1;
    if (n > 0) {
        do {
            G3_BEGIN = kTriStrip;
            j = z;
            if ((int)mRows > 0) {
                do {
                    Vertex *base = mCells;
                    int rows = (int)mRows;
                    Vertex *v1 = &base[i + j * rows];
                    Vertex *v2 = &base[(i + 1) + j * rows];
                    int vx, vy, vz;
                    s16 s0, s1, s2;

                    EMIT_VTX7(v1);
                    EMIT_VTX7(v2);

                    j++;
                } while (j < (int)mRows);
            }
            G3_END = z;
            i++;
        } while (i < (int)mCols - 1);
    }
    G3_MTX_POP = 1;
}
