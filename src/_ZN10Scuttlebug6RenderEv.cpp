//cpp
struct Sub {
    virtual int method0();
    virtual int method1();
    virtual int method2();
    virtual int method3();
    virtual int method4();
    virtual int method5(int);
};
struct Obj {
    char pad1[0xb0];
    int flags;
    char pad2[0x20];
    Sub sub;
    char pad3[0x2c3];
    int state39c;
};
extern "C" int _ZN10Scuttlebug6RenderEv(Obj *c) {
    int flag = (c->flags & 0x40000) ? 1 : 0;
    if (flag) goto ret;
    if (!c->state39c) goto ret;
    goto call;
ret:
    return 1;
call:
    c->sub.method5(0);
    return 1;
}
