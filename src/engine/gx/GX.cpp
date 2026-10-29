/* GX -- VRAM bank/transfer helpers (Nitro SDK namespace, not a class).
 * Owns .text 0x02056870..0x02056c1c: the palette/texture upload family
 * (LoadBGPltt .. BeginLoadTex). Below sits GXS::LoadBGPltt at 0x02056808;
 * above is Copy48BytesFixed at 0x02056c1c.
 */
// leftovers: the DMASyncWordTransfer declaration stays wide (local extern):
// the definition's narrower channel inserts a narrowing op at the call. LoadTex's base/top stay plain int (the register web the bytes
// were built against). BeginLoadTex's byte-offset table reads stay a char *
// tower (indexing reschedules the body). data_02099fd0 is the render DMA
// channel, -1 if none.

#include "types.h"
#include "GX.h"

extern int data_02099fd0;  // RENDER_DMA_CHANNEL: DMA channel number, -1 if none
extern u32 data_020a60ac;  // texture upload window: base address
extern u32 data_020a60b0;  // texture-palette VRAM base address
extern u32 data_020a60b4;  // texture-palette bank selector (BeginLoadTexPltt result)
extern u32 data_020a60b8;  // texture bank selector (BeginLoadTex result)
extern u32 data_020a60bc;  // texture upload window: secondary bank base
extern u32 data_020a60c0;  // texture upload window: top of the base window
extern u16 data_02086314[];
extern u16 data_02086324[];
extern u16 data_02086326[];
extern u16 data_02086328[];

extern "C" {
extern u16 func_02054168(void);
extern u16 func_0205417c(void);
extern void func_02059fa8(int ch);
extern void func_02059fd0(int ch, int src, int dst, u32 size, void (*cb)(int), int cbarg);
extern void DMASyncHalfTransfer(u32 channel, void *src, void *dst, u32 numHalfs);
// local extern: u8 channel per the definition inserts a narrowing op at the call
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dest, unsigned int len);
extern void MultiCopyHalf(const void *src, void *dst, s32 size);
extern void MultiCopy_Int(int *src, int *dst, int len);
}

namespace GX {

// @symbol _ZN2GX12BeginLoadTexEv
void BeginLoadTex() {
    int r = func_0205417c();
    int i = r * 6;
    data_020a60b8 = r;
    data_020a60ac = (int)*(u16 *)((char *)data_02086324 + i) << 12;
    data_020a60bc = (int)*(u16 *)((char *)data_02086326 + i) << 12;
    data_020a60c0 = (int)*(u16 *)((char *)data_02086328 + i) << 12;
}

// VRAM texture upload with banked destination: below-top goes to the base
// window, above-top to the secondary bank, straddling splits into two DMA (or
// CPU copy) halves.
// @symbol _ZN2GX7LoadTexEPKvjj
void LoadTex(const void *src, unsigned int offset, unsigned int size) {
    unsigned int dst;
    int base = data_020a60bc;
    if (base == 0) {
        dst = data_020a60ac + offset;
    } else {
        int top = data_020a60c0;
        if (offset + size < top) {
            dst = data_020a60ac + offset;
        } else if (offset >= top) {
            dst = base + offset - top;
        } else {
            top -= offset;
            unsigned int d0 = data_020a60ac + offset;
            int ch = data_02099fd0;
            if (ch != -1) {
                DMASyncWordTransfer(ch, src, (void*)d0, top);
                func_02059fd0(data_02099fd0, (int)((char*)src + top), base, size - top, 0, 0);
                return;
            }
            MultiCopy_Int((int*)src, (int*)d0, top);
            MultiCopy_Int((int*)((char*)src + top), (int*)base, size - top);
            return;
        }
    }
    if (data_02099fd0 != -1) {
        func_02059fd0(data_02099fd0, (int)src, dst, size, 0, 0);
        return;
    }
    MultiCopy_Int((int*)src, (int*)dst, size);
}

// Ends a banked texture upload: waits on the upload DMA channel if one is
// active, restores the texture bank, then clears the four upload-window globals.
// @symbol _ZN2GX10EndLoadTexEv
void EndLoadTex()
{
    if (data_02099fd0 != -1) {
        func_02059fa8(data_02099fd0);
    }
    SetBankForTex(data_020a60b8);
    data_020a60c0 = 0;
    data_020a60bc = 0;
    data_020a60ac = 0;
    data_020a60b8 = 0;
}

// @symbol _ZN2GX16BeginLoadTexPlttEv
void BeginLoadTexPltt() {
    int r = func_02054168();
    data_020a60b4 = r;
    data_020a60b0 = (int)data_02086314[r >> 4] << 12;
}

// Loads a texture palette into VRAM: async DMA when a channel is configured
// (data_02099fd0 != -1), otherwise MultiCopy_Int.
// @symbol _ZN2GX11LoadTexPlttEPKvjj
void LoadTexPltt(const void *src, u32 destSlotAddr, u32 size){
    void *dest = (void *)(data_020a60b0 + destSlotAddr);
    s32 dmaCh = data_02099fd0;
    if (dmaCh != -1) {
        func_02059fd0(dmaCh, (int)src, (int)dest, size, 0, 0);
    } else {
        MultiCopy_Int((int*)src, (int*)dest, size);
    }
}

// @symbol _ZN2GX14EndLoadTexPlttEv
void EndLoadTexPltt(){
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    SetBankForTexPltt(data_020a60b4);
    data_020a60b4 = 0;
    data_020a60b0 = 0;
}

// Loads BG palette data to main-screen (GX) palette VRAM at 0x05000000.
// @symbol _ZN2GX10LoadBGPlttEPKvjj
void LoadBGPltt(const void* src, unsigned int offset, unsigned int size){
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, (void*)src, (void*)(offset + 0x05000000), size);
    } else {
        MultiCopyHalf(src, (void*)(offset + 0x05000000), size);
    }
}

}
