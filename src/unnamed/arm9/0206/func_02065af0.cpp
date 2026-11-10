//cpp
#include "nitro/os/system.h"
#include "private/arm9_wbt.h"

extern "C" {

extern WBTCommand *func_02065b88(void);
extern void MIi_CpuCopy32(int *src, int *dst, int len);
extern int data_020a94c0;
extern int data_020a9570;

int func_02065af0(int *thiz)
{
    OSIntrMode saved = OS_DisableInterrupts();
    WBTCommand *r;
    if (data_020a94c0 != 1) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    r = func_02065b88();
    if (r == 0 || r->command != 0) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    MIi_CpuCopy32(thiz, &data_020a9570, 0x9c);
    OS_RestoreInterrupts(saved);
    return 1;
}
}
