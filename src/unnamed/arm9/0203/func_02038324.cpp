//cpp
// func_02038324 at 0x02038324
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
extern "C" {
int func_02037fc0(void *p);
int func_02037f54(void *p);
int func_020393cc(void *p);
int func_02038298(void *p);
int func_020393b4(void *p);
}
extern void *data_020a0c80[];

struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5();
    virtual void m6();
    virtual void m7();
    virtual void m8();
    virtual void m9(void *a, int v, int b, int c, int d);
};

extern "C" void func_02038324(void *arg, int b, int c, int d)
{
    int idx;
    Obj *obj;
    if (func_02037fc0(arg) == 0) return;
    idx = func_02037f54(arg);
    obj = (Obj *)data_020a0c80[idx];
    if (obj == 0) return;
    if (func_020393cc(obj) == 0) return;
    if (func_02038298(arg) == 0) return;
    obj->m9(arg, func_020393b4(data_020a0c80[idx]), b, c, d);
}