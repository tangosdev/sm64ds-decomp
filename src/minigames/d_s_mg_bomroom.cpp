//cpp
/* Bob-omb sorting minigame. Each bomb is one of two colors; a stylus
 * grab carries it by the offset at the touch. Its own pen settles it;
 * the wrong pen explodes it, which knocks that pen's settled bombs and the
 * loose ones back into play. When 40 bombs of one color are settled the
 * round ends, and mWinColor records the color.
 *
 * A bomb is sorted once dropped in its pen: x under 0x40 or over 0xc0, y
 * between 0x40 and 0x80.
 *
 * This TU is the class's whole linker unit: 81 functions, .text
 * 0x020d5a54..0x020d95a4. It opens with the destructor, which the header
 * declares first and out of line, so this file is the key function and
 * emits the vtable and the RTTI chain. Then come the unnamed helpers and
 * round states, StartRound, Render, Behavior and InitResources, and it
 * closes with the registry factory dScMgBomroom_c_classInit. Functions run
 * in ROM order under
 * `#pragma defer_codegen off`; do not reorder.
 */

// leftovers: the roaming steps read the table into a block-scoped fx16
// before FX_MUL: FX_CosIdx inside the FX_MUL argument swaps the table and
// speed registers.

#include "dScMgBomroom_c.h"
#include "OamAttr.h"
#include "nitro/hw/registers.h"
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "cstd.h"
#include "decl_common.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
#include "Sound.h"
#include "nitro/gx/g2.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/gx_bgcnt.h"
#include "PlayerInput.h"
#include "nitro/mi/memory.h"
#include "SpawnInfo.h"

