#include "types.h"
/* _ZN3GXS11LoadBG0CharEPKvjj at 0x020561f4
 * Loads BG0 char data using DMA if a channel is assigned, else CPU copy.
 */
extern u32 RENDER_DMA_CHANNEL;

extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void DMASyncWordTransfer(u32 ch, const void *src, void *dst, u32 size);
extern void MultiCopy_Int(const void *src, void *dst, u32 size);

void _ZN3GXS11LoadBG0CharEPKvjj(const void *src, u32 offset, u32 size)
{
    void *charPtr = _ZN3G2S13GetBG0CharPtrEv();
    if (RENDER_DMA_CHANNEL != (u32)-1)
        DMASyncWordTransfer(RENDER_DMA_CHANNEL, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int(src, (char *)charPtr + offset, size);
}
