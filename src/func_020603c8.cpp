//cpp
#include "types.h"
extern "C" {
extern char data_020a8180[];
extern int data_020a6134[];
void OS_SleepThread(void* p);
void func_02060364(int arg);
void func_020607e8(void* thiz);
}
namespace IRQ { unsigned int Disable(); void Restore(unsigned int); }

struct Sched {
    char pad00[0x28];
    int d;      // 0x28
    int e;      // 0x2c
    char pad30[0x34-0x30];
    u32 flags;  // 0x34
    char pad38[0xd4-0x38];
    char sub[4]; // 0xd4
};

extern "C" void func_020603c8(int arg)
{
    Sched* g = (Sched*)data_020a8180;
    unsigned int irq = IRQ::Disable();
    if (g->flags & 4) {
        do {
            OS_SleepThread(g->sub);
        } while (g->flags & 4);
    }
    u32* fp = (u32*)(((unsigned long long)((int)g + 0x34)) & 0xFFFFFFFFFFFFFFFFULL);
    *fp |= 4;
    IRQ::Restore(irq);
    func_02060364(arg);
    g->d = 0;
    g->e = 0;
    *(int*)(data_020a8180 + 0xd0) = data_020a6134[2];
    func_020607e8(g);
}