extern "C" {
extern void Hud_RenderSprite(OamAttr *a0, int a1, int a2, int a3, int a4);
OamAttr data_ov006_02134514[5] = {
    {0x8000, 0x01fd, 0x101a, 0x0000}, {0x80f0, 0x01fd, 0x101a, 0x0000},
    {0x0010, 0x01fd, 0x105a, 0x0000}, {0x0018, 0x01fd, 0x105a, 0x0000},
    {0x0020, 0x01fd, 0x101b, 0xffff},
};
OamAttr data_ov006_0213466c[10] = {
    {0x00ef, 0x4000, 0x1094, 0x0000}, {0x80ef, 0x0010, 0x1096, 0x0000},
    {0x40ff, 0x0000, 0x10d4, 0x0000}, {0x00ff, 0x0010, 0x10d6, 0x0000},
    {0x0017, 0x4000, 0x10d7, 0x0000}, {0x8017, 0x0010, 0x10d9, 0x0000},
    {0x4007, 0x0000, 0x10f4, 0x0000}, {0x0007, 0x0010, 0x10f6, 0x0000},
    {0x400f, 0x0000, 0x10b7, 0x0000}, {0x000f, 0x0010, 0x10b9, 0xffff},
};
OamAttr data_ov006_021345ec[8] = {
    {0x000f, 0x4000, 0x1014, 0x0000}, {0x800f, 0x0010, 0x1016, 0x0000},
    {0x401f, 0x0000, 0x1054, 0x0000}, {0x001f, 0x0010, 0x1056, 0x0000},
    {0x00ef, 0x4000, 0x1014, 0x0000}, {0x80ef, 0x0010, 0x1016, 0x0000},
    {0x40ff, 0x0000, 0x1054, 0x0000}, {0x00ff, 0x0010, 0x1056, 0xffff},
};
OamAttr data_ov006_02134484[4] = {
    {0x800f, 0x8000, 0x1017, 0x0000},
    {0x800f, 0x4010, 0x1019, 0x0000},
    {0x80e7, 0x8000, 0x1097, 0x0000},
    {0x80e7, 0x4010, 0x1099, 0xffff},
};
OamAttr data_ov006_0213456c[8] = {
    {0x8010, 0x01fd, 0x101a, 0x0000}, {0x80e0, 0x01fd, 0x101a, 0x0000},
    {0x0000, 0x01fd, 0x101b, 0x0000}, {0x0030, 0x01fd, 0x101b, 0x0000},
    {0x00f0, 0x01fd, 0x105a, 0x0000}, {0x00f8, 0x01fd, 0x105a, 0x0000},
    {0x0020, 0x01fd, 0x105a, 0x0000}, {0x0028, 0x01fd, 0x105a, 0xffff},
};
OamAttr data_ov006_021344c4[5] = {
    {0x8000, 0x01fb, 0x101a, 0x0000}, {0x0010, 0x01fb, 0x105a, 0x0000},
    {0x0018, 0x01fb, 0x105a, 0x0000}, {0x0020, 0x01fb, 0x101b, 0x0000},
    {0x80f0, 0x01fb, 0x101a, 0xffff},
};
OamAttr data_ov006_0213453c[6] = {
    {0x80ef, 0x91f0, 0x1094, 0x0000}, {0x80ef, 0x51e8, 0x1096, 0x0000},
    {0x000f, 0x51f0, 0x10b7, 0x0000}, {0x800f, 0x11e8, 0x10b9, 0x0000},
    {0x401f, 0x11f0, 0x10f7, 0x0000}, {0x001f, 0x11e8, 0x10f9, 0xffff},
};
OamAttr data_ov006_0213462c[8] = {
    {0x000f, 0x51f0, 0x1014, 0x0000}, {0x800f, 0x11e8, 0x1016, 0x0000},
    {0x401f, 0x11f0, 0x1054, 0x0000}, {0x001f, 0x11e8, 0x1056, 0x0000},
    {0x00ef, 0x51f0, 0x1014, 0x0000}, {0x80ef, 0x11e8, 0x1016, 0x0000},
    {0x40ff, 0x11f0, 0x1054, 0x0000}, {0x00ff, 0x11e8, 0x1056, 0xffff},
};
OamAttr data_ov006_021344a4[4] = {
    {0x80e7, 0x91f0, 0x1097, 0x0000},
    {0x80e7, 0x51e8, 0x1099, 0x0000},
    {0x800f, 0x91f0, 0x1017, 0x0000},
    {0x800f, 0x51e8, 0x1019, 0xffff},
};
OamAttr data_ov006_021345ac[8] = {
    {0x8010, 0x01fb, 0x101a, 0x0000}, {0x0020, 0x01fb, 0x105a, 0x0000},
    {0x0028, 0x01fb, 0x105a, 0x0000}, {0x0030, 0x01fb, 0x101b, 0x0000},
    {0x80e0, 0x01fb, 0x101a, 0x0000}, {0x00f0, 0x01fb, 0x105a, 0x0000},
    {0x00f8, 0x01fb, 0x105a, 0x0000}, {0x0000, 0x01fb, 0x101b, 0xffff},
};
OamAttr *sBomroomPenGateSprites[] = {
    data_ov006_02134514, data_ov006_0213466c, data_ov006_021345ec, data_ov006_02134484,
    data_ov006_0213456c, data_ov006_021344c4, data_ov006_0213453c, data_ov006_0213462c,
    data_ov006_021344a4, data_ov006_021345ac,
};
extern int sBomroomPenGateX[];
extern int sBomroomPenGateY[];
OamAttr data_ov006_02134340[6] = {
    {0x00fb, 0x01f8, 0x101c, 0x0000}, {0x00fb, 0x01f0, 0x101c, 0x0000},
    {0x00fb, 0x01e8, 0x101c, 0x0000}, {0x00fb, 0x0000, 0x101c, 0x0000},
    {0x00fb, 0x0008, 0x101c, 0x0000}, {0x00fb, 0x0010, 0x101c, 0xffff},
};
OamAttr data_ov006_021342d0[4] = {
    {0x40f8, 0x01e7, 0x1097, 0x0000},
    {0x00f8, 0x01f7, 0x1099, 0x0000},
    {0x40f8, 0x1009, 0x1097, 0x0000},
    {0x00f8, 0x1001, 0x1099, 0xffff},
};
OamAttr data_ov006_021342f0[4] = {
    {0x80f0, 0x01e4, 0x101a, 0x0000},
    {0x80f0, 0x0014, 0x101a, 0x0000},
    {0x40f8, 0x01f0, 0x003c, 0x0000},
    {0x40f8, 0x0000, 0x003d, 0xffff},
};
OamAttr data_ov006_02134310[6] = {
    {0x40f8, 0x0018, 0x1097, 0x0000}, {0x00f8, 0x0028, 0x1099, 0x0000},
    {0x40f8, 0x11d8, 0x1097, 0x0000}, {0x00f8, 0x11d0, 0x1099, 0x0000},
    {0x40f8, 0x01f0, 0x005c, 0x0000}, {0x40f8, 0x0000, 0x005d, 0xffff},
};
OamAttr data_ov006_02134370[6] = {
    {0x00fb, 0x01e0, 0x101c, 0x0000}, {0x00fb, 0x01d8, 0x101c, 0x0000},
    {0x00fb, 0x01d0, 0x101c, 0x0000}, {0x00fb, 0x0018, 0x101c, 0x0000},
    {0x00fb, 0x0020, 0x101c, 0x0000}, {0x00fb, 0x0028, 0x101c, 0xffff},
};
OamAttr *sBomroomHatchPanelSprites[] = {
    data_ov006_02134340, data_ov006_021342d0, data_ov006_021342f0,
    data_ov006_02134310, data_ov006_02134370,
};
OamAttr data_ov006_02134404[8] = {
    {0x00f4, 0x41e8, 0x1014, 0x0000}, {0x80f4, 0x01f8, 0x1016, 0x0000},
    {0x4004, 0x01e8, 0x1054, 0x0000}, {0x0004, 0x01f8, 0x1056, 0x0000},
    {0x00f4, 0x5008, 0x1014, 0x0000}, {0x80f4, 0x1000, 0x1016, 0x0000},
    {0x4004, 0x1008, 0x1054, 0x0000}, {0x0004, 0x1000, 0x1056, 0xffff},
};
OamAttr data_ov006_021343a0[2] = {
    {0x80f4, 0x81e8, 0x1017, 0x0000},
    {0x80f4, 0x9008, 0x1017, 0xffff},
};
OamAttr data_ov006_021343e4[4] = {
    {0x80f4, 0x41e5, 0x101f, 0x0000},
    {0x80f4, 0x4013, 0x101f, 0x0000},
    {0x0014, 0x01e5, 0x107a, 0x0000},
    {0x0014, 0x0013, 0x107a, 0xffff},
};
OamAttr data_ov006_021343c4[4] = {
    {0x80f4, 0x8017, 0x1017, 0x0000},
    {0x80f4, 0x4027, 0x1019, 0x0000},
    {0x80f4, 0x91d9, 0x1017, 0x0000},
    {0x80f4, 0x51d1, 0x1019, 0xffff},
};
OamAttr data_ov006_02134444[8] = {
    {0x00f4, 0x4018, 0x1014, 0x0000}, {0x80f4, 0x0028, 0x1016, 0x0000},
    {0x4004, 0x0018, 0x1054, 0x0000}, {0x0004, 0x0028, 0x1056, 0x0000},
    {0x00f4, 0x51d8, 0x1014, 0x0000}, {0x80f4, 0x11d0, 0x1016, 0x0000},
    {0x4004, 0x11d8, 0x1054, 0x0000}, {0x0004, 0x11d0, 0x1056, 0xffff},
};
OamAttr *sBomroomTopDoorSprites[] = {
    data_ov006_02134404, data_ov006_021343a0, data_ov006_021343e4,
    data_ov006_021343c4, data_ov006_02134444,
};
extern int sBomroomHatchPanelX[];
extern int sBomroomHatchPanelY[];
extern unsigned char *sBomroomBombFrameTimes[];
extern unsigned char sBomroomBombFrameCounts[];
extern void Mg_RenderSpriteBoth(OamAttr *a0, int a1, int a2, int a3, int a4);
extern unsigned char *sBomroomBombSprites[];
extern unsigned char *sBomroomBombLitSprites[];
extern "C" char data_ov006_02133dd0[];
extern "C" char data_ov006_02133dc8[];
extern "C" char data_ov006_02133dc0[];
extern "C" char data_ov006_02133db8[];
extern "C" char data_ov006_02133e00[];
extern "C" char data_ov006_02133df8[];
extern "C" char data_ov006_02133df0[];
extern "C" char data_ov006_02133de8[];
extern "C" char data_ov006_02133de0[];
extern "C" char data_ov006_02133dd8[];
extern "C" char data_ov006_02133c98[];
extern "C" char data_ov006_02133c90[];
extern "C" char data_ov006_02133c88[];
extern "C" char data_ov006_02133c80[];
extern "C" char data_ov006_02133c78[];
extern "C" char data_ov006_02133c70[];
extern "C" char data_ov006_02133ca8[];
extern "C" char data_ov006_02133cb8[];
extern "C" char data_ov006_02133ca0[];
extern "C" char data_ov006_02133cc0[];
extern "C" char data_ov006_02133cc8[];
extern "C" char data_ov006_02133cb0[];
extern "C" char data_ov006_02133ce8[];
extern "C" char data_ov006_02133d50[];
extern OamAttr *sBomroomBombsSprites[];
extern void RenderOamMainScreen(OamAttr *a0, int a1, int a2, int a3, int a4);
OamAttr data_ov006_02134f34[21] = {
    {0x00f8, 0x01f8, 0x011d, 0x0000}, {0x00f8, 0x01f0, 0x011d, 0x0000},
    {0x00f8, 0x01e8, 0x011d, 0x0000}, {0x00f8, 0x01e0, 0x011d, 0x0000},
    {0x00f8, 0x01d8, 0x011d, 0x0000}, {0x00f8, 0x01d0, 0x011d, 0x0000},
    {0x00f8, 0x01c8, 0x011d, 0x0000}, {0x00f8, 0x01c0, 0x011d, 0x0000},
    {0x00f8, 0x01b8, 0x011d, 0x0000}, {0x00f8, 0x01b0, 0x011d, 0x0000},
    {0x00f8, 0x0040, 0x011d, 0x0000}, {0x00f8, 0x0038, 0x011d, 0x0000},
    {0x00f8, 0x0030, 0x011d, 0x0000}, {0x00f8, 0x0028, 0x011d, 0x0000},
    {0x00f8, 0x0020, 0x011d, 0x0000}, {0x00f8, 0x0018, 0x011d, 0x0000},
    {0x00f8, 0x0010, 0x011d, 0x0000}, {0x00f8, 0x0008, 0x011d, 0x0000},
    {0x00f8, 0x0000, 0x011d, 0x0000}, {0x00f8, 0x01f8, 0x011d, 0x0000},
    {0x00f8, 0x0048, 0x011d, 0xffff},
};
OamAttr *sBomroomRoundOverSprites = data_ov006_02134f34;
OamAttr data_ov006_02133b60[8] = {
    {0x00f0, 0x41f0, 0x0110, 0x0000}, {0x4000, 0x01f0, 0x0150, 0x0000},
    {0x80f0, 0x0000, 0x0111, 0x0000}, {0x0000, 0x0000, 0x0151, 0x0000},
    {0x80f0, 0x0008, 0x0112, 0x0000}, {0x0000, 0x0008, 0x0152, 0x0000},
    {0x80f8, 0x0010, 0x0133, 0x0000}, {0x00f0, 0x0010, 0x0154, 0xffff},
};
OamAttr data_ov006_02133ba0[8] = {
    {0x00f0, 0x41f0, 0x0114, 0x0000}, {0x80f0, 0x0000, 0x0115, 0x0000},
    {0x80f0, 0x0008, 0x0116, 0x0000}, {0x4000, 0x01f0, 0x0150, 0x0000},
    {0x4000, 0x0000, 0x0151, 0x0000}, {0x0000, 0x0010, 0x0153, 0x0000},
    {0x00f8, 0x0010, 0x0133, 0x0000}, {0x00f0, 0x0010, 0x0154, 0xffff},
};
OamAttr data_ov006_02133af0[7] = {
    {0x00f0, 0x41f0, 0x0117, 0x0000}, {0x00f0, 0x4000, 0x0118, 0x0000},
    {0x4000, 0x01f0, 0x0150, 0x0000}, {0x4000, 0x0000, 0x0151, 0x0000},
    {0x0000, 0x0010, 0x0153, 0x0000}, {0x00f8, 0x0010, 0x0133, 0x0000},
    {0x00f0, 0x0010, 0x0154, 0xffff},
};
OamAttr data_ov006_02133b28[7] = {
    {0x00f0, 0x41f0, 0x011a, 0x0000}, {0x00f0, 0x4000, 0x011b, 0x0000},
    {0x4000, 0x01f0, 0x0150, 0x0000}, {0x4000, 0x0000, 0x0151, 0x0000},
    {0x0000, 0x0010, 0x0153, 0x0000}, {0x00f8, 0x0010, 0x0133, 0x0000},
    {0x00f0, 0x0010, 0x0154, 0xffff},
};
OamAttr *sBomroomMarkerPlainSprites[] = {data_ov006_02133b60, data_ov006_02133ba0,
                                         data_ov006_02133af0, data_ov006_02133b28};
OamAttr data_ov006_02133ab0[3] = {
    {0x40fc, 0x21f0, 0x0134, 0x0000},
    {0x40fc, 0x2000, 0x0135, 0x0000},
    {0x00fc, 0x0010, 0x0113, 0xffff},
};
OamAttr data_ov006_02133a98[3] = {
    {0x40fc, 0x21f0, 0x0137, 0x0000},
    {0x40fc, 0x2000, 0x0138, 0x0000},
    {0x00fc, 0x0010, 0x0113, 0xffff},
};
OamAttr data_ov006_02133a80[3] = {
    {0x40fc, 0x21f0, 0x013a, 0x0000},
    {0x40fc, 0x2000, 0x013b, 0x0000},
    {0x00fc, 0x0010, 0x0113, 0xffff},
};
OamAttr data_ov006_02133ac8[3] = {
    {0x40fc, 0x21f0, 0x0130, 0x0000},
    {0x40fc, 0x2000, 0x0131, 0x0000},
    {0x00fc, 0x0010, 0x0113, 0xffff},
};
OamAttr *sBomroomMarkerLitSprites[] = {data_ov006_02133ab0, data_ov006_02133a98,
                                       data_ov006_02133a80, data_ov006_02133ac8};
extern int sBomroomMarkerY[];
extern int Mg_GetSubmittedScore(void);
extern u16 sBomroomSpawnDelays[];
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);

extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
}
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

/* Runtime asset handles resolved through the overlay 0 file table; the names
   are the ROM's MG/ file names (tools/asset_catalog.py resolve). */
enum dScMgBomroom_c_AssetId {
    ASSET_D_2D_BLUE_SHADOW_NSC_BIN = 0x0002,
    ASSET_D_2D_ELEVATOR_NSC_BIN = 0x0003,
    ASSET_D_2D_LIGHT_UP_NSC_BIN = 0x0007,
    ASSET_D_2D_MG_BG_BOMROOM_DOWN_NSC_BIN = 0x0022,
    ASSET_D_2D_MG_BG_BOMROOM_NCG_BIN = 0x0023,
    ASSET_D_2D_MG_BG_BOMROOM_NCL_BIN = 0x0024,
    ASSET_D_2D_MG_BG_BOMROOM_UP_NSC_BIN = 0x0025,
    ASSET_D_2D_MG_BOMROOM_NCG_BIN = 0x00b5,
    ASSET_D_2D_MG_BOMROOM_NCL_BIN = 0x00b6,
};

/* A random integer in [0, n): the top 15 bits of the minigames' shared
   generator, scaled (the PACHINKO_RAND of d_s_mg_pachinko.cpp). */
#define BOMROOM_RAND(n) (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * (n)) >> 15)

