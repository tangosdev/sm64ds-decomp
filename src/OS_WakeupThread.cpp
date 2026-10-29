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

struct Manager {
    char padc[0xc];
    struct Node *first;
};

extern struct Manager data_020a6134;

void func_02057f54(void);

void OS_WakeupThread(u16 *self) {
    u32 r4 = IRQ::Disable();
    u32 mask = *self;
    if (mask != 0) {
        struct Node *node = data_020a6134.first;
        if (node != 0) {
            u32 r1 = 1;
            do {
                if (mask & (r1 << node->unk6c)) {
                    node->unk64 = r1;
                }
                node = node->unk68;
            } while (node != 0);
        }
        *self = 0;
        func_02057f54();
    }
    IRQ::Restore(r4);
}
}
