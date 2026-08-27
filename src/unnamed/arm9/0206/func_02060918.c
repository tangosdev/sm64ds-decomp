#include "types.h"
struct S8180 {
    char pad0[0x18];
    u32 f18;
    u32 f1c;
    u32 f20;
    u32 f24;
    u32 f28;
    u32 f2c;
    char pad30[4];
    u32 f34;
    char pad38[0xd0 - 0x38];
    u32 fd0;
    char fd4;
};

struct S6134 {
    char pad[8];
    u32 f8;
};

extern struct S8180 data_020a8180;
extern char data_020a8780;
extern struct S6134 data_020a6134;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32);

void OS_SleepThread(void *p);
void func_02059f58(int a);
int func_02060b64(char *p);
void func_0206090c(void);
void func_02060228(void *fn);
void func_02060a30(struct S8180 *p);

#pragma opt_propagation off
void func_02060918(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, int a6)
{
    struct S8180 *t = &data_020a8180;
    char *cb = &data_020a8780;
    u32 mask;

    mask = _ZN3IRQ7DisableEv();
    while (t->f34 & 4) {
        OS_SleepThread(&t->fd4);
    }
    {
        u32 *p34 = (u32 *)(((int)t + 0x34));
        *p34 |= 4;
    }
    _ZN3IRQ7RestoreEj(mask);

    t->f24 = a0;
    t->f18 = a1;
    t->f1c = a2;
    t->f20 = a3;
    t->f28 = a4;
    t->f2c = a5;

    if (a0 <= 3)
        func_02059f58(a0);

    if (func_02060b64(cb)) {
        if (a6 != 0)
            return;
        func_0206090c();
        return;
    }

    if (a6 != 0) {
        func_02060228((void *)func_02060a30);
        return;
    }

    t->fd0 = data_020a6134.f8;
    func_02060a30(t);
}
