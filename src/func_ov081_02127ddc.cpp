//cpp
class Sub {
public:
    virtual void F0();
    virtual void F1();
    virtual void F2();
    virtual void F3();
    virtual void F4();
    virtual void F5(int* v);
};

extern "C" int func_ov081_02127ddc(char *c) {
    int v[3];
    v[0] = *(int*)(c + 0x358);
    v[1] = *(int*)(c + 0x358);
    v[2] = *(int*)(c + 0x358);
    Sub *sub = (Sub*)(c + 0xd4);
    sub->F5(v);
    return 1;
}
