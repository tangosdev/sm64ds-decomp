//cpp
// @symbol _ZN3GXS10LoadBGPlttEPKvjj
/* GXS::LoadBGPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
// _ZN3GXS10LoadBGPlttEPKvjj - GXS::LoadBGPltt(src, byteOffset, byteSize)
// Loads BG palette data to sub-screen (GXS) hardware VRAM




extern int data_02099fd0;

extern "C" {
extern void DMASyncHalfTransfer(int channel, const void* src, void* dest, unsigned int count);
extern void MultiCopyHalf(const void* src, void* dest, unsigned int count);
}

namespace GXS {
void LoadBGPltt(const void* src, unsigned int offset, unsigned int size){
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void*)((char*)0x05000400 + offset), size);
    } else {
        MultiCopyHalf(src, (void*)((char*)0x05000400 + offset), size);
    }
}
}
