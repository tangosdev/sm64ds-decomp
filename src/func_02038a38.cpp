//cpp
// func_02038a38 at 0x02038a38
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
struct A {
    virtual int f0();
    virtual int f1();
    virtual int f2();
    virtual int f3();
    virtual int f4();
    virtual int f5();
    virtual int f6();
    virtual int f7();
    virtual int f8(void*);
};

extern "C" int func_020393b4(struct A *p);
extern "C" int func_020393ac(struct A *p);
extern "C" int func_02035354(void *a, int b);
extern "C" void func_02037fec(char *c, int p1, int p2, int p3, struct A *p4);
extern "C" void func_020379d0(void *c, int b, int d, int e, struct A *f);
extern "C" void func_0203799c(void *c, int b, int d, int e, struct A *f);
extern "C" void func_02037968(void *c, int b, int d, int e, struct A *f);
extern "C" void func_02037a38(void *o);

extern "C" struct A *data_020a0c80;

extern "C" int func_02038a38(void *arg0)
{
    struct A *o = data_020a0c80;
    int ret = 0;
    int flags;
    if (o != 0 && func_02035354(arg0, func_020393b4(o)) == 0
        && (flags = o->f8(arg0)) != 0) {
        func_02037fec((char*)arg0 + 0x10, 0, func_020393ac(o), func_020393b4(o), o);
        if (flags & 1)
            func_020379d0(arg0, 0, func_020393ac(o), func_020393b4(o), o);
        if (flags & 2)
            func_0203799c(arg0, 0, func_020393ac(o), func_020393b4(o), o);
        if (flags & 4)
            func_02037968(arg0, 0, func_020393ac(o), func_020393b4(o), o);
        ret = 1;
    }
    func_02037a38(arg0);
    return ret;
}