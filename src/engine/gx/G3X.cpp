//cpp
/* engine/gx/G3X.cpp -- the 3D engine's setup and register writers (the Nitro
 * SDK's gx_g3x.c), arm9 .text 0x02055454..0x02055998.
 *
 * mwccarm emits .text in reverse source order, so the file reads top-down
 * from G3X_Init (func_0205583c) at the top of the run. G3X_Init and
 * G3X_SetToonTable keep their address names, with the SDK name in each
 * comment: their callers name them that way. The file's
 * last function, the asm FIFO flush GXi_NopClearFifo128_ (0x020553c0),
 * stays its own source: port/tools/hostgen.py skips any file that holds an
 * asm block, and the port hostgens this one.
 * G3X_GetBoxTestResult, G3X_GetVectorMtx, G3X_GetClipMtx and
 * G3X_SetEdgeColorTable are not in this ROM; func_020555a4 (the toon table)
 * is not in pret's file.
 *
 * SetClearColor reads the bool slot as an int (byte-proven widening; the
 * header's bool spelling emits a narrower compare at the call).
 * InitTable's split volatile zeroing (init for a, store for b) is as
 * built: merging b into an init reschedules the body. GXi_DmaId is the
 * shared render DMA channel (named at its GX.cpp use).
 *
 * PC port: smoke_gx builds G3X::ClearFifo from this file through
 * port/tools/hostgen.py, which emits the whole file. The rest calls SDK
 * code the host build doesn't link, so it sits under #ifndef _MSC_VER.
 */
#include "G3X.h"
#include "nitro/hw/registers.h"

extern "C" {
void MultiCopyHalf(const void *src, void *dst, int size);
void Copy32Bytes(void *src, void *dst);
extern int GXi_DmaId;
void MI_DmaFill32Async(unsigned int dmaNo, void *dest, unsigned int data, unsigned int size,
                       void (*callback)(void *), void *arg);
void DMASyncFillTransfer(unsigned int channel, void *dst, unsigned int data,
                         unsigned int size);
void MultiStore_Int(int val, int *dst, int len);
void GXi_NopClearFifo128_(unsigned addr);
void func_020555a4(void *dst);
void func_0205583c(void);
}

#ifndef _MSC_VER

// G3X_Init
extern "C" void func_0205583c(void) {
    G3X::ClearFifo();
    *(volatile unsigned int *)0x4000504 = 0;
    while (*(volatile unsigned int *)0x4000600 & 0x8000000)
        ;
    *(volatile unsigned short *)0x4000060 = 0;
    *(volatile unsigned int *)0x4000600 = 0;
    *(volatile unsigned int *)0x4000010 = 0;
    *(volatile unsigned short *)0x4000060 |= 0x2000;
    *(volatile unsigned short *)0x4000060 |= 0x1000;
    *(volatile unsigned short *)0x4000060 &= ~0x3002;
    *(volatile unsigned short *)0x4000060 =
        (*(volatile unsigned short *)0x4000060 & ~0x3000) | 0x10;
    *(volatile unsigned short *)0x4000060 &= 0xcffb;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    *(volatile unsigned int *)0x4000600 =
        (*(volatile unsigned int *)0x4000600 & ~0xc0000000) | 0x80000000;
    G3X::InitMtxStack();
    *(volatile unsigned int *)0x4000350 = 0;
    *(volatile unsigned short *)0x4000354 = 0x7fff;
    *(volatile unsigned short *)0x4000356 = 0;
    *(volatile unsigned int *)0x4000358 = 0;
    *(volatile unsigned short *)0x400035c = 0;
    *(volatile unsigned short *)0x4000008 &= ~3;
    G3X::InitTable();
    *(volatile unsigned int *)0x40004a4 = 0x1f0080;
    *(volatile unsigned int *)0x40004a8 = 0;
    *(volatile unsigned int *)0x40004ac = 0;
}

void G3X::Reset(void) {
    while (*(volatile unsigned int *)0x4000600 & 0x8000000)
        ;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    *(volatile unsigned short *)0x4000060 |= 0x2000;
    *(volatile unsigned short *)0x4000060 |= 0x1000;
    ResetMtxStack();
    *(volatile unsigned int *)0x40004a4 = 0x1f0080;
    *(volatile unsigned int *)0x40004a8 = 0;
    *(volatile unsigned int *)0x40004ac = 0;
}

#endif

// Flushes the geometry FIFO, then waits until the engine is idle.
void G3X::ClearFifo(void) {
    GXi_NopClearFifo128_(0x4000400);
    while (*(volatile unsigned *)0x4000600 & 0x8000000)
        ;
}

