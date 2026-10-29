//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

struct Storage {
    void *a;
    void *b;
};

struct Obj {
    char data[1];
};

extern struct Storage data_020a8064;
extern struct Storage data_020a806c;
extern struct Obj data_0209a7d8;

u32 func_0205e904(u32 newVal);
u32 func_0205de9c(struct Obj *obj, void *a, void *b);
void Crash(void);

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
    u32 irqState = IRQ::Disable();
    r5->a = r7;
    r5->b = r6;
    IRQ::Restore(irqState);
}
}
