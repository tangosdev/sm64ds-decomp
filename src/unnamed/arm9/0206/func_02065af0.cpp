//cpp
#include "IRQ.h"
#include "private/arm9_wbt.h"

extern "C" {

extern WBTCommand *func_02065b88(void);
extern void MultiCopy_Int(int *src, int *dst, int len);
extern int data_020a94c0;
extern int data_020a9570;

int func_02065af0(int *thiz)
{
    int saved = IRQ::Disable();
    WBTCommand *r;
    if (data_020a94c0 != 1) {
        IRQ::Restore(saved);
        return 0;
    }
    r = func_02065b88();
    if (r == 0 || r->command != 0) {
        IRQ::Restore(saved);
        return 0;
    }
    MultiCopy_Int(thiz, &data_020a9570, 0x9c);
    IRQ::Restore(saved);
    return 1;
}
}
