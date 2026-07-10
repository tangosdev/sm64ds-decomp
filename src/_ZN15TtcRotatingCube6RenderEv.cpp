//cpp
struct Base {
    virtual void method0();
    virtual void method1();
    virtual void method2();
    virtual void method3();
    virtual void method4();
    virtual void method5(int x);
};
extern "C" int _ZN15TtcRotatingCube6RenderEv(char *c)
{
    int flags;
    int b;
    flags = *(int*)(c + 0xb0);
    b = flags & 8;
    b = (b != 0);
    if (!b) {
        ((Base*)(c + 0xd4))->method5(0);
        ((Base*)(c + 0x320))->method5(0);
    }
    return 1;
}
