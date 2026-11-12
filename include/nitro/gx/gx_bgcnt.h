#ifndef NITRO_GX_GX_BGCNT_H
#define NITRO_GX_GX_BGCNT_H

#include "nitro/hw/registers.h"

/* NitroSDK's OBJ character base getter (gx_bgcnt.h): a static inline that
 * returns the sub engine's OBJ VRAM. Read through the inline, the base is not
 * folded with an offset added to it, and the ROM builds 0x06602000 as
 * `mov #0x6600000 / add #0x2000` (func_ov002_020f39ec) where a literal
 * address would load it from the pool. */

/* The two engines' OBJ character VRAM, under the SDK's memory-map names. */
#define HW_OBJ_VRAM 0x06400000
#define HW_DB_OBJ_VRAM 0x06600000

/* The SDK's BGxCNT field setters, as macros rather than its static inlines:
 * a macro costs an includer no @NNN numbers. */

/* A background's display priority, 0 (front) to 3, in BGxCNT bits 0-1. */
#define G2_SetBG0Priority(priority) (REG_BG0CNT = (u16)((REG_BG0CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2_SetBG1Priority(priority) (REG_BG1CNT = (u16)((REG_BG1CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2_SetBG2Priority(priority) (REG_BG2CNT = (u16)((REG_BG2CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2_SetBG3Priority(priority) (REG_BG3CNT = (u16)((REG_BG3CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2S_SetBG0Priority(priority) (REG_DB_BG0CNT = (u16)((REG_DB_BG0CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2S_SetBG1Priority(priority) (REG_DB_BG1CNT = (u16)((REG_DB_BG1CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2S_SetBG2Priority(priority) (REG_DB_BG2CNT = (u16)((REG_DB_BG2CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))
#define G2S_SetBG3Priority(priority) (REG_DB_BG3CNT = (u16)((REG_DB_BG3CNT & ~BG_CONTROL_PRIORITY_MASK) | (priority)))

/* BGxCNT bit 6, a background's mosaic enable. */
#define G2_BG0Mosaic(enable) ((enable) ? (void)(REG_BG0CNT |= BG_CONTROL_MOSAIC) : (void)(REG_BG0CNT &= ~BG_CONTROL_MOSAIC))
#define G2_BG1Mosaic(enable) ((enable) ? (void)(REG_BG1CNT |= BG_CONTROL_MOSAIC) : (void)(REG_BG1CNT &= ~BG_CONTROL_MOSAIC))
#define G2_BG2Mosaic(enable) ((enable) ? (void)(REG_BG2CNT |= BG_CONTROL_MOSAIC) : (void)(REG_BG2CNT &= ~BG_CONTROL_MOSAIC))
#define G2_BG3Mosaic(enable) ((enable) ? (void)(REG_BG3CNT |= BG_CONTROL_MOSAIC) : (void)(REG_BG3CNT &= ~BG_CONTROL_MOSAIC))
#define G2S_BG0Mosaic(enable) ((enable) ? (void)(REG_DB_BG0CNT |= BG_CONTROL_MOSAIC) : (void)(REG_DB_BG0CNT &= ~BG_CONTROL_MOSAIC))
#define G2S_BG1Mosaic(enable) ((enable) ? (void)(REG_DB_BG1CNT |= BG_CONTROL_MOSAIC) : (void)(REG_DB_BG1CNT &= ~BG_CONTROL_MOSAIC))
#define G2S_BG2Mosaic(enable) ((enable) ? (void)(REG_DB_BG2CNT |= BG_CONTROL_MOSAIC) : (void)(REG_DB_BG2CNT &= ~BG_CONTROL_MOSAIC))
#define G2S_BG3Mosaic(enable) ((enable) ? (void)(REG_DB_BG3CNT |= BG_CONTROL_MOSAIC) : (void)(REG_DB_BG3CNT &= ~BG_CONTROL_MOSAIC))

#ifdef __cplusplus
extern "C" {
#endif

/* The BG base-address getters, defined in src/engine/gx/GX_BgCnt.c. Each
 * returns the layer's VRAM address, or null where the current mode gives the
 * layer no such base. */
void *G2_GetBG0ScrPtr(void);
void *G2S_GetBG0ScrPtr(void);
void *G2_GetBG1ScrPtr(void);
void *G2S_GetBG1ScrPtr(void);
void *G2_GetBG2ScrPtr(void);
void *G2S_GetBG2ScrPtr(void);
void *G2_GetBG3ScrPtr(void);
void *G2S_GetBG3ScrPtr(void);
void *G2_GetBG0CharPtr(void);
void *G2S_GetBG0CharPtr(void);
void *G2_GetBG1CharPtr(void);
void *G2S_GetBG1CharPtr(void);
void *G2_GetBG2CharPtr(void);
void *G2S_GetBG2CharPtr(void);
void *G2_GetBG3CharPtr(void);
void *G2S_GetBG3CharPtr(void);

#ifdef __cplusplus
}
#endif

/* The two engines' OBJ character bases. The address is fixed, so the SDK
 * reads it through an inline, not a call. Read through the getter, the ROM
 * builds 0x06401280 as `mov #0x6400000 / add #0x1280` where a literal
 * address would load it from the pool. */
static inline void *G2_GetOBJCharPtr(void) {
    return (void *)HW_OBJ_VRAM;
}

static inline void *G2S_GetOBJCharPtr(void) {
    return (void *)HW_DB_OBJ_VRAM;
}

#endif
