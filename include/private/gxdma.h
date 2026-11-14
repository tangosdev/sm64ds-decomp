#ifndef PRIVATE_GXDMA_H
#define PRIVATE_GXDMA_H
#ifndef __cplusplus
#define MI_WaitDma _Z10MI_WaitDmaj
#define MI_DmaCopy32Async _Z17MI_DmaCopy32AsyncjPKvPvjPFvS1_ES1_
#define MI_DmaCopy16 _Z12MI_DmaCopy16jPKvPvj
#define MI_DmaCopy32 _Z12MI_DmaCopy32jPKvPvj
#endif

/* The gx library's DMA helpers, shared by its VRAM loaders (gx_load2d.c,
 * src/engine/gx/GXS.cpp, and gx_load3d.c, src/engine/gx/GX.cpp). Each takes
 * the loader DMA channel, GXi_DmaId, and falls back to the matching CPU copy
 * when no channel is set. Only those two files include this header: its
 * inline bodies would renumber any other C++ includer's @NNN symbols. */

#include "nitro/mi/memory.h"
#include "nitro/mi/dma.h"

/* GXi_DmaId's "no DMA" value. */
#define GX_DMA_NOT_USE ((u32)~0)

static inline void GXi_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size) {
    if (dmaNo != GX_DMA_NOT_USE) {
        MI_DmaCopy16(dmaNo, src, dest, size);
    } else {
        MI_CpuCopy16(src, dest, size);
    }
}

static inline void GXi_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size) {
    if (dmaNo != GX_DMA_NOT_USE) {
        MI_DmaCopy32(dmaNo, src, dest, size);
    } else {
        MI_CpuCopy32(src, dest, size);
    }
}

static inline void GXi_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                                      MIDmaCallback callback, void *arg) {
    if (dmaNo != GX_DMA_NOT_USE) {
        MI_DmaCopy32Async(dmaNo, src, dest, size, callback, arg);
    } else {
        MI_CpuCopy32(src, dest, size);
    }
}

static inline void GXi_WaitDma(u32 dmaNo) {
    if (dmaNo != GX_DMA_NOT_USE) {
        MI_WaitDma(dmaNo);
    }
}

#endif
