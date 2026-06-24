//cpp
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(void* p);
};
struct G2 { int w[2]; };
extern G2 data_ov084_02130df4;

extern "C" int func_ov084_0212fcdc(char* c)
{
    if (*(int*)(c + 0x80) == 0)
        return 1;
    ((Obj*)(c + 0x110))->m5(c + 0x80);
    if (*(int*)(c + 0x170) == data_ov084_02130df4.w[1])
        ((Obj*)(c + 0x174))->m5(c + 0x428);
    return 1;
}
