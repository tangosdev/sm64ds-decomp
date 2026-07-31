#include "types.h"
extern void _ZN4CP159EnableMPUEv(void);
extern void _ZN4CP1510DisableMPUEv(void);

typedef void (*MPUFunc)(u32*, u32);
extern MPUFunc gMPUCallback;
extern u32 gMPUCallbackArg;
extern u32 gMPUCallbackData;

void func_0205950c(void)
{
    MPUFunc f;
    f = gMPUCallback;
    if (f == 0)
        return;
    asm {
        mov r0, sp
        ldr r1, =0x9f
        msr cpsr_fsxc, r1
        mov sp, r0
    }
    _ZN4CP159EnableMPUEv();
    {
        u32 a1 = gMPUCallbackArg;
        MPUFunc fn = (MPUFunc)gMPUCallback;
        u32* a0 = &gMPUCallbackData;
        fn(a0, a1);
    }
    _ZN4CP1510DisableMPUEv();
}
