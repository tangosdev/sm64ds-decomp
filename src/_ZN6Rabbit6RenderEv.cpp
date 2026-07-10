//cpp
extern "C" {
extern signed char data_0209f2f8;
extern signed char data_02092120;
extern void func_ov085_0212c150(void* self);
}

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m14(void* arg);
};

extern "C" int _ZN6Rabbit6RenderEv(char* c)
{
    if (*(unsigned char*)(c + 0x428) == 1) return 1;

    {
        int b = (*(int*)(c + 0xb0) & 0x40000) != 0;
        if (b) return 1;
    }

    *(int*)(c + 0x80) = 0x1500;
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);

    {
        int** base = (int**)(((long long)(int)(c + 0x308)) & 0xFFFFFFFFFFFFFFFFLL);
        int* r3 = base[0];
        char* r1 = (char*)base[1];
        for (unsigned int i = 0; i < *(unsigned int*)((char*)r3 + 0x24); i++) {
            *(int*)(r1 + 0x20) = *(int*)(c + 0x468);
            r1 += 0x30;
        }
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3) {
        func_ov085_0212c150(c);
    }

    ((VObj*)(c + 0x300))->m14(c + 0x80);
    return 1;
}
