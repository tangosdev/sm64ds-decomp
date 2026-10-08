// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (msr cpsr_fsxc: notes/asm-policy.md restored-ten table, 2026-07-25
// correction), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#include "types.h"
extern void _ZN4CP159EnableMPUEv(void);
extern void _ZN4CP1510DisableMPUEv(void);

typedef void (*MPUFunc)(u32*, u32);
extern MPUFunc data_020a63a0;
extern u32 data_020a63a4;
extern u32 data_020a63ac;

void func_0205950c(void)
{
    MPUFunc f;
    f = data_020a63a0;
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
        u32 a1 = data_020a63a4;
        MPUFunc fn = (MPUFunc)data_020a63a0;
        u32* a0 = &data_020a63ac;
        fn(a0, a1);
    }
    _ZN4CP1510DisableMPUEv();
}
