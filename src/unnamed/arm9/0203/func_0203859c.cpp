//cpp
// func_0203859c at 0x0203859c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
extern "C" int func_020393b4(void *p);
extern "C" int func_02035354(void *a, void *b);
extern "C" int func_020393ac(void *p);
extern "C" void func_02037fec(void *c, int p1, int p2, int p3, void *e);
extern void *data_020a0c80[];

class C {
public:
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
    virtual int v7(void *arg);
};

extern "C" int func_0203859c(void *obj)
{
    C *e = (C *)data_020a0c80[0];
    int result = 0;
    if (e != 0) {
        if (func_02035354(obj, (void *)func_020393b4(e)) == 0) {
            if (e->v7(obj) != 0) {
                int a = func_020393ac(e);
                int b = func_020393b4(e);
                func_02037fec((char *)obj + 0x10, 0, a, b, e);
                result = 1;
            }
        }
    }
    return result;
}
