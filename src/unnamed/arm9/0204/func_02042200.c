#include "nitro/os/thread.h"

extern char data_020a11e4[];
extern void func_02042254(void);

/* Start the file-share parent's loader thread (func_02042254).
   data_020a11e4 is its 0x400-byte stack, and the OSThread follows at +0x400
   (also the symbol data_020a15e4, which func_020423c8 wakes).
   OS_CreateThread takes (thread, entry, arg, stack top, stack size,
   priority 15 here), and OS_WakeupThreadDirect then wakes the thread.
   The halfword at +0x494 lies just past the OSThread (0x94 bytes in this SDK;
   OS_Thread.cpp asserts it), still inside the block that ends at
   data_020a1680. It is cleared here before OS_CreateThread runs, and nothing
   else in src/ reads it. The (int) launder keeps the one base register the
   ROM uses for every address: `char *base = data_020a11e4` and direct
   data_020a11e4[0x400] addressing both fold base + 0x400 into a second
   literal-pool constant (15 of 21 words differ). */
void func_02042200(void) {
    char *base = (char *)(int)data_020a11e4;
    *(short *)(base + 0x494) = 0;
    OS_CreateThread((OSThread *)&base[0x400], (void (*)(void *))func_02042254, base, &base[0x400],
                    0x400, 0xf);
    OS_WakeupThreadDirect((OSThread *)&base[0x400]);
}
