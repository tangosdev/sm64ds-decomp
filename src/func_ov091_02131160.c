// NONMATCHING: register/schedule (div~119 from draft). Starting near-miss (mwccarm 1.2/sp2p3).
// Counts as decompiled, not matched.
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];

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

    Matrix4x3_FromRotationY(c + 0x348, *(short*)(c + 0x8e));
    *(int*)(c + 0x36c) = *(int*)(c + 0x5c) >> 3;

    int idx = *(unsigned short*)(c + 0x8c) >> 4;
    int s = data_02082214[idx << 1];
    int sa = s < 0 ? -s : s;
    int scaled = (int)(((long long)sa * 0xa0000 + 0x800) >> 12);
    int b5 = *(unsigned char*)(c + 0x395);
    *(int*)(c + 0x370) = (*(int*)(c + 0x60) - ((&v0.x)[b5] + scaled)) >> 3;
    *(int*)(c + 0x374) = *(int*)(c + 0x64) >> 3;

    int r1 = *(int*)(c + 0x60) - *(int*)(c + 0x37c);
    if (r1 <= 0x1000) r1 = 0x1000;
    int r3 = (&v3.x)[b5];
    if (r3 < r1 + 0x100000) r3 = r1 + 0x100000;
    *(int*)(c + 0xb4) = -((int)(r1 + ((unsigned)r1 >> 31)) >> 1);
    *(int*)(c + 0xb8) = (int)(r3 + ((unsigned)r3 >> 31)) >> 4;

    int idx2 = *(unsigned short*)(c + 0x8c) >> 4;
    int s2 = data_02082214[(idx2 << 1) + 1];
    int s2a = s2 < 0 ? -s2 : s2;
    int scaled2 = (int)(((long long)s2a * (0xa0000 - r1) + 0x800) >> 12);
    return _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x320, c + 0x348, r1, (&v2.x)[b5] + scaled2, r3 - r1, 0xf);
}
