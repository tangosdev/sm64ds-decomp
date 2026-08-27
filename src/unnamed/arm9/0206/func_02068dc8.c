#include "types.h"
extern char* data_020a9db4;
extern int func_02068e44(int, int, int);

int func_02068dc8(int a, int b, int c)
{
    if (*(u8*)(data_020a9db4 + 0x1000 + 0x411))
        return 0;
    if (func_02068e44(a, b, c) == 0)
        return 0;
    *(u16*)(data_020a9db4 + 0x1300 + 0xe) = c;
    *(u16*)(data_020a9db4 + 0x1400) = a;
    *(u16*)(data_020a9db4 + 0x1400 + 2) = b;
    return 1;
}
