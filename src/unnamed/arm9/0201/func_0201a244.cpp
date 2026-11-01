//cpp
#include "types.h"
#include "AsyncJob.h"
#include "Memory.h"
#include "nitro/os/thread.h"

extern "C" {

extern unsigned char data_0209d4f8;
extern int data_0209d518;
extern OSThread data_0209d5b8;

#pragma opt_propagation off
int func_0201a244(AsyncJobCallback callback, int argument, int priority, int stackBase,
                  int stackSize) {
    int stack;
    int size;
    if (data_0209d4f8 != 0) {
        callback(argument);
        return 1;
    }
    data_0209d4f8 = 1;
    size = stackSize;
    stack = stackBase;
    if (stackBase == 0) {
        stack = (int)Memory::Allocate(size, -4);
        data_0209d518 = stack;
    }
    /* The job's int argument and stack address go to the SDK as pointers. */
    OS_CreateThread(&data_0209d5b8, (void (*)(void *))callback, (void *)argument,
                    (void *)(stack + size), size, priority);
    OS_SetThreadStackWarningOffset(&data_0209d5b8, 0x400);
    OS_WakeupThreadDirect(&data_0209d5b8);
    return 0;
}
}
