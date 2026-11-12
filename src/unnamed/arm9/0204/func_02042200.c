#include "nitro/os/thread.h"

/* The file-share parent's loader thread at data_020a11e4: its 0x400-byte
   stack, then its OSThread (0x94 bytes in this SDK; OS_Thread.cpp asserts
   it; also the symbol data_020a15e4, which func_020423c8 wakes), then a
   halfword func_02042200 clears before creating the thread and nothing else
   in src/ reads. The block ends at data_020a1680. func_020421b4 declares the
   same layout. */
typedef struct FileShareLoaderThread {
    u8 stack[0x400]; /* 0x000 */
    OSThread thread; /* 0x400 */
    u16 unk494;      /* 0x494 */
} FileShareLoaderThread;

extern FileShareLoaderThread data_020a11e4;
extern void func_02042254(void *arg);

/* Start the file-share parent's loader thread (func_02042254) at priority 15
   on its 0x400-byte stack, with the block as its argument, and wake it.
   The (int) launder keeps the one base register the ROM uses for every
   address: a plain `&data_020a11e4` lets mwccarm fold base + 0x400 into a
   second literal-pool constant (15 of 21 words differ). */
void func_02042200(void) {
    FileShareLoaderThread *work = (FileShareLoaderThread *)(int)&data_020a11e4;
    work->unk494 = 0;
    OS_CreateThread(&work->thread, func_02042254, work, work->stack + 0x400, 0x400, 0xf);
    OS_WakeupThreadDirect(&work->thread);
}