#ifndef _MSC_VER

void G3X::InitMtxStack(void) {
    int a, b;
    *(volatile u32 *)0x4000600 |= 0x8000;
    while (GetMtxStackLevelPV(&a))
        ;
    while (GetMtxStackLevelPJ(&b))
        ;
    *(volatile u32 *)0x4000440 = 3;
    *(volatile u32 *)0x4000454 = 0;
    *(volatile u32 *)0x4000440 = 0;
    if (b != 0)
        *(volatile u32 *)0x4000448 = b;
    *(volatile u32 *)0x4000454 = 0;
    *(volatile u32 *)0x4000440 = 2;
    *(volatile u32 *)0x4000448 = a;
    *(volatile u32 *)0x4000454 = 0;
}

void G3X::ResetMtxStack(void) {
    int a;
    int b;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    while (GetMtxStackLevelPV(&a))
        ;
    while (GetMtxStackLevelPJ(&b))
        ;
    *(volatile unsigned int *)0x4000440 = 3;
    *(volatile unsigned int *)0x4000454 = 0;
    *(volatile unsigned int *)0x4000440 = 0;
    if (b != 0)
        *(volatile unsigned int *)0x4000448 = b;
    *(volatile unsigned int *)0x4000440 = 2;
    *(volatile unsigned int *)0x4000448 = a;
    *(volatile unsigned int *)0x4000454 = 0;
}

/* The Nitro SDK's G3X_SetFog(BOOL enable, GXFogBlend, GXFogSlope, int): the
   flag is an int. The body tests it the same either way, and dScStage_c's
   RenderFog passes the stage's fog byte with no 0/1 conversion, which a bool
   parameter would insert. */
void G3X::SetFog(int enable, int a, int b, int c) {
    if (enable) {
        *(volatile unsigned short *)0x400035c = (unsigned short)c;
        *(volatile unsigned short *)0x4000060 =
            (b << 8) | (a << 6) | 0x80 | (*(volatile unsigned short *)0x4000060 & ~0x3f40);
    } else {
        *(volatile unsigned short *)0x4000060 = *(volatile unsigned short *)0x4000060 & 0xcf7f;
    }
}

// FOG_TABLE (0x4000360) takes the whole 32-byte table from the caller buffer.
void G3X::SetFogTable(void *table) {
    Copy32Bytes(table, (void *)0x4000360);
}

// G3X_SetToonTable: the 32-entry toon table to TOON_TABLE (0x4000380).
extern "C" void func_020555a4(void *dst) {
    MultiCopyHalf(dst, (void *)0x04000380, 0x40);
}

void G3X::SetClearColor(unsigned short a, int b, int c, int d, bool e) {
    unsigned int v = (a | (b << 16)) | (d << 24);
    if (*(int *)&e)
        v |= 0x8000; // bool slot reads whole-word (see the banner)
    *(volatile unsigned int *)0x4000350 = v;
    *(volatile unsigned short *)0x4000354 = (unsigned short)c;
}

void G3X::InitTable(void) {
    int i;
    if (GXi_DmaId != -1) {
        MI_DmaFill32Async(GXi_DmaId, (void *)0x4000330, 0, 0x10, 0, 0);
        DMASyncFillTransfer(GXi_DmaId, (void *)0x4000360, 0, 0x50);
    } else {
        volatile int a = 0;
        volatile int b;
        MultiStore_Int(a, (int *)0x4000330, 0x10);
        b = 0;
        MultiStore_Int(b, (int *)0x4000360, 0x50);
    }
    for (i = 0; i < 0x20; i++)
        *(volatile int *)0x40004d0 = 0;
}

// The position/vector stack level from GXSTAT.
int G3X::GetMtxStackLevelPV(int *out) {
    if (*(volatile unsigned int *)0x4000600 & 0x4000)
        return -1;
    *out = (*(volatile unsigned int *)0x4000600 & 0x1f00) >> 8;
    return 0;
}

// The projection stack level from GXSTAT.
int G3X::GetMtxStackLevelPJ(int *out) {
    if (*(volatile unsigned int *)0x4000600 & 0x4000)
        return -1;
    *out = (*(volatile unsigned int *)0x4000600 & 0x2000) >> 13;
    return 0;
}

// G3X_SetHOffset: the 3D layer's horizontal offset (BG0HOFS).
void G3X::SetHOffset(int v) {
    REG_BG0OFS = v;
}

#endif
