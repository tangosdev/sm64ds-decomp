#include "types.h"
extern void func_0202043c(void *self);

void func_02020820(char *self, int *src, int arg2, int arg3, u8 arg5, u8 arg6)
{
    *(int *)(self + 0) = src[0];
    *(int *)(self + 0x10) = src[1];
    func_0202043c(self);
    *(signed char *)(self + 0x22) = -1;
    *(signed char *)(self + 0x23) = -1;
    *(int *)(self + 8) = 0x1000;
    *(int *)(self + 0xc) = 0x1000;
    *(short *)(self + 0x20) = 0;
    *(u8 *)(self + 0x24) = (u8)arg3;
    *(u8 *)(self + 0x25) = (u8)arg5;
    *(u8 *)(self + 0x26) = (u8)arg6;
    *(int *)(self + 0x28) = arg2;
}