/* The state tables' entry types: the round and round-end states take no
   argument, the per-slot and per-bomb states take the slot. */
typedef void (dScMgBomroom_c::*PMF)(int);
typedef void (dScMgBomroom_c::*PMFv)();

/* Stores the scene's level, held to 9998. The ROM stores the new value and
   reloads it for the clamp, the shape of an inline setter on the base
   (d_s_mg_amida.cpp and d_mg3d_esp_anim_set.cpp keep the same clamp, the
   latter even after a store of 0); taking a dScMgBomroom_c * instead adds
   two words. */
static inline void SetSceneLevel(dScMgBase_c *scene, u32 level) {
    scene->mLevel = level;
    if (scene->mLevel > 9998)
        scene->mLevel = 9998;
}

#pragma defer_codegen off

/* No member needs explicit destruction: the empty body is the compiler's
 * own vtable store and base-destructor call. The deleting variant reaches
 * dScMgBase_c's operator delete, its immediate base's. */
dScMgBomroom_c::~dScMgBomroom_c()
{
}

/* Draws the round-over sprite once mRoundOver is set. */

void dScMgBomroom_c::DrawRoundOver() {
    if (mRoundOver == 0)
        return;
    RenderOamMainScreen(sBomroomRoundOverSprites, 128, 192, -1, 1);
}

void dScMgBomroom_c::SetRoundOver() {
    mRoundOver = 1;
}

/* Steps the BG2 y offset mBg2Y (fx32): state 1 raises it by 3 a
 * frame to 0xc0, state 2 waits out the mBg2Hold count, then lowers it by 3
 * a frame back to 0. */

void dScMgBomroom_c::UpdateBg2Lift() {
    u8 state = mBg2State;

    if (state == 0) {
        return;
    }

    if (state == 1) {
        s32 v;
        s32 shifted;
        mBg2Y += 0x3000;
        v = mBg2Y;
        shifted = v >> 0xc;
        if (shifted >= 0xc0) {
            mBg2Y = 0xc0000;
            mBg2State = 2;
            shifted = 0xc0;
            mBg2Hold = shifted;
            Sound::PlayBank2_2D(0x1de);
            HideExplodedBombs();
        }
        SetBg2Offset(0, shifted);
        return;
    }

    if (state != 2) {
        return;
    }

    if (mBg2Hold != 0) {
        mBg2Hold -= 1;
        if (mBg2Hold != 0) {
            return;
        }
        Sound::PlayBank2_2D(0x1df);
        return;
    }

    {
        s32 v;
        s32 shifted;
        mBg2Y -= 0x3000;
        v = mBg2Y;
        shifted = v >> 0xc;
        if (shifted <= 0) {
            shifted = 0;
            mBg2Y = 0;
            mBg2State = 0;
            Sound::PlayBank2_2D(0x1de);
        }
        SetBg2Offset(0, shifted);
    }
}

void dScMgBomroom_c::ResetBg2Lift() {
    mBg2Y = 0;
    mBg2State = 0;
    mBg2Hold = 0;
    SetBg2Offset(0, 0);
}

/* Draws the two mMarkers sprite records that are active. */

void dScMgBomroom_c::Marker_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mMarkers[i].gate != 0) {
            int xv = mMarkers[i].x;
            int yv = mMarkers[i].y;
            int idx = mMarkers[i].idx;
            int sx = xv >> 12;
            int sy = yv >> 12;
            Hud_RenderSprite((i != 0) ? sBomroomMarkerLitSprites[idx]
                                      : sBomroomMarkerPlainSprites[idx],
                             sx, sy, -1, 1);
        }
    }
}

void dScMgBomroom_c::Marker_UpdateAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mMarkers[i].state) {
            mMarkers[i].timer = mMarkers[i].timer + 1;
            if (mMarkers[i].timer >= 4) {
                mMarkers[i].timer = 0;
                mMarkers[i].idx = mMarkers[i].idx + 1;
                mMarkers[i].idx = mMarkers[i].idx & 3;
            }
        }
    }
}

void dScMgBomroom_c::Marker_Show(int idx) {
    mMarkers[idx].state = 1;
    mMarkers[idx].gate = 1;
    mMarkers[idx].x = 0x80000;
    mMarkers[idx].y = sBomroomMarkerY[idx] << 12;
    mMarkers[idx].timer = 0;
    mMarkers[idx].idx = 0;
}

/* The scene keeps its per-slot UI state in the two-entry Slot arrays
 * mHatchPanels, mPenGates and mMarkers. */

void dScMgBomroom_c::Marker_HideAll() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mMarkers[slot].state = 0;
        mMarkers[slot].gate = 0;
    }
}

void Bomroom_DrawScore(void)
{
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 4);
}

void dScMgBomroom_c::TopDoor_Close() {
    mTopDoor[0].state = 3;
    Sound::PlayBank2_2D(0x1e3);
}

void dScMgBomroom_c::TopDoor_Open() {
    mTopDoor[0].state = 1;
    Sound::PlayBank2_2D(0x1e2);
}

void dScMgBomroom_c::TopDoor_Draw() {
    if (mTopDoor[0].level == 0)
        return;
    RenderOamMainScreen(sBomroomTopDoorSprites[mTopDoor[0].idx], mTopDoor[0].x >> 0xc,
                        mTopDoor[0].y >> 0xc, -1, 2);
}

// The top door is a one-record array: its state handlers take the index the
// shared PMF signature passes (TopDoor_Update always passes 0), the same way
// the two-slot mPenGates handlers index theirs.

void dScMgBomroom_c::TopDoor_StateClosing(int index) {
    if (mTopDoor[index].idx != 0) {
        mTopDoor[index].timer++;
        if (mTopDoor[index].timer < 3)
            return;
        mTopDoor[index].timer = 0;
        mTopDoor[index].idx--;
    } else {
        mTopDoor[index].state = 0;
    }
}

void dScMgBomroom_c::TopDoor_StateOpen(int index) {}

void dScMgBomroom_c::TopDoor_StateOpening(int index) {
    if (mTopDoor[index].idx >= 4) {
        mTopDoor[index].idx = 4;
        mTopDoor[index].state = 2;
        return;
    }
    mTopDoor[index].timer++;
    if (mTopDoor[index].timer < 3) {
        return;
    }
    mTopDoor[index].timer = 0;
    mTopDoor[index].idx++;
    if (mTopDoor[index].idx >= 4) {
        mTopDoor[index].idx = 4;
        mTopDoor[index].state = 2;
    }
}

void dScMgBomroom_c::TopDoor_StateShut(int index) {
    mTopDoor[index].timer = 0;
}

extern "C" PMF sBomroomTopDoorStateTable[];
void dScMgBomroom_c::TopDoor_Update() {
    if (!mTopDoor[0].gate)
        return;
    (this->*sBomroomTopDoorStateTable[mTopDoor[0].state])(0);
}

void dScMgBomroom_c::TopDoor_Show() {
    mTopDoor[0].gate = 1;
    mTopDoor[0].state = 0;
    mTopDoor[0].idx = 0;
    mTopDoor[0].level = 1;
    mTopDoor[0].timer = 0;
    mTopDoor[0].x = 0x80000;
    mTopDoor[0].y = 0x8000;
}

void dScMgBomroom_c::TopDoor_Hide() {
    mTopDoor[0].gate = 0;
    mTopDoor[0].level = 0;
}

void dScMgBomroom_c::PenGate_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        int xv = mPenGates[i].x >> 12;
        int yv = mPenGates[i].y >> 12;
        int k = mPenGates[i].level;
        if (i != 0) k += 5;
        Hud_RenderSprite(sBomroomPenGateSprites[k], xv, yv, -1, -1);
    }
}

void dScMgBomroom_c::PenGate_StateClosing(int index) {
    if (mPenGates[index].level != 0) {
        mPenGates[index].timer++;
        if (mPenGates[index].timer < 4)
            return;
        mPenGates[index].timer = 0;
        mPenGates[index].level--;
    } else {
        mPenGates[index].state = 0;
    }
}

void dScMgBomroom_c::PenGate_StateHold(int index) {
    if (mPenGates[index].timer != 0) {
        mPenGates[index].timer--;
        if ((s16)mPenGates[index].timer < 0)
            mPenGates[index].timer = 0;
        return;
    }

    mPenGates[index].state = 3;
}

void dScMgBomroom_c::PenGate_StateOpening(int index) {
    mPenGates[index].timer++;
    if (mPenGates[index].timer < 5)
        return;
    mPenGates[index].timer = 0;
    mPenGates[index].level++;
    if (mPenGates[index].level >= 4) {
        mPenGates[index].state = 2;
        mPenGates[index].timer = 32;
    }
}

void dScMgBomroom_c::PenGate_StateShut(int index) {
    mPenGates[index].timer = 0;
}

extern "C" PMF sBomroomPenGateStateTable[];
void dScMgBomroom_c::PenGate_UpdateAll() {
    for (int slot = 0; slot < 2; slot++) {
        if (mPenGates[slot].gate) {
            u8 state = mPenGates[slot].state;
            (this->*sBomroomPenGateStateTable[state])(slot);
        }
    }
}

void dScMgBomroom_c::PenGate_ShowAll() {
    int i;
    for (i = 0; i < 2; i++) {
        mPenGates[i].gate = 1;
        mPenGates[i].state = 0;
        mPenGates[i].idx = 1;
        mPenGates[i].x = sBomroomPenGateX[i] << 0xc;
        mPenGates[i].y = sBomroomPenGateY[i] << 0xc;
        mPenGates[i].level = 0;
        mPenGates[i].timer = 0;
    }
}

void dScMgBomroom_c::PenGate_Open(int index) {
    unsigned char st = mPenGates[index].state;
    if (st != 0) {
        if (st == 2) {
            mPenGates[index].timer = 32;
        } else {
            mPenGates[index].state = 1;
        }
        return;
    }
    mPenGates[index].state = 1;
    SetBg0Offset(0x100, 0);
}

