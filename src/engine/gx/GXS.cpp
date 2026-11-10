/* GXS -- 2D VRAM uploads (the Nitro SDK's gx_load2d.c).
 * Owns .text 0x02055dec..0x020568cc: the VRAM load family -- BG character and
 * screen loads, the OBJ/OAM/palette loads, and the extended-palette banked
 * upload family (GXS_EndLoadOBJExtPltt .. GXS_BeginLoadBGExtPltt). The file
 * holds both engines' loaders, GX_ and GXS_ alike, under pret's names
 * (pokediamond arm9/lib/NitroSDK/src/GX_load2d.c, nitro/gx/gx_load.h) and in
 * its source order. The 3D uploads above 0x020568cc are gx_load3d.c
 * (src/engine/gx/GX.cpp).
 */
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_bgcnt.h"

/* OBJ character VRAM has a fixed base, so the SDK's getter is an inline, not a
 * call. LoadOBJ reads the base through it, which keeps the base in a register
 * where a literal would fold into each copy's address add. The sub engine's
 * twin, G2S_GetOBJCharPtr, is in nitro/gx/gx_bgcnt.h. */
static inline void *G2_GetOBJCharPtr(void) {
    return (void *)HW_OBJ_VRAM;
}

/* GX sub-screen extended palette VRAM bases */
#define GXS_OBJ_EXT_PLTT_BASE 0x068a0000u
#define GXS_BG_EXT_PLTT_BASE  0x06898000u

extern int data_02099fd0;  // RENDER_DMA_CHANNEL: DMA channel number, -1 if none
extern u32 data_020a60a4;  // BG ext-palette bank saved by BeginLoadBG, restored by EndLoadBG
extern u32 data_020a60a8;  // OBJ ext-palette bank saved by BeginLoadOBJ, restored by EndLoadOBJ

extern "C" {
extern void MI_WaitDma(u32 dmaNo);
extern void MI_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                              void (*callback)(void *), void *arg);
extern void DMASyncWordTransfer(u32 ch, const void *src, void *dst, u32 size);
extern void DMASyncHalfTransfer(u32 ch, const void *src, volatile void *dst, u32 count);
extern void MultiCopyHalf(const void *src, volatile void *dst, int count);
extern void MultiCopy_Int(int *src, int *dst, int len);

// Loads BG palette data to main-screen (GX) palette VRAM at 0x05000000.
void GX_LoadBGPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000000 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000000 + offset), size);
    }
}

// Loads BG palette data to sub-screen (GXS) palette VRAM at 0x05000400.
void GXS_LoadBGPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000400 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000400 + offset), size);
    }
}

// Loads OBJ palette data to main screen (GX) palette VRAM at 0x05000200.
void GX_LoadOBJPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000200 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000200 + offset), size);
    }
}

// Loads OBJ palette data to sub-screen (GXS) palette VRAM at 0x05000600.
void GXS_LoadOBJPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000600 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000600 + offset), size);
    }
}

// Copies count words from src to VRAM address (0x07000000 + offset) -- the
// main-engine OAM block (ARM9 view). DMA channel if assigned, else CPU copy.
void GX_LoadOAM(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(0x7000000 + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(0x7000000 + offset), count);
    }
}

// Copies count words from src to VRAM address (0x07000400 + offset) -- the
// sub-engine OAM block (ARM9 view). DMA channel if assigned, else CPU copy.
void GXS_LoadOAM(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(0x07000400 + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(0x07000400 + offset), count);
    }
}

// Copies count words from src to VRAM address (0x6400000 + offset) -- main
// OBJ VRAM.
void GX_LoadOBJ(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    unsigned int base = (unsigned int)G2_GetOBJCharPtr();
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(base + offset), count);
    }
}

// Copies count words from src to VRAM address (0x6600000 + offset) -- sub
// OBJ VRAM. GX_LoadOBJ's sub-engine twin.
void GXS_LoadOBJ(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    unsigned int base = (unsigned int)G2S_GetOBJCharPtr();
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(base + offset), count);
    }
}

