/* The 2D engines' BG base-address getters (the Nitro SDK's gx_bgcnt.c),
 * arm9 .text 0x02054d38..0x020551f0.
 *
 * For each of BG0..BG3 there is a screen-base and a character-base getter
 * for the main engine (G2_, BGxCNT at 0x04000008..0x0400000e, VRAM
 * 0x06000000) and for the sub engine (G2S_, 0x04001008..0x0400100e, VRAM
 * 0x06200000). Only the main engine's getters add DISPCNT's screen and
 * character offsets (bits 27-29 and 24-26); the sub engine has fixed bases.
 * BG2 and BG3 depend on the mode: where the layer is extended-affine (BG2 in
 * mode 5, BG3 in modes 3-5), bit 7 of its control word selects a bitmap,
 * whose screen base counts in 16 KiB units and which has no character base.
 * BG3 has no layer in modes 6 and 7.
 *
 * pret's pokediamond GX_bgcnt.c defines the sixteen getters in this order,
 * each G2S_ getter directly after its G2_ twin: BG0 screen through BG3
 * character. mwccarm emits the bodies in reverse, so G2S_GetBG3CharPtr is
 * lowest at 0x02054d38 and G2_GetBG0ScrPtr highest at 0x020551bc.
 *
 * Declared in include/nitro/gx/gx_bgcnt.h.
 */
#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"

void *G2_GetBG0ScrPtr(void) {
    int off = (REG_BG0CNT & 0x1f00) >> 8;
    unsigned int sbase = REG_DISPCNT & 0x38000000;
    return (void *)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}

void *G2S_GetBG0ScrPtr(void) {
    int v = REG_DB_BG0CNT;
    return (void *)((((v & 0x1f00) >> 8) << 0xb) + 0x6200000);
}

void *G2_GetBG1ScrPtr(void) {
    int off = (REG_BG1CNT & 0x1f00) >> 8;
    unsigned int sbase = REG_DISPCNT & 0x38000000;
    return (void *)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}

void *G2S_GetBG1ScrPtr(void) {
    int v = REG_DB_BG1CNT;
    return (void *)((((v & 0x1f00) >> 8) << 0xb) + 0x6200000);
}

/* Mode 6 (large bitmap) returns the engine's VRAM base itself; mode 7 and
 * anything else returns null. */
void *G2_GetBG2ScrPtr(void) {
    unsigned int mode = REG_DISPCNT & 7;
    unsigned short bg2cnt = REG_BG2CNT;
    unsigned int sbase = ((REG_DISPCNT & 0x38000000) >> 0x1b) << 0x10;
    unsigned int off = ((unsigned int)(bg2cnt & 0x1f00)) >> 8;
    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return (void *)(0x6000000 + sbase + (off << 11));
    case 5:
        if (bg2cnt & 0x80)
            return (void *)(0x6000000 + (off << 0xe));
        return (void *)(0x6000000 + sbase + (off << 11));
    case 6:
        return (void *)0x6000000;
    default:
        return (void *)0;
    }
}

void *G2S_GetBG2ScrPtr(void) {
    int m = REG_DISPCNT_SUB & 7;
    unsigned int v = REG_DB_BG2CNT;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return (void *)((r1 << 0xb) + 0x6200000);
    case 5:
        if (v & 0x80)
            return (void *)((r1 << 0xe) + 0x6200000);
        return (void *)((r1 << 0xb) + 0x6200000);
    case 6:
        return 0;
    default:
        return 0;
    }
}

void *G2_GetBG3ScrPtr(void) {
    unsigned int mode = REG_DISPCNT & 7;
    unsigned short bg3cnt = REG_BG3CNT;
    unsigned int sbase = ((REG_DISPCNT & 0x38000000) >> 0x1b) << 0x10;
    unsigned int off = ((unsigned int)(bg3cnt & 0x1f00)) >> 8;
    switch (mode) {
    case 0:
    case 1:
    case 2:
        return (void *)(0x6000000 + sbase + (off << 11));
    case 3:
    case 4:
    case 5:
        if (bg3cnt & 0x80)
            return (void *)(0x6000000 + (off << 0xe));
        return (void *)(0x6000000 + sbase + (off << 11));
    case 6:
        return (void *)0;
    default:
        return (void *)0;
    }
}

void *G2S_GetBG3ScrPtr(void) {
    int m = REG_DISPCNT_SUB & 7;
    unsigned int v = REG_DB_BG3CNT;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
        return (void *)((r1 << 0xb) + 0x6200000);
    case 3:
    case 4:
    case 5:
        if (v & 0x80)
            return (void *)((r1 << 0xe) + 0x6200000);
        return (void *)((r1 << 0xb) + 0x6200000);
    case 6:
        return 0;
    default:
        return 0;
    }
}

void *G2_GetBG0CharPtr(void) {
    int off = (REG_BG0CNT & 0x3c) >> 2;
    unsigned int sbase = REG_DISPCNT & 0x7000000;
    return (void *)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}

void *G2S_GetBG0CharPtr(void) {
    int v = REG_DB_BG0CNT;
    return (void *)((((v & 0x3c) >> 2) << 0xe) + 0x6200000);
}

void *G2_GetBG1CharPtr(void) {
    int off = (REG_BG1CNT & 0x3c) >> 2;
    unsigned int sbase = REG_DISPCNT & 0x7000000;
    return (void *)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}

void *G2S_GetBG1CharPtr(void) {
    int v = REG_DB_BG1CNT;
    return (void *)((((v & 0x3c) >> 2) << 0xe) + 0x6200000);
}

void *G2_GetBG2CharPtr(void) {
    int mode = REG_DISPCNT & 7;
    unsigned int bg2cnt = REG_BG2CNT;
    if (mode < 5 || !(bg2cnt & 0x80))
        return (void *)(0x6000000 + (((REG_DISPCNT & 0x7000000) >> 0x18) << 0x10) +
                        (((bg2cnt & 0x3c) >> 2) << 14));
    return (void *)0;
}

void *G2S_GetBG2CharPtr(void) {
    int v1 = REG_DISPCNT_SUB;
    unsigned short v2 = REG_DB_BG2CNT;
    if (!((v1 & 7) >= 5 && (v2 & 0x80)))
        return (void *)(((unsigned)(v2 & 0x3c) >> 2 << 0xe) + 0x6200000);
    return 0;
}

void *G2_GetBG3CharPtr(void) {
    unsigned int mode = REG_DISPCNT & 7;
    unsigned short h = REG_BG3CNT;
    if ((int)mode < 3 || ((int)mode < 6 && !(h & 0x80))) {
        unsigned int v = REG_DISPCNT;
        return (void *)(0x6000000 + (((v & 0x7000000) >> 0x18) << 0x10) +
                        (((unsigned int)(h & 0x3c) >> 2) << 14));
    }
    return 0;
}

void *G2S_GetBG3CharPtr(void) {
    int cfg = REG_DISPCNT_SUB;
    unsigned int v = REG_DB_BG3CNT;
    int m = cfg & 7;
    if (m < 3 || (m < 6 && !(v & 0x80)))
        return (void *)((((v & 0x3c) >> 2) << 0xe) + 0x6200000);
    return 0;
}