void dScMgBomroom_c::PenGate_HideAll() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mPenGates[slot].gate = 0;
        mPenGates[slot].idx = 0;
    }
}

void dScMgBomroom_c::HatchPanel_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mHatchPanels[i].idx != 0) {
            int xv = mHatchPanels[i].x >> 12;
            int yv = mHatchPanels[i].y >> 12;
            int k = mHatchPanels[i].level;
            OamAttr *v = (i == 0) ? sBomroomTopDoorSprites[k] : sBomroomHatchPanelSprites[k];
            Hud_RenderSprite(v, xv, yv, -1, 1);
        }
    }
}

void dScMgBomroom_c::HatchPanel_StateRevert(int index) {
    if (mHatchPanels[index].level != 0) {
        mHatchPanels[index].timer++;
        if (mHatchPanels[index].timer < 3)
            return;
        mHatchPanels[index].timer = 0;
        mHatchPanels[index].level--;
    } else {
        mHatchPanels[index].state = 0;
    }
}

void dScMgBomroom_c::HatchPanel_StateHeld(int index) {}

void dScMgBomroom_c::HatchPanel_StateAnimate(int index) {
    mHatchPanels[index].timer += 1;
    if (mHatchPanels[index].timer < 5)
        return;
    mHatchPanels[index].timer = 0;
    mHatchPanels[index].level += 1;
    if (index == 0) {
        u8 level = mHatchPanels[index].level;
        if (level == 2 || level == 4) {
            mMarkers[0].y += 0x8000;
        }
    }
    if (mHatchPanels[index].level == 3 && index == 1)
        Marker_Show(index);
    if (mHatchPanels[index].level >= 4)
        mHatchPanels[index].state = 2;
}

void dScMgBomroom_c::HatchPanel_StateIdle(int index) {
    mHatchPanels[index].timer = 0;
}

extern "C" PMF sBomroomHatchPanelStateTable[];
void dScMgBomroom_c::HatchPanel_UpdateAll() {
    for (int slot = 0; slot < 2; slot++) {
        if (mHatchPanels[slot].gate) {
            u8 state = mHatchPanels[slot].state;
            (this->*sBomroomHatchPanelStateTable[state])(slot);
        }
    }
}

void dScMgBomroom_c::HatchPanel_ShowAll() {
    int i;
    for (i = 0; i < 2; i++) {
        mHatchPanels[i].gate = 1;
        mHatchPanels[i].state = 0;
        mHatchPanels[i].x = sBomroomHatchPanelX[i] << 0xc;
        mHatchPanels[i].y = sBomroomHatchPanelY[i] << 0xc;
        mHatchPanels[i].level = 0;
        mHatchPanels[i].timer = 0;
        mHatchPanels[i].idx = 1;
    }
}

void dScMgBomroom_c::HatchPanel_HideAll() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mHatchPanels[slot].gate = 0;
        mHatchPanels[slot].idx = 0;
    }
}

void dScMgBomroom_c::HatchPanel_Activate(int index) {
    if (mHatchPanels[index].state != 0)
        return;
    mHatchPanels[index].state = 1;
    Sound::PlayBank2_2D(0x1e2);
    if (index != 0) return;
    Marker_Show(index);
}

void dScMgBomroom_c::DrawRemaining() {
    if (mShowTally == 0)
        return;
    Mg_DrawBigNumber(0x80, 0xc, mTallyCount, 1, -1, 0, 0);
}

void dScMgBomroom_c::CountSortedBombs() {
    int plain, colored, i;
    colored = 0;
    plain = 0;
    for (i = 0; i < 112; i++) {
        if (mBombs[i].active == 0) continue;
        if (mBombs[i].state != 5) continue;
        if (mBombs[i].color != 0) colored++;
        else plain++;
    }
    if (plain >= 0x28) {
        mSceneState = 3;
        mSubState = 0;
        mBlastPen = 0;
        mWinColor = 0;
        SetSceneLevel(this, mLevel + 1);
        mFusesFrozen = 1;
        Sound::PlayBank2_2D(0x1e6);
    } else if (colored >= 0x28) {
        mSceneState = 3;
        mSubState = 0;
        mBlastPen = 0;
        mWinColor = 1;
        SetSceneLevel(this, mLevel + 1);
        mFusesFrozen = 1;
        Sound::PlayBank2_2D(0x1e6);
    }
}

/* Bomb `picked` has gone off. Its pen byte is 0 when it
 * was loose: every other live bomb not yet sorted (state 5) is knocked into
 * state 3 for 0x20 frames. Otherwise only the bombs of that pen's color and
 * the idle ones are. The first blast also records its pen in mBlastPen and
 * sets mSceneState to 3. */
void dScMgBomroom_c::OnBombExploded(int picked) {
    int i;
    for (i = 0; i < 112; i++) {
        if (mBombs[i].active == 0 || i == picked)
            continue;
        if (mBombs[picked].pen == 0) {
            if (mBombs[i].state != 5) {
                mBombs[i].state = 3;
                mBombs[i].marchDelay = 32;
            }
        } else {
            if (mBombs[i].state == 5 && mBombs[i].color + 1 == mBombs[picked].pen) {
                mBombs[i].state = 3;
                mBombs[i].marchDelay = 32;
            }
            if (mBombs[i].state <= 1) {
                mBombs[i].state = 3;
                mBombs[i].marchDelay = 32;
            }
        }
    }
    if (mBlastPen != 0)
        return;
    mBlastPen = mBombs[picked].pen + 1;
    mStateTimer = 96;
    mSceneState = 3;
    mSubState = 0;
}

/* Steps one bomb's animation: the frame advances when its counter
 * reaches the per-type frame time, playing its sound effects, and
 * wraps (or holds, for type 3) at the per-type frame count. */

void dScMgBomroom_c::Bomb_Animate(int index) {
    int type;
    int frame;
    int max;

    mBombs[index].counter += 1;
    type = mBombs[index].type;
    frame = mBombs[index].frame;

    if (type != 3 && type != 0 && mSceneState != 3) {
        mBombs[index].sound = func_02012468(mBombs[index].sound, 2, 0x1db, 4, 0, 0,
                               func_020126e8(mBombs[index].x), 0);
    }

    if (mBombs[index].counter >= sBomroomBombFrameTimes[type][frame]) {
        frame++;
        mBombs[index].frame = frame;
        mBombs[index].counter = 0;
        if (type != 3 && mBombs[index].state != 2 && (type != 0 || mBombs[index].state != 5)) {
            if (mBombs[index].active == 1 && mSceneState != 3) {
                if (mBombs[index].color != 0) {
                    func_02012718(0x1d9, mBombs[index].x);
                } else {
                    func_02012718(0x1da, mBombs[index].x);
                }
            }
        }
        if (type == 3 && frame == 1) {
            func_02012718(0x1e1, mBombs[index].x);
        }
    }

    max = sBomroomBombFrameCounts[type];
    if (frame < max) {
        return;
    }
    if (type == 3) {
        mBombs[index].active = 0;
        mBombs[index].visible = 0;
    } else {
        max = 0;
    }
    mBombs[index].frame = max;
}

void dScMgBomroom_c::Bomb_CheckSorted(int index) {
    u8 color = mBombs[index].color;
    int x = mBombs[index].x >> 0xc;
    int y = mBombs[index].y >> 0xc;
    if (color != 0) {
        if (x <= 0xc0) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        mBombs[index].state = 5;
        mBombs[index].type = 0;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].speed = 0x999;
        func_02012718(0x1dc, mBombs[index].x);
    } else {
        if (x >= 0x40) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        mBombs[index].state = 5;
        mBombs[index].type = 0;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].speed = 0x999;
        func_02012718(0x1dc, mBombs[index].x);
    }
}

void dScMgBomroom_c::Bomb_CheckWrongPen(int index) {
    unsigned char color = mBombs[index].color;
    int x = mBombs[index].x >> 12;
    int y = mBombs[index].y >> 12;

    if (color != 0) {
        if (x >= 0x40)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        mBombs[index].fuse = 0;
        mBombs[index].state = 4;
        mBombs[index].type = 3;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].pen = 1;
        mBombs[index].exploded = 1;
        OnBombExploded(index);
    } else {
        if (x <= 0xc0)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        mBombs[index].fuse = 0;
        mBombs[index].state = 4;
        mBombs[index].type = 3;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].pen = 2;
        mBombs[index].exploded = 1;
        OnBombExploded(index);
    }
}

/* Picks up a bomb under the stylus: a touch within 12 units across and 15
 * down of it, while nothing is held (mHeldColor is 0xff). grabX/grabY keep the
 * stylus-minus-bomb offset. */

void dScMgBomroom_c::Bomb_CheckGrab(int index) {
    u8 slot = gActivePlayerSlot;
    int touchBegan = 0;
    int dx, dy;
    if (gTouchInfo[slot].held != 0)
        touchBegan = gTouchInfo[slot].edge != 0;
    if (touchBegan == 0)
        return;
    if (mHeldColor != 0xff) return;
    dx = gTouchInfo[slot].x - (mBombs[index].x >> 0xc);
    dy = gTouchInfo[slot].y - (mBombs[index].y >> 0xc);
    if (dx > 0xc) return;
    if (dx < -0xc) return;
    if (dy > 0xf) return;
    if (dy < -0xf) return;
    mHeldColor = mBombs[index].color;
    mBombs[index].type = 1;
    mBombs[index].state = 2;
    mBombs[index].grabX = dx << 0xc;
    mBombs[index].grabY = dy << 0xc;
    mBombs[index].overPen = 0;
    func_02012718(0x1d2, mBombs[index].x);
}

