//cpp
extern "C" {
extern void func_02037464(void *c);
extern int func_020393b4(void *p);
extern int func_02035354(void *a, int b);
extern int func_020393ac(void *p);
extern void func_02037fec(char *c, int p1, int p2, int p3, void *p4);
}

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual int  v6(void *arg);
};

extern "C" Obj *data_020a0c80;

extern "C" int func_02038ea4(void *thiz)
{
    int r = 0;
    Obj *o;
    func_02037464(thiz);
    o = data_020a0c80;
    if (o != 0) {
        if (func_02035354(thiz, func_020393b4(o)) == 0) {
            if (o->v6(thiz) != 0) {
                int a = func_020393ac(o);
                int b = func_020393b4(o);
                func_02037fec((char *)thiz + 0x10, 0, a, b, o);
                r = 1;
            }
        }
    }
    return r;
}
