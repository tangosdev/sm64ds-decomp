#include "types.h"
extern int func_02055490(int *out);
extern int func_02055464(int *out);

void func_020556d0(void)
{
    int a, b;
    *(volatile u32 *)0x4000600 |= 0x8000;
    while (func_02055490(&a)) ;
    while (func_02055464(&b)) ;
    *(volatile u32 *)0x4000440 = 3;
    *(volatile u32 *)0x4000454 = 0;
    *(volatile u32 *)0x4000440 = 0;
    if (b != 0) *(volatile u32 *)0x4000448 = b;
    *(volatile u32 *)0x4000454 = 0;
    *(volatile u32 *)0x4000440 = 2;
    *(volatile u32 *)0x4000448 = a;
    *(volatile u32 *)0x4000454 = 0;
}
