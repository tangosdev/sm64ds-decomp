//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

extern void func_020589ac(void* node, void* self);
extern void OS_SleepThread(void* self);

struct Node {
    char pad_0[0x78];
    void* f78;
};

struct Obj {
    char pad_0[4];
    struct Node* f4;
    int f8;
};

struct Globals { char pad_0[8]; struct Node* f8; };
extern struct Globals data_020a6134;

void func_02058b08(struct Obj* self)
{
    u32 saved = IRQ::Disable();
    struct Node* cur = data_020a6134.f8;

    for (;;) {
        if (self->f4 == 0) {
            self->f4 = cur;
            *(int*)(((int)self + 8)) += 1;
            func_020589ac(cur, self);
            break;
        }
        if (self->f4 == cur) {
            *(int*)(((int)self + 8)) += 1;
            break;
        }
        cur->f78 = self;
        OS_SleepThread(self);
        cur->f78 = 0;
    }

    IRQ::Restore(saved);
}
}
