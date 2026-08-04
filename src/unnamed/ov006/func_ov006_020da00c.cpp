//cpp
extern "C" void Vec2_Sub(int* o, int* a, int* b);
extern "C" void func_0203d680(int* out, int* in, int scale);
extern "C" int func_ov006_020da8e4(void);

struct C {
    virtual int m0();
    virtual int m1();
    virtual int m2();
    int f4;      // +4
    int f8;      // +8
    int fc;      // +0xc
    int f10;     // +0x10
    int f14;     // +0x14
    int f18;     // +0x18
    int pad1c;   // +0x1c
    int pad20;   // +0x20
    int f24;     // +0x24
    short f28;   // +0x28
    unsigned char f2a; // +0x2a
    unsigned char f2b; // +0x2b
    unsigned char f2c; // +0x2c
    unsigned char f2d; // +0x2d
};

extern "C" void func_ov006_020da00c(C* c){
    int buf[4];
    c->f4 = c->fc;
    c->f8 = c->f10;
    c->f8 = c->m2();
    c->f4 = 0x80000;
    Vec2_Sub(&buf[0], &c->fc, &c->f4);
    func_0203d680(&buf[2], &buf[0], 0x100);
    c->f14 = buf[2];
    c->f18 = buf[3];
    if (c->f14 < 0) c->f14 = -c->f14;
    c->f2b = 0;
    c->f24 = 0;
    c->f2c = 0;
    c->f28 = 0;
    c->f2d = 0;
    c->f2a = (unsigned char)func_ov006_020da8e4();
}