void dScMgBomroom_c::Bomb_PushApart(int index) {
    int x, y, py, dx, dy1, ang, a, dy2, sy, px;

    x = mBombs[index].x >> 12;
    y = mBombs[index].y >> 12;

    if (x + 12 > 0x100) {
        mBombs[index].angle = 0x8000 - mBombs[index].angle;
        mBombs[index].x = 0xF4000;
    } else if (x - 12 < 0) {
        mBombs[index].angle = 0x8000 - mBombs[index].angle;
        mBombs[index].x = 0xC000;
    }

    if (y + 12 > 0xB8) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0xAC000;
    } else if (y - 12 < 0) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0xC000;
    }

    px = mBombs[index].x >> 12;
    py = mBombs[index].y >> 12;

    if (px + 12 > 0xC0 && py + 12 > 0x40 && py - 12 < 0x80) {
        dx = px - 0xC0;
        dy1 = py - 0x40;
        dy2 = 0x80 - py;
        if (px <= 0xC0 && py >= 0x40 && py <= 0x80) {
            mBombs[index].angle = 0x8000 - mBombs[index].angle;
            mBombs[index].x = 0xB4000;
        } else if (px > 0xC0 && py < 0x40) {
            mBombs[index].angle = 0 - mBombs[index].angle;
            mBombs[index].y = 0x34000;
        } else if (px > 0xC0 && py > 0x80) {
            mBombs[index].angle = 0 - mBombs[index].angle;
            mBombs[index].y = 0x8C000;
        } else if (px > 0xC0 && py > 0x40 && py < 0x80) {
            if (dy1 < dy2) {
                if (dx < dy1) {
                    mBombs[index].angle = 0x8000 - mBombs[index].angle;
                    mBombs[index].x = 0xB4000;
                } else {
                    mBombs[index].angle = 0 - mBombs[index].angle;
                    mBombs[index].y = 0x34000;
                }
            } else {
                if (dx < dy2) {
                    mBombs[index].angle = 0x8000 - mBombs[index].angle;
                    mBombs[index].x = 0xB4000;
                } else {
                    mBombs[index].angle = 0 - mBombs[index].angle;
                    mBombs[index].y = 0x8C000;
                }
            }
        } else {
            if (py >= 0x60) {
                sy = 0xC0 - px;
            } else {
                dy2 = 0x40 - py;
                sy = 0xC0 - px;
            }
            cstd::sqrt((sy * sy + dy2 * dy2));
            ang = (u16)cstd::atan2(dy2, sy);
            mBombs[index].x = (0xC0 - FX_MUL(FX_CosIdx(ang >> 4), 0xF)) << 12;
            if (py >= 0x60) {
                mBombs[index].y = (0x80 - FX_MUL(FX_SinIdx(ang >> 4), 0xF)) << 12;
            } else {
                mBombs[index].y = (0x40 - FX_MUL(FX_SinIdx(ang >> 4), 0xF)) << 12;
            }
            a = mBombs[index].angle;
            if (FX_CosIdx(a >> 4) > 0) {
                mBombs[index].angle = 0x8000 - a;
            } else {
                mBombs[index].angle = 0 - a;
            }
        }
    }

    if (px - 12 >= 0x40) return;
    if (py + 12 <= 0x40) return;
    if (py - 12 >= 0x80) return;

    dy2 = 0x40 - px;
    dy1 = py - 0x40;
    sy = 0x80 - py;
    if (px >= 0x40 && py >= 0x40 && py <= 0x80) {
        mBombs[index].angle = 0x8000 - mBombs[index].angle;
        mBombs[index].x = 0x4C000;
        return;
    }
    if (px < 0x40 && py < 0x40) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0x34000;
        return;
    }
    if (px < 0x40 && py > 0x80) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0x8C000;
        return;
    }
    if (px < 0x40 && py > 0x40 && py < 0x80) {
        if (dy1 < sy) {
            if (dy2 < dy1) {
                mBombs[index].angle = 0x8000 - mBombs[index].angle;
                mBombs[index].x = 0x4C000;
                return;
            }
            mBombs[index].angle = 0 - mBombs[index].angle;
            mBombs[index].y = 0x34000;
            return;
        }
        if (dy2 < sy) {
            mBombs[index].angle = 0x8000 - mBombs[index].angle;
            mBombs[index].x = 0x4C000;
            return;
        }
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0x8C000;
        return;
    }

    if (py < 0x60) {
        sy = 0x40 - py;
    }
    cstd::sqrt((dy2 * dy2 + sy * sy));
    ang = (u16)cstd::atan2(sy, dy2);
    mBombs[index].x = (0x40 - FX_MUL(FX_CosIdx(ang >> 4), 0xF)) << 12;
    if (py >= 0x60) {
        mBombs[index].y = (0x80 - FX_MUL(FX_SinIdx(ang >> 4), 0xF)) << 12;
    } else {
        mBombs[index].y = (0x40 - FX_MUL(FX_SinIdx(ang >> 4), 0xF)) << 12;
    }
    a = mBombs[index].angle;
    if (FX_CosIdx(a >> 4) < 0) {
        mBombs[index].angle = 0x8000 - a;
    } else {
        mBombs[index].angle = 0 - a;
    }
}

void dScMgBomroom_c::DrawBombs() {
    int i;
    for (i = 0; i < 112; i++) {
        if (mBombs[i].visible == 0)
            continue;
        {
            int type = mBombs[i].type;
            int palette = 1;
            int frame, color;
            unsigned char sprite;
            int state;
            int fy;
            int x, y;
            if (type == 3)
                palette = 0;
            color = mBombs[i].color;
            frame = mBombs[i].frame;
            if (color != 0) {
                sprite = sBomroomBombLitSprites[type][frame];
            } else {
                sprite = sBomroomBombSprites[type][frame];
            }
            state = mBombs[i].state;
            x = mBombs[i].x >> 12;
            fy = mBombs[i].y;
            y = fy >> 12;
            if (state == 6) {
                if (mBombs[i].substate == 4) {
                    y = (fy - mBg2Y) >> 12;
                }
            }
            Mg_RenderSpriteBoth(sBomroomBombsSprites[sprite], x, y, -1, palette);
        }
    }
}

void dScMgBomroom_c::CheckColorComplete() {
    int nColored = 0;
    int nPlain = 0;
    int nLined = 0;
    int i;

    for (i = 0; i < 112; i++) {
        if (mBombs[i].active != 0) {
            if (mBlastPen == 1) {
                if (mBombs[i].color != 0) {
                    int col;
                    int row;
                    mBombs[i].substate = 3;
                    col = nColored;
                    row = 0;
                    mBombs[i].marchDelay = (unsigned short)(nLined * 8);
                    while (col >= 5) {
                        col -= 5;
                        row++;
                    }
                    mBombs[i].lineSlot = (unsigned char)(row * 10 + col);
                    nColored++;
                    nLined++;
                } else {
                    int col;
                    int row;
                    /* The ROM keeps this store, overwritten below. */
                    mBombs[i].lineSlot = (unsigned char)nPlain;
                    mBombs[i].substate = 3;
                    col = nPlain;
                    mBombs[i].marchDelay = (unsigned short)(nLined * 8);
                    row = 0;
                    while (col >= 5) {
                        col -= 5;
                        row++;
                    }
                    mBombs[i].lineSlot = (unsigned char)(row * 10 + col + 5);
                    nPlain++;
                    nLined++;
                }
            } else if (mBlastPen != 0) {
                mBombs[i].lineSlot = (unsigned char)nLined;
                mBombs[i].substate = 3;
                mBombs[i].marchDelay = (unsigned short)(nLined * 8);
                nLined++;
            } else {
                if (mWinColor == mBombs[i].color && mBombs[i].state == 6) {
                    mBombs[i].lineSlot = (unsigned char)nLined;
                    mBombs[i].substate = 3;
                    mBombs[i].marchDelay = (unsigned short)(nLined * 8);
                    nLined++;
                }
            }
        }
    }
    TopDoor_Open();
}

void dScMgBomroom_c::BombMarch_LinedUp(int index) {}

void dScMgBomroom_c::BombMarch_ToLine(int index) {
    if (mBombs[index].marchDelay != 0) {
        mBombs[index].marchDelay -= 1;
        if ((s16)mBombs[index].marchDelay < 0)
            mBombs[index].marchDelay = 0;
        return;
    }
    {
        int slot = mBombs[index].lineSlot;
        int row = 0;
        int curX, curY, targetY, targetX;
        while (slot >= 10) { slot -= 10; row++; }
        targetX = slot * 16 + 0x38;
        targetY = row * 20 - 0xC0;
        curX = mBombs[index].x >> 12;
        curY = mBombs[index].y >> 12;
        if (targetX == curX && targetY == curY) {
            mBombs[index].substate = 4;
            if (mBombs[index].color != 0)
                func_02012718(0x1d9, mBombs[index].x);
            else
                func_02012718(0x1da, mBombs[index].x);
            return;
        }
        if (targetY != curY) {
            mBombs[index].y += mBombs[index].speed;
            if ((mBombs[index].y >> 12) >= targetY)
                mBombs[index].y = targetY << 12;
            return;
        }
        if (targetX > curX) {
            mBombs[index].x += mBombs[index].speed;
            if (targetX <= (mBombs[index].x >> 12))
                mBombs[index].x = targetX << 12;
        } else if (targetX < curX) {
            mBombs[index].x -= mBombs[index].speed;
            if (targetX >= curX)
                mBombs[index].x = targetX << 12;
        }
    }
}

void dScMgBomroom_c::BombMarch_Offscreen(int index) {}

void dScMgBomroom_c::BombMarch_Exit(int index) {
    int color, x;
    {
        fx16 cos = FX_CosIdx(mBombs[index].angle >> 4);
        mBombs[index].x += FX_MUL(cos, mBombs[index].speed);
    }
    {
        fx16 sin = FX_SinIdx(mBombs[index].angle >> 4);
        mBombs[index].y += FX_MUL(sin, mBombs[index].speed);
    }
    color = mBombs[index].color;
    x = mBombs[index].x >> 12;
    if (color != 0) {
        if (x >= 0x110) {
            mBombs[index].substate = 2;
            mBombs[index].x = 0x80000;
            mBombs[index].y = -0xf0000;
        }
    } else {
        if (x <= -0x10) {
            mBombs[index].substate = 2;
            mBombs[index].x = 0x80000;
            mBombs[index].y = -0xf0000;
        }
    }
    PenGate_Open(mBombs[index].color);
}

