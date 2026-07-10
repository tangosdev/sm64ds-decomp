//cpp
extern "C" {
struct Mtx { int w[12]; };
extern void Matrix4x3_FromTranslation(Mtx* m, int x, int y, int z);
}

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

struct Elem { int w[3]; };

extern "C" int _ZN11PyramidLift6RenderEv(char* c){
    {
        Sub* s = (Sub*)(c + 0xd4);
        s->m(0);
    }
    int i = *(unsigned char*)(c + 0x3f8);
    if (i < 0xa) {
        Elem* e = (Elem*)(c + i * 0xc);
        do {
            Matrix4x3_FromTranslation((Mtx*)(c + 0x33c),
                e->w[(0x37c)/4] >> 3,
                e->w[(0x380)/4] >> 3,
                e->w[(0x384)/4] >> 3);
            Sub* s2 = (Sub*)(c + 0x320);
            s2->m(0);
            i++;
            e = (Elem*)((char*)e + 0xc);
        } while (i < 0xa);
    }
    return 1;
}
