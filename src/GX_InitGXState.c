#include "private/gx_state.h"
#include "nitro/hw/registers.h"

/* NitroSDK gx_state.c: no user holds a VRAM bank, and every bank is off. */
void GX_InitGXState(void) {
    gGXState.lcdc = 0;
    gGXState.bg = 0;
    gGXState.obj = 0;
    gGXState.arm7 = 0;
    gGXState.tex = 0;
    gGXState.texPltt = 0;
    gGXState.clrImg = 0;
    gGXState.bgExtPltt = 0;
    gGXState.objExtPltt = 0;
    gGXState.sub_bg = 0;
    gGXState.sub_obj = 0;
    gGXState.sub_bgExtPltt = 0;
    gGXState.sub_objExtPltt = 0;

    REG_VRAMCNT = 0;
    REG_VRAMCNT_E = 0;
    REG_VRAMCNT_F = 0;
    REG_VRAMCNT_G = 0;
    REG_VRAM_HI_CNT = 0;
}
