//cpp
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};
extern "C" void func_ov006_02107d80(char* c){
    ((Sub*)(c + 0x10))->m(0);
    ((Sub*)(c + 0x60))->m(0);
}
