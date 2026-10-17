#include "types.h"
extern u32 data_0208685c[];

int func_02067480(u32 idx, u32 addr, u32 size)
{
    switch (data_0208685c[idx]) {
    case 2:
        if (addr >= 0x27ffe00 && addr + size <= 0x27fff60)
            return 1;
        break;
    case 0:
        if (addr >= 0x2000000 && addr + size <= 0x22c0000)
            return 1;
        break;
    case 1:
        if (addr >= 0x22c0000 && addr + size <= 0x2300000) return 1;
        if (addr >= 0x2000000 && addr + size <= 0x2300000)
            return 1;
        break;
    default:
        return 0;
    }
    return 0;
}
