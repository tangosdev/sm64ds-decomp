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
 * port/tools/hostgen.py, which emits the whole file and routes each literal
 * MMIO dereference through the host's register model. G3X_ClearFifo keeps
 * its literal addresses for that reason (a register macro from a header
 * would reach the host unrewritten). The rest calls SDK code the host build
 * doesn't link, so it sits under #ifndef _MSC_VER.
 */
#include "nitro/gx/g3x.h"
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/hw/registers.h"
#include "private/gxdma.h"

extern "C" {
extern int GXi_DmaId;
void GXi_NopClearFifo128_(unsigned addr);
}

#ifndef _MSC_VER

/* The SDK's g3x.h inlines this file uses that the shared header leaves out
 * (an inline body there renumbers its includers' @NNN symbols). */
static inline bool G3X_IsGeometryBusy(void) {
    return (REG_GXSTAT & REG_G3X_GXSTAT_B_MASK) != 0;
}

static inline void G3X_SetFifoIntrCond(int cond) {
    REG_GXSTAT = (REG_GXSTAT & ~REG_G3X_GXSTAT_FI_MASK) | (cond << REG_G3X_GXSTAT_FI_SHIFT);
}

/* POLYGON_ATTR's reset value: opaque (alpha 31), front faces drawn. */
#define G3X_POLYGON_ATTR_DEFAULT 0x001f0080

void G3X_Init(void) {
    G3X_ClearFifo();
    REG_END_VTXS = 0;
    while (G3X_IsGeometryBusy())
        ;
    REG_DISP3DCNT = 0;
    REG_GXSTAT = 0;
    REG_BG0OFS = 0;
    REG_DISP3DCNT |= REG_G3X_DISP3DCNT_GO_MASK;
    REG_DISP3DCNT |= REG_G3X_DISP3DCNT_RO_MASK;
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(true);
    G3X_AlphaTest(false, 0);
    REG_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    G3X_SetFifoIntrCond(GX_FIFOINTR_COND_EMPTY);
    G3X_InitMtxStack();
    REG_CLEAR_COLOR = 0;
    REG_CLEAR_DEPTH = 0x7fff;
    REG_CLRIMAGE_OFFSET = 0;
    REG_FOG_COLOR = 0;
    REG_FOG_OFFSET = 0;
    G2_SetBG0Priority(0);
    G3X_InitTable();
    REG_POLYGON_ATTR = G3X_POLYGON_ATTR_DEFAULT;
    reg_G3_TEXIMAGE_PARAM = 0;
    reg_G3_TEXPLTT_BASE = 0;
}

void G3X_Reset(void) {
    while (G3X_IsGeometryBusy())
        ;
    REG_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    REG_DISP3DCNT |= REG_G3X_DISP3DCNT_GO_MASK;
    REG_DISP3DCNT |= REG_G3X_DISP3DCNT_RO_MASK;
    G3X_ResetMtxStack();
    REG_POLYGON_ATTR = G3X_POLYGON_ATTR_DEFAULT;
    reg_G3_TEXIMAGE_PARAM = 0;
    reg_G3_TEXPLTT_BASE = 0;
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
    int PV_level, PJ_level;
    REG_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    while (G3X_GetMtxStackLevelPV(&PV_level))
        ;
    while (G3X_GetMtxStackLevelPJ(&PJ_level))
        ;
    REG_MTX_MODE = GX_MTXMODE_TEXTURE;
    REG_MTX_IDENTITY = 0;
    REG_MTX_MODE = GX_MTXMODE_PROJECTION;
    if (PJ_level != 0)
        REG_MTX_POP = PJ_level;
    REG_MTX_IDENTITY = 0;
    REG_MTX_MODE = GX_MTXMODE_POSITION_VECTOR;
    REG_MTX_POP = PV_level;
    REG_MTX_IDENTITY = 0;
}

