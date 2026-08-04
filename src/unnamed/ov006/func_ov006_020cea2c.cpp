//cpp
extern "C" {
void AddVec3(void *a, void *b, void *c);
void _ZN9Animation7AdvanceEv(void *anim);
}

struct C;
typedef void (C::*PMF)();

extern "C" void func_ov006_020cea2c(char *c)
{
    PMF *pp = (PMF *)c;
    (((C *)c)->**pp)();
    AddVec3(c + 8, c + 0x2c, c + 8);
    _ZN9Animation7AdvanceEv(c + 0x194);
    *(int *)(c + 0x14) = *(int *)(c + 8);
    *(int *)(c + 0x18) = *(int *)(c + 0xc);
    *(int *)(c + 0x1c) = *(int *)(c + 0x10);
    *(int *)(c + 0x50) = *(int *)(c + 0x38);
    *(int *)(c + 0x54) = *(int *)(c + 0x3c);
    *(int *)(c + 0x58) = *(int *)(c + 0x40);
    *(int *)(c + 0x5c) = *(int *)(c + 0x44);
    *(int *)(c + 0x60) = *(int *)(c + 0x48);
    *(int *)(c + 0x64) = *(int *)(c + 0x4c);
}
