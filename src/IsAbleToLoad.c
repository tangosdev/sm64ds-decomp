#include "types.h"

extern u32 MBi_defaultLoadSegList[];

int MBi_IsAbleToRecv(u32 idx, u32 addr, u32 size) {
    switch (MBi_defaultLoadSegList[idx]) {
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

/* The SDK's static IsAbleToLoad (ntrtwl's libraries/mb/src/mb_fileinfo.c,
 * next to MBi_IsAbleToRecv above): the ROM header and ARM9 static take
 * MBi_IsAbleToRecv's answer; the ARM7 static may also load anywhere in main
 * RAM below the boot loader's area, or in ARM7 WRAM.
 */
int IsAbleToLoad(u32 idx, u32 addr, u32 size) {
    switch (MBi_defaultLoadSegList[idx]) {
    case 0:
    case 2:
        return MBi_IsAbleToRecv(idx, addr, size);
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
