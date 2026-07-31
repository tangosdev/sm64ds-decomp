#include "types.h"
/* DMA transfer function for GX (nitroSDK internal) */
extern void FUN_02059fd0(u32 dmaId, const void* src, void* dst, u32 size, u32 unk0, u32 unk1);
extern void MultiCopy_Int(const void* src, void* dst, u32 count);

/* GX sub-screen OBJ extended palette base address (VRAM-D slot B) */
#define GXS_OBJ_EXT_PLTT_BASE 0x068a0000u

extern u32 GXi_DmaId;   /* RENDER_DMA_CHANNEL: 0x02099fd0 */

void _ZN3GXS14LoadOBJExtPlttEPKvjj(const void* src, u32 destSlotAddr, u32 size) {
    u32 dmaId = GXi_DmaId;
    if (dmaId != (u32)-1) {
        FUN_02059fd0(dmaId, src, (void*)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int(src, (void*)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size);
    }
}
