//cpp
/* GX -- display-engine bring-up and display control (the Nitro SDK's gx.c).
 * Owns arm9 .text 0x02053a90..0x02053d9c: GX_Init, the H/VBlank interrupt
 * toggles, display off/on, the two graphics-mode setters and the
 * master-brightness helper. mwccarm emits .text in reverse source order, so
 * the file reads top-down from GX_Init at 0x02053c40 to the brightness
 * helper at 0x02053a90.
 *
 * GX_DispOff parks the display mode (DISPCNT bits 16-17) in data_020a6084
 * and clears it, which blanks engine A; GX_DispOn puts it back, or selects
 * "graphics" when no mode was parked. data_02099fcc is the display-on flag:
 * while it is clear, GX::SetGraphicsMode records the requested mode but
 * writes mode 0, so a mode change cannot light a display that is off.
 *
 * data_02099fd0 is the GX DMA channel (-1: none), shared by the whole GX
 * library. GX_InitGXState (func_02053d9c, the file above) clears the VRAM
 * state block. The empty function just below this run (func_02053a8c) is
 * FX_Init, which the boot sequence calls just before GX_Init; it belongs to
 * the fixed-point library, not to this file.
 *
 * This TU owns .text only: the three variables above stay at their ROM
 * addresses. Callers reach the address-named members under C linkage; those
 * keep their ROM names here.
 */

#include "types.h"
#include "nitro/hw/registers.h"

#define REG_DISPSTAT      (*(volatile u16 *)0x04000004)
#define REG_MASTER_BRIGHT (*(volatile u16 *)0x0400006c)
#define REG_BG2PA         (*(volatile u16 *)0x04000020)
#define REG_BG2PD         (*(volatile u16 *)0x04000026)
#define REG_BG3PA         (*(volatile u16 *)0x04000030)
#define REG_BG3PD         (*(volatile u16 *)0x04000036)
#define REG_DB_BG2PA      (*(volatile u16 *)0x04001020)
#define REG_DB_BG2PD      (*(volatile u16 *)0x04001026)
#define REG_DB_BG3PD      (*(volatile u16 *)0x04001036)

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

extern "C" {

extern u16 data_02099fcc; // display on (TRUE until GX_DispOff)
extern int data_02099fd0; // GX DMA channel, -1 if none
extern u16 data_020a6084; // display mode parked by GX_DispOff

void func_02053d9c(void);
void DMASyncFillTransfer(u32 channel, void *dst, u32 data, u32 size);
void MultiStore_Int(int val, int *dst, int len);

// GX_Init: power the display engines, reset the VRAM state, and clear both
// engines' 2D registers. The ROM spills each zero of the CPU path to its own
// stack slot and reloads it for the call; the volatile locals reproduce that.
// @symbol func_02053c40
void func_02053c40(void) {
    REG_POWCNT1 |= POWCNT_DISPLAY_SWAP;
    REG_POWCNT1 = (u16)((REG_POWCNT1 & ~POWCNT_ALL_ENGINES) | POWCNT_ALL_ENGINES);
    REG_POWCNT1 |= POWER_CONTROL_ENABLE;

    func_02053d9c();

    REG_DISPSTAT = 0;
    REG_DISPCNT = 0;

    if (data_02099fd0 != -1) {
        DMASyncFillTransfer(data_02099fd0, REG_BG0CNT_ADDR, 0, 0x60);
        REG_MASTER_BRIGHT = 0;
        DMASyncFillTransfer(data_02099fd0, REG_DB_DISPCNT_ADDR, 0, 0x70);
    } else {
        volatile u32 zero = 0;
        MultiStore_Int(zero, (int *)REG_BG0CNT_ADDR, 0x60);
        REG_MASTER_BRIGHT = 0;
        volatile u32 subZero = 0;
        MultiStore_Int(subZero, (int *)REG_DB_DISPCNT_ADDR, 0x70);
    }

    // Identity scale for the affine BGs of both engines.
    REG_BG2PA = 0x100;
    REG_BG2PD = 0x100;
    REG_BG3PA = 0x100;
    REG_BG3PD = 0x100;
    REG_DB_BG2PA = 0x100;
    REG_DB_BG2PD = 0x100;
    *REG_DB_BG3PA = 0x100;
    REG_DB_BG3PD = 0x100;
}

// GX_HBlankIntr: returns the previous enable bit.
// @symbol func_02053c10
int func_02053c10(int enable) {
    int old = REG_DISPSTAT & DISPSTAT_HBLANK_IRQ;
    if (enable)
        REG_DISPSTAT |= DISPSTAT_HBLANK_IRQ;
    else
        REG_DISPSTAT &= ~DISPSTAT_HBLANK_IRQ;
    return old;
}

// GX_VBlankIntr: returns the previous enable bit.
// @symbol func_02053be0
int func_02053be0(int enable) {
    int old = REG_DISPSTAT & DISPSTAT_VBLANK_IRQ;
    if (enable)
        REG_DISPSTAT |= DISPSTAT_VBLANK_IRQ;
    else
        REG_DISPSTAT &= ~DISPSTAT_VBLANK_IRQ;
    return old;
}

// GX_DispOff
// @symbol func_02053b98
void func_02053b98(void) {
    u32 cnt = REG_DISPCNT;
    data_02099fcc = 0;
    data_020a6084 = (u16)((cnt & DISPCNT_MODE_MASK) >> DISPCNT_MODE_SHIFT);
    REG_DISPCNT = cnt & ~DISPCNT_MODE_MASK;
}

} // extern "C"

namespace GX {

// @symbol _ZN2GX6DispOnEv
void DispOn() {
    data_02099fcc = 1;
    if (data_020a6084 != 0)
        REG_DISPCNT =
            (REG_DISPCNT & ~DISPCNT_MODE_MASK) | ((u32)data_020a6084 << DISPCNT_MODE_SHIFT);
    else
        REG_DISPCNT |= DISPMODE_GRAPHICS << DISPCNT_MODE_SHIFT;
}

// @symbol _ZN2GX15SetGraphicsModeEiii
void SetGraphicsMode(int dispMode, int bgMode, int bg0As3d) {
    u32 cnt = REG_DISPCNT;
    data_020a6084 = (u16)dispMode;
    if (data_02099fcc == 0)
        dispMode = 0;
    cnt &= ~(DISPCNT_BGMODE_MASK | DISPCNT_BG0_3D | DISPCNT_MODE_MASK | DISPCNT_VRAM_MASK);
    REG_DISPCNT =
        cnt | (dispMode << DISPCNT_MODE_SHIFT) | bgMode | (bg0As3d << DISPCNT_BG0_3D_SHIFT);
    if (data_020a6084 == 0)
        data_02099fcc = 0;
}

} // namespace GX

namespace GXS {

// @symbol _ZN3GXS15SetGraphicsModeEi
void SetGraphicsMode(int bgMode) {
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~DISPCNT_BGMODE_MASK) | bgMode;
}

} // namespace GXS

extern "C" {

// GXx_SetMasterBrightness_: reg is either engine's MASTER_BRIGHT. Positive
// levels fade up toward white, negative ones down toward black.
// @symbol func_02053a90
void func_02053a90(u16 *reg, int brightness) {
    if (brightness == 0)
        *reg = 0;
    else if (brightness > 0)
        *reg = (u16)(MASTER_BRIGHT_UP | brightness);
    else
        *reg = (u16)(MASTER_BRIGHT_DOWN | -brightness);
}

} // extern "C"
