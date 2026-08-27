//cpp
extern "C" {

struct Sub {
    virtual int v0();
    virtual int v1();
    virtual int m2();
    virtual int v3();
    virtual int v4();
    virtual int m5();
};

int func_02034d34(char *thiz)
{
    int r = ((Sub *)(thiz + 0x50))->m5();
    if (r) return r;
    return ((Sub *)(thiz + 0x50))->m2();
}

}
