//cpp
// NONMATCHING: different op / idiom (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct OamAttr;
struct Matrix2x2;

extern "C" void func_ov006_020bfec0(void *a, void *b, short *c);
extern "C" int func_02053200(int x);
extern "C" void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
    bool en, OamAttr *attr, int a, int b, int c, int d, Matrix2x2 *m);

extern unsigned char data_ov006_02140400;
extern void *data_ov006_02141a44;
extern unsigned short data_ov006_02140404;
extern short SINE_TABLE[];
extern OamAttr *data_ov006_02134d1c;

struct VObj;
typedef void (*VFn)(VObj *o, void *arg);
struct VObj {
    char *tbl;
};
extern char data_ov006_0212ddd0;

struct Mtx {
    int a, b, c, d;
    Mtx() { a = 0; b = 0; c = 0; d = 0; }
};

extern "C" void func_ov006_020c7734(char *self)
{
    short v[2];        // sp+0xc, sp+0xe
    int r1res;
    int r2res;
    int g;
    int t;

    if (*(unsigned char *)(self + 0x35) == 0)
        return;

    if (data_ov006_02140400 != 0) {
        func_ov006_020bfec0(data_ov006_02141a44, self + 0x14, v);

        g = data_ov006_02140404;
        t = SINE_TABLE[(g >> 4) * 2];
        v[1] = v[1] - (((t << 2) + 0x30000) >> 12);
        r1res = func_02053200((t >> 2) + 0x1000);

        g = data_ov006_02140404;
        r2res = -func_02053200((SINE_TABLE[(g >> 4) * 2 + 1] >> 2) + 0x1000);

        Mtx m;
        m.d = r2res;
        m.a = r1res;
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
            false, data_ov006_02134d1c, v[0], v[1], -1, -1, (Matrix2x2 *)&m);
    }

    {
        VObj *o = (VObj *)(self + 0x4c);
        (*(VFn *)(o->tbl + 0x14))(o, &data_ov006_0212ddd0);
    }
}
