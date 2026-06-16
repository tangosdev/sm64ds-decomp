//cpp
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};
extern "C" int func_ov070_021201c0(char *c) {
    int b = ((*(unsigned int*)(c+0xb0) & 0x40000) != 0);
    if (b) return 1;
    Obj *o = (Obj*)(c+0x300);
    o->Target(0);
    return 1;
}
