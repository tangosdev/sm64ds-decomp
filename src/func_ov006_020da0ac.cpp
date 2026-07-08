//cpp
// Copy a 4-word source block into the object, refresh two fields from a virtual call
// and a fixed-point transform pair, take the absolute value of one result, then reset a
// few trailing fields. Sibling of func_ov006_020da00c.
extern "C" void func_0203d6d0(int *o, int *a, int *b);
extern "C" void func_0203d680(int *out, int *in, int scale);
extern "C" int func_ov006_020da8e4(void);

struct C {
    virtual int m0();
    virtual int m1();
    virtual int m2();
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int pad1c;
    int pad20;
    int f24;
    short f28;
    unsigned char f2a;
    unsigned char f2b;
    unsigned char f2c;
    unsigned char f2d;
};
struct Src { int f0; int f4; int f8; int fc; };

extern "C" void func_ov006_020da0ac(C *c, Src *s)
{
    int buf[4];
    c->f4 = s->f0;
    c->f8 = s->f4;
    {
        int *b = (int *)(((int)c + 4) & 0xFFFFFFFFFFFFFFFF);
        c->fc = b[0];
        c->f10 = b[1];
    }
    c->f8 = c->m2();
    c->f4 = 0x80000;
    func_0203d6d0(&buf[0], &c->fc, &c->f4);
    func_0203d680(&buf[2], &buf[0], 0x100);
    c->f14 = buf[2];
    c->f18 = buf[3];
    if (c->f14 < 0) c->f14 = -c->f14;
    c->f24 = 0;
    c->f2c = 0;
    c->f2a = (unsigned char)func_ov006_020da8e4();
}
