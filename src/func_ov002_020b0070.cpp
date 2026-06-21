//cpp
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void func5(int x);
};

extern "C" {

unsigned int func_ov002_020b0070(char *self)
{
    if (*(unsigned char *)(self + 0x38e) == 0 || *(unsigned char *)(self + 0x38f) == 0)
        return 1;
    {
        int b = (*(unsigned int *)(self + 0xb0) & 0x40000) ? 1 : 0;
        if (b)
            return 1;
    }
    {
        Obj *o = (Obj *)(self + 0x300);
        o->func5(0);
    }
    return 1;
}

}
