//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

struct Obj {
    char pad[0x64];
    u32 unk64;
};

void func_02057f54(void);

void func_02058048(struct Obj *self) {
    u32 r4 = IRQ::Disable();
    self->unk64 = 1;
    func_02057f54();
    IRQ::Restore(r4);
}
}
