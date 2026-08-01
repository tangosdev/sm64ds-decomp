#include "types.h"
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void func_02035860(char *o, void *src);
extern void func_ov002_020e9464(char *p);
extern void func_ov002_020e7454(char *self);
extern void func_ov002_020e9448(unsigned char *p);

void func_ov002_020e8abc(char *self)
{
    char *a;

    a = (char *)_ZN5Actor10FindWithIDEj(*(unsigned int *)(self + 0x434));
    if (a == 0) {
        _ZN9ActorBase18MarkForDestructionEv(self);
        return;
    }

    if (*(int *)(self + 0x444) == 9) {
        u16 *f;
        *(int *)(self + 0x5c) = *(int *)(self + 0x454);
        *(int *)(self + 0x60) = *(int *)(self + 0x458);
        *(int *)(self + 0x64) = *(int *)(self + 0x45c);
        func_02035860(self + 0x150, self + 0x5c);
        *(int *)(self + 0x440) = *(int *)(self + 0x444);
        func_ov002_020e9464(self);
        *(int *)(((long long)(int)(self + 0x128))) |= 1;
        f = (u16 *)(((long long)(int)(self + 0x4a2)));
        *f &= ~2;
        *f |= 8;
        *f &= ~0x30;
        if (*(int *)(a + 8) & 0x20) {
            return;
        }
        {
            u8 *q = (u8 *)(((long long)(int)(a + 0x1db)));
            *q &= ~1;
            *q |= 2;
        }
        *(int *)(a + 0x1d0) = 0;
        return;
    }

    {
        unsigned long fl = *(u16 *)(self + 0x4a2);
        if ((fl << 0x19) >> 0x1f) {
            func_ov002_020e7454(self);
            return;
        }
    }

    *(int *)(self + 0x5c) = *(int *)(self + 0x454);
    *(int *)(self + 0x60) = *(int *)(self + 0x458);
    *(int *)(self + 0x64) = *(int *)(self + 0x45c);
    func_02035860(self + 0x150, self + 0x5c);
    *(int *)(self + 0x440) = *(int *)(self + 0x444);
    *(int *)(self + 0xa8) = 0x20000;
    func_ov002_020e9448((unsigned char *)self);
    *(u16 *)(self + 0x100) = 0xf;
}
