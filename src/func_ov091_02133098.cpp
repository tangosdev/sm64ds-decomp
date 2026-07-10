//cpp
// NONMATCHING: register allocation (div=43). Logic verified vs ROM (mwccarm 1.2/sp2p3).
// Counts as decompiled, not matched.
extern "C" {
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void*,void*,void*,int,int,int,unsigned int);
struct M4x3 { int w[12]; };
void func_ov091_02133098(char* c){
    int d = *(int*)(c + 0x60) - *(int*)(c + 0x394);
    int sub2 = 0x20000;
    int sub;
    int* m;
    int rx, ry;
    int* b8;
    int* dst = (int*)(c + 0x360);
    if (d <= 0x14000) { d = 0x14000; sub2 = 0; }
    sub = (int)(((long long)d * 0x60 + 0x800) >> 12);
    m = (int*)*(void**)(c + 0x320);
    b8 = (int*)(c + 0xb8);
    rx = m[4] - sub;
    ry = m[5] - sub;
    *b8 = d + 0x8c000;
    if (rx < 0xa000) rx = 0xa000;
    if (ry < 0xa000) ry = 0xa000;
    if (*b8 < 0x200000) *b8 = 0x200000;
    *b8 = *b8 >> 3;
    *(struct M4x3*)dst = *(struct M4x3*)(c + 0xf0);
    *(int*)(c + 0x384) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x388) = (*(int*)(c + 0x60) - sub2) >> 3;
    *(int*)(c + 0x38c) = *(int*)(c + 0x64) >> 3;
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x338, dst, rx, d + 0x28000, ry, 0xf);
}
}
