typedef unsigned int u32;
typedef long long s64;
enum { false, true };
struct ShadowModel;
struct Matrix4x3;
struct M48 { int w[12]; };
extern struct M48 data_02082128;
extern int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(char *self, struct ShadowModel *sm, struct Matrix4x3 *m, int fix, int t, u32 f);

#pragma opt_common_subs off
#pragma opt_strength_reduction off

void func_ov002_020e8398(char *c)
{
    int r2, rad, delta, r8, t;
    int flag2;

    if ((((unsigned int)*(unsigned short *)(c + 0x400 + 0xa2) << 30) >> 31) == 0)
        return;
    flag2 = *(int *)(c + 0xb0) & 0x40000;
    flag2 = flag2 != 0;
    if (flag2 != false)
        return;
    {
        int v2 = *(unsigned short *)(c + 0x400 + 0xa2);
        unsigned int v3 = (unsigned int)(v2 << 31);
        v3 = v3 >> 31;
        if (v3 != 0)
            return;
    }

    r2 = *(int *)(c + 0x80);
    rad = r2 * 0x64;
    flag2 = *(unsigned short *)(c + 0xc);
    flag2 = flag2 == 0xb2;
    if (flag2 != false)
        rad = r2 * 0xa0;

    delta = *(int *)(c + 0x60) - *(int *)(c + 0x42c);
    if (delta <= 0x1000)
        delta = 0x1000;

    r8 = rad - (int)(((s64)delta * 0x180 + 0x800) >> 12);
    if (r8 < 0xa000)
        r8 = 0xa000;

    t = delta + 0x28000;

    *(struct M48 *)(c + 0x3fc) = data_02082128;

    *(int *)(c + 0x420) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x424) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x428) = *(int *)(c + 0x64) >> 3;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, (struct ShadowModel *)(c + 0x3d4), (struct Matrix4x3 *)(c + 0x3fc), r8, t, 0xf);
}
