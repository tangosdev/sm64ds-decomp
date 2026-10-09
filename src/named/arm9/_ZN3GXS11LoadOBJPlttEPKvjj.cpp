//cpp
// @symbol _ZN3GXS11LoadOBJPlttEPKvjj
/* GXS::LoadOBJPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
// _ZN3GXS11LoadOBJPlttEPKvjj - GXS::LoadOBJPltt(src, byteOffset, byteSize)
// Loads OBJ palette data to sub-screen (GXS) hardware VRAM




// The global at 0x02099fd0: DMA channel number (or -1)
extern int data_02099fd0;

extern "C" {
extern void DMASyncHalfTransfer(int channel, const void* src, void* dest, unsigned int count);
extern void MultiCopyHalf(const void* src, void* dest, unsigned int count);
}

namespace GXS {
void LoadOBJPltt(const void* src, unsigned int offset, unsigned int size){
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void*)((char*)0x05000600 + offset), size);
    } else {
        MultiCopyHalf(src, (void*)((char*)0x05000600 + offset), size);
    }
}
}
