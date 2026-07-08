//cpp
// NONMATCHING: base materialization / addressing (div=21). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
typedef long long s64;
struct Matrix4x3 { int m[12]; };
struct ShadowModel;
struct Actor {
    void DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &mtx, Fix12 a, int b, unsigned int c);
};
extern Matrix4x3 _ZN9Matrix3x38IDENTITYE;

extern "C" void func_ov074_02121800(char *c)
{
    Matrix4x3 mtx;
    int i;
    int sl;
    int fp;
    char *self;
    char *src;
    char *m;
    char *sm;

    if (*(unsigned char *)(c + 0x608) != 0) return;

    mtx = _ZN9Matrix3x38IDENTITYE;

    self = c;
    src = c;
    m = c + 0x2ec;
    sm = c + 0x274;
    for (i = 0; i < 3; i++) {
        sl = *(int *)(c + 0x60) - *(int *)(c + 0x5d4);
        if (sl <= 0x1000) sl = 0x1000;
        fp = (*(int *)(c + 0x40 * (i + 1) + 0x114) << 1)
             - (int)(((s64)sl * 0x180 + 0x800) >> 12);
        *(Matrix4x3 *)(self + 0x2ec) = mtx;
        if (fp < 0xa000) fp = 0xa000;
        *(int *)(self + 0x310) = *(int *)(src + 0x3ac) >> 3;
        *(int *)(self + 0x314) = *(int *)(c + 0x60) >> 3;
        *(int *)(self + 0x318) = *(int *)(src + 0x3b4) >> 3;
        sl = sl + 0x28000;
        ((Actor *)c)->DropShadowRadHeight(
            *(ShadowModel *)sm,
            *(Matrix4x3 *)m,
            fp, sl, 0xf);
        self += 0x30;
        src += 0xc;
        m += 0x30;
        sm += 0x28;
    }
}
