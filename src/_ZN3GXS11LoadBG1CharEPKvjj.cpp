//cpp
// @symbol _ZN3GXS11LoadBG1CharEPKvjj
/* GXS::LoadBG1Char() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
/* _ZN3GXS11LoadBG1CharEPKvjj at 0x02056134 (size 0x60)
 * GXS::LoadBG1Char(src, offset, size) - loads character data into SUB BG1 VRAM.
 * Calls G2S::GetBG1CharPtr() to get base pointer, then DMA or CPU copy.
 */

extern unsigned int data_02099fd0;

extern "C" {
extern void *_ZN3G2S13GetBG1CharPtrEv(void);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);
}

namespace GXS {
void LoadBG1Char(const void *src, unsigned int offset, unsigned int count){
    void *base = _ZN3G2S13GetBG1CharPtrEv();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int(src, (char *)base + offset, count);
    }
}
}
