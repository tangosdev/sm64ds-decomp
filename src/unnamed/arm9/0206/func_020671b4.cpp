//cpp
#include "IRQ.h"

extern "C" {

extern int func_02065edc(unsigned int x);
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

extern char *data_020a9db8;

void *func_020671b4(unsigned int idx)
{
    unsigned int saved = IRQ::Disable();
    if (data_020a9db8 != 0 && func_02065edc(idx) != 0) {
        char *base = data_020a9db8;
        CpuCopy8(base + 0x1340 + (idx - 1) * 0x16, base + 0x1772, 0x16);
        IRQ::Restore(saved);
        return data_020a9db8 + 0x1772;
    }
    IRQ::Restore(saved);
    return 0;
}
}
