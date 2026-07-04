//cpp
typedef unsigned short u16;

struct VObj {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void m5(int);
};

extern "C" {
void _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}

extern "C" int func_ov002_020f0ea8(char* c)
{
    if (*(u16*)(c + 0x100 + 0x4c) != 0) {
        *(u16*)(((long long)(int)(c + 0x14c)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
        return 1;
    }
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x124, c + 0xdc);
    ((VObj*)(c + 0xd4))->m5(0);
    return 1;
}
