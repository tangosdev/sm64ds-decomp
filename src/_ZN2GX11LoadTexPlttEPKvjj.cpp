//cpp
// @symbol _ZN2GX11LoadTexPlttEPKvjj
/* GX::LoadTexPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
/* GX::LoadTexPltt at 0x02056924
 * Loads a texture palette into VRAM. If a DMA channel is configured
 * (data_02099fd0 != -1), uses async DMA; otherwise uses MultiCopy_Int.
 *
 * Globals:
 *   data_02099fd0 (0x02099fd0): s32 DMA channel number, -1 if none
 *   data_020a60b0 (0x020a60b0): u32 palette VRAM base address value
 *
 * Unknown callee at 0x02059fd0 is a DMA transfer function:
 *   func_02059fd0(dmaChannel, src, dest, size, 0, 0)
 */
extern u32 data_020a60b0;   /* 0x020a60b0: palette VRAM base address */
extern s32 data_02099fd0;   /* 0x02099fd0: DMA channel (-1 if none) */

extern "C" {
extern void MultiCopy_Int(const void *src, void *dst, u32 size);  /* 0x0205a490 */
extern void func_02059fd0(s32 dmaChannel, const void *src, void *dst, u32 size, u32 arg4, u32 arg5);
}

namespace GX {
void LoadTexPltt(const void *src, u32 destSlotAddr, u32 size){
    void *dest = (void *)(data_020a60b0 + destSlotAddr);
    s32 dmaCh = data_02099fd0;
    if (dmaCh != -1) {
        func_02059fd0(dmaCh, src, dest, size, 0, 0);
    } else {
        MultiCopy_Int(src, dest, size);
    }
}
}
