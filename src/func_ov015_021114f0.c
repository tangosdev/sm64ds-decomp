// NONMATCHING: smull dest/reg coloring after Matrix call (div=12). Prologue+epilogue byte-identical with #pragma opt_propagation off + r5-first decl.
#pragma opt_propagation off
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];
int func_ov015_021114f0(char *c) {
    int r5;
    int r4 = 0;
    int idx = *(unsigned short*)(c+0x8c) >> 4;
    int s = *(short*)((char*)data_02082214 + (idx << 2));
    int r2 = (int)(((long long)s * 0x7d0000 + 0x800) >> 12);
    r5 = 0x8000;
    if (r2 <= 0) { r5 = (unsigned short)(r5 + 0x8000); r2 = -r2; }
    r4 = r4 + r2;
    Matrix4x3_FromRotationY(c+0x348, (short)(*(short*)(c+0x8e) + r5));
    int half = r4 >> 1;
    s = *(short*)(c+0x8e);
    {
        int prod;
        int v;
        s = (short)(s + r5);
        s = (unsigned short)s;
        s = s >> 4;
        s = (s << 1) + 1;
        s = data_02082214[s];
        v = *(int*)(c+0x378);
        prod = (int)(((long long)s * half + 0x800) >> 12);
        *(int*)(c+0x36c) = (v + prod) >> 3;
    }
    *(int*)(c+0x370) = *(int*)(c+0x37c) >> 3;
    {
        int prod;
        int v = *(int*)(c+0x380);
        s = *(short*)(c+0x8e);
        s = (short)(s + r5);
        s = (unsigned short)s;
        s = s >> 4;
        s = data_02082214[s << 1];
        prod = (int)(((long long)s * half + 0x800) >> 12);
        *(int*)(c+0x374) = (v + prod) >> 3;
    }
    if (*(unsigned char*)(c+0x397) >= 2)
        *(int*)(c+0x370) = *(int*)(c+0x384) >> 3;
    return _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x320, c+0x348, 0xc8000, 0x12c000, r4, 0xf);
}
