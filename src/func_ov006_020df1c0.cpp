//cpp
extern "C" void func_ov004_020b0a54(int c);

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

extern "C" void func_ov006_020df1c0(char* c)
{
    *(int*)(((int)c + 0x541c) & 0xFFFFFFFFFFFFFFFF) -= 1;
    if (*(int*)(c + 0x5000 + 0x41c) > 0) return;
    if (*(unsigned char*)(c + 0x5000 + 0x469) != 0) {
        if (*(int*)(c + 0xb4) < 0x270f) *(int*)(((int)c + 0xb4) & 0xFFFFFFFFFFFFFFFF) += 1;
        if (*(int*)(c + 0xb4) > *(int*)(c + 0xb8)) *(int*)(c + 0xb8) = *(int*)(c + 0xb4);
        func_ov004_020b0a54(0);
        *(int*)(c + 0x5000 + 0x418) = 7;
    } else {
        if (*(int*)(c + 0xa8) > 0) {
            ((Obj*)c)->v18(-1);
        } else {
            func_ov004_020b0a54(0x12);
            *(int*)(c + 0x5000 + 0x418) = 7;
        }
    }
}
