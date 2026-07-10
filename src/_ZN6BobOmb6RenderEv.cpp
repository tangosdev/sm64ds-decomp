//cpp
struct VBase {
    virtual void method0() = 0;
    virtual void method1() = 0;
    virtual void method2() = 0;
    virtual void method3() = 0;
    virtual void method4() = 0;
    virtual void method5(char *arg);
};
extern "C" int _ZN6BobOmb6RenderEv(char *c) {
    int result = 1;
    if (*(unsigned char*)(c + 0x3f3) != 0) {
        int flags = *(int*)(c + 0xb0);
        int b = (flags & 0x40000) != 0;
        if (!b) {
            VBase *obj = (VBase*)(c + 0x300);
            obj->method5(c + 0x80);
        }
    }
    return result;
}
