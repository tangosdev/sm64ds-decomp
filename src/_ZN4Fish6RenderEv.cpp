//cpp
struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};
extern "C" int _ZN4Fish6RenderEv(char *c) {
    if (*(unsigned char*)(c + 0x159) == 0) {
        Base *b = (Base*)(c + 0xd4);
        b->m(0);
    }
    return 1;
}
