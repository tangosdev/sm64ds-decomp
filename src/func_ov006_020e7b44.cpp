//cpp
struct ModelComponents;

struct TextureSequence {
    void Update(ModelComponents& m);
};

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(int a);
};

extern "C" {
    void func_02046208(char* a, int b, int c);
    void func_ov006_020e7b0c(char* c);
    void func_ov006_020e777c(char* c);
}

extern int data_ov006_0213c744[];

extern "C" void func_ov006_020e7b44(char* c)
{
    int* p = (int*)(((int)c + 0x210) & 0xFFFFFFFFFFFFFFFFLL);
    int* d = data_ov006_0213c744;
    if (p[0] == d[0]) {
        if (p[1] == d[1]) return;
        if (*(int*)(c + 0x210) == 0) return;
    }

    func_02046208(c + 0x14, *(unsigned char*)(c + 0x21a), 0);

    *(int*)(c + 0x78) = (unsigned)((*(short*)(c + 0x218)) << 0x11) >> 4;
    ((TextureSequence*)(c + 0x70))->Update(*(ModelComponents*)(c + 0x14));

    func_ov006_020e7b0c(c);

    ((Obj*)(c + 0xc))->m5(0);

    func_ov006_020e777c(c + 0x84);
}
