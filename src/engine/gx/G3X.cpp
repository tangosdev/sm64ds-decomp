//cpp
/* engine/gx/G3X.cpp -- the 3D engine's setup and register writers (the Nitro
 * SDK's gx_g3x.c), arm9 .text 0x02055454..0x02055998.
 *
 * mwccarm emits .text in reverse source order, so the file reads top-down
 * from G3X_Init at the top of the run. Every function is the SDK's, under
 * its SDK name with C linkage (include/nitro/gx/g3x.h; ntrtwl/NitroSDK
 * libraries/gx/src/g3x.c). The file's last function, the asm FIFO flush
 * GXi_NopClearFifo128_ (0x020553c0), stays its own source:
 * port/tools/hostgen.py skips any file that holds an asm block, and the port
 * hostgens this one.
 * G3X_GetBoxTestResult, G3X_GetVectorMtx, G3X_GetClipMtx,
 * G3X_SetEdgeColorTable, G3X_SetDisp1DotDepth and the position/vector test
 * getters are not in this ROM. pret/pokediamond's GX_g3x.c has no
 * G3X_SetToonTable and calls G3X_Reset and G3X_ResetMtxStack
 * "G3X_ResetMtxStack" and "G3X_ResetMtxStack_2".
 *
 * InitTable's split volatile zeroing (init for a, store for b) is as
 * built: merging b into an init reschedules the body. GXi_DmaId is the
 * shared render DMA channel (named at its GX.cpp use).
 *
 * PC port: smoke_gx builds G3X_ClearFifo from this file through
 * port/tools/hostgen.py, which emits the whole file. The rest calls SDK
 * code the host build doesn't link, so it sits under #ifndef _MSC_VER.
 */
#include "nitro/gx/g3x.h"
#include "nitro/hw/registers.h"
#include "nitro/mi/memory.h"

extern "C" {
extern int GXi_DmaId;
void MI_DmaFill32Async(unsigned int dmaNo, void *dest, unsigned int data, unsigned int size,
                       void (*callback)(void *), void *arg);
void MI_DmaFill32(unsigned int channel, void *dst, unsigned int data, unsigned int size);
void GXi_NopClearFifo128_(unsigned addr);
}

#ifndef _MSC_VER

void G3X_Init(void) {
    G3X_ClearFifo();
    *(volatile unsigned int *)0x4000504 = 0;
    while (*(volatile unsigned int *)0x4000600 & 0x8000000)
        ;
    REG_DISP3DCNT = 0;
    *(volatile unsigned int *)0x4000600 = 0;
    REG_BG0OFS = 0;
    REG_DISP3DCNT |= 0x2000;
    REG_DISP3DCNT |= 0x1000;
    REG_DISP3DCNT &= ~0x3002;
    REG_DISP3DCNT = (REG_DISP3DCNT & ~0x3000) | 0x10;
    REG_DISP3DCNT &= 0xcffb;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    *(volatile unsigned int *)0x4000600 =
        (*(volatile unsigned int *)0x4000600 & ~0xc0000000) | 0x80000000;
    G3X_InitMtxStack();
    *(volatile unsigned int *)0x4000350 = 0;
    *(volatile unsigned short *)0x4000354 = 0x7fff;
    *(volatile unsigned short *)0x4000356 = 0;
    *(volatile unsigned int *)0x4000358 = 0;
    *(volatile unsigned short *)0x400035c = 0;
    REG_BG0CNT &= ~3;
    G3X_InitTable();
    *(volatile unsigned int *)0x40004a4 = 0x1f0080;
    *(volatile unsigned int *)0x40004a8 = 0;
    *(volatile unsigned int *)0x40004ac = 0;
}

void G3X_Reset(void) {
    while (*(volatile unsigned int *)0x4000600 & 0x8000000)
        ;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    REG_DISP3DCNT |= 0x2000;
    REG_DISP3DCNT |= 0x1000;
    G3X_ResetMtxStack();
    *(volatile unsigned int *)0x40004a4 = 0x1f0080;
    *(volatile unsigned int *)0x40004a8 = 0;
    *(volatile unsigned int *)0x40004ac = 0;
}

#endif

// Flushes the geometry FIFO, then waits until the engine is idle.
void G3X_ClearFifo(void) {
    GXi_NopClearFifo128_(0x4000400);
    while (*(volatile unsigned *)0x4000600 & 0x8000000)
        ;
}

#ifndef _MSC_VER

void G3X_InitMtxStack(void) {
    int a, b;
    *(volatile u32 *)0x4000600 |= 0x8000;
    while (G3X_GetMtxStackLevelPV(&a))
        ;
    while (G3X_GetMtxStackLevelPJ(&b))
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

void G3X_ResetMtxStack(void) {
    int a;
    int b;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    while (G3X_GetMtxStackLevelPV(&a))
        ;
    while (G3X_GetMtxStackLevelPJ(&b))
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
void G3X_SetFog(int enable, int a, int b, int c) {
    if (enable) {
        *(volatile unsigned short *)0x400035c = (unsigned short)c;
        REG_DISP3DCNT = (b << 8) | (a << 6) | 0x80 | (REG_DISP3DCNT & ~0x3f40);
    } else {
        REG_DISP3DCNT = REG_DISP3DCNT & 0xcf7f;
    }
}

// FOG_TABLE (0x4000360) takes the whole 32-byte table from the caller buffer.
void G3X_SetFogTable(void *table) {
    MI_Copy32B(table, (void *)0x4000360);
}

// The 32-entry toon table to TOON_TABLE (0x4000380).
void G3X_SetToonTable(void *dst) {
    MI_CpuCopy16(dst, (void *)0x04000380, 0x40);
}

void G3X_SetClearColor(unsigned short a, int b, int c, int d, int fog) {
    unsigned int v = (a | (b << 16)) | (d << 24);
    if (fog)
        v |= 0x8000;
    *(volatile unsigned int *)0x4000350 = v;
    *(volatile unsigned short *)0x4000354 = (unsigned short)c;
}

void G3X_InitTable(void) {
    int i;
    if (GXi_DmaId != -1) {
        MI_DmaFill32Async(GXi_DmaId, (void *)0x4000330, 0, 0x10, 0, 0);
        MI_DmaFill32(GXi_DmaId, (void *)0x4000360, 0, 0x50);
    } else {
        volatile int a = 0;
        volatile int b;
        MI_CpuFill32((int *)0x4000330, a, 0x10);
        b = 0;
        MI_CpuFill32((int *)0x4000360, b, 0x50);
    }
    for (i = 0; i < 0x20; i++)
        *(volatile int *)0x40004d0 = 0;
}

// The position/vector stack level from GXSTAT.
int G3X_GetMtxStackLevelPV(int *out) {
    if (*(volatile unsigned int *)0x4000600 & 0x4000)
        return -1;
    *out = (*(volatile unsigned int *)0x4000600 & 0x1f00) >> 8;
    return 0;
}

// The projection stack level from GXSTAT.
int G3X_GetMtxStackLevelPJ(int *out) {
    if (*(volatile unsigned int *)0x4000600 & 0x4000)
        return -1;
    *out = (*(volatile unsigned int *)0x4000600 & 0x2000) >> 13;
    return 0;
}

// The 3D layer's horizontal offset (BG0HOFS).
void G3X_SetHOffset(int v) {
    REG_BG0OFS = v;
}

#endif
