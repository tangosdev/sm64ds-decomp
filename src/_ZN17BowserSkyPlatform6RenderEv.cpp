//cpp
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};
extern "C" int _ZN17BowserSkyPlatform6RenderEv(unsigned char* c) {
    if (*(int*)(c + 0x170) != 0) return 1;
    if (*(unsigned char*)(c + 0x1ae) < 8) return 1;
    ((Sub*)(c + 0xd4))->m(0);
    return 1;
}
