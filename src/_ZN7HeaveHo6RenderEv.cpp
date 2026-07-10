//cpp
extern int data_0209f32c;

struct Cls {
    virtual void method0();
    virtual void method1();
    virtual void method2();
    virtual void method3();
    virtual void method4();
    virtual void method5(int);  /* at vtable offset 0x14 */
};

extern "C" int _ZN7HeaveHo6RenderEv(char* c) {
    if (*(int*)(c+0x60) < data_0209f32c) return 1;
    Cls *obj = (Cls*)(c + 0x340);
    obj->method5(0);
    return 1;
}
