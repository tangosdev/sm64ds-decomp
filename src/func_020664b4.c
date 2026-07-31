#include "types.h"
extern void *func_020619ac(int a, int b);
extern void func_0206655c(void *p, int b);
extern char *data_020a9db8;

void func_020664b4(int a)
{
    u16 i;
    for (i = 0; i < 0x10; i++) {
        char *e = data_020a9db8 + i * 0x5c4;
        if (*(unsigned char *)(e + 0x1d4a) != 0)
            *(u16 *)(e + 0x1d42) = 0;
    }
    for (i = 1; i <= 0xf; i++) {
        u16 *p = (u16 *)func_020619ac(a, i);
        if (p == 0)
            continue;
        if (*p == 0xffff)
            continue;
        if (*p == 0)
            continue;
        func_0206655c(p, i);
    }
}
