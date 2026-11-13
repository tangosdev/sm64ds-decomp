/* GX -- 3D VRAM uploads (the Nitro SDK's gx_load3d.c).
 * Owns .text 0x020568cc..0x02056c1c: the texture and texture-palette upload
 * family (GX_EndLoadTexPltt .. GX_BeginLoadTex). Below sits GX_LoadBGPltt at
 * 0x02056870, the last function of gx_load2d.c (src/engine/gx/GXS.cpp);
 * above is GX_SendFifo48B at 0x02056c1c.
 *
 * A Begin call takes the texture (or texture-palette) banks into LCDC mode and
 * records where their slots appear there; Load copies into that window; End
 * waits for the copy and gives the banks back.
 */
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_vramcnt.h"
#include "private/gxdma.h"

/* One row per texture-bank combination (GX_ResetBankForTex's result): the
 * LCDC addresses of the texture slots' first and second blocks and the size of
 * the first block, all in 4 KiB units. */
typedef struct {
    u16 blk1;
    u16 blk2;
    u16 szBlk1;
} GXTexStartAddr;

extern int GXi_DmaId;                        // RENDER_DMA_CHANNEL: DMA channel number, -1 if none
extern u32 data_020a60ac;                    // texture slots' first LCDC block
extern u32 data_020a60b0;                    // texture-palette slots' LCDC block
extern int data_020a60b4;                    // texture-palette banks taken by BeginLoadTexPltt
extern int data_020a60b8;                    // texture banks taken by BeginLoadTex
extern u32 data_020a60bc;                    // texture slots' second LCDC block, 0 if contiguous
extern u32 data_020a60c0;                    // size of the first block
extern const u16 data_02086314[8];           // texture-palette LCDC block, by bank combination
extern const GXTexStartAddr data_02086324[]; // texture blocks, by bank combination

void GX_BeginLoadTex() {
    data_020a60b8 = GX_ResetBankForTex();
    data_020a60ac = data_02086324[data_020a60b8].blk1 << 12;
    data_020a60bc = data_02086324[data_020a60b8].blk2 << 12;
    data_020a60c0 = data_02086324[data_020a60b8].szBlk1 << 12;
}

// A copy that straddles the end of the first block is split into two. That
// branch tests the DMA channel once for both halves rather than going
// through the GXi_ helpers, which would test it again before the second.
void GX_LoadTex(const void *pSrc, u32 destSlotAddr, u32 szByte) {
    void *pLCDC;
    if (data_020a60bc == 0) {
        pLCDC = (void *)(data_020a60ac + destSlotAddr);
    } else if (destSlotAddr + szByte < data_020a60c0) {
        pLCDC = (void *)(data_020a60ac + destSlotAddr);
    } else if (destSlotAddr >= data_020a60c0) {
        pLCDC = (void *)(data_020a60bc + destSlotAddr - data_020a60c0);
    } else {
        void *pLCDC2 = (void *)data_020a60bc;
        u32 sz = data_020a60c0 - destSlotAddr;
        pLCDC = (void *)(data_020a60ac + destSlotAddr);
        if (GXi_DmaId != GX_DMA_NOT_USE) {
            MI_DmaCopy32(GXi_DmaId, pSrc, pLCDC, sz);
            MI_DmaCopy32Async(GXi_DmaId, (const u8 *)pSrc + sz, pLCDC2, szByte - sz, 0, 0);
        } else {
            MI_CpuCopy32(pSrc, pLCDC, sz);
            MI_CpuCopy32((const u8 *)pSrc + sz, pLCDC2, szByte - sz);
        }
        return;
    }
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, pLCDC, szByte, 0, 0);
}

void GX_EndLoadTex() {
    GXi_WaitDma(GXi_DmaId);
    GX_SetBankForTex(data_020a60b8);
    data_020a60ac = data_020a60bc = data_020a60c0 = 0;
    data_020a60b8 = 0;
}

void GX_BeginLoadTexPltt() {
    data_020a60b4 = GX_ResetBankForTexPltt();
    data_020a60b0 = data_02086314[data_020a60b4 >> 4] << 12;
}

void GX_LoadTexPltt(const void *pSrc, u32 destSlotAddr, u32 szByte) {
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(data_020a60b0 + destSlotAddr), szByte, 0, 0);
}

void GX_EndLoadTexPltt() {
    GXi_WaitDma(GXi_DmaId);
    GX_SetBankForTexPltt(data_020a60b4);
    data_020a60b4 = 0;
    data_020a60b0 = 0;
}

// extern "C"
