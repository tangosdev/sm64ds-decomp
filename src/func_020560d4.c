#include "types.h"
/* func_020560d4 at 0x020560d4 (likely _ZN3GXS11LoadBG2CharEPKvjj)
 * Loads BG2 char data using DMA if a channel is assigned, else CPU copy.
 */
extern u32 RENDER_DMA_CHANNEL;

extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void DMASyncWordTransfer(u32 ch, const void *src, void *dst, u32 size);
extern void MultiCopy_Int(const void *src, void *dst, u32 size);

void func_020560d4(const void *src, u32 offset, u32 size)
{
    void *charPtr = _ZN2G213GetBG2CharPtrEv();
    if (RENDER_DMA_CHANNEL != (u32)-1)
        DMASyncWordTransfer(RENDER_DMA_CHANNEL, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int(src, (char *)charPtr + offset, size);
}
