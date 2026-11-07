/* GXS -- 2D VRAM uploads (the Nitro SDK's gx_load2d.c).
 * Owns .text 0x02055dec..0x020568cc: the VRAM load family -- BG character and
 * screen loads, the OBJ/OAM/palette loads, and the extended-palette banked
 * upload family (EndLoadOBJExtPltt .. BeginLoadBGExtPltt). The file holds
 * both engines' loaders, so the GX:: members inside this stretch (LoadBGPltt,
 * LoadOBJPltt, LoadOBJ, LoadBG0Char, LoadBG1Char) are defined here too. The
 * 3D uploads above 0x020568cc are gx_load3d.c (src/engine/gx/GX.cpp).
 */
// The func_ helpers keep address names (no recovered names exist).

#include "types.h"
#include "GX.h"

namespace G2 {
void *GetBG0ScrPtr(void);
void *GetBG1ScrPtr(void);
void *GetBG2CharPtr(void);
void *GetBG2ScrPtr(void);
void *GetBG3ScrPtr(void);

/* OBJ character VRAM has a fixed base, so the SDK's getter is an inline, not a
 * call. LoadOBJ reads the base through it, which keeps the base in a register
 * where a literal would fold into each copy's address add. */
inline void *GetOBJCharPtr(void) {
    return (void *)0x6400000;
}
}

namespace G2S {
unsigned int GetBG0CharPtr(void);
unsigned int GetBG0ScrPtr(void);
unsigned int GetBG1CharPtr(void);
unsigned int GetBG1ScrPtr(void);
unsigned int GetBG2CharPtr(void);
unsigned int GetBG2ScrPtr(void);
unsigned int GetBG3CharPtr(void);
unsigned int GetBG3ScrPtr(void);

/* See G2::GetOBJCharPtr. */
inline void *GetOBJCharPtr(void) {
    return (void *)0x6600000;
}
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

extern int func_02054d88(void);
extern void *func_02054ea8(void);
extern void *func_02054efc(void);
}

namespace GX {

// Loads BG palette data to main-screen (GX) palette VRAM at 0x05000000.
void LoadBGPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000000 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000000 + offset), size);
    }
}

}

namespace GXS {

// Loads BG palette data to sub-screen (GXS) palette VRAM at 0x05000400.
void LoadBGPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000400 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000400 + offset), size);
    }
}

}

namespace GX {

// Loads OBJ palette data to main screen (GX) palette VRAM at 0x05000200.
void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000200 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000200 + offset), size);
    }
}

}

namespace GXS {

// Loads OBJ palette data to sub-screen (GXS) palette VRAM at 0x05000600.
void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void *)((char *)0x05000600 + offset), size);
    } else {
        MultiCopyHalf(src, (void *)((char *)0x05000600 + offset), size);
    }
}

}

extern "C" {

// Copies count words from src to VRAM address (0x07000000 + offset) -- the
// main-engine OAM block (ARM9 view). DMA channel if assigned, else CPU copy.
void func_020566dc(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(0x7000000 + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(0x7000000 + offset), count);
    }
}

// Copies count words from src to VRAM address (0x07000400 + offset) -- the
// sub-engine OAM block (ARM9 view). DMA channel if assigned, else CPU copy.
void func_02056674(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(0x07000400 + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(0x07000400 + offset), count);
    }
}

}

namespace GX {

// Copies count words from src to VRAM address (0x6400000 + offset) -- main
// OBJ VRAM.
void LoadOBJ(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    unsigned int base = (unsigned int)G2::GetOBJCharPtr();
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(base + offset), count);
    }
}

}

extern "C" {

// Copies count words from src to VRAM address (0x6600000 + offset) -- sub
// OBJ VRAM. GX::LoadOBJ's sub-engine twin.
void func_020565b4(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    unsigned int base = (unsigned int)G2S::GetOBJCharPtr();
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(base + offset), count);
    }
}

