#ifndef NITRO_GX_GX_H
#define NITRO_GX_GX_H

/* NitroSDK's display control (gx.h), defined in src/engine/gx/GX_Init.cpp
 * (pret pokediamond arm9/lib/NitroSDK/src/GX.c). The int parameters stand
 * for the SDK's GXDispMode / GXBGMode / GXBG0As enums, and the blank
 * interrupt toggles take and return int for the SDK's BOOL and u32. Parameters stay
 * unnamed: a name in a shared header takes an @NNN in every includer. */

#include "nitro/hw/registers.h"

/* DISPCNT's BG fields, under the SDK's names: the BG mode, and the main
 * engine's character and screen offsets (64 KiB units). */
#define REG_GX_DISPCNT_BGMODE_SHIFT         0
#define REG_GX_DISPCNT_BGMODE_MASK          0x00000007
#define REG_GX_DISPCNT_BGCHAROFFSET_SHIFT   24
#define REG_GX_DISPCNT_BGCHAROFFSET_MASK    0x07000000
#define REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT 27
#define REG_GX_DISPCNT_BGSCREENOFFSET_MASK  0x38000000

/* The SDK's GXDispMode, GXBGMode and GXBG0As, as macros: the setters take
 * int. A display mode that shows a VRAM bank carries the bank above the
 * mode bits. */
#define GX_DISPMODE_OFF      0
#define GX_DISPMODE_GRAPHICS 1
#define GX_DISPMODE_VRAM_A   2
#define GX_DISPMODE_VRAM_B   6
#define GX_DISPMODE_VRAM_C   10
#define GX_DISPMODE_VRAM_D   14
#define GX_DISPMODE_MMEM     3

#define GX_BGMODE_0 0
#define GX_BGMODE_1 1
#define GX_BGMODE_2 2
#define GX_BGMODE_3 3
#define GX_BGMODE_4 4
#define GX_BGMODE_5 5
#define GX_BGMODE_6 6

#define GX_BG0_AS_2D 0
#define GX_BG0_AS_3D 1

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

#define GXS_SetOBJVRamModeBmp(mode) (REG_DISPCNT_SUB = (u32)((REG_DISPCNT_SUB & ~0x400060) | (mode)))

/* DISPCNT bits 13-15, the visible windows (WIN0, WIN1, OBJ window). The
 * SDK's GXWndMask values. */
#define GX_WNDMASK_NONE 0
#define GX_WNDMASK_W0   1
#define GX_WNDMASK_W1   2
#define GX_WNDMASK_OW   4
#define GX_SetVisibleWnd(window) (REG_DISPCNT = (u32)((REG_DISPCNT & ~0xe000) | ((window) << 13)))
#define GXS_SetVisibleWnd(window) (REG_DISPCNT_SUB = (u32)((REG_DISPCNT_SUB & ~0xe000) | ((window) << 13)))

/* Engine A's BG character and screen base offsets, DISPCNT bits 24-26 and
 * 27-29, in 64 KB steps (the SDK's GX_BGCHAROFFSET_0x?0000 and
 * GX_BGSCROFFSET_0x?0000 are 0 to 7). */
#define GX_SetBGCharOffset(offset) \
    (REG_DISPCNT = (u32)((REG_DISPCNT & ~REG_GX_DISPCNT_BGCHAROFFSET_MASK) | ((offset) << REG_GX_DISPCNT_BGCHAROFFSET_SHIFT)))
#define GX_SetBGScrOffset(offset) \
    (REG_DISPCNT = (u32)((REG_DISPCNT & ~REG_GX_DISPCNT_BGSCREENOFFSET_MASK) | ((offset) << REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT)))

/* DISPCNT bit 23: OBJ processing during H-blank. */
#define GX_HBlankOBJProc(proc) ((proc) ? (void)(REG_DISPCNT |= 0x800000) : (void)(REG_DISPCNT &= ~0x800000))
#define GXS_HBlankOBJProc(proc) ((proc) ? (void)(REG_DISPCNT_SUB |= 0x800000) : (void)(REG_DISPCNT_SUB &= ~0x800000))

/* Engine B's display on/off, DISPCNT bit 16. Engine A goes through the
 * out-of-line GX_DispOn/GX_DispOff, which park the display mode. */
#define GXS_DispOn() (REG_DISPCNT_SUB |= 0x10000)
#define GXS_DispOff() (REG_DISPCNT_SUB &= ~0x10000)

/* POWCNT1's engine power bits, the SDK's GXPower values, and the setter that
 * replaces them all at once. */
#define GX_POWER_OFF     0
#define GX_POWER_2D_MAIN 0x0002
#define GX_POWER_RE      0x0004
#define GX_POWER_GE      0x0008
#define GX_POWER_2D_SUB  0x0200
#define GX_POWER_2D      (GX_POWER_2D_MAIN | GX_POWER_2D_SUB)
#define GX_POWER_3D      (GX_POWER_GE | GX_POWER_RE)
#define GX_POWER_ALL     (GX_POWER_2D | GX_POWER_3D)
#define GX_SetPower(power) (REG_POWCNT1 = (u16)((REG_POWCNT1 & ~GX_POWER_ALL) | (power)))

/* POWCNT1 bit 15, which engine drives the top LCD: the SDK's GXDispSelect. */
#define GX_DISP_SELECT_SUB_MAIN 0
#define GX_DISP_SELECT_MAIN_SUB 1
#define GX_SetDispSelect(sel) (REG_POWCNT1 = (u16)((REG_POWCNT1 & ~0x8000) | ((sel) << 15)))
#define GX_GetDispSelect()    ((REG_POWCNT1 & 0x8000) >> 15)

#endif
