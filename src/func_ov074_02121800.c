typedef long long s64;
struct Matrix4x3 { int m[12]; };
extern struct Matrix4x3 IDENTITY_MATRIX4X3;
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int a, int b, unsigned int c);

void func_ov074_02121800(char *c)
{
    struct Matrix4x3 mtx;
    int i, sl, fp, r1_310, r2_314, r3_318;
    char *self, *src, *m, *sm;

    if (*(unsigned char *)(c + 0x608) != 0)
        return;

    mtx = IDENTITY_MATRIX4X3;
    self = c;
    src = c;
    m = c + 0x2ec;
    sm = c + 0x274;
    for (i = 0; i < 3; i++) {
        sl = *(int *)(c + 0x60) - *(int *)(c + 0x5d4);
        if (sl <= 0x1000)
            sl = 0x1000;
        fp = (*(int *)(c + 0x40 * (i + 1) + 0x114) << 1)
             - (int)(((s64)sl * 0x180 + 0x800) >> 12);
        if (fp < 0xa000)
            fp = 0xa000;
        r3_318 = fp;
        *(struct Matrix4x3 *)(self + 0x2ec) = mtx;
        *(int *)(self + 0x310) = (r1_310 = *(int *)(src + 0x3ac) >> 3);
        r2_314 = *(int *)(c + 0x60);
        *(int *)(self + 0x314) = (r3_318 ? r2_314 : r2_314) >> 3;
        fp = *(int *)(src + 0x3b4);
        sl += 0x28000;
        *(int *)(self + 0x318) = fp >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, sm, m, r3_318, sl, 0xf);
        self += 0x30;
        src += 0xc;
        m += 0x30;
        sm += 0x28;
    }
}