// Loads BG0 screen data to the main-engine plane (G2_GetBG0ScrPtr).
void GX_LoadBG0Scr(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2_GetBG0ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG0 screen data to the sub-engine plane (G2S_GetBG0ScrPtr).
void GXS_LoadBG0Scr(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2S_GetBG0ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG1 screen data to the main-engine plane (G2_GetBG1ScrPtr).
void GX_LoadBG1Scr(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2_GetBG1ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG1 screen data to the sub-engine plane (G2S_GetBG1ScrPtr).
void GXS_LoadBG1Scr(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2S_GetBG1ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG2 screen data to the main-engine plane (G2_GetBG2ScrPtr).
void GX_LoadBG2Scr(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = G2_GetBG2ScrPtr();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads BG2 screen data to the sub-engine plane (G2S_GetBG2ScrPtr).
void GXS_LoadBG2Scr(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = (void *)G2S_GetBG2ScrPtr();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads BG3 screen data to the main-engine plane (G2_GetBG3ScrPtr).
void GX_LoadBG3Scr(const void *src, u32 offset, u32 count) {
    void *bgPtr = G2_GetBG3ScrPtr();
    s32 ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    } else {
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
    }
}

// Loads BG3 screen data to the sub-engine plane (G2S_GetBG3ScrPtr).
void GXS_LoadBG3Scr(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = (void *)G2S_GetBG3ScrPtr();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads character data into MAIN BG0 VRAM (G2_GetBG0CharPtr, 0x2054efc).
void GX_LoadBG0Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = G2_GetBG0CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG0 character data into SUB BG0 VRAM (G2S_GetBG0CharPtr).
void GXS_LoadBG0Char(const void *src, u32 offset, u32 size) {
    void *charPtr = (void *)G2S_GetBG0CharPtr();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

// Loads character data into MAIN BG1 VRAM (G2_GetBG1CharPtr, 0x2054ea8).
void GX_LoadBG1Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = G2_GetBG1CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG1 character data into SUB BG1 VRAM (G2S_GetBG1CharPtr).
void GXS_LoadBG1Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)G2S_GetBG1CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG2 char data into MAIN BG2 VRAM (G2_GetBG2CharPtr).
void GX_LoadBG2Char(const void *src, u32 offset, u32 size) {
    void *charPtr = G2_GetBG2CharPtr();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

// Loads BG2 char data into SUB BG2 VRAM (G2S_GetBG2CharPtr).
void GXS_LoadBG2Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)G2S_GetBG2CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG3 char data into MAIN BG3 VRAM (0x2054d88 getter).
void GX_LoadBG3Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)G2_GetBG3CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG3 char data into SUB BG3 VRAM (G2S_GetBG3CharPtr).
void GXS_LoadBG3Char(const void *src, u32 offset, u32 size) {
    void *charPtr = (void *)G2S_GetBG3CharPtr();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

// Clears the sub-engine BG ext-palette enable bit and unmaps the bank back to
// LCDC (GX_ResetBankForSubBGExtPltt); the result is held for
// GXS_EndLoadBGExtPltt.
void GXS_BeginLoadBGExtPltt() {
    data_020a60a4 = GX_ResetBankForSubBGExtPltt();
}

void GXS_LoadBGExtPltt(const void *src, u32 destSlotAddr, u32 size) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_DmaCopy32Async(dmaId, src, (void *)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int((int *)src, (int *)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size);
    }
}

void GXS_EndLoadBGExtPltt() {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_WaitDma(dmaId);
    }
    GX_SetBankForSubBGExtPltt(data_020a60a4);
    data_020a60a4 = 0;
}

// The OBJ counterpart (GX_ResetBankForSubOBJExtPltt); the result is held for
// GXS_EndLoadOBJExtPltt.
void GXS_BeginLoadOBJExtPltt() {
    data_020a60a8 = GX_ResetBankForSubOBJExtPltt();
}

void GXS_LoadOBJExtPltt(const void *src, u32 destSlotAddr, u32 size) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_DmaCopy32Async(dmaId, src, (void *)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int((int *)src, (int *)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size);
    }
}

void GXS_EndLoadOBJExtPltt() {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_WaitDma(dmaId);
    }
    GX_SetBankForSubOBJExtPltt(data_020a60a8);
    data_020a60a8 = 0;
}

} // extern "C"
