//cpp
#include "IRQ.h"

extern "C" {

extern void OS_SleepThread(void *);
extern char data_020a8180[];

int func_0205ffd0(void)
{
    int *g = (int *)data_020a8180;
    unsigned int saved = IRQ::Disable();
    if (g[13] & 4) {
        do {
            OS_SleepThread((char *)g + 0xd4);
        } while (g[13] & 4);
    }
    IRQ::Restore(saved);
    return *(int *)g[0] == 0;
}
}
