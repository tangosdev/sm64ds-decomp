//cpp
extern "C" {
struct V { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual int m5(int); };
int _ZN5Stump6RenderEv(char *c)
{
    if (*(int*)(c+0x374) == 1) return 1;
    int b = (*(int*)(c+0xb0) & 0x40000) != 0;
    if (b) return 1;
    ((V*)(c+0x300))->m5(0);
    return 1;
}
}
