//cpp
extern "C" void func_02012e1c(char *c);
extern "C" void func_ov004_020b91fc(char *c);
extern "C" void Enable3dEngines();

struct Base {
    virtual void d0();
    virtual void d1();
    virtual void fn2();
    virtual void fn3();
    virtual void fn4();
    virtual void fn5();
    virtual void fn6();
    virtual void fn7();
    virtual void fn8();
    virtual void fn9();
    virtual void fn10();
    virtual void fn11();
    virtual void fn12();
    virtual void fn13();
    virtual void fn14();
    virtual void fn15();
    virtual void fn16();
    virtual void fn17();
    virtual void fn18();
    virtual void fn19();
    virtual void fn20();
    virtual void fn21();
    virtual void fn22();
    virtual void fn23();
    virtual void fn24();
    virtual void fn25();
    virtual int Init();
};

struct Obj : Base {
    virtual int Init();
    char pad[0x4627];
};

extern "C" void func_ov004_020af04c(Obj *self) {
    func_02012e1c((char*)self);
    *(int*)((char*)self + 0x4628) = 0;
    func_ov004_020b91fc((char*)self + 0xf4);
    int r = self->Init();
    if (r == 0) return;
    Enable3dEngines();
}
