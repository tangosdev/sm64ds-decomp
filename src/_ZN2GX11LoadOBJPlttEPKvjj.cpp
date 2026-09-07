//cpp
// @symbol _ZN2GX11LoadOBJPlttEPKvjj
/* GX::LoadOBJPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
// _ZN2GX11LoadOBJPlttEPKvjj - GX::LoadOBJPltt(src, byteOffset, byteSize)
// Loads OBJ palette data to main screen (GX) hardware VRAM


// The global at 0x02099fd0: DMA channel number (or -1)
extern int data_02099fd0;

extern "C" {
extern void DMASyncHalfTransfer(int channel, const void* src, void* dest, unsigned int count);
extern void MultiCopyHalf(const void* src, void* dest, unsigned int count);
}

namespace GX {
void LoadOBJPltt(const void* src, unsigned int offset, unsigned int size){
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void*)((char*)0x05000200 + offset), size);
    } else {
        MultiCopyHalf(src, (void*)((char*)0x05000200 + offset), size);
    }
}
}
