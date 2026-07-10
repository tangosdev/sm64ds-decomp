//cpp
struct A;
struct B {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(A* arg);
};

extern "C" int _ZN10ChainChomp6RenderEv(char *c) {
    B *b = (B*)(c+0x150);
    b->m5((A*)(c+0x80));
    
    int j = 0;
    char *p2 = c + 0x1dc;
    for (;;) {
        B *b2 = (B*)p2;
        b2->m5((A*)0);
        j++;
        p2 += 0x50;
        if (j >= 7) break;
    }
    return 1;
}
