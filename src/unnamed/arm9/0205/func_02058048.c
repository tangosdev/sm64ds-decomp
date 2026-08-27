#include "types.h"
struct Obj {
    char pad[0x64];
    u32 unk64;
};

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void func_02057f54(void);

void func_02058048(struct Obj *self) {
    u32 r4 = _ZN3IRQ7DisableEv();
    self->unk64 = 1;
    func_02057f54();
    _ZN3IRQ7RestoreEj(r4);
}