void dScMgBomroom_c::BombMarch_ToGate(int index) {
    int color, x, z, tx, ty;

    tx = (mBombs[index].color != 0) ? 0x100 : 0;
    ty = 0x60;
    tx -= mBombs[index].x >> 12;
    ty -= mBombs[index].y >> 12;

    mBombs[index].angle = cstd::atan2(ty, tx);

    {
        fx16 cos = FX_CosIdx(mBombs[index].angle >> 4);
        mBombs[index].x += FX_MUL(cos, mBombs[index].speed);
    }
    {
        fx16 sin = FX_SinIdx(mBombs[index].angle >> 4);
        mBombs[index].y += FX_MUL(sin, mBombs[index].speed);
    }

    color = mBombs[index].color;
    x = mBombs[index].x >> 12;
    z = mBombs[index].y >> 12;
    tx = color ? 0x100 : 0;

    if (x <= tx + 2 && x >= tx - 2 && z <= 0x62 && z >= 0x5e) {
        mBombs[index].substate = 1;
        if (mBombs[index].color != 0)
            mBombs[index].angle = 0;
        else
            mBombs[index].angle = 0x8000;
    }

    PenGate_Open(mBombs[index].color);
}

/* Runs the bomb's current handler: its substate indexes the
 * pointer-to-member table sBomroomBombMarchTable. */

extern "C" PMF sBomroomBombMarchTable[];
void dScMgBomroom_c::Bomb_StateMarch(int index) {
    unsigned char state = mBombs[index].substate;
    (this->*sBomroomBombMarchTable[state])(index);
}

/* Roaming step: advance x/y along the angle at speed (sine table, fx32),
 * then bounce off the box selected by color (0xc0..0x100 x 0x40..0x80 for
 * color 1, 0..0x40 x 0x40..0x80 for color 0): a vertical wall mirrors the
 * angle (0x8000 - a), a horizontal one negates it, and the coordinate is
 * pinned just inside the wall. */

void dScMgBomroom_c::Bomb_StateInPen(int idx) {
    int xv;
    int yv;

    {
        fx16 cos = FX_CosIdx(mBombs[idx].angle >> 4);
        mBombs[idx].x += FX_MUL(cos, mBombs[idx].speed);
    }
    {
        fx16 sin = FX_SinIdx(mBombs[idx].angle >> 4);
        mBombs[idx].y += FX_MUL(sin, mBombs[idx].speed);
    }
    xv = mBombs[idx].x >> 12;
    yv = mBombs[idx].y >> 12;
    if (mBombs[idx].color != 0) {
        if (xv + 0xc > 0x100) {
            mBombs[idx].angle = 0x8000 - mBombs[idx].angle;
            mBombs[idx].x = 0xf4000;
        } else if (xv - 0xc < 0xc0) {
            mBombs[idx].angle = 0x8000 - mBombs[idx].angle;
            mBombs[idx].x = 0xcc000;
        }
        if (yv + 0xc > 0x80) {
            mBombs[idx].angle = -mBombs[idx].angle;
            mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            mBombs[idx].angle = -mBombs[idx].angle;
            mBombs[idx].y = 0x4c000;
        }
    } else {
        if (xv + 0xc > 0x40) {
            mBombs[idx].angle = 0x8000 - mBombs[idx].angle;
            mBombs[idx].x = 0x34000;
        } else if (xv - 0xc < 0) {
            mBombs[idx].angle = 0x8000 - mBombs[idx].angle;
            mBombs[idx].x = 0xc000;
        }
        if (yv + 0xc > 0x80) {
            mBombs[idx].angle = -mBombs[idx].angle;
            mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            mBombs[idx].angle = -mBombs[idx].angle;
            mBombs[idx].y = 0x4c000;
        }
    }
}

void dScMgBomroom_c::Bomb_StateExploding(int i) {
    if (mBombs[i].exploded == 0)
        return;
    if (mBombs[i].frame != 0) return;
    if (mBombs[i].counter != 1) return;
    Sound::PlayBank2_2D(0x1e0);
    mDispatchDelay = 96;
}

void dScMgBomroom_c::Bomb_StateChainDelay(int idx) {
    if (mBombs[idx].marchDelay != 0) {
        mBombs[idx].marchDelay--;
        if ((s16)mBombs[idx].marchDelay < 0)
            mBombs[idx].marchDelay = 0;
    } else {
        mBombs[idx].state = 4;
        mBombs[idx].type = 3;
        mBombs[idx].frame = 0;
        mBombs[idx].counter = 0;
    }
}

/* A held bomb follows the stylus less its grab offset, and overPen records
 * whether it is over its own pen. With nothing held (mHeldColor is 0xff) the
 * bomb goes back to state 1 instead. */

void dScMgBomroom_c::Bomb_StateHeld(int idx) {
    if (mHeldColor == 0xff) {
        mBombs[idx].state = 1;
        mBombs[idx].fuse += 0x40;
        Bomb_CheckSorted(idx);
        Bomb_CheckWrongPen(idx);
    } else {
        int old_x = mBombs[idx].x;
        int old_y = mBombs[idx].y;
        int cx, cy;
        u8 was;
        int t = gActivePlayerSlot;

        mBombs[idx].x = (gTouchInfo[t].x << 12) - mBombs[idx].grabX;
        mBombs[idx].y = (gTouchInfo[t].y << 12) - mBombs[idx].grabY;
        cx = old_x >> 12;
        cy = old_y >> 12;

        Bomb_PushApart(idx);

        {
            int t2 = gActivePlayerSlot;
            mBombs[idx].x = (gTouchInfo[t2].x << 12) - mBombs[idx].grabX;
            mBombs[idx].y = (gTouchInfo[t2].y << 12) - mBombs[idx].grabY;
        }

        was = mBombs[idx].overPen;
        if (mBombs[idx].color != 0) {
            if (cx > 0xc0 && cy > 0x40 && cy < 0x80) {
                mBombs[idx].overPen = 1;
            } else {
                mBombs[idx].overPen = 0;
            }
        } else {
            if (cx < 0x40 && cy > 0x40 && cy < 0x80) {
                mBombs[idx].overPen = 1;
            } else {
                mBombs[idx].overPen = 0;
            }
        }

        if (was == 0 && mBombs[idx].overPen != 0) {
            func_02012718(0x1e4, mBombs[idx].x);
        }

        mBombs[idx].dragSound = func_02012468(mBombs[idx].dragSound, 2, 0x1e5, 4, 0, 0,
                                              func_020126e8(mBombs[idx].x), 0);
        mBombs[idx].prevX = mBombs[idx].x;
        mBombs[idx].prevY = mBombs[idx].y;
    }
}

/* While the bomb's fuse runs it counts down (unless mFusesFrozen is
 * set) and the bomb moves along its angle; once it reaches 0 the bomb goes
 * to state 4 and OnBombExploded takes over. */

void dScMgBomroom_c::Bomb_StateWalk(int idx) {
    if (mBombs[idx].fuse != 0) {
        if (mFusesFrozen == 0) {
            mBombs[idx].fuse--;
        }
        if ((short)mBombs[idx].fuse < 0) {
            mBombs[idx].fuse = 0;
        }
        if (mBombs[idx].fuse <= 0x40) {
            mBombs[idx].type = 2;
        }
    } else {
        mBombs[idx].fuse = 0;
        mBombs[idx].state = 4;
        mBombs[idx].type = 3;
        mBombs[idx].frame = 0;
        mBombs[idx].counter = 0;
        mBombs[idx].pen = 0;
        mBombs[idx].exploded = 1;
        OnBombExploded(idx);
        return;
    }

    if (mFusesFrozen != 0) {
        return;
    }

    {
        fx16 cos = FX_CosIdx(mBombs[idx].angle >> 4);
        mBombs[idx].x += FX_MUL(cos, mBombs[idx].speed);
    }
    {
        fx16 sin = FX_SinIdx(mBombs[idx].angle >> 4);
        mBombs[idx].y += FX_MUL(sin, mBombs[idx].speed);
    }
    Bomb_PushApart(idx);
    Bomb_CheckGrab(idx);
    mBombs[idx].prevX = mBombs[idx].x;
    mBombs[idx].prevY = mBombs[idx].y;
}

void dScMgBomroom_c::Bomb_StateWaiting(int i) {
    if (mBombs[i].marchDelay != 0) {
        mBombs[i].marchDelay--;
    } else {
        mBombs[i].visible = 1;
        mBombs[i].type = 1;
        mBombs[i].state = 1;
    }
}

/* Per-frame bomb pass: with no touch, nothing is held (mHeldColor = 0xff);
 * then every live bomb runs its state handler from the pointer-to-member
 * table sBomroomBombStateTable, indexed by its state, and Bomb_Animate. */

extern PMF sBomroomBombStateTable[];
void dScMgBomroom_c::UpdateBombs() {
    if (gTouchInfo[gActivePlayerSlot].held == 0)
        mHeldColor = 0xff;
    int i;
    for (i = 0; i < 112; i++) {
        u8 flag;
        flag = mBombs[i].active;
        if (flag != 0) {
            (this->*sBomroomBombStateTable[mBombs[i].state])(i);
            Bomb_Animate(i);
        }
    }
}

/* Spawner: once mSpawnDelay runs out, picks a step from the spawn
 * count mSpawnCount, drops that many new bombs into free records with a
 * random color and angle, and reloads the delay from
 * sBomroomSpawnDelays. */
