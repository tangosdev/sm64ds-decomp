//cpp
struct Sub {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void g(int);  // vtable offset 0x14
};

extern "C" int func_ov002_020eacb8(char *self) {
    unsigned char b = *(unsigned char *)(self + 0x1db);
    if (((unsigned)(b << 30)) >> 31) {
        Sub *s = (Sub *)(self + 0x114);
        s->g(0);
    }
    return 1;
}
