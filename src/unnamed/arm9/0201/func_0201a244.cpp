//cpp
#include "types.h"
#include "AsyncJob.h"
#include "Memory.h"
#include "nitro/os/thread.h"

extern "C" {

extern unsigned char data_0209d4f8;
extern void *data_0209d518;
extern OSThread data_0209d5b8;

int func_0201a244(AsyncJobCallback callback, int argument, int priority, void *stackBase,
                  int stackSize) {
    void *stack;
    u32 size;
    if (data_0209d4f8 != 0) {
        callback(argument);
        return 1;
    }
    data_0209d4f8 = 1;
    size = stackSize;
    stack = stackBase;
    if (stackBase == 0) {
        stack = Memory::Allocate(size, -4);
        data_0209d518 = stack;
    }
    /* The job's int argument goes to the SDK as a pointer. */
    OS_CreateThread(&data_0209d5b8, (void (*)(void *))callback, (void *)argument,
                    (u8 *)stack + size, size, priority);
    OS_SetThreadStackWarningOffset(&data_0209d5b8, 0x400);
    OS_WakeupThreadDirect(&data_0209d5b8);
    return 0;
}
}
