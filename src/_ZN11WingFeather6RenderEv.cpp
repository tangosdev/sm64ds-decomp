//cpp
struct Sub {
    virtual ~Sub() {}
    virtual void a() {}
    virtual void b() {}
    virtual void c() {}
    virtual int f4(int) = 0;
};
extern "C" int _ZN11WingFeather6RenderEv(void *obj) {
    unsigned char b = *(unsigned char*)((char*)obj + 0x384);
    if (b < 0x2d) {
        if (b & 1) return 1;
    }
    Sub *sub = (Sub*)((char*)obj + 0xd4);
    sub->f4(0);
    return 1;
}
