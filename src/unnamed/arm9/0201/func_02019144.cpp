//cpp
#include "OAM.h"
#include "dGraph_c.h"
#include "nitro/gx/g3x.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"



extern dGraph_c::callback_c *gGraphCallback;
extern unsigned char gMainScreenLayers;
extern unsigned char gSubScreenLayers;
extern short gBg0HOffset;
extern short gBg0VOffset;
extern short gBg1HOffset;
extern short gBg1VOffset;
extern short gBg2HOffset;
extern short gBg2VOffset;
extern short gBg3HOffset;
extern short gBg3VOffset;
extern short gSubBg0HOffset;
extern short gSubBg0VOffset;
extern short gSubBg1HOffset;
extern short gSubBg1VOffset;
extern short gSubBg2HOffset;
extern short gSubBg2VOffset;
extern short gSubBg3HOffset;
extern short gSubBg3VOffset;

/* The VBlank IRQ's display commit (IRQ2.cpp). The scene's GraphCallback2
   (gGraphCallback) can skip it by returning 0. Otherwise flush and load
   OAM, put the layer enables (DISPCNT bits 8-12) from gMainScreenLayers on
   the main screen and gSubScreenLayers on the sub screen, and write both
   screens' BG scroll registers from their shadow words (each H/V pair is
   set by its SetBgNOffset or SetSubBgNOffset). With BG0 in 3D
   mode (DISPCNT bit 3) its horizontal offset goes through
   G3X_SetHOffset instead. */
void func_02019144(void)
{
    dGraph_c::callback_c *callback = gGraphCallback;
    if (callback != 0) {
        if (callback->GraphCallback2() == 0)
            return;
    }
    OAM::Flush();
    OAM::Load();
    GX_SetVisiblePlane(gMainScreenLayers);
    GXS_SetVisiblePlane(gSubScreenLayers);
    if (!(REG_DISPCNT & 8)) {
        G2_SetBG0Offset(gBg0HOffset, gBg0VOffset);
    } else {
        G3X_SetHOffset(gBg0HOffset);
    }
    G2_SetBG1Offset(gBg1HOffset, gBg1VOffset);
    G2_SetBG2Offset(gBg2HOffset, gBg2VOffset);
    G2_SetBG3Offset(gBg3HOffset, gBg3VOffset);
    REG_DISPCNT_SUB;
    G2S_SetBG0Offset(gSubBg0HOffset, gSubBg0VOffset);
    G2S_SetBG1Offset(gSubBg1HOffset, gSubBg1VOffset);
    G2S_SetBG2Offset(gSubBg2HOffset, gSubBg2VOffset);
    G2S_SetBG3Offset(gSubBg3HOffset, gSubBg3VOffset);
}
