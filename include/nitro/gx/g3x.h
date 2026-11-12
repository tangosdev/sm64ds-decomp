#ifndef NITRO_GX_G3X_H
#define NITRO_GX_G3X_H

#include "types.h"
#include "nitro/hw/registers.h"

/* NitroSDK's DISP3DCNT setters (g3x.h). Every write masks off bits 12 and 13,
 * the color-buffer underflow and polygon/vertex RAM overflow
 * acknowledgements, which a 1 would clear. The SDK's BOOL is spelled bool
 * in these setters. Not in
 * g3imm.h: an inline body there renumbers ov075/d_s_entry_entry's pinned
 * @NNN (src/unnamed/ov007/func_ov007_020c4684.c). */

#define REG_G3X_DISP3DCNT_TME_MASK 0x0001
#define REG_G3X_DISP3DCNT_THS_MASK 0x0002
#define REG_G3X_DISP3DCNT_ATE_MASK 0x0004
#define REG_G3X_DISP3DCNT_ABE_MASK 0x0008
#define REG_G3X_DISP3DCNT_AAE_MASK 0x0010
#define REG_G3X_DISP3DCNT_EME_MASK 0x0020
#define REG_G3X_DISP3DCNT_FMOD_MASK 0x0040
#define REG_G3X_DISP3DCNT_FME_MASK  0x0080
#define REG_G3X_DISP3DCNT_FOG_SHIFT_MASK 0x0f00
#define REG_G3X_DISP3DCNT_RO_MASK  0x1000
#define REG_G3X_DISP3DCNT_GO_MASK  0x2000
#define REG_G3X_DISP3DCNT_THS_SHIFT 1
#define REG_G3X_DISP3DCNT_FMOD_SHIFT 6
#define REG_G3X_DISP3DCNT_FOG_SHIFT_SHIFT 8

/* GXSTAT (REG_GXSTAT): the position/vector and projection stack levels, the
 * stack busy and stack error flags (writing 1 to SE clears it), the geometry
 * engine's busy flag and the FIFO interrupt condition. */
#define REG_G3X_GXSTAT_PV_MASK 0x00001f00
#define REG_G3X_GXSTAT_PJ_MASK 0x00002000
#define REG_G3X_GXSTAT_SB_MASK 0x00004000
#define REG_G3X_GXSTAT_SE_MASK 0x00008000
#define REG_G3X_GXSTAT_B_MASK  0x08000000
#define REG_G3X_GXSTAT_FI_MASK 0xc0000000
#define REG_G3X_GXSTAT_PV_SHIFT 8
#define REG_G3X_GXSTAT_PJ_SHIFT 13
#define REG_G3X_GXSTAT_FI_SHIFT 30

/* G3X_SetFifoIntrCond's conditions: the FIFO interrupt never fires, fires
 * below half full, or fires when the FIFO is empty. */
#define GX_FIFOINTR_COND_DISABLE   0
#define GX_FIFOINTR_COND_UNDERHALF 1
#define GX_FIFOINTR_COND_EMPTY     2

/* gx_g3x.c, under the SDK's names with C linkage (ntrtwl/NitroSDK
 * libraries/gx/src/g3x.c). Parameters unnamed: a named parameter here
 * renumbers every includer's @NNN.
 * G3X_SetFog and G3X_SetClearColor take the SDK's BOOL as an int: callers
 * pass a stored flag byte straight through. GXRgb is spelled unsigned short.
 * The stack-level getters' s32 * is spelled int *. */
#ifdef __cplusplus
extern "C" {
#endif
void G3X_Init(void);
void G3X_Reset(void);
void G3X_ClearFifo(void);
void G3X_InitMtxStack(void);
void G3X_ResetMtxStack(void);
void G3X_SetFog(int, int, int, int);
void G3X_SetFogTable(void *);
void G3X_SetToonTable(void *);
void G3X_SetClearColor(unsigned short, int, int, int, int);
void G3X_InitTable(void);
int G3X_GetMtxStackLevelPV(int *);
int G3X_GetMtxStackLevelPJ(int *);
void G3X_SetHOffset(int);
#ifdef __cplusplus
}
#endif

typedef enum { GX_SHADING_TOON = 0, GX_SHADING_HIGHLIGHT = 1 } GXShading;

static inline void G3X_SetShading(GXShading shading) {
    REG_DISP3DCNT =
        (u16)((REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_THS_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK)) |
              (shading << REG_G3X_DISP3DCNT_THS_SHIFT));
}

static inline void G3X_AlphaBlend(bool enable) {
    if (enable) {
        REG_DISP3DCNT =
            (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK) |
                  REG_G3X_DISP3DCNT_ABE_MASK);
    } else {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_ABE_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    }
}

static inline void G3X_AntiAlias(bool enable) {
    if (enable) {
        REG_DISP3DCNT =
            (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK) |
                  REG_G3X_DISP3DCNT_AAE_MASK);
    } else {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_AAE_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    }
}

static inline void G3X_EdgeMarking(bool enable) {
    if (enable) {
        REG_DISP3DCNT =
            (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK) |
                  REG_G3X_DISP3DCNT_EME_MASK);
    } else {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_EME_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    }
}

/* G3X_AlphaTest and G3X_SetFogColor are static inlines in NitroSDK's g3x.h
 * (ntrtwl/NitroSDK include/nitro/gx/g3x.h). They are spelled as macros here:
 * a new inline body in this header would renumber every includer's @NNN. */
#define G3X_AlphaTest(enable, ref)                                                     \
    do {                                                                               \
        if (enable) {                                                                  \
            REG_DISP3DCNT = (u16)(REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK |         \
                                                    REG_G3X_DISP3DCNT_GO_MASK) |        \
                                  REG_G3X_DISP3DCNT_ATE_MASK);                          \
            REG_ALPHA_TEST_REF = (u16)(ref);                                           \
        } else {                                                                       \
            REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_ATE_MASK |                      \
                                     REG_G3X_DISP3DCNT_RO_MASK |                       \
                                     REG_G3X_DISP3DCNT_GO_MASK);                       \
        }                                                                              \
    } while (0)

#define G3X_SetFogColor(rgb, alpha) (REG_FOG_COLOR = (u32)((rgb) | ((alpha) << 16)))

#endif
