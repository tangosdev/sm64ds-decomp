//cpp
class Sub {
public:
    virtual void F0();
    virtual void F1();
    virtual void F2();
    virtual void F3();
    virtual void F4();
    virtual void F5(int n);
};

extern "C" int _ZN11CrazedCrate6RenderEv(char *c) {
    int r1 = *(int*)(c + 0xb0);
    r1 = r1 & 0x40000;
    r1 = r1 ? 1 : 0;
    if (r1) return 1;
    Sub *sub = (Sub*)(c + 0xd4);
    sub->F5(0);
    return 1;
}
