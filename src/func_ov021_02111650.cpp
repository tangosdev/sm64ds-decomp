//cpp
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};
extern "C" int func_ov021_02111650(char *c) {
    unsigned short v = *(unsigned short*)(c+0xc00+0x74);
    if (v < 0x2d && (v & 1)) return 1;
    Obj *o = (Obj*)(c+0xd4);
    o->Target(0);
    int arg = 0;
    int i = 0;
    char *p = c+0x320;
    do {
        Obj *o2 = (Obj*)p;
        o2->Target(arg);
        i++;
        p += 0x50;
    } while (i < 4);
    return 1;
}