void G3X_ResetMtxStack(void) {
    int PV_level;
    int PJ_level;
    REG_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    while (G3X_GetMtxStackLevelPV(&PV_level))
        ;
    while (G3X_GetMtxStackLevelPJ(&PJ_level))
        ;
    REG_MTX_MODE = GX_MTXMODE_TEXTURE;
    REG_MTX_IDENTITY = 0;
    REG_MTX_MODE = GX_MTXMODE_PROJECTION;
    if (PJ_level != 0)
        REG_MTX_POP = PJ_level;
    REG_MTX_MODE = GX_MTXMODE_POSITION_VECTOR;
    REG_MTX_POP = PV_level;
    REG_MTX_IDENTITY = 0;
}

/* The Nitro SDK's G3X_SetFog(BOOL enable, GXFogBlend, GXFogSlope, int): the
   flag is an int. The body tests it the same either way, and dScStage_c's
   RenderFog passes the stage's fog byte with no 0/1 conversion, which a bool
   parameter would insert. */
void G3X_SetFog(int enable, int fogMode, int fogSlope, int fogOffset) {
    if (enable) {
        REG_FOG_OFFSET = (u16)fogOffset;
        REG_DISP3DCNT =
            (fogSlope << REG_G3X_DISP3DCNT_FOG_SHIFT_SHIFT) |
            (fogMode << REG_G3X_DISP3DCNT_FMOD_SHIFT) | REG_G3X_DISP3DCNT_FME_MASK |
            (REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_FOG_SHIFT_MASK | REG_G3X_DISP3DCNT_FMOD_MASK |
                               REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK));
    } else {
        REG_DISP3DCNT =
            REG_DISP3DCNT & (u16) ~(REG_G3X_DISP3DCNT_FME_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                    REG_G3X_DISP3DCNT_GO_MASK);
    }
}

void G3X_SetFogTable(void *fogTable) {
    MI_Copy32B(fogTable, (void *)REG_FOG_TABLE_ADDR);
}

void G3X_SetToonTable(void *toonTable) {
    MI_CpuCopy16(toonTable, (void *)REG_TOON_TABLE_ADDR, 0x40);
}

void G3X_SetClearColor(unsigned short color, int alpha, int depth, int polygonID, int fog) {
    unsigned int val = (color | (alpha << 16)) | (polygonID << 24);
    if (fog)
        val |= 0x8000;
    REG_CLEAR_COLOR = val;
    REG_CLEAR_DEPTH = (unsigned short)depth;
}

void G3X_InitTable(void) {
    int i;
    if (GXi_DmaId != GX_DMA_NOT_USE) {
        MI_DmaClear32Async(GXi_DmaId, (void *)REG_EDGE_COLOR_ADDR, 0x10, 0, 0);
        MI_DmaClear32(GXi_DmaId, (void *)REG_FOG_TABLE_ADDR, 0x50);
    } else {
        volatile int a = 0;
        volatile int b;
        MI_CpuFill32((void *)REG_EDGE_COLOR_ADDR, a, 0x10);
        b = 0;
        MI_CpuFill32((void *)REG_FOG_TABLE_ADDR, b, 0x50);
    }
    for (i = 0; i < 0x20; i++)
        REG_SHININESS = 0;
}

int G3X_GetMtxStackLevelPV(int *level) {
    if (REG_GXSTAT & REG_G3X_GXSTAT_SB_MASK)
        return -1;
    *level = (REG_GXSTAT & REG_G3X_GXSTAT_PV_MASK) >> REG_G3X_GXSTAT_PV_SHIFT;
    return 0;
}

int G3X_GetMtxStackLevelPJ(int *level) {
    if (REG_GXSTAT & REG_G3X_GXSTAT_SB_MASK)
        return -1;
    *level = (REG_GXSTAT & REG_G3X_GXSTAT_PJ_MASK) >> REG_G3X_GXSTAT_PJ_SHIFT;
    return 0;
}

// The 3D layer's horizontal offset (BG0HOFS).
void G3X_SetHOffset(int hOffset) {
    REG_BG0OFS = hOffset;
}

#endif
