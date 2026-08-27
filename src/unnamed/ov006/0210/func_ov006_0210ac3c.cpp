//cpp
extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);
extern "C" void func_ov004_020b1ba0(void* c, int delta);
extern "C" void func_ov004_020b1b78(void* c, int val);

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

struct Obj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int a);
    virtual void v19();
};

extern "C" void func_ov006_0210ac3c(char* c)
{
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        if (*(int*)(c + 0x5000) == 3) {
            if (*(int*)(c + 0xb4) < 0x270f)
                *(int*)(((int)c + 0xb4)) += 1;
            if (*(int*)(c + 0xb4) > *(int*)(c + 0xb8))
                *(int*)(c + 0xb8) = *(int*)(c + 0xb4);
            ((Obj*)c)->v18(4);
        } else {
            if (*(int*)(c + 0xa8) <= 0) {
                *(unsigned char*)(c + 0x503e) = 0x78;
                *(int*)(c + 0x5000) = 6;
            } else {
                ((Obj*)c)->v18(5);
            }
        }
    }

    if (*(int*)(c + 0x5000) == 3) {
        if (*(unsigned char*)(c + 0x503e) != 0x14)
            return;
        if (*(int*)(c + 0x5014) <= 0)
            return;
        *(int*)(((int)c + 0x5014)) -= 1;
        func_ov004_020b1ba0(c, 1);
        Sound::PlayBank2_2D(0x149);
        *(unsigned char*)(((int)c + 0x503e)) += 5;
        return;
    }

    if (*(int*)(c + 0x5000) != 4)
        return;

    {
        unsigned char b = *(unsigned char*)(c + 0x503e);
        if (b != 0x14 && b != 0x28 && b != 0x3c)
            return;
    }

    func_ov004_020b1b78(c, 1);
    Sound::PlayBank2_2D(0x14a);
}
