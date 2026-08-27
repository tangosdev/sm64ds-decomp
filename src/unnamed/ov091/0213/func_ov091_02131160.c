#include "types.h"
extern void Matrix4x3_FromRotationY(void *m, s16 ang);
extern int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern s16 data_02082214[];

struct V3 { int x, y, z; };
extern struct V3 data_ov091_02134bac;
extern struct V3 data_ov091_02134bd0;
extern struct V3 data_ov091_02134bb8;
extern struct V3 data_ov091_02134ba0;

int func_ov091_02131160(char *c) {
    struct V3 v0 = data_ov091_02134bac;
    struct V3 v1 = data_ov091_02134bd0;
    struct V3 v2 = data_ov091_02134bb8;
    struct V3 v3 = data_ov091_02134ba0;

    Matrix4x3_FromRotationY(c + 0x348, *(s16 *)(c + 0x8e));
    *(int *)(c + 0x36c) = *(int *)(c + 0x5c) >> 3;

    int idx = *(u16 *)(c + 0x8c) >> 4;
    int s = data_02082214[idx << 1];
    int sa = s < 0 ? -s : s;
    int scaled = (int)(((long long)sa * 0xa0000 + 0x800) >> 12);
    int b5 = *(u8 *)(c + 0x395);
    int base = (&v0.x)[b5];
    int sum = base + scaled;
    int py = *(int *)(c + 0x60);
    *(int *)(c + 0x370) = (py - sum) >> 3;
    *(int *)(c + 0x374) = *(int *)(c + 0x64) >> 3;

    int b5b = *(u8 *)(c + 0x395);
    int h = *(int *)(c + 0x60) - *(int *)(c + 0x37c);
    if (h <= 0x1000) h = 0x1000;
    int r3 = (&v3.x)[b5b];
    if (h + 0x100000 >= r3) r3 = h + 0x100000;
    *(int *)(c + 0xb4) = -((int)(h + ((unsigned)h >> 31)) >> 1);
    *(int *)(c + 0xb8) = (int)(r3 + ((unsigned)r3 >> 31)) >> 4;

    int r5 = (int)(((long long)h * 32 + 0x800) >> 12);
    int idx2 = *(u16 *)(c + 0x8c) >> 4;
    int b5c = *(u8 *)(c + 0x395);
    int a_arg = (&v1.x)[b5c] - r5;
    int s2 = data_02082214[(idx2 << 1) + 1];
    int fac = 0xa0000 - r5;
    if (s2 < 0) s2 = -s2;
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x320, c + 0x348, a_arg, h,
        (&v2.x)[b5c] + (int)(((long long)fac * s2 + 0x800) >> 12), 0xf);
}
