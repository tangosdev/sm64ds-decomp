/* GX -- 3D VRAM uploads (the Nitro SDK's gx_load3d.c).
 * Owns .text 0x020568cc..0x02056c1c: the texture and texture-palette upload
 * family (EndLoadTexPltt .. BeginLoadTex). Below sits GX::LoadBGPltt at
 * 0x02056870, the last function of gx_load2d.c (src/engine/gx/GXS.cpp);
 * above is Copy48BytesFixed at 0x02056c1c.
 */
// leftovers: LoadTex's base/top stay plain int (the register web the bytes
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
extern const u16 data_02086314[8];
extern u16 data_02086324[];
extern u16 data_02086326[];
extern u16 data_02086328[];

extern "C" {
extern void MI_WaitDma(u32 dmaNo);
extern void MI_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                              void (*callback)(void *), void *arg);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dest, unsigned int len);
extern void MultiCopy_Int(int *src, int *dst, int len);
}

namespace GX {

void BeginLoadTex() {
    int r = GX::ResetBankForTex();
    int i = r * 6;
    data_020a60b8 = r;
    data_020a60ac = (int)*(u16 *)((char *)data_02086324 + i) << 12;
    data_020a60bc = (int)*(u16 *)((char *)data_02086326 + i) << 12;
    data_020a60c0 = (int)*(u16 *)((char *)data_02086328 + i) << 12;
}

// VRAM texture upload with banked destination: below-top goes to the base
// window, above-top to the secondary bank, straddling splits into two DMA (or
// CPU copy) halves.
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
                MI_DmaCopy32Async(data_02099fd0, (char *)src + top, (void *)base, size - top, 0, 0);
                return;
            }
            MultiCopy_Int((int*)src, (int*)d0, top);
            MultiCopy_Int((int*)((char*)src + top), (int*)base, size - top);
            return;
        }
    }
    if (data_02099fd0 != -1) {
        MI_DmaCopy32Async(data_02099fd0, src, (void *)dst, size, 0, 0);
        return;
    }
    MultiCopy_Int((int*)src, (int*)dst, size);
}

// Ends a banked texture upload: waits on the upload DMA channel if one is
// active, restores the texture bank, then clears the four upload-window globals.
void EndLoadTex()
{
    if (data_02099fd0 != -1) {
        MI_WaitDma(data_02099fd0);
    }
    SetBankForTex(data_020a60b8);
    data_020a60c0 = 0;
    data_020a60bc = 0;
    data_020a60ac = 0;
    data_020a60b8 = 0;
}

void BeginLoadTexPltt() {
    int r = GX::ResetBankForTexPltt();
    data_020a60b4 = r;
    data_020a60b0 = (int)data_02086314[r >> 4] << 12;
}

// Loads a texture palette into VRAM: async DMA when a channel is configured
// (data_02099fd0 != -1), otherwise MultiCopy_Int.
void LoadTexPltt(const void *src, u32 destSlotAddr, u32 size){
    void *dest = (void *)(data_020a60b0 + destSlotAddr);
    s32 dmaCh = data_02099fd0;
    if (dmaCh != -1) {
        MI_DmaCopy32Async(dmaCh, src, dest, size, 0, 0);
    } else {
        MultiCopy_Int((int*)src, (int*)dest, size);
    }
}

void EndLoadTexPltt(){
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_WaitDma(dmaId);
    }
    SetBankForTexPltt(data_020a60b4);
    data_020a60b4 = 0;
    data_020a60b0 = 0;
}

}
