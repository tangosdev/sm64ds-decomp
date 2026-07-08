//cpp
extern int WATER_HEIGHT;

struct Cls {
    virtual void method0();
    virtual void method1();
    virtual void method2();
    virtual void method3();
    virtual void method4();
    virtual void method5(int);  /* at vtable offset 0x14 */
};

extern "C" int func_ov077_02126e38(char* c) {
    if (*(int*)(c+0x60) < WATER_HEIGHT) return 1;
    Cls *obj = (Cls*)(c + 0x340);
    obj->method5(0);
    return 1;
}
