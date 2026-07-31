#include "types.h"
void func_ov002_020b363c(char *c)
{
    char *p = *(char**)(c + 0x328);
    int id;
    if (p == 0)
        return;
    id = *(u16*)(p + 0xc);
    if (id <= 0x121) {
        if (id == 0x121)
            *(u8*)(p + 0x3b0) = 0;
        return;
    }
    switch (id) {
    case 0x141:
    case 0x142:
    case 0x143:
    case 0x144:
        *(u8*)(p + 0xd6) = 1;
    }
}
