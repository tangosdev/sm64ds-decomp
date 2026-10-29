//cpp
#pragma long_calls on
#include "types.h"
#include "IRQ.h"

extern "C" {

typedef struct UnkStruct_0205d4a0 {
    char pad0[0x10];
    int unk10;
    char pad14[0x18];
    int unk2c;
    int unk30;
    short unk34;
    short unk36;
} UnkStruct_0205d4a0;

typedef struct Obj Obj;

typedef struct Owner {
    int pad[6];
    Obj *cur; /* 0x18 */
} Owner;

struct Obj {
    int f0;
    int f4;
    Owner *owner; /* 0x08 */
    u32 flags;    /* 0x0c */
    int f10;
    int f14;              /* 0x14 */
    unsigned short queue; /* 0x18 */
};

extern void func_0205cdf4(UnkStruct_0205d4a0 *self, int a);
extern void OS_SleepThread(unsigned short *self);
extern int func_0205cf5c(Obj *);
extern void func_0205d688(void *, int, int, int);

void func_0205d4a0(UnkStruct_0205d4a0 *self, int a, int b) {
    if (self->unk10 != 5) {
        self->unk34 = 0;
        self->unk36 = 0;
    }
    self->unk2c = a;
    self->unk30 = b;
    func_0205cdf4(self, 5);
}

int func_0205d3f4(Obj *t) {
    int woken = 0;
    u32 irq = IRQ::Disable();
    int busy = (t->flags & 1) ? 1 : 0;
    if ((int)((long long)busy) != 0) {
        woken = (t->owner->cur != t) ? 1 : 0;
        if (woken) {
            *(u32 *)(int)((int)t + 0xc) |= 4;
        }
        do {
            OS_SleepThread(&t->queue);
            busy = (t->flags & 1) ? 1 : 0;
        } while ((int)((long long)busy) != 0);
    }
    IRQ::Restore(irq);
    if (woken) {
        return func_0205cf5c(t);
    }
    return t->f14 == 0;
}

void func_0205d3e4(void *a, int b, int c) {
    func_0205d688(a, b, c, 1);
}
}