// Loads BG0 screen data to the main-engine plane (G2::GetBG0ScrPtr).
void func_02056554(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2::GetBG0ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG0 screen data to the sub-engine plane (G2S::GetBG0ScrPtr).
void func_020564f4(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2S::GetBG0ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG1 screen data to the main-engine plane (G2::GetBG1ScrPtr).
void func_02056494(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2::GetBG1ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG1 screen data to the sub-engine plane (G2S::GetBG1ScrPtr).
void func_02056434(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)G2S::GetBG1ScrPtr();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG2 screen data to the main-engine plane (G2::GetBG2ScrPtr).
void func_020563d4(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = G2::GetBG2ScrPtr();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads BG2 screen data to the sub-engine plane (G2S::GetBG2ScrPtr).
void func_02056374(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = (void *)G2S::GetBG2ScrPtr();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads BG3 screen data to the main-engine plane (G2::GetBG3ScrPtr).
void func_02056314(const void *src, u32 offset, u32 count) {
    void *bgPtr = G2::GetBG3ScrPtr();
    s32 ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    } else {
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
    }
}

// Loads BG3 screen data to the sub-engine plane (G2S::GetBG3ScrPtr).
void func_020562b4(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = (void *)G2S::GetBG3ScrPtr();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

}

namespace GX {

// Loads character data into MAIN BG0 VRAM (G2::GetBG0CharPtr, 0x2054efc).
void LoadBG0Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = func_02054efc();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

}

namespace GXS {

// Loads BG0 character data into SUB BG0 VRAM (G2S::GetBG0CharPtr).
void LoadBG0Char(const void *src, u32 offset, u32 size) {
    void *charPtr = (void *)G2S::GetBG0CharPtr();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

}

namespace GX {

// Loads character data into MAIN BG1 VRAM (G2::GetBG1CharPtr, 0x2054ea8).
void LoadBG1Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = func_02054ea8();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

}

namespace GXS {

// Loads BG1 character data into SUB BG1 VRAM (G2S::GetBG1CharPtr).
void LoadBG1Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)G2S::GetBG1CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

}

extern "C" {

// Loads BG2 char data into MAIN BG2 VRAM (G2::GetBG2CharPtr).
void func_020560d4(const void *src, u32 offset, u32 size) {
    void *charPtr = G2::GetBG2CharPtr();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

// Loads BG2 char data into SUB BG2 VRAM (G2S::GetBG2CharPtr).
void func_02056074(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)G2S::GetBG2CharPtr();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG3 char data into MAIN BG3 VRAM (0x2054d88 getter).
void func_02056014(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)func_02054d88();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG3 char data into SUB BG3 VRAM (G2S::GetBG3CharPtr).
void func_02055fb4(const void *src, u32 offset, u32 size) {
    void *charPtr = (void *)G2S::GetBG3CharPtr();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

}

namespace GXS {

// Clears the sub-engine BG ext-palette enable bit and unmaps the bank back to
// LCDC (GX::ResetBankForSubBGExtPltt); the result is held for EndLoadBGExtPltt.
void BeginLoadBGExtPltt() {
    data_020a60a4 = GX::ResetBankForSubBGExtPltt();
}

void LoadBGExtPltt(const void *src, u32 destSlotAddr, u32 size) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_DmaCopy32Async(dmaId, src, (void *)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int((int *)src, (int *)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size);
    }
}

void EndLoadBGExtPltt() {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_WaitDma(dmaId);
    }
    GX::SetBankForSubBGExtPltt(data_020a60a4);
    data_020a60a4 = 0;
}

// The OBJ counterpart (GX::ResetBankForSubOBJExtPltt); the result is held for
// EndLoadOBJExtPltt.
void BeginLoadOBJExtPltt() {
    data_020a60a8 = GX::ResetBankForSubOBJExtPltt();
}

void LoadOBJExtPltt(const void *src, u32 destSlotAddr, u32 size) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_DmaCopy32Async(dmaId, src, (void *)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int((int *)src, (int *)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size);
    }
}

void EndLoadOBJExtPltt() {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        MI_WaitDma(dmaId);
    }
    GX::SetBankForSubOBJExtPltt(data_020a60a8);
    data_020a60a8 = 0;
}

}
