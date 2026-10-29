//cpp
#include "IRQ.h"

extern "C" {

int IPCSend(int cmd, int a, int b);

typedef struct {
    void (*fp)(int,int,int);
    char pad[0x34 - 4];
    volatile unsigned short s34;
    volatile unsigned short s36;
} G_t;
extern G_t data_020a80cc;

void func_0205f040(void)
{
    unsigned int saved = IRQ::Disable();
    int ok = (IPCSend(6, 0x3000000, 0) >= 0);
    if (!ok) {
        void (*fp)(int,int,int);
        IRQ::Restore(saved);
        fp = data_020a80cc.fp;
        data_020a80cc.s34 |= 1;
        if (fp == 0) return;
        fp(0, 4, 0);
        return;
    }
    data_020a80cc.s36 |= 1;
    data_020a80cc.s34 &= ~1;
    IRQ::Restore(saved);
}
}
