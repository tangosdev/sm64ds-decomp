//cpp
#include "types.h"
#include "G2.h"
#include "GX.h"
#include "nitro/hw/registers.h"

extern "C" {

extern void Disable3dEngines(void);
extern s32 GetLanguageIndexVS(void);
extern void *func_02054ea8(void);
extern void DecompressLZ16(const void *src, void *dst);
extern void SetBg1Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);

/* The visible-plane masks of the two engines, kept beside DISPCNT bits 8-12
   (BG0-BG3, OBJ): d_a_demo.cpp sets and clears bits here and writes the
   mask back to the register the same way. */
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern void *data_020927b0[];
extern void *data_020927c4[];
extern void *data_020927d8[];
extern u8 data_0209440c;
extern u8 data_0209fc58;

/* Put up the same language-indexed BG1 picture on both screens: hide every
   plane, map the VRAM banks, set engine A to graphics mode 1, load the
   characters, palette and one of two screen maps (data_0209fc58 bits 4-5
   choose) on both engines, then show BG1 alone. */
void func_02030500(void) {
    u32 tmp;
    void *p;

    gMainScreenLayers = 0;
    gSubScreenLayers = 0;
    REG_DISPCNT &= ~0x1f00;
    REG_DISPCNT_SUB &= ~0x1f00;

    Disable3dEngines();
    GX::DisableAllBanks();
    GX::SetBankForBG(1);
    GX::SetBankForOBJ(2);
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(8);

    /* engine A's character and screen base offsets (bits 24-26, 27-29) to 0 */
    tmp = REG_DISPCNT & ~0x7000000;
    REG_DISPCNT = tmp;
    tmp = REG_DISPCNT & ~0x38000000;
    REG_DISPCNT = tmp;
    REG_POWCNT1 |= 0x8000; /* bit 15: engine A on the top screen */
    REG_BLDCNT = 0;
    REG_DB_BLDCNT = 0;
    /* the window enables (bits 13-15) off on both engines */
    tmp = REG_DISPCNT & ~0xe000;
    REG_DISPCNT = tmp;
    REG_DISPCNT_SUB &= ~0xe000;
    GX::SetGraphicsMode(1, 0, 0);

    REG_DISPCNT &= 0xffcfffef;
    REG_BG1CNT = (REG_BG1CNT & 0x43) | 8;
    REG_BG1CNT &= ~0x40;
    GXS::SetGraphicsMode(0);

    REG_DISPCNT_SUB &= 0xffcfffef;
    REG_DB_BG1CNT = (REG_DB_BG1CNT & 0x43) | 8;
    REG_DB_BG1CNT &= ~0x40;

    DecompressLZ16(data_020927b0[GetLanguageIndexVS()], func_02054ea8());
    DecompressLZ16(data_020927b0[GetLanguageIndexVS()], G2S::GetBG1CharPtr());

    GX::LoadBGPltt(&data_0209440c, 0, 0x40);
    GXS::LoadBGPltt(&data_0209440c, 0, 0x40);

    if (data_0209fc58 & 0x30) {
        p = data_020927d8[GetLanguageIndexVS()];
    } else {
        p = data_020927c4[GetLanguageIndexVS()];
    }
    DecompressLZ16(p, G2::GetBG1ScrPtr());
    DecompressLZ16(p, G2S::GetBG1ScrPtr());

    REG_BG1CNT &= ~3; /* priority 0 */
    REG_DB_BG1CNT &= ~3;
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    gMainScreenLayers = DISPLAY_LAYER_BG1;
    gSubScreenLayers = 2;
    REG_DISPCNT = (REG_DISPCNT & ~0x1f00) | 0x200;
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~0x1f00) | 0x200;
}
}
