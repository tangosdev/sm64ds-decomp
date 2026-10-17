/* GXS -- sub-screen VRAM upload helpers (Nitro SDK namespace, not a class).
 * Owns .text 0x02055dec..0x02056870: the VRAM load family -- BG character and
 * screen loads, the OBJ/OAM/palette loads, and the extended-palette banked
 * upload family (EndLoadOBJExtPltt .. BeginLoadBGExtPltt). The GX:: members
 * inside this stretch (LoadBG1Char, LoadBG0Char, LoadOBJ, LoadOBJPltt) are
 * defined here too: GX/GXS are one original file pair.
 */
// leftovers: opt_propagation off is file-global for LoadOBJ and
// func_020565b4 (constant VRAM destinations 0x6400000/0x6600000); every other
// member matches either way. DMASyncWordTransfer and the SetBankFor* calls
// keep wide u32 spellings (local extern): the definitions' narrower params
// insert narrowing ops at the call. The func_ helpers keep address names (no
// recovered names exist).

#include "types.h"

/* opt_propagation off is what LoadOBJ and func_020565b4 (the two
 * constant-destination VRAM copies, 0x6400000/0x6600000) were built under. */
#pragma opt_propagation off

/* GX sub-screen extended palette VRAM bases */
#define GXS_OBJ_EXT_PLTT_BASE 0x068a0000u
#define GXS_BG_EXT_PLTT_BASE  0x06898000u

extern int data_02099fd0;  // RENDER_DMA_CHANNEL: DMA channel number, -1 if none
extern u32 data_020a60a4;  // BG ext-palette bank saved by BeginLoadBG, restored by EndLoadBG
extern u32 data_020a60a8;  // OBJ ext-palette bank saved by BeginLoadOBJ, restored by EndLoadOBJ

extern "C" {
extern u16 func_020540f0(void);
extern u16 func_02054118(void);
extern void func_02059fa8(int ch);
extern void func_02059fd0(int ch, int src, int dst, u32 size, void (*cb)(int), int cbarg);
// local extern: u8 channel per the definition inserts a narrowing op at the call
extern void DMASyncWordTransfer(u32 ch, const void *src, void *dst, u32 size);
extern void DMASyncHalfTransfer(u32 ch, const void *src, volatile void *dst, u32 count);
extern void MultiCopyHalf(const void *src, volatile void *dst, int count);
extern void MultiCopy_Int(int *src, int *dst, int len);

/* VRAM base-pointer getters, kept as literal symbol spellings (no header
 * declares them), returned the way each definition spells it: G2S for the
 * sub screen, G2 for the main screen. */
extern unsigned int _ZN3G2S13GetBG3CharPtrEv(void);
extern unsigned int _ZN3G2S13GetBG2CharPtrEv(void);
extern unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
extern unsigned int _ZN3G2S13GetBG0CharPtrEv(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern int func_02054d88(void);
extern void *func_02054ea8(void);
extern void *func_02054efc(void);
extern unsigned int _ZN3G2S12GetBG3ScrPtrEv(void);
extern void *_ZN2G212GetBG3ScrPtrEv(void);
extern unsigned int _ZN3G2S12GetBG2ScrPtrEv(void);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern unsigned int _ZN3G2S12GetBG1ScrPtrEv(void);
extern void *_ZN2G212GetBG1ScrPtrEv(void);
extern unsigned int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void *_ZN2G212GetBG0ScrPtrEv(void);

// local extern: u16 parameter per the definition inserts narrowing shifts at the call
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u32 bank);
extern void _ZN2GX23SetBankForSubOBJExtPlttEt(u32 bank);
}

namespace GXS {

// Loads BG palette data to sub-screen (GXS) palette VRAM at 0x05000400.
// @symbol _ZN3GXS10LoadBGPlttEPKvjj
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
// @symbol _ZN2GX11LoadOBJPlttEPKvjj
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
// @symbol _ZN3GXS11LoadOBJPlttEPKvjj
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
// @symbol func_020566dc
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
// @symbol func_02056674
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
// OBJ VRAM. Needs opt_propagation off: the constant is kept out of the
// address arithmetic.
// @symbol _ZN2GX7LoadOBJEPKvjj
void LoadOBJ(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    unsigned int base = 0x6400000;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(base + offset), count);
    }
}

}

extern "C" {

// Copies count words from src to VRAM address (0x6600000 + offset) -- sub
// OBJ VRAM. Same opt_propagation-off build as GX::LoadOBJ.
// @symbol func_020565b4
void func_020565b4(const void *src, unsigned int offset, unsigned int count) {
    unsigned int channel = data_02099fd0;
    unsigned int base = 0x6600000;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int((int *)src, (int *)(base + offset), count);
    }
}

// Loads BG0 screen data to the main-engine plane (G2::GetBG0ScrPtr).
// @symbol func_02056554
void func_02056554(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)_ZN2G212GetBG0ScrPtrEv();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG0 screen data to the sub-engine plane (G2S::GetBG0ScrPtr).
// @symbol func_020564f4
void func_020564f4(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)_ZN3G2S12GetBG0ScrPtrEv();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG1 screen data to the main-engine plane (G2::GetBG1ScrPtr).
// @symbol func_02056494
void func_02056494(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)_ZN2G212GetBG1ScrPtrEv();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG1 screen data to the sub-engine plane (G2S::GetBG1ScrPtr).
// @symbol func_02056434
void func_02056434(const void *src, int offset, int count) {
    volatile unsigned char *ptr = (volatile unsigned char *)_ZN3G2S12GetBG1ScrPtrEv();
    int ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, ptr + offset, count);
    } else {
        MultiCopyHalf(src, ptr + offset, count);
    }
}

