#ifndef NITRO_GX_GX_H
#define NITRO_GX_GX_H

/* NitroSDK's display control (gx.h), defined in src/engine/gx/GX_Init.cpp
 * (pret pokediamond arm9/lib/NitroSDK/src/GX.c). The int parameters stand
 * for the SDK's GXDispMode / GXBGMode / GXBG0As enums, and the blank
 * interrupt toggles take and return int for the SDK's BOOL and u32. Parameters stay
 * unnamed: a name in a shared header takes an @NNN in every includer. */

#include "nitro/hw/registers.h"

#ifdef __cplusplus
extern "C" {
#endif

void GX_Init(void);
int GX_HBlankIntr(int);
int GX_VBlankIntr(int);
void GX_DispOff(void);
void GX_DispOn(void);
void GX_SetGraphicsMode(int, int, int);
void GXS_SetGraphicsMode(int);
void GXx_SetMasterBrightness_(volatile unsigned short *, int);

#ifdef __cplusplus
}
#endif

/* The SDK's public wrappers over GXx_SetMasterBrightness_, one per engine.
 * Macros rather than the SDK's static inlines: an inline body in a shared
 * header renumbers every includer's @NNN symbols. */
#define GX_SetMasterBrightness(brightness)     GXx_SetMasterBrightness_(&REG_MASTER_BRIGHT, (brightness))
#define GXS_SetMasterBrightness(brightness)     GXx_SetMasterBrightness_(&REG_DB_MASTER_BRIGHT, (brightness))

/* DISPCNT bits 8-12, the visible BG0-BG3/OBJ planes (DISPLAY_LAYER_* in
 * registers.h, shifted down to bit 0). */
#define GX_SetVisiblePlane(plane) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0x1f00) | ((plane) << 8)))
#define GXS_SetVisiblePlane(plane) (REG_DISPCNT_SUB = (u32)((REG_DISPCNT_SUB & ~0x1f00) | ((plane) << 8)))

/* DISPCNT's OBJ character mapping: bit 4 picks 1D over 2D, bits 20-21 the
 * 1D boundary. The SDK's GXOBJVRamModeChar values. */
#define GX_OBJVRAMMODE_CHAR_2D      0x000000
#define GX_OBJVRAMMODE_CHAR_1D_32K  0x000010
#define GX_OBJVRAMMODE_CHAR_1D_64K  0x100010
#define GX_OBJVRAMMODE_CHAR_1D_128K 0x200010
#define GX_OBJVRAMMODE_CHAR_1D_256K 0x300010
#define GX_SetOBJVRamModeChar(mode) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0x300010) | (mode)))
#define GXS_SetOBJVRamModeChar(mode) (REG_DISPCNT_SUB = (u32)((REG_DISPCNT_SUB & ~0x300010) | (mode)))

/* DISPCNT's OBJ bitmap mapping: bits 5-6 the 2D width or 1D, bit 22 the 1D
 * boundary. The SDK's GXOBJVRamModeBmp values. */
#define GX_OBJVRAMMODE_BMP_2D_W128 0x000000
#define GX_OBJVRAMMODE_BMP_2D_W256 0x000020
#define GX_OBJVRAMMODE_BMP_1D_128K 0x000040
#define GX_OBJVRAMMODE_BMP_1D_256K 0x400040
#define GX_SetOBJVRamModeBmp(mode) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0x400060) | (mode)))

/* DISPCNT bits 13-15, the visible windows (WIN0, WIN1, OBJ window). */
#define GX_SetVisibleWnd(window) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0xe000) | ((window) << 13)))
#define GXS_SetVisibleWnd(window) (REG_DISPCNT_SUB = (u32)((REG_DISPCNT_SUB & ~0xe000) | ((window) << 13)))

/* Engine A's BG character and screen base offsets, DISPCNT bits 24-26 and
 * 27-29, in 64 KB steps (the SDK's GX_BGCHAROFFSET_0x?0000 and
 * GX_BGSCROFFSET_0x?0000 are 0 to 7). */
#define GX_SetBGCharOffset(offset) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0x07000000) | ((offset) << 24)))
#define GX_SetBGScrOffset(offset) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0x38000000) | ((offset) << 27)))

/* DISPCNT bit 23: OBJ processing during H-blank. */
#define GX_HBlankOBJProc(proc) ((proc) ? (void)(REG_DISPCNT |= 0x800000) : (void)(REG_DISPCNT &= ~0x800000))

#endif
