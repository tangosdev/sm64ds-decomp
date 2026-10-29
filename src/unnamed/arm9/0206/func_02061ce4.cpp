//cpp
#include "IRQ.h"

extern "C" {

extern int func_0206152c(void);
extern void *WM_GetSystemWork(void);

int func_02061ce4(int x)
{
    int token = IRQ::Disable();
    int r = func_0206152c();
    if (r != 0) {
        IRQ::Restore(token);
        return r;
    }
    *(int *)((unsigned char *)WM_GetSystemWork() + 0xa8) = x;
    IRQ::Restore(token);
    return 0;
}
}