void dScMgBomroom_c::SpawnBombs() {
    if (mSpawnDelay != 0) {
        mSpawnDelay--;
        if ((s16)mSpawnDelay < 0)
            mSpawnDelay = 0;
        return;
    }

    int stage = 0;
    if (mSpawnCount >= 300)
        stage = 12;
    else if (mSpawnCount >= 198)
        stage = 11;
    else if (mSpawnCount >= 159)
        stage = 10;
    else if (mSpawnCount >= 132)
        stage = 9;
    else if (mSpawnCount >= 92)
        stage = 8;
    else if (mSpawnCount >= 57)
        stage = 7;
    else if (mSpawnCount >= 33)
        stage = 6;
    else if (mSpawnCount >= 27)
        stage = 5;
    else if (mSpawnCount >= 21)
        stage = 4;
    else if (mSpawnCount >= 15)
        stage = 3;
    else if (mSpawnCount >= 9)
        stage = 2;
    else if (mSpawnCount >= 3)
        stage = 1;

    int count = 1;
    int step = 0;
    if (stage >= 5)
        count = 2;
    if (stage == 9)
        count = 1;
    int hatch = 0;
    if (stage == 7) {
        count = 2;
        hatch = (mSpawnHatchFlip & 1) + 1;
        mSpawnHatchFlip ^= 1;
        step = 0x3000;
    }
    if (stage == 8) {
        int bit = mSpawnHatchFlip & 1;
        count = bit + 2;
        mSpawnHatchFlip ^= 1;
        hatch = bit + 1;
        if (count == 2)
            step = 0x3000;
        else
            step = 0x1800;
    }
    if (stage == 10) {
        count = 4;
        step = 0x3000;
    }
    if (stage == 11) {
        count = 3;
        hatch = (mSpawnHatchFlip & 1) + 1;
        mSpawnHatchFlip ^= 1;
        step = 0x1800;
    }
    if (stage >= 12) {
        count = 6;
        step = 0x1800;
    }

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < 112; j++) {
            if (mBombs[j].active == 0) {
                mBombs[j].active = 1;
                mBombs[j].state = 0;
                mBombs[j].substate = 0;
                mBombs[j].lineSlot = 0;
                mBombs[j].exploded = 0;
                mBombs[j].x = 0x80000;
                mBombs[j].marchDelay = 4;
                mBombs[j].fuse = 0x200;
                /* The color is BOMROOM_RAND(2) rolled into rnd first: the
                   ROM computes the color's address after the roll, and
                   the macro inside the store computes it before. */
                u32 rnd = RandomIntInternal(&data_0209d4b8.value);
                mBombs[j].color = (((rnd >> 16) & 0x7fff) * 2) >> 15;
                mBombs[j].speed = 0x999;
                mBombs[j].dragSound = 0;
                if (stage == 0) {
                    mBombs[j].color = 1;
                    HatchPanel_Activate(0);
                }
                if (stage <= 1) {
                    mBombs[j].angle = (BOMROOM_RAND(4) << 12) + 0x2000;
                    mBombs[j].y = 0;
                } else if (hatch != 0) {
                    if (hatch == 1) {
                        mBombs[j].angle = step * i + 0x2000;
                        mBombs[j].y = 0;
                        HatchPanel_Activate(0);
                    } else {
                        mBombs[j].angle = step * i + 0xa000;
                        mBombs[j].y = 0xb8000;
                        HatchPanel_Activate(1);
                    }
                } else if (mSpawnCount & 1) {
                    if (step != 0)
                        mBombs[j].angle = step * (i >> 1) + 0x2800;
                    else
                        mBombs[j].angle = (BOMROOM_RAND(4) << 12) + 0x2000;
                    mBombs[j].y = 0;
                    HatchPanel_Activate(0);
                } else {
                    if (step != 0)
                        mBombs[j].angle = step * (i >> 1) + 0xa800;
                    else
                        mBombs[j].angle = (BOMROOM_RAND(4) << 12) + 0xa000;
                    mBombs[j].y = 0xb8000;
                    HatchPanel_Activate(1);
                }
                mSpawnCount++;
                break;
            }
        }
    }
    mSpawnDelay = sBomroomSpawnDelays[stage];
}

void dScMgBomroom_c::HideExplodedBombs() {
    int i;
    for (i = 0; i < 112; i++) {
        if (mBombs[i].active != 0) {
            if (mBombs[i].state == 6)
                mBombs[i].visible = 0;
        }
    }
}

/* Zeroes all 0x70 mBombs records: 11 words, 4
 * halfwords and 10 bytes each. */
void dScMgBomroom_c::ClearBombs() {
    int i;
    for (i = 0; i < 112; i++) {
        mBombs[i].x = 0;
        mBombs[i].y = 0;
        mBombs[i].grabX = 0;
        mBombs[i].grabY = 0;
        mBombs[i].speed = 0;
        mBombs[i].unk_14 = 0;
        mBombs[i].unk_18 = 0;
        mBombs[i].prevX = 0;
        mBombs[i].prevY = 0;
        mBombs[i].sound = 0;
        mBombs[i].dragSound = 0;
        mBombs[i].angle = 0;
        mBombs[i].counter = 0;
        mBombs[i].marchDelay = 0;
        mBombs[i].fuse = 0;
        mBombs[i].type = 0;
        mBombs[i].frame = 0;
        mBombs[i].color = 0;
        mBombs[i].state = 0;
        mBombs[i].active = 0;
        mBombs[i].substate = 0;
        mBombs[i].visible = 0;
        mBombs[i].pen = 0;
        mBombs[i].lineSlot = 0;
        mBombs[i].exploded = 0;
    }
}

void dScMgBomroom_c::RoundEnd_Finish() {
    int j;

    UpdateBombs();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
        return;
    }

    for (j = 0; j < 112; j++) {
        if (mBombs[j].active == 2) {
            if (mBombs[j].state == 6
             && mBombs[j].substate == 4) {
                mBombs[j].active = 0;
                mBombs[j].visible = 0;
            }
        }
    }

    if (mBlastPen != 0) {
        mStateTimer = 96;
        mSceneState = 4;
        mSubState = 0;
        Mg_SubmitScore(mTallyCount + Mg_GetSubmittedScore());
    } else {
        Mg_SubmitScore(mTallyCount + Mg_GetSubmittedScore());
        mSceneState = 2;
        mSubState = 0;
        mBlastPen = 0;
        mFusesFrozen = 0;
    }
    mShowTally = 0;
    mTallyCount = 0;
    SetBg0Offset(0, 0);
}

void dScMgBomroom_c::RoundEnd_Tally() {
    int found;
    int j;

    UpdateBombs();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer > 0)
            return;
        mStateTimer = 0;
        mTallyDelay = 16;
        if (AnyBombMarching() != 0) {
            mBg2State = 1;
            Sound::PlayBank2_2D(0x1dd);
        }
        return;
    }

    if (mTallyDelay != 0) {
        (mTallyDelay)--;
        return;
    }

    found = 0;
    j = 0;
    do {
        if (mBombs[j].active == 1 && mBombs[j].state == 6 && mBombs[j].substate == 4)
            found = j + 1;
        j++;
    } while (j < 112);

    (mTallyTick)++;
    if (mTallyTick < 4)
        return;

    if (found != 0) {
        mBombs[found - 1].active = 2;
        mTallyTick = 0;
        (mTallyCount)++;
        Sound::PlayBank2_2D(0x1bc);
    } else {
        mTallyTick = 0;
        mStateTimer = 64;
        mSubState = 3;
    }
}

int dScMgBomroom_c::AnyBombMarching() {
    int i;
    i = 0;
    do {
        if (mBombs[i].active == 1 && mBombs[i].state == 6)
            return 1;
        i++;
    } while (i < 112);
    return 0;
}

void dScMgBomroom_c::RoundEnd_WaitLinedUp() {
    UpdateBombs();
    int c4 = 0;
    int c2 = 0;
    int i = 0;
    do {
        if (mBombs[i].active != 0 && mBombs[i].state == 6) {
            unsigned char v = mBombs[i].substate;
            if (v != 2) c2++;
            if (v != 4) c4++;
        }
        i++;
    } while (i < 112);
    if (c2 == 0 && mBg2State == 0) {
        CheckColorComplete();
    }
    if (c4 != 0) return;
    TopDoor_Close();
    mStateTimer = 16;
    mSubState = 2;
    mShowTally = 1;
}

void dScMgBomroom_c::RoundEnd_StartMarch() {
    int flag;
    int count;
    int i;

    UpdateBombs();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
        return;
    }

    if (mBlastPen != 0) {
        flag = 0;
        count = 0;
        i = 0;
        do {
            if (mBombs[i].active != 0 && mBombs[i].state != 6) {
                mBombs[i].state = 6;
                mBombs[i].substate = 0;
                mBombs[i].speed = 0x4000;
                if (mBombs[i].color != 0)
                    flag = 1;
                count++;
            }
            i++;
        } while (i < 112);
        if (count != 0) {
            Sound::PlayBank2_2D(0x1e6);
            if (mBlastPen == 1) {
                PenGate_Open(0);
                PenGate_Open(1);
            } else {
                PenGate_Open(flag);
            }
        }
    } else {
        i = 0;
        do {
            if (mBombs[i].active != 0 && mBombs[i].state == 5) {
                if (mWinColor == mBombs[i].color) {
                    mBombs[i].state = 6;
                    mBombs[i].substate = 0;
                    mBombs[i].speed = 0x4000;
                }
            }
            i++;
        } while (i < 112);
        Sound::PlayBank2_2D(0x1e6);
        PenGate_Open(mWinColor);
    }
    mSubState = 1;
}

void dScMgBomroom_c::Round_StateGameOver() {
    if (mStateTimer == 0)
        return;
    mStateTimer -= 1;
    if ((s16)mStateTimer > 0)
        return;
    mStateTimer = 0;
    Mg_SetSharedState(MG_STATE_RESULTS);
    mPromptEnabled = 0;
}

