#include "types.h"

extern u32 data_0208685c[];

int func_02067480(u32 idx, u32 addr, u32 size) {
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
        if (addr >= 0x22c0000 && addr + size <= 0x2300000)
            return 1;
        if (addr >= 0x2000000 && addr + size <= 0x2300000)
            return 1;
        break;
    default:
        return 0;
    }
    return 0;
}

/* func_0206736c at 0x0206736c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
int func_0206736c(u32 idx, u32 addr, u32 size) {
    switch (data_0208685c[idx]) {
    case 0:
    case 2:
        return func_02067480(idx, addr, size);
    case 1:
        if (addr >= 0x2000000 && addr < 0x23fe800) {
            u32 end = addr + size;
            if (addr < 0x2300000 && end > 0x2300000)
                return 0;
            if (end <= 0x2300000)
                return 1;
            if (end < 0x23fe800 && size <= 0x40000)
                return 1;
            return 0;
        }
        if (addr >= 0x37f8000 && addr < 0x380f000)
            return addr + size <= 0x380f000;
        break;
    default:
        return 0;
    }
    return 0;
}
