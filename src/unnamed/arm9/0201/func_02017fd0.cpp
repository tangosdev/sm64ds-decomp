//cpp
#include "types.h"
#include "CP15.h"
#include "nitro/fs/overlay.h"

extern "C" {

extern void MultiStore_Int(s32 val, void *dst, s32 size);

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

    CP15::InvalidateInstructionCache((u32)ptr, len);
    CP15::InvalidateDataCache((u32)ptr, len);

    fillVal = 0xe6000010;
    MultiStore_Int(fillVal, ptr, len);
}
}
