//cpp
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(int p);
};

extern "C" int func_ov102_0214d278(char* c)
{
    int b = (*(int*)(c + 0xb0) & 0x40000) != 0;
    if (b != 0)
        return 1;
    {
        unsigned char v = *(unsigned char*)(c + 0x3c6);
        if (v < 0x2d) {
            if (v & 1)
                return 1;
        }
    }
    ((Obj*)(c + 0x300))->m5(0);
    return 1;
}
