//cpp
struct OamAttr;
struct Matrix2x2;

extern "C" void func_ov006_020bfec0(void *a, void *b, short *c);
extern "C" int func_02053200(int x);
extern "C" void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
    bool en, OamAttr *attr, int a, int b, int c, int d, Matrix2x2 *m);

extern unsigned char data_ov006_02140400;
extern void *data_ov006_02141a44;
extern unsigned short data_ov006_02140404;
extern short data_02082214[];
extern OamAttr *data_ov006_02134d1c;
extern char data_ov006_0212ddd0;

struct Mtx { int a, b, c, d; };

struct VBase {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(void *arg);
};

extern "C" void func_ov006_020c7734(char *self)
{
    short v[2];
    int r1res;
    int r2res;
    int g;
    int t;
    Mtx m;

    if (*(unsigned char *)(self + 0x35) == 0)
        return;

    if (data_ov006_02140400 != 0) {
        func_ov006_020bfec0(data_ov006_02141a44, self + 0x14, v);

        g = data_ov006_02140404;
        t = data_02082214[(g >> 4) * 2];
        v[1] = v[1] - (((t << 2) + 0x30000) >> 12);
        r1res = func_02053200((t >> 2) + 0x1000);

        g = data_ov006_02140404;
        r2res = -func_02053200((data_02082214[(g >> 4) * 2 + 1] >> 2) + 0x1000);

        int *mp = (int *)&m;
        int z = 0;
        mp[0] = z; mp[1] = z; mp[2] = z; mp[3] = z;
        m.d = r2res;
        m.a = r1res;
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
            false, data_ov006_02134d1c, v[0], v[1], -1, -1, (Matrix2x2 *)&m);
    }

    ((VBase *)(self + 0x4c))->m5(&data_ov006_0212ddd0);
}
