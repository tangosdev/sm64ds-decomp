//cpp
/* GX -- display-engine bring-up and display control (the Nitro SDK's gx.c).
 * Owns arm9 .text 0x02053a90..0x02053d9c: GX_Init, the H/VBlank interrupt
 * toggles, display off/on, the two graphics-mode setters and the
 * master-brightness helper. mwccarm emits .text in reverse source order, so
 * the file reads top-down from GX_Init at 0x02053c40 to the brightness
 * helper at 0x02053a90.
 *
 * GX_DispOff parks the display mode (DISPCNT bits 16-17) in sDispMode
 * and clears it, which blanks engine A; GX_DispOn puts it back, or selects
 * "graphics" when no mode was parked. sIsDispOn is the display-on flag:
 * while it is clear, GX_SetGraphicsMode records the requested mode but
 * writes mode 0, so a mode change cannot light a display that is off.
 *
 * GXi_DmaId is the GX DMA channel (-1: none), shared by the whole GX
 * library. GX_InitGXState (src/GX_InitGXState.c, the file above) clears the VRAM
 * state block. The empty function just below this run is
 * FX_Init (src/FX_Init.c), which the boot sequence calls just before
 * GX_Init; it belongs to the fixed-point library's fx.c, not to this file.
 *
 * This TU owns .text only: the three variables above stay at their ROM
 * addresses, which is why gx.c's two file statics, sIsDispOn and sDispMode,
 * are declared extern below. Every function has C linkage and pret's GX.c
 * name, and nitro/gx/gx.h declares them all.
 */

#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx.h"
#include "private/gxdma.h"

/* The 2D registers GX_Init clears: engine A 0x60 bytes from BG0CNT, up to
   (not including) DISP_MMEM_FIFO at 0x04000068; engine B 0x70 bytes from
   its DISPCNT, through (including) its MASTER_BRIGHT at 0x0400106c. */
#define REG_BG0CNT_ADDR   ((void *)0x04000008)
#define REG_DB_DISPCNT_ADDR ((void *)0x04001000)

#define POWCNT_DISPLAY_SWAP 0x8000
#define POWCNT_ALL_ENGINES  0x020e  /* 2D A, 3D render, 3D geometry, 2D B */

#define DISPSTAT_VBLANK_IRQ 0x0008
#define DISPSTAT_HBLANK_IRQ 0x0010

#define DISPCNT_BGMODE_MASK 0x00000007
#define DISPCNT_BG0_3D      0x00000008
#define DISPCNT_MODE_MASK   0x00030000
#define DISPCNT_VRAM_MASK   0x000c0000
#define DISPCNT_MODE_SHIFT  16
#define DISPCNT_BG0_3D_SHIFT 3

#define DISPMODE_GRAPHICS   1

#define MASTER_BRIGHT_UP    0x4000
#define MASTER_BRIGHT_DOWN  0x8000

extern u16 sIsDispOn;
// display on (TRUE until GX_DispOff)
extern int GXi_DmaId;
// GX DMA channel, -1 if none
extern u16 sDispMode;
extern "C" {
// display mode parked by GX_DispOff

void GX_InitGXState(void);
}

// Power the display engines, reset the VRAM state, and clear both
// engines' 2D registers. The ROM spills each zero of the CPU path to its own
// stack slot and reloads it for the call; the volatile locals reproduce that.
void GX_Init(void) {
    REG_POWCNT1 |= POWCNT_DISPLAY_SWAP;
    REG_POWCNT1 = (u16)((REG_POWCNT1 & ~POWCNT_ALL_ENGINES) | POWCNT_ALL_ENGINES);
    REG_POWCNT1 |= POWER_CONTROL_ENABLE;

    GX_InitGXState();

    REG_DISPSTAT = 0;
    REG_DISPCNT = 0;

    if (GXi_DmaId != GX_DMA_NOT_USE) {
        MI_DmaClear32(GXi_DmaId, REG_BG0CNT_ADDR, 0x60);
        REG_MASTER_BRIGHT = 0;
        MI_DmaClear32(GXi_DmaId, REG_DB_DISPCNT_ADDR, 0x70);
    } else {
        volatile u32 zero = 0;
        MI_CpuFill32(REG_BG0CNT_ADDR, zero, 0x60);
        REG_MASTER_BRIGHT = 0;
        volatile u32 subZero = 0;
        MI_CpuFill32(REG_DB_DISPCNT_ADDR, subZero, 0x70);
    }

    // Identity scale for the affine BGs of both engines.
    REG_BG2PA = 0x100;
    REG_BG2PD = 0x100;
    REG_BG3PA = 0x100;
    REG_BG3PD = 0x100;
    REG_DB_BG2PA = 0x100;
    REG_DB_BG2PD = 0x100;
    REG_DB_BG3PA = 0x100;
    REG_DB_BG3PD = 0x100;
}

// Returns the previous H-blank IRQ enable bit.
int GX_HBlankIntr(int enable) {
    int old = REG_DISPSTAT & DISPSTAT_HBLANK_IRQ;
    if (enable)
        REG_DISPSTAT |= DISPSTAT_HBLANK_IRQ;
    else
        REG_DISPSTAT &= ~DISPSTAT_HBLANK_IRQ;
    return old;
}

// Returns the previous V-blank IRQ enable bit.
int GX_VBlankIntr(int enable) {
    int old = REG_DISPSTAT & DISPSTAT_VBLANK_IRQ;
    if (enable)
        REG_DISPSTAT |= DISPSTAT_VBLANK_IRQ;
    else
        REG_DISPSTAT &= ~DISPSTAT_VBLANK_IRQ;
    return old;
}

void GX_DispOff(void) {
    u32 cnt = REG_DISPCNT;
    sIsDispOn = 0;
    sDispMode = (u16)((cnt & DISPCNT_MODE_MASK) >> DISPCNT_MODE_SHIFT);
    REG_DISPCNT = cnt & ~DISPCNT_MODE_MASK;
}

void GX_DispOn(void) {
    sIsDispOn = 1;
    if (sDispMode != 0)
        REG_DISPCNT = (REG_DISPCNT & ~DISPCNT_MODE_MASK) | ((u32)sDispMode << DISPCNT_MODE_SHIFT);
    else
        REG_DISPCNT |= DISPMODE_GRAPHICS << DISPCNT_MODE_SHIFT;
}

void GX_SetGraphicsMode(int dispMode, int bgMode, int bg0As3d) {
    u32 cnt = REG_DISPCNT;
    sDispMode = (u16)dispMode;
    if (sIsDispOn == 0)
        dispMode = 0;
    cnt &= ~(DISPCNT_BGMODE_MASK | DISPCNT_BG0_3D | DISPCNT_MODE_MASK | DISPCNT_VRAM_MASK);
    REG_DISPCNT =
        cnt | (dispMode << DISPCNT_MODE_SHIFT) | bgMode | (bg0As3d << DISPCNT_BG0_3D_SHIFT);
    if (sDispMode == 0)
        sIsDispOn = 0;
}

void GXS_SetGraphicsMode(int bgMode) {
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~DISPCNT_BGMODE_MASK) | bgMode;
}

// reg is either engine's MASTER_BRIGHT. Positive
// levels fade up toward white, negative ones down toward black.
void GXx_SetMasterBrightness_(volatile u16 *reg, int brightness) {
    if (brightness == 0)
        *reg = 0;
    else if (brightness > 0)
        *reg = (u16)(MASTER_BRIGHT_UP | brightness);
    else
        *reg = (u16)(MASTER_BRIGHT_DOWN | -brightness);
}

// extern "C"
