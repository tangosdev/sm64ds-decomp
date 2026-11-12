/* The 2D engines' BG base-address getters (the Nitro SDK's gx_bgcnt.c),
 * arm9 .text 0x02054d38..0x020551f0.
 *
 * For each of BG0..BG3 there is a screen-base and a character-base getter
 * for the main engine (G2_, VRAM at HW_BG_VRAM) and for the sub engine
 * (G2S_, HW_DB_BG_VRAM). Only the main engine's getters add DISPCNT's
 * screen and character offsets, in 64 KiB blocks; the sub engine has fixed
 * bases. A screen base counts in 2 KiB units and a character base in 16 KiB
 * units. BG2 and BG3 depend on the BG mode: where the layer is
 * extended-affine (BG2 in mode 5, BG3 in modes 3-5), the colour-mode bit of
 * its control word selects a bitmap, whose screen base counts in 16 KiB units
 * and which has no character base. BG3 has no layer in mode 6.
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
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_bgcnt.h"

/* The BG control words' base fields, under the SDK's names. */
#define REG_G2_BG0CNT_SCREENBASE_SHIFT     8
#define REG_G2_BG0CNT_SCREENBASE_MASK      0x1f00
#define REG_G2_BG0CNT_CHARBASE_SHIFT       2
#define REG_G2_BG0CNT_CHARBASE_MASK        0x003c
#define REG_G2_BG1CNT_SCREENBASE_SHIFT     8
#define REG_G2_BG1CNT_SCREENBASE_MASK      0x1f00
#define REG_G2_BG1CNT_CHARBASE_SHIFT       2
#define REG_G2_BG1CNT_CHARBASE_MASK        0x003c
#define REG_G2_BG2CNT_SCREENBASE_SHIFT     8
#define REG_G2_BG2CNT_SCREENBASE_MASK      0x1f00
#define REG_G2_BG2CNT_COLORMODE_MASK       0x0080
#define REG_G2_BG2CNT_CHARBASE_SHIFT       2
#define REG_G2_BG2CNT_CHARBASE_MASK        0x003c
#define REG_G2_BG3CNT_SCREENBASE_SHIFT     8
#define REG_G2_BG3CNT_SCREENBASE_MASK      0x1f00
#define REG_G2_BG3CNT_COLORMODE_MASK       0x0080
#define REG_G2_BG3CNT_CHARBASE_SHIFT       2
#define REG_G2_BG3CNT_CHARBASE_MASK        0x003c

#define REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT 8
#define REG_G2S_DB_BG0CNT_SCREENBASE_MASK  0x1f00
#define REG_G2S_DB_BG0CNT_CHARBASE_SHIFT   2
#define REG_G2S_DB_BG0CNT_CHARBASE_MASK    0x003c
#define REG_G2S_DB_BG1CNT_SCREENBASE_SHIFT 8
#define REG_G2S_DB_BG1CNT_SCREENBASE_MASK  0x1f00
#define REG_G2S_DB_BG1CNT_CHARBASE_SHIFT   2
#define REG_G2S_DB_BG1CNT_CHARBASE_MASK    0x003c
#define REG_G2S_DB_BG2CNT_SCREENBASE_SHIFT 8
#define REG_G2S_DB_BG2CNT_SCREENBASE_MASK  0x1f00
#define REG_G2S_DB_BG2CNT_COLORMODE_MASK   0x0080
#define REG_G2S_DB_BG2CNT_CHARBASE_SHIFT   2
#define REG_G2S_DB_BG2CNT_CHARBASE_MASK    0x003c
#define REG_G2S_DB_BG3CNT_SCREENBASE_SHIFT 8
#define REG_G2S_DB_BG3CNT_SCREENBASE_MASK  0x1f00
#define REG_G2S_DB_BG3CNT_COLORMODE_MASK   0x0080
#define REG_G2S_DB_BG3CNT_CHARBASE_SHIFT   2
#define REG_G2S_DB_BG3CNT_CHARBASE_MASK    0x003c

void *G2_GetBG0ScrPtr(void) {
    int offset = (REG_BG0CNT & REG_G2_BG0CNT_SCREENBASE_MASK) >> REG_G2_BG0CNT_SCREENBASE_SHIFT;
    u32 blockOffset =
        (REG_DISPCNT & REG_GX_DISPCNT_BGSCREENOFFSET_MASK) >> REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT;

    return (void *)(HW_BG_VRAM + blockOffset * 0x10000 + offset * 0x800);
}

void *G2S_GetBG0ScrPtr(void) {
    int offset =
        (REG_DB_BG0CNT & REG_G2S_DB_BG0CNT_SCREENBASE_MASK) >> REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT;

    return (void *)(HW_DB_BG_VRAM + offset * 0x800);
}

