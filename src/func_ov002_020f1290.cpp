//cpp
struct Sub {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(int);   // vtable +0x14
};

struct Obj {
    char pad[0x60];
    int a;                  // 0x60
    char pad2[0xd4 - 0x64];
    Sub sub;                // 0xd4
    char pad3[0x320 - (0xd4 + sizeof(Sub))];
    int b;                  // 0x320
};

extern "C" int func_ov002_020f1290(Obj *p) {
    if (p->a > p->b) {
        p->sub.m5(0);
    }
    return 1;
}
