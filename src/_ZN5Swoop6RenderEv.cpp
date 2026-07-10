//cpp
struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

extern "C" int _ZN5Swoop6RenderEv(char *c);
extern "C" int _ZN5Swoop6RenderEv(char *c)
{
    int flag = (*(int *)(c + 0xb0) & 0x40000) != 0;
    if (flag != 0) return 1;
    if (*(unsigned char *)(c + 0x43c) == 1) {
        Base *b = (Base *)(c + 0x300);
        b->m(0);
    } else {
        Base *b = (Base *)(c + 0x364);
        b->m(0);
    }
    return 1;
}
