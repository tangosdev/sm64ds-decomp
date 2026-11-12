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

/* The rest of BGxCNT, set whole by G2_SetBGnControl*: what each keeps is
 * the priority and mosaic. Field values are the SDK's GXBGScrSizeText,
 * GXBGColorMode, GXBGScrBase (2 KB steps), GXBGCharBase (16 KB steps),
 * GXBGExtPltt, GXBGAreaOver, GXBGScrSizeDcBmp and GXBGBmpScrBase (16 KB
 * steps). */
#define GX_BG_SCRSIZE_TEXT_256x256 0
#define GX_BG_SCRSIZE_TEXT_512x256 1
#define GX_BG_SCRSIZE_TEXT_256x512 2
#define GX_BG_SCRSIZE_TEXT_512x512 3
#define GX_BG_SCRSIZE_DCBMP_128x128 0
#define GX_BG_SCRSIZE_DCBMP_256x256 1
#define GX_BG_SCRSIZE_DCBMP_512x256 2
#define GX_BG_SCRSIZE_DCBMP_512x512 3
#define GX_BG_COLORMODE_16 0
#define GX_BG_COLORMODE_256 1
#define GX_BG_EXTPLTT_01 0
#define GX_BG_EXTPLTT_23 1
#define GX_BG_AREAOVER_XLU 0
#define GX_BG_AREAOVER_REPEAT 1
#define GX_BG_SCRBASE_0x0000 0
#define GX_BG_SCRBASE_0x0800 1
#define GX_BG_SCRBASE_0x1000 2
#define GX_BG_SCRBASE_0x1800 3
#define GX_BG_SCRBASE_0x2000 4
#define GX_BG_SCRBASE_0x2800 5
#define GX_BG_SCRBASE_0x3000 6
#define GX_BG_SCRBASE_0x3800 7
#define GX_BG_SCRBASE_0x4000 8
#define GX_BG_SCRBASE_0x4800 9
#define GX_BG_SCRBASE_0x5000 10
#define GX_BG_SCRBASE_0x5800 11
#define GX_BG_SCRBASE_0x6000 12
#define GX_BG_SCRBASE_0x6800 13
#define GX_BG_SCRBASE_0x7000 14
#define GX_BG_SCRBASE_0x7800 15
#define GX_BG_SCRBASE_0x8000 16
#define GX_BG_SCRBASE_0x8800 17
#define GX_BG_SCRBASE_0x9000 18
#define GX_BG_SCRBASE_0x9800 19
#define GX_BG_SCRBASE_0xa000 20
#define GX_BG_SCRBASE_0xa800 21
#define GX_BG_SCRBASE_0xb000 22
#define GX_BG_SCRBASE_0xb800 23
#define GX_BG_SCRBASE_0xc000 24
#define GX_BG_SCRBASE_0xc800 25
#define GX_BG_SCRBASE_0xd000 26
#define GX_BG_SCRBASE_0xd800 27
#define GX_BG_SCRBASE_0xe000 28
#define GX_BG_SCRBASE_0xe800 29
#define GX_BG_SCRBASE_0xf000 30
#define GX_BG_SCRBASE_0xf800 31
#define GX_BG_CHARBASE_0x00000 0
#define GX_BG_CHARBASE_0x04000 1
#define GX_BG_CHARBASE_0x08000 2
#define GX_BG_CHARBASE_0x0c000 3
#define GX_BG_CHARBASE_0x10000 4
#define GX_BG_CHARBASE_0x14000 5
#define GX_BG_CHARBASE_0x18000 6
#define GX_BG_CHARBASE_0x1c000 7
#define GX_BG_CHARBASE_0x20000 8
#define GX_BG_CHARBASE_0x24000 9
#define GX_BG_CHARBASE_0x28000 10
#define GX_BG_CHARBASE_0x2c000 11
#define GX_BG_CHARBASE_0x30000 12
#define GX_BG_CHARBASE_0x34000 13
#define GX_BG_CHARBASE_0x38000 14
#define GX_BG_CHARBASE_0x3c000 15
#define GX_BG_BMPSCRBASE_0x00000 0
#define GX_BG_BMPSCRBASE_0x04000 1
#define GX_BG_BMPSCRBASE_0x08000 2
#define GX_BG_BMPSCRBASE_0x0c000 3
#define GX_BG_BMPSCRBASE_0x10000 4
#define GX_BG_BMPSCRBASE_0x14000 5
#define GX_BG_BMPSCRBASE_0x18000 6
#define GX_BG_BMPSCRBASE_0x1c000 7
#define GX_BG_BMPSCRBASE_0x20000 8
#define GX_BG_BMPSCRBASE_0x24000 9
#define GX_BG_BMPSCRBASE_0x28000 10
#define GX_BG_BMPSCRBASE_0x2c000 11
#define GX_BG_BMPSCRBASE_0x30000 12
#define GX_BG_BMPSCRBASE_0x34000 13
#define GX_BG_BMPSCRBASE_0x38000 14
#define GX_BG_BMPSCRBASE_0x3c000 15
#define GX_BG_BMPSCRBASE_0x40000 16
#define GX_BG_BMPSCRBASE_0x44000 17
#define GX_BG_BMPSCRBASE_0x48000 18
#define GX_BG_BMPSCRBASE_0x4c000 19
#define GX_BG_BMPSCRBASE_0x50000 20
#define GX_BG_BMPSCRBASE_0x54000 21
#define GX_BG_BMPSCRBASE_0x58000 22
#define GX_BG_BMPSCRBASE_0x5c000 23
#define GX_BG_BMPSCRBASE_0x60000 24
#define GX_BG_BMPSCRBASE_0x64000 25
#define GX_BG_BMPSCRBASE_0x68000 26
#define GX_BG_BMPSCRBASE_0x6c000 27
#define GX_BG_BMPSCRBASE_0x70000 28
#define GX_BG_BMPSCRBASE_0x74000 29
#define GX_BG_BMPSCRBASE_0x78000 30
#define GX_BG_BMPSCRBASE_0x7c000 31

