#include "types.h"
struct Obj {
    char pad[0x64];
    u32 unk64;
    char pad2[0x28];
    u16 unk90;
};

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void OS_SleepThread(u16 *self);

void func_0205816c(struct Obj *self) {
    u32 r4 = _ZN3IRQ7DisableEv();
    u32 r1 = self->unk64;
    if (r1 != 2) {
        OS_SleepThread(&self->unk90);
    }
    _ZN3IRQ7RestoreEj(r4);
}
