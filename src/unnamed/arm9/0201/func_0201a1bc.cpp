//cpp
#include "Memory.h"
#include "nitro/os/thread.h"

extern "C" {

extern unsigned char data_0209d4f8;
extern OSThread data_0209d5b8;
extern void *data_0209d518;

/* Whether the background job is done: 1 if none was started (data_0209d4f8
   clear) or its thread (data_0209d5b8, func_0201a244 starts it) has ended,
   in which case its allocated stack is freed and the job slot cleared;
   0 while it runs. The boot, entry and download-play scenes poll it. */
int func_0201a1bc(void)
{
    if (data_0209d4f8 == 0) return 1;
    if (OS_IsThreadTerminated(&data_0209d5b8) == 0)
        return 0;
    if (data_0209d518 != 0) {
        Memory::Deallocate(data_0209d518);
        data_0209d518 = 0;
    }
    data_0209d4f8 = 0;
    return 1;
}
}
