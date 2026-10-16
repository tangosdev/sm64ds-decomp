#include "types.h"
struct Storage {
    void *a;
    void *b;
};

struct Obj {
    char data[20];
};

extern struct Storage data_020a8064;
extern struct Storage data_020a806c;
static struct Obj data_0209a7d8 = { {
    0x7a, 0x42, 0x80, 0x27, 0x13, 0xad, 0xc5, 0x9f, 0x06, 0xec,
    0xf7, 0xdb, 0x7e, 0xf7, 0xa1, 0x24, 0x99, 0xb3, 0xd3, 0xe7
} };

u32 func_0205e904(u32 newVal);
u32 func_0205de9c(struct Obj *obj, void *a, void *b);
void Crash(void);
u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);

void func_0205db88(u32 r0, void *r1, void *r2) {
    struct Storage *r5;
    void *r7 = r1;
    void *r6 = r2;
    if (r0 == 0) {
        r5 = &data_020a8064;
    } else {
        r5 = &data_020a806c;
    }
    if (r7 != 0 && r0 == 0) {
        u32 r4 = func_0205e904(1);
        if (!func_0205de9c(&data_0209a7d8, r7, r6)) {
            Crash();
        }
        func_0205e904(r4);
    }
    u32 irqState = _ZN3IRQ7DisableEv();
    r5->a = r7;
    r5->b = r6;
    _ZN3IRQ7RestoreEj(irqState);
}
