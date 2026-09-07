//cpp
// @symbol _ZN2GX11LoadBG1CharEPKvjj
/* GX::LoadBG1Char() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
/* _ZN2GX11LoadBG1CharEPKvjj at 0x02056194 (size 0x60)
 * GX::LoadBG1Char(src, offset, size) - loads character data into MAIN BG1 VRAM.
 * Calls G2::GetBG1CharPtr()-equivalent (0x2054ea8) to get base, then DMA or CPU copy.
 */

extern unsigned int data_02099fd0;

extern "C" {
extern void *func_02054ea8(void);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);
}

namespace GX {
void LoadBG1Char(const void *src, unsigned int offset, unsigned int count){
    void *base = func_02054ea8();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int(src, (char *)base + offset, count);
    }
}
}
