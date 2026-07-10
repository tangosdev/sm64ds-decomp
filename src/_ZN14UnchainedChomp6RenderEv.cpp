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

extern "C" int _ZN14UnchainedChomp6RenderEv(char *c) {
    B *b = (B*)(c+0x30c);
    b->m5((A*)(c+0x80));
    
    int j = 0;
    char *p2 = c + 0x370;
    for (;;) {
        B *b2 = (B*)p2;
        b2->m5((A*)0);
        j++;
        p2 += 0x50;
        if (j >= 5) break;
    }
    return 1;
}
