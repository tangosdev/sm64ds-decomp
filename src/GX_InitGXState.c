#include "private/gx_state.h"
#include "nitro/hw/registers.h"

/* NitroSDK gx_state.c: no user holds a VRAM bank, and every bank is off. */
void GX_InitGXState(void) {
    data_020a6088.lcdc = 0;
    data_020a6088.bg = 0;
    data_020a6088.obj = 0;
    data_020a6088.arm7 = 0;
    data_020a6088.tex = 0;
    data_020a6088.texPltt = 0;
    data_020a6088.clrImg = 0;
    data_020a6088.bgExtPltt = 0;
    data_020a6088.objExtPltt = 0;
    data_020a6088.sub_bg = 0;
    data_020a6088.sub_obj = 0;
    data_020a6088.sub_bgExtPltt = 0;
    data_020a6088.sub_objExtPltt = 0;

    REG_VRAMCNT = 0;
    REG_VRAMCNT_E = 0;
    REG_VRAMCNT_F = 0;
    REG_VRAMCNT_G = 0;
    REG_VRAM_HI_CNT = 0;
}
