//cpp
// @symbol _ZN3GXS11LoadBG0CharEPKvjj
/* GXS::LoadBG0Char() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
/* _ZN3GXS11LoadBG0CharEPKvjj at 0x020561f4
 * Loads BG0 char data using DMA if a channel is assigned, else CPU copy.
 */
extern u32 data_02099fd0;

extern "C" {
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void DMASyncWordTransfer(u32 ch, const void *src, void *dst, u32 size);
extern void MultiCopy_Int(const void *src, void *dst, u32 size);
}

namespace GXS {
void LoadBG0Char(const void *src, u32 offset, u32 size){
    void *charPtr = _ZN3G2S13GetBG0CharPtrEv();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int(src, (char *)charPtr + offset, size);
}
}
