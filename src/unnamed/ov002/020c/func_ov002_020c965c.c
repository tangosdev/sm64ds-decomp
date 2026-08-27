#include "types.h"
extern void func_ov002_020c7ff8(void* thiz);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, int a, int b, int c, u32 d);

int func_ov002_020c965c(char* c)
{
    u8 st;

    if (*(u8*)(c + 0x6de) != 0) {
        if (*(u8*)(c + 0x706) == 0) goto done;
    }

    *(int*)(c + 0xa8) = 0;
    st = *(u8*)(c + 0x70a);
    if (st == 0 || st == 1 || st == 0x11 || (u8)(st + 0xfe) <= 1) {
        func_ov002_020c7ff8(c);
        goto done;
    }
    if (st == 8) {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6b, 0x40000000, 0x1000, 0);
        *(u8*)(c + 0x6e3) = 9;
        goto done;
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
    *(u8*)(c + 0x6e3) = 3;

done:
    return 0;
}
