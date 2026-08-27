//cpp
struct C {
    virtual int f0();
    virtual int f1();
    virtual int f2();
    virtual int f3();
    virtual int f4();
    virtual int f5();
    virtual int f6();
    virtual int f7();
    virtual int f8();
    virtual int f9();
    virtual int f10();
    virtual int f11();
    virtual int f12();
    virtual int f13();
    virtual int f14();
    virtual int f15();
    virtual int f16();
    virtual int f17();
    virtual int f18();
    virtual int f19();
    virtual int f20();
    virtual int f21();
    virtual int f22();
    virtual int f23();
    virtual int f24();
    virtual int f25();
    virtual int f26();
};
extern "C" {
extern C* data_ov004_020beb68;
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, int a5);
}

struct BF { unsigned int lo:10; unsigned int hi:22; };
struct S { int x; BF y; };

extern "C" void func_ov004_020aea78(S* self, int a1, int a2, int a3)
{
    S v = *self;
    if (data_ov004_020beb68->f26() == 2) {
        v.y.lo = (self->y.lo - 0x200) & 0x3ff;
    }
    func_ov004_020b1c68(&v, a1, a2, 0, -1, a3);
}
