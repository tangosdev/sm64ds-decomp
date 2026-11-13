//cpp
#include "nitro/mi/memory.h"


/* Fills rows 32-entry rows of a BG screen with tile (rows << 6 bytes). */
void func_ov007_020c02d8(void *screen, int rows, unsigned short tile) {
    volatile unsigned short tmp = tile;
    MI_CpuFill16(screen, tmp, rows << 6);
}

