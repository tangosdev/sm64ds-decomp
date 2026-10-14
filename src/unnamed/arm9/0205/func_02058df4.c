#include "types.h"
extern char data_023c0000;
/* NitroSDK linker constants: the address is the value (the LCF assigns
 * SDK_SYS_STACKSIZE = 0 and SDK_IRQ_STACKSIZE = 0x600). */
extern void SDK_IRQ_STACKSIZE(void);
extern void SDK_SYS_STACKSIZE(void);

int func_02058df4(unsigned int idx)
{
    switch (idx) {
    case 0:
        return 0x23c0000;
    case 2:
        return 0x2700000;
    case 3:
        return 0x2000000;
    case 4: {
        u32 irqStackLo = (u32)&data_023c0000 + 0x3f80 - (s32)SDK_IRQ_STACKSIZE;
        u32 sysStackLo;

        if (!(s32)SDK_SYS_STACKSIZE) {
            sysStackLo = (u32)&data_023c0000;
            if (sysStackLo < 0x23c0020) {
                sysStackLo = 0x23c0020;
            }
        } else if ((s32)SDK_SYS_STACKSIZE < 0) {
            sysStackLo = (u32)&data_023c0000 - (s32)SDK_SYS_STACKSIZE;
        } else {
            sysStackLo = irqStackLo - (s32)SDK_SYS_STACKSIZE;
        }
        return sysStackLo;
    }
    case 5:
        return 0x27ff800;
    case 6:
        return 0x37f8000;
    default:
        return 0;
    }
}
