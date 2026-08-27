//cpp
struct Obj {
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
    virtual int v7();
};
extern "C" {
extern Obj* data_0209f5bc;
extern int data_0209d4b0;
}
extern "C" void FUN_02029934(void) {
    Obj* o = data_0209f5bc;
    if (o->v7() == 0) {
        data_0209d4b0 = 0;
        *(volatile unsigned short*)0x4000050 = 0;
    }
}
