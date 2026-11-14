//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/fs/overlay.h"
#include "nitro/mi/memory.h"

extern "C" {

/* Poison an unloaded overlay's RAM (UnloadOverlay): invalidate both caches
   over its code and bss, then fill them with 0xe6000010, an ARM undefined
   instruction, so a stale call into it traps. */
void func_02017fd0(FSOverlayInfo *info) {
    u32 ramSize;
    u32 bssSize;
    void *ptr;
    u32 len;
    volatile u32 fillVal;

    ramSize = info->header.ram_size;
    bssSize = info->header.bss_size;
    ptr = info->header.ram_address;
    len = ramSize + bssSize;

    IC_InvalidateRange(ptr, len);
    DC_InvalidateRange(ptr, len);

    fillVal = 0xe6000010;
    MI_CpuFill32(ptr, fillVal, len);
}
}
