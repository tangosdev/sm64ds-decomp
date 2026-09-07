//cpp
// @symbol _ZN3GXS13LoadBGExtPlttEPKvjj
/* GXS::LoadBGExtPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
/* DMA transfer function for GX (nitroSDK internal) */



/* GX sub-screen BG extended palette base address */
#define GXS_BG_EXT_PLTT_BASE 0x06898000u

extern u32 data_02099fd0;   /* RENDER_DMA_CHANNEL: 0x02099fd0 */

extern "C" {
extern void func_02059fd0(u32 dmaId, const void* src, void* dst, u32 size, u32 unk0, u32 unk1);
extern void MultiCopy_Int(const void* src, void* dst, u32 count);
}

namespace GXS {
void LoadBGExtPltt(const void* src, u32 destSlotAddr, u32 size){
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fd0(dmaId, src, (void*)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int(src, (void*)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size);
    }
}
}