void *G2_GetBG1ScrPtr(void) {
    int offset = (REG_BG1CNT & REG_G2_BG1CNT_SCREENBASE_MASK) >> REG_G2_BG1CNT_SCREENBASE_SHIFT;
    u32 blockOffset =
        (REG_DISPCNT & REG_GX_DISPCNT_BGSCREENOFFSET_MASK) >> REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT;

    return (void *)(HW_BG_VRAM + blockOffset * 0x10000 + offset * 0x800);
}

void *G2S_GetBG1ScrPtr(void) {
    int offset =
        (REG_DB_BG1CNT & REG_G2S_DB_BG1CNT_SCREENBASE_MASK) >> REG_G2S_DB_BG1CNT_SCREENBASE_SHIFT;

    return (void *)(HW_DB_BG_VRAM + offset * 0x800);
}

/* Mode 6 (the large bitmap) returns the engine's VRAM itself. */
void *G2_GetBG2ScrPtr(void) {
    u32 bgMode = (REG_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_BG2CNT;
    u32 blockOffset = 0x10000 * ((REG_DISPCNT & REG_GX_DISPCNT_BGSCREENOFFSET_MASK) >>
                                 REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT);
    u32 offset = (bg & REG_G2_BG2CNT_SCREENBASE_MASK) >> REG_G2_BG2CNT_SCREENBASE_SHIFT;

    switch (bgMode) {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
    case GX_BGMODE_3:
    case GX_BGMODE_4:
        return (void *)(HW_BG_VRAM + blockOffset + offset * 0x800);
    case GX_BGMODE_5:
        if (bg & REG_G2_BG2CNT_COLORMODE_MASK)
            return (void *)(HW_BG_VRAM + offset * 0x4000);
        return (void *)(HW_BG_VRAM + blockOffset + offset * 0x800);
    case GX_BGMODE_6:
        return (void *)HW_BG_VRAM;
    default:
        return 0;
    }
}

void *G2S_GetBG2ScrPtr(void) {
    int bgMode = (REG_DISPCNT_SUB & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_DB_BG2CNT;
    u32 offset = (bg & REG_G2S_DB_BG2CNT_SCREENBASE_MASK) >> REG_G2S_DB_BG2CNT_SCREENBASE_SHIFT;

    switch (bgMode) {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
    case GX_BGMODE_3:
    case GX_BGMODE_4:
        return (void *)(HW_DB_BG_VRAM + offset * 0x800);
    case GX_BGMODE_5:
        if (bg & REG_G2S_DB_BG2CNT_COLORMODE_MASK)
            return (void *)(HW_DB_BG_VRAM + offset * 0x4000);
        return (void *)(HW_DB_BG_VRAM + offset * 0x800);
    case GX_BGMODE_6:
        return 0;
    default:
        return 0;
    }
}

void *G2_GetBG3ScrPtr(void) {
    u32 bgMode = (REG_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_BG3CNT;
    u32 blockOffset = 0x10000 * ((REG_DISPCNT & REG_GX_DISPCNT_BGSCREENOFFSET_MASK) >>
                                 REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT);
    u32 offset = (bg & REG_G2_BG3CNT_SCREENBASE_MASK) >> REG_G2_BG3CNT_SCREENBASE_SHIFT;

    switch (bgMode) {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
        return (void *)(HW_BG_VRAM + blockOffset + offset * 0x800);
    case GX_BGMODE_3:
    case GX_BGMODE_4:
    case GX_BGMODE_5:
        if (bg & REG_G2_BG3CNT_COLORMODE_MASK)
            return (void *)(HW_BG_VRAM + offset * 0x4000);
        return (void *)(HW_BG_VRAM + blockOffset + offset * 0x800);
    case GX_BGMODE_6:
        return 0;
    default:
        return 0;
    }
}

void *G2S_GetBG3ScrPtr(void) {
    int bgMode = (REG_DISPCNT_SUB & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_DB_BG3CNT;
    u32 offset = (bg & REG_G2S_DB_BG3CNT_SCREENBASE_MASK) >> REG_G2S_DB_BG3CNT_SCREENBASE_SHIFT;

    switch (bgMode) {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
        return (void *)(HW_DB_BG_VRAM + offset * 0x800);
    case GX_BGMODE_3:
    case GX_BGMODE_4:
    case GX_BGMODE_5:
        if (bg & REG_G2S_DB_BG3CNT_COLORMODE_MASK)
            return (void *)(HW_DB_BG_VRAM + offset * 0x4000);
        return (void *)(HW_DB_BG_VRAM + offset * 0x800);
    case GX_BGMODE_6:
        return 0;
    default:
        return 0;
    }
}

void *G2_GetBG0CharPtr(void) {
    int offset = (REG_BG0CNT & REG_G2_BG0CNT_CHARBASE_MASK) >> REG_G2_BG0CNT_CHARBASE_SHIFT;
    u32 blockOffset =
        (REG_DISPCNT & REG_GX_DISPCNT_BGCHAROFFSET_MASK) >> REG_GX_DISPCNT_BGCHAROFFSET_SHIFT;

    return (void *)(HW_BG_VRAM + blockOffset * 0x10000 + offset * 0x4000);
}

void *G2S_GetBG0CharPtr(void) {
    int offset =
        (REG_DB_BG0CNT & REG_G2S_DB_BG0CNT_CHARBASE_MASK) >> REG_G2S_DB_BG0CNT_CHARBASE_SHIFT;

    return (void *)(HW_DB_BG_VRAM + offset * 0x4000);
}

void *G2_GetBG1CharPtr(void) {
    int offset = (REG_BG1CNT & REG_G2_BG1CNT_CHARBASE_MASK) >> REG_G2_BG1CNT_CHARBASE_SHIFT;
    u32 blockOffset =
        (REG_DISPCNT & REG_GX_DISPCNT_BGCHAROFFSET_MASK) >> REG_GX_DISPCNT_BGCHAROFFSET_SHIFT;

    return (void *)(HW_BG_VRAM + blockOffset * 0x10000 + offset * 0x4000);
}

void *G2S_GetBG1CharPtr(void) {
    int offset =
        (REG_DB_BG1CNT & REG_G2S_DB_BG1CNT_CHARBASE_MASK) >> REG_G2S_DB_BG1CNT_CHARBASE_SHIFT;

    return (void *)(HW_DB_BG_VRAM + offset * 0x4000);
}

/* A bitmap layer has no character base. */
void *G2_GetBG2CharPtr(void) {
    int bgMode = (REG_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_BG2CNT;

    if (bgMode < GX_BGMODE_5 || !(bg & REG_G2_BG2CNT_COLORMODE_MASK)) {
        u32 blockOffset =
            (REG_DISPCNT & REG_GX_DISPCNT_BGCHAROFFSET_MASK) >> REG_GX_DISPCNT_BGCHAROFFSET_SHIFT;
        u32 offset = (bg & REG_G2_BG2CNT_CHARBASE_MASK) >> REG_G2_BG2CNT_CHARBASE_SHIFT;

        return (void *)(HW_BG_VRAM + blockOffset * 0x10000 + offset * 0x4000);
    }
    return 0;
}

void *G2S_GetBG2CharPtr(void) {
    int bgMode = (REG_DISPCNT_SUB & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_DB_BG2CNT;

    if (bgMode < GX_BGMODE_5 || !(bg & REG_G2S_DB_BG2CNT_COLORMODE_MASK)) {
        u32 offset = (bg & REG_G2S_DB_BG2CNT_CHARBASE_MASK) >> REG_G2S_DB_BG2CNT_CHARBASE_SHIFT;

        return (void *)(HW_DB_BG_VRAM + offset * 0x4000);
    }
    return 0;
}

void *G2_GetBG3CharPtr(void) {
    int bgMode = (REG_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_BG3CNT;

    if (bgMode < GX_BGMODE_3 || (bgMode < GX_BGMODE_6 && !(bg & REG_G2_BG3CNT_COLORMODE_MASK))) {
        u32 blockOffset =
            (REG_DISPCNT & REG_GX_DISPCNT_BGCHAROFFSET_MASK) >> REG_GX_DISPCNT_BGCHAROFFSET_SHIFT;
        u32 offset = (bg & REG_G2_BG3CNT_CHARBASE_MASK) >> REG_G2_BG3CNT_CHARBASE_SHIFT;

        return (void *)(HW_BG_VRAM + blockOffset * 0x10000 + offset * 0x4000);
    }
    return 0;
}

void *G2S_GetBG3CharPtr(void) {
    int bgMode = (REG_DISPCNT_SUB & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT;
    u32 bg = REG_DB_BG3CNT;

    if (bgMode < GX_BGMODE_3 ||
        (bgMode < GX_BGMODE_6 && !(bg & REG_G2S_DB_BG3CNT_COLORMODE_MASK))) {
        u32 offset = (bg & REG_G2S_DB_BG3CNT_CHARBASE_MASK) >> REG_G2S_DB_BG3CNT_CHARBASE_SHIFT;

        return (void *)(HW_DB_BG_VRAM + offset * 0x4000);
    }
    return 0;
}
