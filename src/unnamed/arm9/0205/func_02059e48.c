#include "types.h"
extern void func_0205b858(void);
extern int func_0205ba3c(int bit, int word);
extern void func_0205ba64(int a, void *fn);
extern void func_02059e04(void);

extern char data_020a6458[];

void func_02059e48(void)
{
    if (*(u16 *)data_020a6458 != 0) return;
    *(u16 *)data_020a6458 = 1;
    func_0205b858();
    while (func_0205ba3c(0xc, 1) == 0)
        ;
    func_0205ba64(0xc, (void *)func_02059e04);
}