// Loads BG2 screen data to the main-engine plane (G2::GetBG2ScrPtr).
// @symbol func_020563d4
void func_020563d4(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = _ZN2G212GetBG2ScrPtrEv();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads BG2 screen data to the sub-engine plane (G2S::GetBG2ScrPtr).
// @symbol func_02056374
void func_02056374(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = (void *)_ZN3G2S12GetBG2ScrPtrEv();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

// Loads BG3 screen data to the main-engine plane (G2::GetBG3ScrPtr).
// @symbol func_02056314
void func_02056314(const void *src, u32 offset, u32 count) {
    void *bgPtr = _ZN2G212GetBG3ScrPtrEv();
    s32 ch = data_02099fd0;
    if (ch != -1) {
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    } else {
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
    }
}

// Loads BG3 screen data to the sub-engine plane (G2S::GetBG3ScrPtr).
// @symbol func_020562b4
void func_020562b4(const void *src, u32 offset, u32 count) {
    void *bgPtr;
    u32 ch;

    bgPtr = (void *)_ZN3G2S12GetBG3ScrPtrEv();
    ch = data_02099fd0;
    if (ch != (u32)-1)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}

}

namespace GX {

// Loads character data into MAIN BG0 VRAM (G2::GetBG0CharPtr, 0x2054efc).
// @symbol _ZN2GX11LoadBG0CharEPKvjj
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
// @symbol _ZN3GXS11LoadBG0CharEPKvjj
void LoadBG0Char(const void *src, u32 offset, u32 size) {
    void *charPtr = (void *)_ZN3G2S13GetBG0CharPtrEv();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

}

namespace GX {

// Loads character data into MAIN BG1 VRAM (G2::GetBG1CharPtr, 0x2054ea8).
// @symbol _ZN2GX11LoadBG1CharEPKvjj
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
// @symbol _ZN3GXS11LoadBG1CharEPKvjj
void LoadBG1Char(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)_ZN3G2S13GetBG1CharPtrEv();
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
// @symbol func_020560d4
void func_020560d4(const void *src, u32 offset, u32 size) {
    void *charPtr = _ZN2G213GetBG2CharPtrEv();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

// Loads BG2 char data into SUB BG2 VRAM (G2S::GetBG2CharPtr).
// @symbol func_02056074
void func_02056074(const void *src, unsigned int offset, unsigned int count) {
    void *base = (void *)_ZN3G2S13GetBG2CharPtrEv();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int((int *)src, (int *)((char *)base + offset), count);
    }
}

// Loads BG3 char data into MAIN BG3 VRAM (0x2054d88 getter).
// @symbol func_02056014
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
// @symbol func_02055fb4
void func_02055fb4(const void *src, u32 offset, u32 size) {
    void *charPtr = (void *)_ZN3G2S13GetBG3CharPtrEv();
    if (data_02099fd0 != (u32)-1)
        DMASyncWordTransfer(data_02099fd0, src, (char *)charPtr + offset, size);
    else
        MultiCopy_Int((int *)src, (int *)((char *)charPtr + offset), size);
}

}

namespace GXS {

// @symbol _ZN3GXS18BeginLoadBGExtPlttEv
// Clears the sub-engine BG ext-palette enable bit and unmaps the bank back to
// LCDC (func_02054118); the result is held for EndLoadBGExtPltt.
void BeginLoadBGExtPltt() {
    data_020a60a4 = func_02054118();
}

// @symbol _ZN3GXS13LoadBGExtPlttEPKvjj
void LoadBGExtPltt(const void *src, u32 destSlotAddr, u32 size) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fd0(dmaId, (int)src, (int)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int((int *)src, (int *)(destSlotAddr + GXS_BG_EXT_PLTT_BASE), size);
    }
}

// @symbol _ZN3GXS16EndLoadBGExtPlttEv
void EndLoadBGExtPltt() {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX22SetBankForSubBGExtPlttEt(data_020a60a4);
    data_020a60a4 = 0;
}

// @symbol _ZN3GXS19BeginLoadOBJExtPlttEv
// The OBJ counterpart (func_020540f0); the result is held for
// EndLoadOBJExtPltt.
void BeginLoadOBJExtPltt() {
    data_020a60a8 = func_020540f0();
}

// @symbol _ZN3GXS14LoadOBJExtPlttEPKvjj
void LoadOBJExtPltt(const void *src, u32 destSlotAddr, u32 size) {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fd0(dmaId, (int)src, (int)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size, 0, 0);
    } else {
        MultiCopy_Int((int *)src, (int *)(destSlotAddr + GXS_OBJ_EXT_PLTT_BASE), size);
    }
}

// @symbol _ZN3GXS17EndLoadOBJExtPlttEv
void EndLoadOBJExtPltt() {
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX23SetBankForSubOBJExtPlttEt(data_020a60a8);
    data_020a60a8 = 0;
}

}