/* Runs the mSubState sub-state through the pointer-to-member table
 * sBomroomRoundEndTable, then the three per-frame passes. */

extern "C" PMFv sBomroomRoundEndTable[];
void dScMgBomroom_c::Round_StateEnd() {
    (this->*sBomroomRoundEndTable[mSubState])();
    HatchPanel_UpdateAll();
    PenGate_UpdateAll();
    TopDoor_Update();
}

void dScMgBomroom_c::Round_StatePlay() {
    HatchPanel_UpdateAll();
    SpawnBombs();
    UpdateBombs();
    CountSortedBombs();
}

void dScMgBomroom_c::Round_StateStart() {
    if (mPromptBlinkCount == 0) { mPromptEnabled = 1; mPromptBlinkCount = 1; mPromptBlinkTimer = 0; }
    mSceneState = 2;
}

void dScMgBomroom_c::Round_StateSetup() {
    HatchPanel_ShowAll();
    PenGate_ShowAll();
    TopDoor_Show();
    mSceneState = 1;
}

/* Resets the round: clears every bomb record and the round counters
 * from mSpawnCount to mSpawnHatchFlip, then runs the other reset helpers. */

void dScMgBomroom_c::ResetRound() {
    ClearBombs();
    mSpawnDelay = 0;
    unk_62e4 = 0;
    unk_62e6 = 0;
    mHeldColor = 0xff;
    unk_62f7 = 0;
    mTallyTick = 0;
    mBlastPen = 0;
    mSpawnCount = 0;
    mDispatchDelay = 0;
    mShowTally = 0;
    mFusesFrozen = 0;
    mSpawnHatchFlip = 0;
    Mg_SubmitScore(0);
    HatchPanel_HideAll();
    PenGate_HideAll();
    TopDoor_Hide();
    Marker_HideAll();
    ResetBg2Lift();
    SetRoundOver();
}

/* Slot 18 override of dScMgBase_c::StartRound(int); the signature
 * repeats the base declaration exactly, or mwcc appends a slot instead of
 * overriding. */
void dScMgBomroom_c::StartRound(int /* arg */) {
    ResetRound();
    mTallyCount = 0;
    mSceneState = 0;
    G2_SetBlendAlpha(1, 0x1c, 4, 3);
    SetBg0Offset(0, 0);
}

s32 dScMgBomroom_c::Render()
{
    Bomroom_DrawScore();
    PenGate_DrawAll();
    TopDoor_Draw();
    DrawRemaining();
    DrawBombs();
    HatchPanel_DrawAll();
    Marker_DrawAll();
    DrawRoundOver();
    return 1;
}

/* Waits out mDispatchDelay, then runs the round state mSceneState from
 * the pointer-to-member table sBomroomRoundStateTable and steps the sprite
 * records and the BG2 offset. */
extern "C" PMFv sBomroomRoundStateTable[];
s32 dScMgBomroom_c::Behavior()
{
    if (mDispatchDelay != 0) {
        mDispatchDelay--;
        if ((s16)mDispatchDelay <= 0)
            mDispatchDelay = 0;
    } else {
        (this->*sBomroomRoundStateTable[mSceneState])();
        Marker_UpdateAll();
        UpdateBg2Lift();
    }
    return 1;
}

/* Loads the main- and sub-screen backgrounds, palettes and object
 * graphics, sets the blend, then resets the round into state 1. */
s32 dScMgBomroom_c::InitResources() {
    char *b;
    /* The ROM spills the BG fill value through the stack (strh/ldrh at sp)
       before MIi_CpuClear16. A plain u16 compiles InitResources to 0x328
       against the ROM's 0x330; no non-volatile spelling is known (the same
       spill as Message.cpp's MIi_CpuClear32 fills). */
    volatile u16 fill;
    void *f;
    void *arc;

    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    G2_SetBG3Priority(2);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);

    f = LoadFile(ASSET_D_2D_MG_BG_BOMROOM_NCG_BIN);
    DecompressLZ16(f, (void *)((int)G2_GetBG3CharPtr() + 0x4000));
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_BOMROOM_NCL_BIN);
    GX_LoadBGPltt(f, 0x180, 0x80);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_BOMROOM_UP_NSC_BIN);
    GX_LoadBG3Scr(f, 0, 0x800);
    Deallocate(f);

    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    G2_SetBG2Priority(1);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                         GX_BG_CHARBASE_0x10000);

    f = LoadFile(ASSET_D_2D_ELEVATOR_NSC_BIN);
    b = (char *)G2_GetBG2ScrPtr();
    fill = 0xf23f;
    MI_CpuFill16(b, fill, 0x1000);
    GX_LoadBG2Scr(f, 0, 0x800);
    Deallocate(f);

    gMainScreenLayers |= DISPLAY_LAYER_BG0;
    G2_SetBG0Priority(0);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                     GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);

    f = LoadFile(ASSET_D_2D_BLUE_SHADOW_NSC_BIN);
    GX_LoadBG0Scr(f, 0, 0x800);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_LIGHT_UP_NSC_BIN);
    GX_LoadBG0Scr(f, 0x800, 0x800);
    Deallocate(f);

    G2_SetBlendAlpha(1, 0x1c, 4, 3);

    arc = LoadFile(ASSET_D_2D_MG_BOMROOM_NCG_BIN);
    f = LoadFile(ASSET_D_2D_MG_BOMROOM_NCL_BIN);
    DecompressLZ16(arc, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(f, 0, 0x100);

    gSubScreenLayers |= DISPLAY_LAYER_BG3;
    G2S_SetBG3Priority(1);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x10000);

    {
        void *f7 = LoadFile(ASSET_D_2D_MG_BG_BOMROOM_NCG_BIN);
        DecompressLZ16(f7, (char *)G2S_GetBG3CharPtr() + 0x4000);
        Deallocate(f7);

        f7 = LoadFile(ASSET_D_2D_MG_BG_BOMROOM_NCL_BIN);
        GXS_LoadBGPltt(f7, 0x180, 0x80);
        Deallocate(f7);

        f7 = LoadFile(ASSET_D_2D_MG_BG_BOMROOM_DOWN_NSC_BIN);
        GXS_LoadBG3Scr(f7, 0, 0x800);
        Deallocate(f7);

        DecompressLZ16(arc, G2S_GetOBJCharPtr());
        GXS_LoadOBJPltt(f, 0, 0x100);
        Deallocate(arc);
        Deallocate(f);
    }

    ResetRound();
    HatchPanel_ShowAll();
    PenGate_ShowAll();
    TopDoor_Show();
    mSceneState = 1;
    mTallyCount = 0;
    Mg_SetTopScreenYOffset(0x20);
    Mg_SubmitScore(0);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgBomroom_c through RTTI,
 * allocation size, vtable identity, and the MG_BOMROOM registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgSortOrSplode_Spawn. */
void *dScMgBomroom_c_classInit() {
    return new dScMgBomroom_c;
}

BaseSpawnInfo<void> g_profile_MG_BOMROOM = {dScMgBomroom_c_classInit, 0x172, 0x172};

/* The seven state tables. mwcc emits __sinit_d_s_mg_bomroom.cpp from these
 * definitions: each entry is one pointer-to-member, copied from the {fn, 0}
 * constant mwcc emits for each &dScMgBomroom_c:: initializer. Round and
 * round-end take no argument; the other five take the slot. */
extern "C" PMFv sBomroomRoundStateTable[5] = {
    &dScMgBomroom_c::Round_StateSetup,    &dScMgBomroom_c::Round_StateStart,
    &dScMgBomroom_c::Round_StatePlay,     &dScMgBomroom_c::Round_StateEnd,
    &dScMgBomroom_c::Round_StateGameOver,
};
extern "C" PMFv sBomroomRoundEndTable[4] = {
    &dScMgBomroom_c::RoundEnd_StartMarch,
    &dScMgBomroom_c::RoundEnd_WaitLinedUp,
    &dScMgBomroom_c::RoundEnd_Tally,
    &dScMgBomroom_c::RoundEnd_Finish,
};
PMF sBomroomBombStateTable[7] = {
    &dScMgBomroom_c::Bomb_StateWaiting,   &dScMgBomroom_c::Bomb_StateWalk,
    &dScMgBomroom_c::Bomb_StateHeld,      &dScMgBomroom_c::Bomb_StateChainDelay,
    &dScMgBomroom_c::Bomb_StateExploding, &dScMgBomroom_c::Bomb_StateInPen,
    &dScMgBomroom_c::Bomb_StateMarch,
};
extern "C" PMF sBomroomBombMarchTable[5] = {
    &dScMgBomroom_c::BombMarch_ToGate,    &dScMgBomroom_c::BombMarch_Exit,
    &dScMgBomroom_c::BombMarch_Offscreen, &dScMgBomroom_c::BombMarch_ToLine,
    &dScMgBomroom_c::BombMarch_LinedUp,
};
extern "C" PMF sBomroomHatchPanelStateTable[4] = {
    &dScMgBomroom_c::HatchPanel_StateIdle,
    &dScMgBomroom_c::HatchPanel_StateAnimate,
    &dScMgBomroom_c::HatchPanel_StateHeld,
    &dScMgBomroom_c::HatchPanel_StateRevert,
};
extern "C" PMF sBomroomPenGateStateTable[4] = {
    &dScMgBomroom_c::PenGate_StateShut,
    &dScMgBomroom_c::PenGate_StateOpening,
    &dScMgBomroom_c::PenGate_StateHold,
    &dScMgBomroom_c::PenGate_StateClosing,
};
extern "C" PMF sBomroomTopDoorStateTable[4] = {
    &dScMgBomroom_c::TopDoor_StateShut,
    &dScMgBomroom_c::TopDoor_StateOpening,
    &dScMgBomroom_c::TopDoor_StateOpen,
    &dScMgBomroom_c::TopDoor_StateClosing,
};

/* sCardSwapRequested at +4, data_ov006_02141770 at +8. */
int sCardSelectLocked[3];