#define G2_SetBG0Control(screenSize, colorMode, screenBase, charBase, bgExtPltt) \
    (REG_BG0CNT = (u16)((REG_BG0CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2) | ((bgExtPltt) << 13)))
#define G2_SetBG1Control(screenSize, colorMode, screenBase, charBase, bgExtPltt) \
    (REG_BG1CNT = (u16)((REG_BG1CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2) | ((bgExtPltt) << 13)))
#define G2_SetBG2ControlText(screenSize, colorMode, screenBase, charBase) \
    (REG_BG2CNT = (u16)((REG_BG2CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2)))
#define G2_SetBG3ControlText(screenSize, colorMode, screenBase, charBase) \
    (REG_BG3CNT = (u16)((REG_BG3CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2)))
#define G2_SetBG2ControlDCBmp(screenSize, areaOver, screenBase) \
    (REG_BG2CNT = (u16)((REG_BG2CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((areaOver) << 13) | \
                ((screenBase) << 8) | 0x84))
#define G2_SetBG3ControlDCBmp(screenSize, areaOver, screenBase) \
    (REG_BG3CNT = (u16)((REG_BG3CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((areaOver) << 13) | \
                ((screenBase) << 8) | 0x84))
#define G2S_SetBG0Control(screenSize, colorMode, screenBase, charBase, bgExtPltt) \
    (REG_DB_BG0CNT = (u16)((REG_DB_BG0CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2) | ((bgExtPltt) << 13)))
#define G2S_SetBG1Control(screenSize, colorMode, screenBase, charBase, bgExtPltt) \
    (REG_DB_BG1CNT = (u16)((REG_DB_BG1CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2) | ((bgExtPltt) << 13)))
#define G2S_SetBG2ControlText(screenSize, colorMode, screenBase, charBase) \
    (REG_DB_BG2CNT = (u16)((REG_DB_BG2CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2)))
#define G2S_SetBG3ControlText(screenSize, colorMode, screenBase, charBase) \
    (REG_DB_BG3CNT = (u16)((REG_DB_BG3CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((colorMode) << 7) | \
                ((screenBase) << 8) | ((charBase) << 2)))
#define G2S_SetBG2ControlDCBmp(screenSize, areaOver, screenBase) \
    (REG_DB_BG2CNT = (u16)((REG_DB_BG2CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((areaOver) << 13) | \
                ((screenBase) << 8) | 0x84))
#define G2S_SetBG3ControlDCBmp(screenSize, areaOver, screenBase) \
    (REG_DB_BG3CNT = (u16)((REG_DB_BG3CNT & (BG_CONTROL_PRIORITY_MASK | BG_CONTROL_MOSAIC)) | ((screenSize) << 14) | ((areaOver) << 13) | \
                ((screenBase) << 8) | 0x84))

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
