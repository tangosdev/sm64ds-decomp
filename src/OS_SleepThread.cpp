//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

struct Node {
    char pad[0x64];
    u32 unk64;
    struct Node *unk68;
    u32 unk6c;
};

struct NodeMgr {
    struct Node *node;
};

extern struct NodeMgr *data_020a612c;

void func_02057f54(void);

void OS_SleepThread(u16 *self) {
    u32 r4 = IRQ::Disable();
    struct Node *r2 = data_020a612c->node;
    if (self != 0) {
        u32 r0 = 1 << r2->unk6c;
        u16 r1 = *self;
        *self = r1 | (u16)r0;
    }
    r2->unk64 = 0;
    func_02057f54();
    IRQ::Restore(r4);
}
}
