typedef unsigned int u32;
typedef long long s64;
struct ShadowModel; struct Matrix4x3;
struct M48 { int w[12]; };
extern int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(char* self, struct ShadowModel* sm, struct Matrix4x3* m, int fix, int t, u32 f);
int func_ov002_020b993c(char* c)
{
    int r3;
    int b = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
    if (b != 0) return b;
    *(struct M48*)(c+0x19c) = *(struct M48*)(c+0x140);
    *(int*)(c+0x1c4) = *(int*)(c+0x3bc) >> 3;
    r3 = 0x78000;
    if (*(int*)(c+0x3c0) == 0) {
        int d = *(int*)(c+0x60) - *(int*)(c+0x3bc);
        if (d <= 0x1000) d = 0x1000;
        r3 = 0x64000 - (int)(((s64)d * 0x180 + 0x800) >> 12);
        if (r3 < 0x3c000) r3 = 0x3c000;
    }
    return _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, (struct ShadowModel*)(c+0x174), (struct Matrix4x3*)(c+0x19c), r3, 0x3c000, 0xf);
}
