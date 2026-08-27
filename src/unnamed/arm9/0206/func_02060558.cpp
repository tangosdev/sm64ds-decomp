//cpp
#include "types.h"
extern "C" {
extern char data_020a8180[];
extern int data_020a6134[];
void OS_SleepThread(void* p);
void func_02060228(void* fn);
int func_0206071c(void* g);
}
namespace IRQ { unsigned int Disable(); void Restore(unsigned int); }

struct Sched {
    char pad00[0x18];
    int a;      // 0x18
    int b;      // 0x1c
    int c;      // 0x20
    char pad24[0x28-0x24];
    int d;      // 0x28
    int e;      // 0x2c
    char pad30[0x34-0x30];
    u32 flags;  // 0x34
    char pad38[0xd4-0x38];
    char sub[4]; // 0xd4
};

extern "C" int func_02060558(int a, int b, int c, int d, int e, int f)
{
    Sched* g = (Sched*)data_020a8180;
    unsigned int irq = IRQ::Disable();
    if (g->flags & 4) {
        do {
            OS_SleepThread(g->sub);
        } while (g->flags & 4);
    }
    u32* fp = (u32*)((unsigned long long)((int)g + 0x34));
    *fp |= 4;
    IRQ::Restore(irq);
    g->a = a;
    g->b = b;
    g->c = c;
    g->d = d;
    g->e = e;
    if (f != 0) {
        func_02060228((void*)func_0206071c);
        return 1;
    }
    *(int*)(data_020a8180 + 0xd0) = data_020a6134[2];
    func_0206071c(g);
    return (*(int**)g)[0] == 0;
}
