#include "types.h"
extern u32 data_0208685c[];

int func_02067480(u32 idx, u32 addr, u32 size)
{
    switch (data_0208685c[idx]) {
    case 2:
        if (addr < 0x27ffe00) goto fail;
        if (addr + size > 0x27fff60) goto fail;
        return 1;
    case 0:
        if (addr < 0x2000000) goto fail;
        if (addr + size > 0x22c0000) goto fail;
        return 1;
    case 1:
        if (addr >= 0x22c0000 && addr + size <= 0x2300000) return 1;
        if (addr < 0x2000000) goto fail;
        if (addr + size > 0x2300000) goto fail;
        return 1;
    default:
        return 0;
    }
fail:
    return 0;
}
