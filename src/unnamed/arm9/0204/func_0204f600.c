#include "types.h"
extern void func_0205adc4(u8 arg);
extern void func_0204f4bc(void *thiz);
extern void *func_0205afb4(void);
int func_0204f600(void *thiz)
{
    func_0205adc4(*(u8 *)((char *)thiz + 0x3c));
    func_0204f4bc(thiz);
    *(void **)((char *)thiz + 0x30) = func_0205afb4();
    *(u8 *)((char *)thiz + 0x2c) = 1;
    return 1;
}
