//cpp
#include "nitro/mi/memory.h"

extern "C" {
/* Fills rows 32-entry rows of a BG screen with tile (rows << 6 bytes). */
void func_ov007_020c02d8(void *screen, int rows, unsigned short tile) {
    volatile unsigned short tmp = tile;
    MIi_CpuClear16(tmp, screen, rows << 6);
}
}
