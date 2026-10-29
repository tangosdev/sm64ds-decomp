//cpp
#include "types.h"
#include "AsyncJob.h"
#include "Memory.h"

extern "C" {

extern void func_02058200(void *c, int f, int a, int p, int s, int x);
extern void func_02057f38(void *c, int v);
extern void func_02058048(void *c);
extern unsigned char data_0209d4f8;
extern int data_0209d518;
extern char data_0209d5b8[];

#pragma opt_common_subs off
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
    /* The unreconstructed OS thread API still takes its entry as a 32-bit address. */
    func_02058200(data_0209d5b8, (int)callback, argument, stack + size, size, priority);
    func_02057f38(data_0209d5b8, 0x400);
    func_02058048(data_0209d5b8);
    return 0;
}
}
