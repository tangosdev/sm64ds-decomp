#include "types.h"
extern char data_020a1fc0[];
extern void func_020418f0(void);
extern int func_02065138(int a, int b);
extern int func_020650a0(int a, int b);
extern int func_020623ec(int a0, int a1, int a2, int a3, u16 a4, u16 a5);

void func_02041930(void)
{
    char *g = data_020a1fc0;
    int f0 = *(int *)g;
    u16 a;
    u16 r;
    if (f0 != 0)
        a = (u16)*(int *)(g + 0x28);
    else
        a = (u16)*(int *)(g + 0x2c);
    if (*(int *)(g + 4) == 0) return;
    {
        char *sub = g + 0x40;
        if (f0 != 0)
            r = (u16)func_02065138((int)sub, a);
        else
            r = (u16)func_020650a0((int)sub, a);
        *(int *)(g + 8) =
            (func_020623ec((int)func_020418f0, (int)sub, r, 0xffff,
                           *(u16 *)(g + 0x30), 3) == 2) ? 1 : 0;
    }
    *(int *)(g + 4) = (*(int *)(data_020a1fc0 + 8) == 0) ? 1 : 0;
}
