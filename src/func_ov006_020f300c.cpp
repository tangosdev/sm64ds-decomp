//cpp
#pragma opt_strength_reduction off

class C {
public:
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
    virtual void v18(int);
};

typedef void (C::*PMF)(int);

extern "C" PMF data_ov006_02142254[];

extern "C" void func_ov006_020f0044(void *c);
extern "C" void func_ov006_020f10ec(void *q);
extern "C" void func_ov006_020f0274(void *s);
extern "C" void func_ov004_020b0a54(int c);

extern "C" void func_ov006_020f300c(char *o)
{
    int matches;
    int i;

    func_ov006_020f0044(o);
    func_ov006_020f10ec(o);
    func_ov006_020f0274(o);

    matches = 0;
    for (i = 0; i < 0x78; i++) {
        if (*(unsigned char *)(o + i + 0x52ed) != 0) {
            int x = *(unsigned char *)(o + i + 0x51fd);
            if (x == 9) {
                matches++;
                (((C *)o)->*data_ov006_02142254[x])(i);
            }
        }
    }
    if (matches != 0)
        return;

    for (i = 0; i < 0x78; i++) {
        unsigned char *p = (unsigned char *)(((int)o + i + 0x53dd) & 0xFFFFFFFFFFFFFFFFLL);
        if (*p == 1)
            *p = 0;
    }

    if (*(unsigned short *)(o + 0x516a) == 0)
        return;
    *(unsigned short *)(o + 0x5164) = 1;
    *(unsigned short *)(((int)o + 0x516a) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (*(short *)(o + 0x516a) > 0)
        return;
    *(unsigned short *)(o + 0x516a) = 0;
    *(unsigned short *)(o + 0x5164) = 0;

    if (*(unsigned char *)(o + 0x5459) != 0) {
        int v = *(int *)(o + 0xbc);
        while (v >= 5)
            v -= 5;
        if (v != 4)
            ((C *)o)->v18(-1);
        else
            func_ov004_020b0a54(1);
    } else {
        func_ov004_020b0a54(0x12);
    }
    *(unsigned char *)(o + 0xc3) = 0;
}
