//cpp
/* Tamaire (scene 0x171, profile MG_TAMAIRE): dScMgPachinko2_c, the whole
 * class in one translation unit. 74 functions, .text 0x020ff420..0x0210428c:
 * the destructor pair, 67 members, StartRound, Render, Behavior,
 * InitResources and the factory.
 *
 * The destructor is declared first in the class header, so this TU owns the
 * key function and emits dScMgPachinko2_c's vtable and RTTI; the ROM's own
 * copies live in ov006 .data, and these are discarded at link time.
 *
 * Functions run in ROM order here, lowest address first, under `#pragma
 * defer_codegen off`. Do not reorder. Each optimisation pragma sits in its
 * own push/pop bracket around the one member that needs it.
 *
 * The scene's three PMF state tables dispatch on this class: cups at
 * sPachinko2CupStateTable (5 states on the two 0x20 records at 0x5620), balls at
 * sPachinko2BallStateTable (8 states on mBalls[]), paddles at
 * sPachinko2PaddleStateTable (15 states on the three 0x40 records at 0x5260). The
 * descriptor records they are built from live at data_ov006_0213da*; the
 * runtime copies are made by __sinit_ov006_02131cd0.
 *
 * leftovers:
 * - PlotSubBg2Pixels keeps an unused receiver: every call passes the
 *   scene in r0 and the body only uses (a1, a2, a3). Memberized with the
 *   others for the uniform call shape.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgPachinko2_c_AssetId {
    ASSET_D_2D_HEI_TOP_DOWN_NSC_BIN = 0x0005,
    ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCG_BIN = 0x005c,
    ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCL_BIN = 0x005d,
    ASSET_D_2D_MG_BG_PACHINKO_DOWN_NSC_BIN = 0x005e,
    ASSET_D_2D_MG_BG_PACHINKO_UP_NCG_BIN = 0x005f,
    ASSET_D_2D_MG_BG_PACHINKO_UP_NCL_BIN = 0x0060,
    ASSET_D_2D_MG_BG_PACHINKO_UP_NSC_BIN = 0x0061,
    ASSET_D_2D_MG_PACHINKO_NCG_BIN = 0x00e0,
    ASSET_D_2D_MG_PACHINKO_NCL_BIN = 0x00e1
};

#pragma defer_codegen off

#include "types.h"
#include "OamAttr.h"
#include "nitro/hw/registers.h"
#include "cstd.h"
#include "dScMgPachinko2_c.h"
#include "SpawnInfo.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
#include "nitro/mi/memory.h"

namespace Sound {
void PlayBank2_2D(unsigned int id);
}

/* The pointer-to-member tables dispatch on this class. */
typedef void (dScMgPachinko2_c::*PMF)(int);

struct Entry {
    PMF pmf;
};

/* A random integer in [0, n): the top 15 bits of the shared generator,
   scaled. The same macro as d_s_mg_pachinko.cpp's. */
#define PACHINKO_RAND(n) (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * (n)) >> 15)

extern "C" {

/* --- shared ov004, main and library helpers --- */
extern void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern int Mg_RenderSpriteBothScaled(OamAttr *a, int b, int c, int d, int e, int f, int g);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
extern int Mg_GetSubmittedScore(void);
extern int Mg_IsNewRecord(void);
extern int Mg_GetTopRecord(void);
extern void Mg_DrawTopLeftCounter(int score);
extern void Mg_DrawTopRightCounter(int, int);
extern void Mg_DrawTopCenterCounter(int a0);
extern void Mg_SubmitScore(int self);
extern void Mg_SetTopScreenYOffset(int);
extern void Mg_SetSharedState(int);
extern void FreeGfxSlotsById(int n);
extern void func_02012718(int, int);
extern void *LoadFile(int handle);
extern void Deallocate(void *);
extern void DecompressLZ16(void *src, void *dst);
/* local extern: nitro/gx/gx_load.h leaves GX_LoadBG3Scr out until
 * decl_common.h's non-const spelling of it goes. */
extern void GX_LoadBG3Scr(void *, u32, u32);

/* --- ROM data this TU reads --- */
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern int sPachinkoPipeSprites[];
/* Sprite lists: each run ends at the entry whose attr3 is 0xffff. */
OamAttr data_ov006_02136c7c[11] = {
    {0x00f0, 0x41e9, 0x501a, 0x0000}, {0x00f4, 0x41f4, 0x501c, 0x0000},
    {0x80f4, 0x0004, 0x501e, 0x0000}, {0x4004, 0x01f4, 0x505c, 0x0000},
    {0x0004, 0x0004, 0x505e, 0x0000}, {0x00e9, 0x41fc, 0x5018, 0x0000},
    {0x00ef, 0x4001, 0x501c, 0x0000}, {0x80ef, 0x0011, 0x501e, 0x0000},
    {0x40ff, 0x0001, 0x505c, 0x0000}, {0x00ff, 0x0011, 0x505e, 0x0000},
    {0x00ed, 0x41f2, 0x501a, 0xffff},
};
OamAttr data_ov006_02136c34[9] = {
    {0x00f3, 0x41f8, 0x501a, 0x0000}, {0x00ea, 0x41e8, 0x5018, 0x0000},
    {0x00e9, 0x0001, 0x5058, 0x0000}, {0x00ed, 0x4007, 0x501a, 0x0000},
    {0x00e4, 0x61f3, 0x5018, 0x0000}, {0x00e7, 0x41f1, 0x501c, 0x0000},
    {0x80e7, 0x0001, 0x501e, 0x0000}, {0x40f7, 0x01f1, 0x505c, 0x0000},
    {0x00f7, 0x0001, 0x505e, 0xffff},
};
OamAttr data_ov006_02136be4[5] = {
    {0x00ee, 0x41f9, 0x5018, 0x0000}, {0x00e6, 0x01ea, 0x5058, 0x0000},
    {0x00e1, 0x0002, 0x5078, 0x0000}, {0x00e5, 0x4009, 0x5018, 0x0000},
    {0x00e0, 0x31f6, 0x5078, 0xffff},
};
OamAttr data_ov006_02136c0c[5] = {
    {0x00f1, 0x01fe, 0x5058, 0x0000}, {0x00e5, 0x01eb, 0x5078, 0x0000},
    {0x00e1, 0x0004, 0x5059, 0x0000}, {0x00ea, 0x000c, 0x5078, 0x0000},
    {0x00df, 0x01f6, 0x5059, 0xffff},
};
OamAttr *sPachinko2SparkSprites[] = {data_ov006_02136c7c, data_ov006_02136c34, data_ov006_02136be4,
                                     data_ov006_02136c0c};
OamAttr data_ov006_02133870[4] = {
    {0x0000, 0x8008, 0x1090, 0x0000},
    {0x4001, 0x91d8, 0x1090, 0x0000},
    {0x4011, 0x81d8, 0x10d0, 0x0000},
    {0x40ee, 0xc1e0, 0x3098, 0xffff},
};
OamAttr *sPachinko2PaddleSprite = data_ov006_02133870;
OamAttr data_ov006_021389f0 = {0x00f0, 0x81f0, 0x7158, 0xffff};
OamAttr *sPachinko2BallSprite = &data_ov006_021389f0;
OamAttr data_ov006_02138cf0[3] = {
    {0x40f0, 0x81f0, 0x716c, 0x0000},
    {0x4000, 0x41f0, 0x71ac, 0x0000},
    {0x4000, 0x81f0, 0x61cc, 0xffff},
};
OamAttr data_ov006_02138cd8[3] = {
    {0x40f1, 0x81f0, 0x7174, 0x0000},
    {0x4001, 0x41f0, 0x71b4, 0x0000},
    {0x4000, 0x81f0, 0x61d0, 0xffff},
};
OamAttr data_ov006_02138cc0[3] = {
    {0x40f0, 0x81f0, 0x7170, 0x0000},
    {0x4000, 0x41f0, 0x71b0, 0x0000},
    {0x40fe, 0x81f0, 0x61d4, 0xffff},
};
OamAttr data_ov006_02138ca8[3] = {
    {0x40f2, 0x81f0, 0x7174, 0x0000},
    {0x4002, 0x41f0, 0x71b4, 0x0000},
    {0x4000, 0x81f0, 0x61d8, 0xffff},
};
OamAttr data_ov006_02138c90[3] = {
    {0x40f0, 0x91f0, 0x716c, 0x0000},
    {0x4000, 0x51f0, 0x71ac, 0x0000},
    {0x4000, 0x91f0, 0x61cc, 0xffff},
};
OamAttr data_ov006_02138c78[3] = {
    {0x40f1, 0x91f0, 0x7174, 0x0000},
    {0x4001, 0x51f0, 0x71b4, 0x0000},
    {0x4000, 0x91f0, 0x61d0, 0xffff},
};
OamAttr data_ov006_02138c60[3] = {
    {0x40f0, 0x91f0, 0x7170, 0x0000},
    {0x4000, 0x51f0, 0x71b0, 0x0000},
    {0x40fe, 0x91f0, 0x61d4, 0xffff},
};
OamAttr data_ov006_02138c48[3] = {
    {0x40f2, 0x91f0, 0x7174, 0x0000},
    {0x4002, 0x51f0, 0x71b4, 0x0000},
    {0x4000, 0x91f0, 0x61d8, 0xffff},
};
OamAttr *sPachinko2BallCountSprites[] = {
    data_ov006_02138cf0, data_ov006_02138cd8, data_ov006_02138cc0, data_ov006_02138ca8,
    data_ov006_02138c90, data_ov006_02138c78, data_ov006_02138c60, data_ov006_02138c48,
};
extern const u8 sPachinko2CupHopRightStartFrames[4] = {6, 5, 4, 0};
extern const u8 sPachinko2CupHopLeftStartFrames[4] = {4, 5, 6, 0};
extern const unsigned char sPachinko2BallWalkOffSprites[4] = {1, 2, 3, 4};
extern const unsigned char sPachinko2PaddleCountPerLanes[8] = {2, 2, 2, 2, 2, 2, 0, 0};
extern const u8 sPachinko2CupHopLeftFrames[8] = {0, 1, 2, 3, 2, 1, 0, 0};
extern const u8 sPachinko2CupHopRightFrames[8] = {7, 8, 9, 10, 9, 8, 0, 0};
extern const unsigned char sPachinko2CupWaitFrames[8] = {11, 12, 13, 14, 15, 14, 13, 12};
extern const unsigned char sPachinko2CupWaitTimes[8] = {4, 2, 3, 2, 4, 2, 3, 2};
extern const int sPachinko2CupX[2] = {0x40, 0xc0};
extern const int sPachinko2CrossRowY[2] = {0xffffff98, 0xffffff68};
extern const int sPachinko2PaddleFlyAwayVX[3] = {0x400, 0xfffffc00, 0x0};
/* Per-cup wait-animation frame runs, read through
 * sPachinko2CupWaitFramesPerCup[cup][animStep]. */
extern const unsigned short data_ov006_0212ec8c[8] = {16, 19, 20, 19, 20, 19, 16, 0};
extern const unsigned short data_ov006_0212ec9c[8] = {16, 17, 18, 17, 18, 17, 16, 0};
extern const unsigned short sPachinko2CupWaitTimesAlt[8] = {5, 5, 80, 8, 80, 5, 75, 0};
extern const s32 sPachinko2CupRightStops[6] = {0x60, 0xe0, 0x58, 0xd8, 0x40, 0xc0};
extern const s32 sPachinko2CupLeftStops[6] = {0x20, 0xa0, 0x28, 0xa8, 0x40, 0xc0};
/* Per-lane paddle limits, indexed [lane][paddle]: descend stops the paddle at
 * a screen Y, sweep caps |vx|, gather is the Y it steers back to. */
extern const int data_ov006_0212ebd0[2] = {0xffffff98, 0xffffff68};
extern const int data_ov006_0212ebe8[2] = {0x00000c00, 0x00000c00};
extern const int data_ov006_0212ebf0[2] = {0x00002000, 0x00000800};
extern const int data_ov006_0212ebf8[2] = {0xffffffb0, 0xffffff70};
extern const int data_ov006_0212ec00[2] = {0xffffff98, 0xffffff68};
extern const int data_ov006_0212ec10[2] = {0xffffffb0, 0xffffff70};
extern const int data_ov006_0212ec18[2] = {0xffffffb0, 0xffffff70};
extern const int data_ov006_0212ec20[2] = {0xffffffb0, 0xffffff70};
extern const int data_ov006_0212ec28[2] = {0xffffffb0, 0xffffff70};
extern const int data_ov006_0212ec38[2] = {0xffffffb0, 0xffffff70};
extern const int data_ov006_0212ec40[2] = {0x00000c00, 0x00000c00};
extern const int data_ov006_0212ec48[2] = {0x00000c00, 0x00000c00};
extern const int data_ov006_0212ec50[2] = {0x00000c00, 0x00000c00};
extern const int data_ov006_0212ec58[2] = {0x00000c00, 0x00000c00};
extern const int data_ov006_0212ec60[2] = {0xffffff80, 0xffffff80};
extern const int data_ov006_0212ec68[2] = {0xffffffb8, 0xffffff58};
extern const int data_ov006_0212ec70[2] = {0xffffff80, 0xffffff80};
extern const int data_ov006_0212ec78[2] = {0xffffffb8, 0xffffff58};
const int *sPachinko2PaddleDescendLimits[6] = {
    data_ov006_0212ec78, data_ov006_0212ec70, data_ov006_0212ec68,
    data_ov006_0212ec60, data_ov006_0212ec00, data_ov006_0212ebd0,
};
const int *sPachinko2PaddleSweepLimits[6] = {
    data_ov006_0212ec58, data_ov006_0212ec50, data_ov006_0212ebf0,
    data_ov006_0212ec48, data_ov006_0212ec40, data_ov006_0212ebe8,
};
const unsigned short *sPachinko2CupWaitFramesPerCup[2] = {
    data_ov006_0212ec8c,
    data_ov006_0212ec9c,
};
/* Leftover: the gather-target pointer table at 0x0213db9c sits in the module
 * .data tail, which is a single shared plain .data section; a named claim
 * cannot be carved inside it, and its symbol extent runs into the class
 * vtable storage. The gather pairs above are indexed by that cartridge table
 * in the order ec28, ec20, ec18, ebf8, ec10, ec38. */
extern int *sPachinko2PaddleGatherTargets[];

/* The three pointer-to-member tables the helpers dispatch through. */
extern Entry sPachinko2CupStateTable[];
extern Entry sPachinko2BallStateTable[];
extern PMF sPachinko2PaddleStateTable[];

} /* extern "C" */
extern int Sound_PlayIfNotActive(int a, int b, int c, short d);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

/* Both variants come from this one definition: D1 stores this class's
   vtable and calls dScMgBase_c's D2; D0 adds dScMgBase_c::operator delete. */
dScMgPachinko2_c::~dScMgPachinko2_c() {}

void dScMgPachinko2_c::CupSlot_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mCups[i].visible) {
            Mg_RenderSpriteBoth((void *)sPachinkoPipeSprites[mCups[i].frame], mCups[i].x >> 12,
                                mCups[i].y >> 12, -1, 2);
        }
    }
}

/* Arm both cups and reset their movement history. */
void dScMgPachinko2_c::CupSlot_ResetAll() {
    int i;

    for (i = 0; i < 2; i++) {
        mCups[i].armed = 1;
        mCups[i].lastMove = 0xff;
        mCups[i].hops = 0;
        mCups[i].repeats = 0;
    }
}

void dScMgPachinko2_c::Cup_PickTarget(int k) {
    int found = 0;
    int i;
    for (i = 0; i < 48; i++) {
        int dx, dy;
        if (mBalls[i].active == 0)
            continue;
        if (mBalls[i].state < 3)
            continue;
        dx = (mBalls[i].x - mCups[k].x) >> 12;
        dy = (mBalls[i].y - mCups[k].y) >> 12;
        if (dx < -0x18)
            continue;
        if (dx > 0x18)
            continue;
        if (dy < 0)
            continue;
        if (dy <= 0x40) {
            found++;
            break;
        }
    }
    if (found == 0)
        return;
    if (PACHINKO_RAND(2)) {
        mCups[k].state = 3;
        mCups[k].phase = 0;
        mCups[k].vx = 0x1800;
        mCups[k].frame = 0x10;
        mCups[k].animStep = 0;
        mCups[k].timer = 0;
        mCups[k].dir = 1;
    } else {
        mCups[k].state = 2;
        mCups[k].phase = 0;
        mCups[k].vx = -0x1800;
        mCups[k].frame = 0x10;
        mCups[k].animStep = 0;
        mCups[k].timer = 0;
        mCups[k].dir = 0;
    }
}

void dScMgPachinko2_c::Cup_StateDecide(int n) {
    int moveSideways;
    moveSideways = 0;
    if (PACHINKO_RAND(2)) {
        moveSideways = 1;
        if (PACHINKO_RAND(4)) {
            mCups[n].state = 3;
            mCups[n].vx = 0x1800;
        } else {
            mCups[n].state = 2;
            mCups[n].vx = -0x1800;
        }
        mCups[n].hops = PACHINKO_RAND(6) + 1;
    } else {
        mCups[n].state = 1;
        mCups[n].hops = PACHINKO_RAND(2);
    }

    {
        unsigned int lastMove = mCups[n].lastMove;
        if (lastMove != 0xff) {
            if (moveSideways == lastMove) {
                mCups[n].repeats += 1;
                if (mCups[n].repeats >= 2) {
                    if (moveSideways != 0) {
                        mCups[n].state = 1;
                        mCups[n].hops = PACHINKO_RAND(2);
                        moveSideways = 0;
                    } else {
                        if (PACHINKO_RAND(2)) {
                            mCups[n].state = 3;
                            mCups[n].vx = 0x1800;
                        } else {
                            mCups[n].state = 2;
                            mCups[n].vx = -0x1800;
                        }
                        mCups[n].hops = PACHINKO_RAND(6) + 1;
                        moveSideways = 1;
                    }
                }
            } else {
                mCups[n].repeats = 0;
            }
        }
    }

    mCups[n].lastMove = moveSideways;
    mCups[n].phase = 0;
    mCups[n].frame = 0x10;
    mCups[n].animStep = 0;
    mCups[n].timer = 0;
    mCups[n].dir = 1;
}

void dScMgPachinko2_c::Cup_StateHopRight(int i) {
    int screenX;

    if (mCups[i].phase == 1) {
        mCups[i].timer++;
        if (mCups[i].timer < 4)
            return;
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 3) {
            mCups[i].animStep = 0;
            mCups[i].state = 2;
            if (mCups[i].armed != 0) {
                if (mCups[i].hops != 0) {
                    mCups[i].phase = 0;
                    mCups[i].hops--;
                    return;
                }
            }
            mCups[i].phase = 2;
            return;
        }
        mCups[i].frame = sPachinko2CupHopRightStartFrames[mCups[i].animStep];
        return;
    }

    mCups[i].timer++;
    if (mCups[i].timer >= 4) {
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 6)
            mCups[i].animStep = 0;
        mCups[i].frame = sPachinko2CupHopRightFrames[mCups[i].animStep];
    }

    mCups[i].x = mCups[i].x + mCups[i].vx;
    screenX = mCups[i].x >> 0xc;

    if (mCups[i].phase == 2) {
        if (screenX >= sPachinko2CupRightStops[i + 4]) {
            mCups[i].x = sPachinko2CupRightStops[i + 4] << 0xc;
            mCups[i].vx = 0;
            mCups[i].phase = 0;
            mCups[i].state = 0;
            return;
        }
    }

    if (screenX >= sPachinko2CupRightStops[i + 2]) {
        if (mCups[i].vx >= 0x800) {
            mCups[i].vx = mCups[i].vx - 0x80;
        }
    } else {
        mCups[i].vx = mCups[i].vx + 0x80;
    }

    if (screenX < sPachinko2CupRightStops[i])
        return;

    mCups[i].x = sPachinko2CupRightStops[i] << 0xc;
    mCups[i].vx = -0x1000;
    mCups[i].phase++;
    mCups[i].timer = 0;
    mCups[i].animStep = 0;
    mCups[i].frame = sPachinko2CupHopRightStartFrames[0];
}

void dScMgPachinko2_c::Cup_StateHopLeft(int i) {
    int screenX;

    if (mCups[i].phase == 1) {
        mCups[i].timer++;
        if (mCups[i].timer < 4)
            return;
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 3) {
            mCups[i].animStep = 0;
            mCups[i].state = 3;
            if (mCups[i].armed != 0) {
                if (mCups[i].hops != 0) {
                    mCups[i].phase = 0;
                    mCups[i].hops--;
                    return;
                }
            }
            mCups[i].phase = 2;
            return;
        }
        mCups[i].frame = sPachinko2CupHopLeftStartFrames[mCups[i].animStep];
        return;
    }

    mCups[i].timer++;
    if (mCups[i].timer >= 4) {
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 6)
            mCups[i].animStep = 0;
        mCups[i].frame = sPachinko2CupHopLeftFrames[mCups[i].animStep];
    }

    mCups[i].x = mCups[i].x + mCups[i].vx;
    screenX = mCups[i].x >> 0xc;

    if (mCups[i].phase == 2) {
        if (screenX <= sPachinko2CupLeftStops[i + 4]) {
            mCups[i].x = sPachinko2CupLeftStops[i + 4] << 0xc;
            mCups[i].vx = 0;
            mCups[i].phase = 0;
            mCups[i].state = 0;
            return;
        }
    }

    if (screenX <= sPachinko2CupLeftStops[i + 2]) {
        if (mCups[i].vx <= -0x800) {
            mCups[i].vx = mCups[i].vx + 0x80;
        }
    } else {
        mCups[i].vx = mCups[i].vx - 0x80;
    }

    if (screenX > sPachinko2CupLeftStops[i])
        return;

    mCups[i].x = sPachinko2CupLeftStops[i] << 0xc;
    mCups[i].vx = 0x1000;
    mCups[i].phase++;
    mCups[i].timer = 0;
    mCups[i].animStep = 0;
    mCups[i].frame = sPachinko2CupHopLeftStartFrames[0];
}

void dScMgPachinko2_c::Cup_StateWait(int k) {
    if (mCups[k].armed != 0) {
        mCups[k].timer++;
        if (mCups[k].timer >= sPachinko2CupWaitTimes[mCups[k].animStep]) {
            mCups[k].timer = 0;
            mCups[k].animStep++;
            if (mCups[k].animStep >= 8) {
                mCups[k].animStep = 0;
                if (mCups[k].hops == 0) {
                    mCups[k].state = 4;
                    mCups[k].frame = 0x10;
                    return;
                }
                mCups[k].hops--;
            }
        }
        mCups[k].frame = sPachinko2CupWaitFrames[mCups[k].animStep];
    } else {
        mCups[k].timer++;
        if (mCups[k].timer >= sPachinko2CupWaitTimesAlt[mCups[k].animStep]) {
            mCups[k].timer = 0;
            mCups[k].animStep++;
            if (mCups[k].animStep >= 7) {
                mCups[k].animStep = 0;
            }
            mCups[k].frame = sPachinko2CupWaitFramesPerCup[k][mCups[k].animStep];
        }
        Cup_PickTarget(k);
    }
}

void dScMgPachinko2_c::Cup_StateInit(int i) {
    mCups[i].frame = 0x10;
    mCups[i].animStep = 0;
    mCups[i].timer = 0;
    mCups[i].visible = 1;
    mCups[i].state = 1;
}

void dScMgPachinko2_c::Cup_DrawAll() {
    for (int i = 0; i < 2; i++) {
        if (mCups[i].active != 0)
            (this->*sPachinko2CupStateTable[mCups[i].state].pmf)(i);
    }
}

void dScMgPachinko2_c::Cup_ShowAll() {
    int i;
    for (i = 0; i < 2; i++) {
        mCups[i].active = 1;
        mCups[i].visible = 0;
        mCups[i].state = 0;
        mCups[i].phase = 0;
        mCups[i].frame = 0;
        mCups[i].animStep = 0;
        mCups[i].dir = 0;
        mCups[i].x = sPachinko2CupX[i] << 12;
        mCups[i].y = 0x68000;
    }
}

void dScMgPachinko2_c::Cup_HideAll() {
    for (int i = 0; i < 2; i++) {
        mCups[i].active = 0;
        mCups[i].visible = 0;
        mCups[i].armed = 0;
    }
}

void dScMgPachinko2_c::ChooseLaneConfig() {
    unsigned int nextConfig, configOffset;
    if (mLevel != 0) {
        nextConfig = PACHINKO_RAND(6);
        if ((unsigned int)mLaneConfig == nextConfig) {
            configOffset = PACHINKO_RAND(5) + 1;
            nextConfig = nextConfig + configOffset;
            if ((int)nextConfig >= 6)
                nextConfig -= 6;
        }
        mLaneConfig = nextConfig;
        return;
    }
    mLaneConfig = PACHINKO_RAND(6);
}

void dScMgPachinko2_c::Popup_DrawAll() {
    for (int i = 0; i < 16; i++) {
        if (mScorePops[i].visible != 0) {
            Mg_DrawBigNumber(mScorePops[i].x >> 12, mScorePops[i].y >> 12, mScorePops[i].score, -1,
                             -1, 0, 0);
        }
    }
}

void dScMgPachinko2_c::Popup_UpdateAll() {
    for (int i = 0; i < 16; i++) {
        if (mScorePops[i].active != 0) {
            if (mScorePops[i].settled == 0) {
                mScorePops[i].y += mScorePops[i].vy;
                mScorePops[i].vy -= 0x200;
                if (mScorePops[i].life == 0x30)
                    mScorePops[i].settled += 1;
            }
            if (mScorePops[i].life != 0) {
                mScorePops[i].life -= 1;
            } else {
                mScorePops[i].visible = 0;
                mScorePops[i].active = 0;
                mScorePops[i].life = 0;
            }
        }
    }
}

void dScMgPachinko2_c::Popup_Spawn(int ballX, int ballY, int value) {
    for (int i = 0; i < 16; i++) {
        if (mScorePops[i].active == 0) {
            mScorePops[i].active = 1;
            mScorePops[i].visible = 1;
            if (ballX >= 0x80000)
                mScorePops[i].x = 0xa4000;
            else
                mScorePops[i].x = 0x5c000;
            mScorePops[i].y = ballY + 0x48000;
            mScorePops[i].unk_0c = 0;
            mScorePops[i].vy = 0;
            mScorePops[i].score = value;
            mScorePops[i].settled = 0;
            mScorePops[i].life = 0x40;
            return;
        }
    }
}

void dScMgPachinko2_c::Spark_DrawAll() {
    for (int i = 0; i < 16; i++) {
        if (mHitFx[i].visible != 0) {
            Mg_RenderSpriteBoth(sPachinko2SparkSprites[mHitFx[i].frame], mHitFx[i].x >> 12,
                                mHitFx[i].y >> 12, -1, -1);
        }
    }
}

/* Hold the first frame until timer reaches eight, then advance each
 * update. The timer is not reset between frames. */
void dScMgPachinko2_c::Spark_UpdateAll() {
    for (int i = 0; i < 16; i++) {
        if (mHitFx[i].active == 0)
            continue;
        mHitFx[i].timer++;
        if (mHitFx[i].timer < 8)
            continue;
        mHitFx[i].frame++;
        if (mHitFx[i].frame >= 4) {
            mHitFx[i].active = 0;
            mHitFx[i].frame = 0;
            mHitFx[i].visible = 0;
        }
    }
}

void dScMgPachinko2_c::Spark_Spawn(int x, int y) {
    int i = 0;
    do {
        if (mHitFx[i].active == 0) {
            mHitFx[i].x = x;
            mHitFx[i].y = y;
            mHitFx[i].active = 1;
            mHitFx[i].timer = 0;
            mHitFx[i].vx = 0;
            mHitFx[i].vy = 0;
            mHitFx[i].visible = 1;
            mHitFx[i].frame = 0;
            return;
        }
        i++;
    } while (i < 16);
}

void dScMgPachinko2_c::DrawScore() {
    if (mPhase < 2)
        return;
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
}

void dScMgPachinko2_c::DrawTopRecord() {
    if (mPhase < 2)
        return;
    Mg_DrawTopLeftCounter(Mg_GetTopRecord());
}

void dScMgPachinko2_c::StartRoundEnd(int a) {
    char *dst;
    /* volatile: the ROM spills the fill value to the stack and reloads it;
       a literal 0 or a plain local is 8 bytes shorter (known unsolved). */
    volatile unsigned short v;
    mPhase = 3;
    mEndTimer = 64;
    dst = (char *)G2S_GetBG2CharPtr();
    v = 0;
    MI_CpuFill16(dst, v, 0x6000);
    mResult = a;
    Ball_ClearAll();
}

void dScMgPachinko2_c::CheckRoundOver() {
    int count;
    int i;

    if (mPaddles[0].ballsHeld && mPaddles[0].active)
        return;
    if (mPaddles[1].ballsHeld && mPaddles[1].active)
        return;
    if (mPaddles[2].ballsHeld && mPaddles[2].active)
        return;
    if (mPaddles[0].state != 0xd && mPaddles[0].active)
        return;
    if (mPaddles[1].state != 0xd && mPaddles[1].active)
        return;
    if (mPaddles[2].state != 0xd && mPaddles[2].active)
        return;

    count = 0;
    for (i = 0; i < 48; i++) {
        if (mBalls[i].active) {
            count++;
            break;
        }
    }
    if (count)
        return;

    if (Mg_IsNewRecord())
        StartRoundEnd(1);
    else
        StartRoundEnd(0);
    Paddle_HideAll();
}

void dScMgPachinko2_c::Paddle_DrawAll() {
    int i;
    for (i = 0; i < 3; i++) {
        if (mPaddles[i].visible) {
            Mg_RenderSpriteBoth(sPachinko2PaddleSprite, mPaddles[i].x >> 12, mPaddles[i].y >> 12,
                                -1, -1);
        }
    }
}

void dScMgPachinko2_c::Paddle_HideAll() {
    for (int i = 0; i < 3; i++) {
        if (mPaddles[i].active != 0) {
            mPaddles[i].state = 0xe;
            mPaddles[i].timer = 136;
        }
    }
}

void dScMgPachinko2_c::Paddle_Stop(int idx) {
    if (mPaddles[idx].active == 0)
        return;
    mPaddles[idx].angle = 0;
    mPaddles[idx].speed = 0;
    mPaddles[idx].timer = 64;
    mPaddles[idx].state = 0xc;
}

void dScMgPachinko2_c::Paddle_CheckBallHits(int idx) {
    if (mPaddles[idx].state == 0)
        return;
    if (mPaddles[idx].state >= 0xc)
        return;

    for (int i = 0; i < 48; i++) {
        if (mBalls[i].active != 0) {
            if (mBalls[i].state >= 2) {
                if (mBalls[i].vy > 0x2000) {
                    int dx = (mPaddles[idx].x - mBalls[i].x) >> 12;
                    int dy = (mPaddles[idx].y - mBalls[i].y) >> 12;
                    if (dx >= -0x30 && dx <= 0x30 && dy >= 0 && dy <= 8) {
                        /* Catch the center; the next two bands bounce the ball. */
                        if (dx >= -0x10 && dx <= 0x10) {
                            mPaddles[idx].ballsHeld++;
                            mBalls[i].visible = 0;
                            mBalls[i].active = 0;
                            Spark_Spawn(mBalls[i].x, mBalls[i].y);
                            func_02012718(0x19a, mBalls[i].x);
                            Paddle_Hit(idx);
                        } else if (dx >= -0x20 && dx <= 0x20) {
                            if (dx < 0)
                                mBalls[i].vx = mPaddles[idx].vx - 0x1800;
                            else
                                mBalls[i].vx = mPaddles[idx].vx + 0x1800;
                            mBalls[i].vy = -0x2000;
                            Sound::PlayBank2_2D(0x19b);
                        } else {
                            if (dx < 0)
                                mBalls[i].vx = 0x1800;
                            else
                                mBalls[i].vx = -0x1800;
                            mBalls[i].vy = -0x2800;
                            Sound::PlayBank2_2D(0x19b);
                        }
                    }
                }
            }
        }
    }
}

void dScMgPachinko2_c::Paddle_StateFlyAway(int i) {
    int screenY;
    u8 dir;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        if ((short)mPaddles[i].timer > 0)
            return;
        mPaddles[i].vy = 0;
        mPaddles[i].vx = sPachinko2PaddleFlyAwayVX[i];
        mPaddles[i].dir = i;
        return;
    }

    mPaddles[i].x += mPaddles[i].vx;
    mPaddles[i].y += mPaddles[i].vy;
    mPaddles[i].vy -= 0x20;
    dir = mPaddles[i].dir;
    if (dir == 0) {
        if (mPaddles[i].vx <= 0xc00)
            mPaddles[i].vx += 0x80;
    } else if (dir == 1) {
        if (mPaddles[i].vx >= -0xc00)
            mPaddles[i].vx -= 0x80;
    }

    screenY = mPaddles[i].y >> 12;
    if (screenY <= -0x120) {
        mPaddles[i].active = 0;
        mPaddles[i].visible = 0;
    }
}

void dScMgPachinko2_c::Paddle_StateDropBalls(int idx) {

    if (mPaddles[idx].timer != 0) {
        mPaddles[idx].timer -= 1;
        if ((short)mPaddles[idx].timer < 0) {
            mPaddles[idx].timer = 0;
        }
        return;
    }
    if (mPaddles[idx].ballsHeld != 0) {
        int x;
        int z;

        mPaddles[idx].ballsHeld -= 1;
        mPaddles[idx].timer = 48;
        x = mPaddles[idx].x >> 12;
        z = mPaddles[idx].y >> 12;
        Ball_HitAt(x, z, mPaddles[idx].ballsHeld & 1);
    }
}

void dScMgPachinko2_c::Paddle_StateGather(int i) {
    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        if ((s16)mPaddles[i].timer < 0)
            mPaddles[i].timer = 0;
        return;
    }

    {
        int targetY = sPachinko2PaddleGatherTargets[mLaneConfig][i];
        int deltaX = 0x80 - (mPaddles[i].x >> 12);
        int currentY = mPaddles[i].y;

        /* Kept: the self-select orders the rsb for deltaX before the y
           subtract. Six plain spellings (no temporaries, deltaY first or
           last, x/y temporaries) all swap those two words. */
        currentY = deltaX ? currentY : currentY;
        mPaddles[i].angle = cstd::atan2(targetY - (currentY >> 12), deltaX);

        {
            fx16 c = FX_CosIdx((u16)mPaddles[i].angle >> 4);
            mPaddles[i].x += FX_MUL(c, mPaddles[i].speed);
        }
        {
            fx16 s = FX_SinIdx((u16)mPaddles[i].angle >> 4);
            mPaddles[i].y += FX_MUL(s, mPaddles[i].speed);
        }

        mPaddles[i].speed += 0x100;

        {
            int dx = (mPaddles[i].x >> 12) - 0x80;
            int dy = (mPaddles[i].y >> 12) - targetY;
            if (dx < -3)
                return;
            if (dx > 3)
                return;
            if (dy < -3)
                return;
            if (dy > 3)
                return;
        }

        mPaddles[i].x = 0x80000;
        mPaddles[i].y = targetY << 12;
        mPaddles[i].state = 0xd;
        CupSlot_ResetAll();
        mPaddles[i].timer = 64;
        if (i & 1) {
            mPaddles[i].timer += 32;
        }
    }
}

void dScMgPachinko2_c::Paddle_StateRiseHome(int idx) {
    if (mPaddles[idx].timer != 0) {
        mPaddles[idx].timer -= 1;
        return;
    }
    mPaddles[idx].y += mPaddles[idx].vy;
    mPaddles[idx].vy -= 0x100;
    int homeY = mPaddles[idx].homeY;
    int y = mPaddles[idx].y;
    if (homeY < y)
        return;
    mPaddles[idx].y = homeY;
    mPaddles[idx].state = mPaddles[idx].savedState;
    mPaddles[idx].vx = mPaddles[idx].savedVx;
    mPaddles[idx].vy = mPaddles[idx].savedVy;
}

void dScMgPachinko2_c::Paddle_StateSwing(int idx) {
    mPaddles[idx].x += mPaddles[idx].vx;

    if (mPaddles[idx].swing == 0) {
        mPaddles[idx].vx += 0x200;
        if (mPaddles[idx].vx >= 0x1800) {
            mPaddles[idx].swing = 1;
        }
    } else if (mPaddles[idx].swing == 1) {
        mPaddles[idx].vx -= 0x200;
        if (mPaddles[idx].vx <= -0x1800) {
            mPaddles[idx].swing = 0;
        }
    }

    if (mPaddles[idx].timer != 0) {
        mPaddles[idx].timer--;
        return;
    }

    if ((mPaddles[idx].x - mPaddles[idx].homeX) >> 12 < -2) {
        return;
    }
    if ((mPaddles[idx].x - mPaddles[idx].homeX) >> 12 > 2) {
        return;
    }

    mPaddles[idx].timer = 16;
    mPaddles[idx].vy = 0;
    mPaddles[idx].state = 0xb;
    mPaddles[idx].x = mPaddles[idx].homeX;
    mPaddles[idx].vx = 0;
}

void dScMgPachinko2_c::Paddle_StateFallToGround(int i) {
    int screenY;
    mPaddles[i].y += mPaddles[i].vy;
    screenY = mPaddles[i].y >> 12;
    if (mPaddles[i].vy <= 0x4000)
        mPaddles[i].vy += 0x200;
    if (screenY < 0xa0)
        return;
    mPaddles[i].y = 0xa0000;
    mPaddles[i].state = 0xa;
    mPaddles[i].vy = 0;
    mPaddles[i].timer = 64;
    if (PACHINKO_RAND(2)) {
        mPaddles[i].vx = 0x1000;
        mPaddles[i].swing = 0;
    } else {
        mPaddles[i].vx = -0x1000;
        mPaddles[i].swing = 1;
    }
}

void dScMgPachinko2_c::Paddle_StateKnockedDown(int i) {
    int distanceBelowHome;
    mPaddles[i].y += mPaddles[i].vy;
    distanceBelowHome = (mPaddles[i].y - mPaddles[i].homeY) >> 12;
    if (distanceBelowHome >= 0x40) {
        if (mPaddles[i].vy >= 0x1000)
            mPaddles[i].vy -= 0x100;
    } else if (mPaddles[i].vy <= 0x4000) {
        mPaddles[i].vy += 0x200;
    }
    if (distanceBelowHome < 0x60)
        return;
    mPaddles[i].timer = 16;
    mPaddles[i].vy = 0;
    mPaddles[i].state = 0xb;
}

void dScMgPachinko2_c::Paddle_Hit(int i) {
    if ((int)mPaddles[i].state >= 8)
        return;

    mPaddles[i].hits += 1;

    if (mPaddles[i].hits < 4)
        return;

    mPaddles[i].homeX = mPaddles[i].x;
    mPaddles[i].homeY = mPaddles[i].y;
    mPaddles[i].savedState = mPaddles[i].state;
    mPaddles[i].savedVx = mPaddles[i].vx;
    mPaddles[i].savedVy = mPaddles[i].vy;
    mPaddles[i].vx = 0;
    mPaddles[i].vx = 0; /* stored twice in the ROM too */
    mPaddles[i].vy = 0x2000;

    if (mPaddles[i].flip != 0)
        mPaddles[i].state = 9;
    else
        mPaddles[i].state = 8;

    mPaddles[i].hits = 0;

    mPaddles[i].flip ^= 1;
}

void dScMgPachinko2_c::Paddle_StatePause(int i) {
    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer -= 1;
        if ((short)mPaddles[i].timer < 0)
            mPaddles[i].timer = 0;
        return;
    }

    if (mLaneConfig == 3)
        mPaddles[i].state = 3;
    else
        mPaddles[i].state = 2;
}

void dScMgPachinko2_c::Paddle_StateCross(int i) {
    int screenX;
    int screenY;
    u8 step;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        return;
    }

    mPaddles[i].x += mPaddles[i].vx;
    mPaddles[i].y += mPaddles[i].vy;
    screenX = mPaddles[i].x >> 12;
    step = mPaddles[i].step;
    screenY = mPaddles[i].y >> 12;

    if (step == 0) {
        if (screenX >= 0xe0) {
            mPaddles[i].timer = 64;
            mPaddles[i].step++;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            return;
        }
        if (screenX >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
            if (i == 0) {
                if (mPaddles[i].vy <= 0xc00)
                    mPaddles[i].vy += 0x10;
            } else {
                if (mPaddles[i].vy >= -0xc00)
                    mPaddles[i].vy -= 0x10;
            }
        } else {
            if (mPaddles[i].vx <= 0xc00)
                mPaddles[i].vx += 0x20;
            if (i == 0) {
                if (screenY != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy >= -0xc00)
                        mPaddles[i].vy -= 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            } else {
                if (screenY != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy <= 0xc00)
                        mPaddles[i].vy += 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            }
        }
    } else if (step == 1) {
        if (screenX <= 0x20) {
            mPaddles[i].timer = 64;
            mPaddles[i].step = 0;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            return;
        }
        if (screenX <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
            if (i == 0) {
                if (mPaddles[i].vy <= 0xc00)
                    mPaddles[i].vy += 0x10;
            } else {
                if (mPaddles[i].vy >= -0xc00)
                    mPaddles[i].vy -= 0x10;
            }
        } else {
            if (mPaddles[i].vx >= -0xc00)
                mPaddles[i].vx -= 0x20;
            if (i == 0) {
                if (screenY != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy >= -0xc00)
                        mPaddles[i].vy -= 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            } else {
                if (screenY != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy <= 0xc00)
                        mPaddles[i].vy += 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            }
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}

void dScMgPachinko2_c::Paddle_StateWeave(int i) {
    int screenX;
    u8 dir;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer -= 1;
        return;
    }

    mPaddles[i].x += mPaddles[i].vx;
    mPaddles[i].y += mPaddles[i].vy;
    dir = mPaddles[i].dir;
    screenX = mPaddles[i].x >> 12;

    if (dir != 0) {
        if (screenX >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
        } else {
            if (mPaddles[i].vx <= 0x1000)
                mPaddles[i].vx += 0x80;
            if (mPaddles[i].step == 0) {
                if (mPaddles[i].vy <= 0x1000)
                    mPaddles[i].vy += 0xc;
            } else {
                if (mPaddles[i].vy >= -0x1000)
                    mPaddles[i].vy -= 0xc;
            }
        }
        if (screenX >= 0xe0) {
            mPaddles[i].x = 0xe0000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].dir ^= 1;
            mPaddles[i].step ^= 1;
            mPaddles[i].timer = 64;
        }
    } else {
        if (screenX <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
        } else {
            if (mPaddles[i].vx >= -0x1000)
                mPaddles[i].vx -= 0x80;
            if (mPaddles[i].step == 0) {
                if (mPaddles[i].vy <= 0x1000)
                    mPaddles[i].vy += 0xc;
            } else {
                if (mPaddles[i].vy >= -0x1000)
                    mPaddles[i].vy -= 0xc;
            }
        }
        if (screenX <= 0x20) {
            mPaddles[i].x = 0x20000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].dir ^= 1;
            mPaddles[i].step ^= 1;
            mPaddles[i].timer = 64;
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}

void dScMgPachinko2_c::Paddle_StateOrbit(int i) {
    mPaddles[i].x = FX_MUL(FX_CosIdx((u16)mPaddles[i].angle >> 4), 0x50000) + 0x80000;
    mPaddles[i].y = FX_MUL(FX_SinIdx((u16)mPaddles[i].angle >> 4), 0x50000) - 0x80000;

    if (mPaddles[i].step != 0) {
        mPaddles[i].angle -= 0x40;
    } else {
        mPaddles[i].angle += 0x40;
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}

void dScMgPachinko2_c::Paddle_StateBoxLoop(int i) {
    int pos;
    u8 step;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        return;
    }

    step = mPaddles[i].step;

    if (step == 0) {
        mPaddles[i].y += mPaddles[i].vy;
        pos = mPaddles[i].y >> 12;
        if (pos >= -0x60) {
            if (mPaddles[i].vy >= 0x400)
                mPaddles[i].vy -= 8;
        } else {
            if (mPaddles[i].vy <= 0x1000)
                mPaddles[i].vy += 0x80;
        }
        if (pos >= -0x40) {
            mPaddles[i].y = -0x40000;
            mPaddles[i].vy = 0;
            mPaddles[i].vx = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step++;
            return;
        }
    } else if (step == 1) {
        mPaddles[i].x += mPaddles[i].vx;
        pos = mPaddles[i].x >> 12;
        if (pos >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
        } else {
            if (mPaddles[i].vx <= 0x1000)
                mPaddles[i].vx += 0x80;
        }
        if (pos >= 0xe0) {
            mPaddles[i].x = 0xe0000;
            mPaddles[i].vy = 0;
            mPaddles[i].vx = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step++;
            return;
        }
    } else if (step == 2) {
        mPaddles[i].y += mPaddles[i].vy;
        pos = mPaddles[i].y >> 12;
        if (pos <= -0xa0) {
            if (mPaddles[i].vy <= -0x400)
                mPaddles[i].vy += 8;
        } else {
            if (mPaddles[i].vy >= -0x1000)
                mPaddles[i].vy -= 0x80;
        }
        if (pos <= -0xc0) {
            mPaddles[i].y = -0xc0000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step++;
        }
    } else {
        mPaddles[i].x += mPaddles[i].vx;
        pos = mPaddles[i].x >> 12;
        if (pos <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
        } else {
            if (mPaddles[i].vx >= -0x1000)
                mPaddles[i].vx -= 0x80;
        }
        if (pos <= 0x20) {
            mPaddles[i].x = 0x20000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step = 0;
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}

void dScMgPachinko2_c::Paddle_StateSweep(int i) {
    int screenX;
    u8 dir;

    mPaddles[i].x += mPaddles[i].vx;
    dir = mPaddles[i].dir;
    screenX = mPaddles[i].x >> 12;

    if (dir != 0) {
        if (screenX >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
        } else {
            int idx = mLaneConfig;
            if (mPaddles[i].vx <= sPachinko2PaddleSweepLimits[idx][i])
                mPaddles[i].vx += 0x80;
        }
        if (screenX >= 0xe0) {
            mPaddles[i].x = 0xe0000;
            mPaddles[i].vx = 0;
            mPaddles[i].dir ^= 1;
            mPaddles[i].timer = PACHINKO_RAND(8) * 3 + 64;
            mPaddles[i].state = 7;
        }
    } else {
        if (screenX <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
        } else {
            int idx = mLaneConfig;
            if (mPaddles[i].vx >= -sPachinko2PaddleSweepLimits[idx][i])
                mPaddles[i].vx -= 0x80;
        }
        if (screenX <= 0x20) {
            mPaddles[i].x = 0x20000;
            mPaddles[i].vx = 0;
            mPaddles[i].dir ^= 1;
            mPaddles[i].timer = PACHINKO_RAND(8) * 3 + 64;
            mPaddles[i].state = 7;
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}

void dScMgPachinko2_c::Paddle_StateDescend(int i) {
    int limit;

    mPaddles[i].y += mPaddles[i].vy;
    limit = sPachinko2PaddleDescendLimits[mLaneConfig][i];

    if ((mPaddles[i].y >> 12) >= limit) {
        mPaddles[i].y = limit << 12;
        mPaddles[i].vy = 0;

        if (i & 2) {
            if (mPaddles[i].vx > 0)
                mPaddles[i].vx = 0xc80;
            else
                mPaddles[i].vx = -0xc80;
        }

        if (mLaneConfig == 1) {
            mPaddles[i].state = 4;
            if (mPaddles[i].vx > 0)
                mPaddles[i].angle = 0x8000;
            else
                mPaddles[i].angle = 0;
        }

        if (mLaneConfig == 3) {
            mPaddles[i].state = 3;
            mPaddles[i].vx = 0;
            if (mPaddles[i].dir != 0) {
                mPaddles[i].vy = 0xc00;
                mPaddles[i].step = 0;
            } else {
                mPaddles[i].vy = -0xc00;
                mPaddles[i].step = 2;
            }
            return;
        }

        if (mLaneConfig == 4) {
            mPaddles[i].state = 5;
            if (i == 0)
                mPaddles[i].step = 0;
            else
                mPaddles[i].step = 1;
        }

        if (mLaneConfig == 5) {
            mPaddles[i].state = 6;
            if (mPaddles[i].dir != 0)
                mPaddles[i].step = 0;
            else
                mPaddles[i].step = 1;
            return;
        }
    } else {
        mPaddles[i].vy -= 0x20;
    }

    Paddle_StateSweep(i);
}

void dScMgPachinko2_c::Paddle_StateSpawn(int idx) {
    int bit;

    if (mPaddles[idx].timer != 0) {
        mPaddles[idx].timer--;

        if ((short)mPaddles[idx].timer < 0)
            mPaddles[idx].timer = 0;

        return;
    }

    mPaddles[idx].state = 1;

    bit = PACHINKO_RAND(2);

    if (idx & 1)
        bit = mPaddles[0].dir ^ 1;

    mPaddles[idx].dir = bit;

    if (bit != 0) {
        mPaddles[idx].visible = 1;
        mPaddles[idx].x = -0x10000;
        mPaddles[idx].vx = 0x200;

        if (mLaneConfig == 1) {
            mPaddles[idx].vx = 0xf00;

            if (idx) {
                mPaddles[idx].step = mPaddles[0].step;
            } else {
                mPaddles[idx].step = PACHINKO_RAND(2);
            }
        }
    } else {
        mPaddles[idx].visible = 1;
        mPaddles[idx].x = 0x110000;
        mPaddles[idx].vx = -0x200;

        if (mLaneConfig == 1) {
            mPaddles[idx].vx = -0xf00;

            if (idx) {
                mPaddles[idx].step = mPaddles[0].step;
            } else {
                mPaddles[idx].step = PACHINKO_RAND(2);
            }
        }
    }

    mPaddles[idx].y = -0xf8000;
    mPaddles[idx].ballsHeld = 0;
    mPaddles[idx].vy = 0x2000;

    mPaddles[idx].sound =
        func_02012468(mPaddles[idx].sound, 2, 0x198, 4, 0, 0, func_020126e8(mPaddles[idx].x), 0);
}

void dScMgPachinko2_c::Paddle_UpdateAll() {
    for (int i = 0; i < 3; i++) {
        if (mPaddles[i].active) {
            (this->*sPachinko2PaddleStateTable[mPaddles[i].state])(i);
            Paddle_CheckBallHits(i);
        }
    }
}

void dScMgPachinko2_c::Paddle_SpawnForLanes() {
    int n = sPachinko2PaddleCountPerLanes[mLaneConfig];
    for (int i = 0; i < n; i++) {
        mPaddles[i].active = 1;
        mPaddles[i].state = 0;
        mPaddles[i].step = 0;
        mPaddles[i].timer = 32;
        mPaddles[i].sound = 0;
        mPaddles[i].spawnClear[0] = 0;
        mPaddles[i].spawnClear[1] = 0;
        mPaddles[i].hits = 0;
        mPaddles[i].flip = 0;
    }
}

void dScMgPachinko2_c::Paddle_ClearAll() {
    for (int i = 0; i < 3; i++) {
        mPaddles[i].active = 0;
        mPaddles[i].visible = 0;
    }
}

void dScMgPachinko2_c::CheckCountdownEnd() {
    /* volatile: the ROM spills the fill value to the stack and reloads it;
       a literal 0 or a plain local is 8 bytes shorter (known unsolved). */
    volatile unsigned short v;
    char *dst;
    if (mCountdown != 0)
        return;
    Paddle_Stop(0);
    Paddle_Stop(1);
    Paddle_Stop(2);
    mPhase = 2;
    mEndTimer = 64;
    dst = (char *)G2S_GetBG2CharPtr();
    v = 0;
    MI_CpuFill16(dst, v, 0x6000);
    mResult = 0;
    Ball_ClearAll();
}

void dScMgPachinko2_c::DrawCountdown() {
    if (mCountdownShown == 0)
        return;
    Mg_DrawTopCenterCounter(mCountdown);
}

void dScMgPachinko2_c::UpdateCountdown() {
    if (mCountdown == 0)
        return;
    mCountdownTick++;
    if (mCountdownTick < 60)
        return;
    mCountdownTick = 0;
    mCountdown--;
    if (mCountdown > 10)
        Sound::PlayBank2_2D(0xa8);
    else if (mCountdown > 3)
        Sound::PlayBank2_2D(0xa7);
    else
        Sound::PlayBank2_2D(0xa6);
    if (mCountdown == 0)
        mCountdownShown = 0;
}

void dScMgPachinko2_c::ClearAimLine() {
    /* volatile: the ROM spills the fill value to the stack and reloads it;
       a literal 0 or a plain local is 8 bytes shorter (known unsolved). */
    volatile unsigned short tmp;
    int n = mHeldBall;
    int dx, dy;

    if (n == 0)
        return;

    dx = 0x80 - (mBalls[n - 1].x >> 12);
    dy = 0x20 - (mBalls[n - 1].y >> 12);

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;
    if (mBalls[n - 1].state != 2)
        return;

    mHeldBall = 0;
    {
        char *dst = (char *)G2S_GetBG2CharPtr();
        tmp = 0;
        MI_CpuFill16(dst, tmp, 0x6000);
    }
}

#pragma push
/* Kept: without opt_loop_invariants off the hoisted x >> 3 and x & 7 swap
 * order and the row and column take other registers (10 of 32 words).
 * Plain (x - 1 + j, y - 1 + i) bodies with it on are 31 words: the ROM
 * spills the shared 0 for j to the stack (measured #821). */
/* Kept (#821 r79): x, lo, hi and col recomputed in the inner loop, do-while loops and an x
   counter are 10, 10 and 34 words off of 32. */
#pragma opt_loop_invariants off
void dScMgPachinko2_c::PlotSubBg2Pixels(int a1, int a2, int a3) {
    int i, j, x, lo, hi, col, row, yb, s;
    u32 *bp;
    s = a1;
    for (i = 0; i < 2; i++) {
        x = a2 - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++) {
            row = hi * 32;
            yb = s - 1;
            a1 = yb + j;
            bp = (u32 *)G2S_GetBG2CharPtr();
            u32 *cell = (u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col);
            *cell |= a3 << ((a1 & 7) * 4);
        }
    }
}
#pragma pop

/* The same guide line as dScMgPachinko_c::DrawAimLine, its twin: two
 * segments from the fixed pivots (0x6c, 0x22) and (0x94, 0x22). */
void dScMgPachinko2_c::DrawAimLine() {
    int k;
    int tx;
    int ty;
    int idx;
    volatile unsigned short tmp; /* fill value; see StartRoundEnd */
    int cx;
    int cy;
    int dx;
    int dy;
    int adx;
    int ady;

    if (mHeldBall == 0)
        return;
    idx = mHeldBall - 1;
    if (mBalls[idx].x != mBalls[idx].prevX || mBalls[idx].y != mBalls[idx].prevY) {
        char *dst = (char *)G2S_GetBG2CharPtr();
        tmp = 0;
        MI_CpuFill16(dst, tmp, 0x6000);

        for (k = 0; k < 2; k++) {
            if (k == 0) {
                cx = 0x6c;
                tx = mBalls[idx].x >> 12;
                cy = 0x22;
                ty = mBalls[idx].y >> 12;
                if (ty >= 0x22)
                    ty += 4;
                else
                    ty -= 4;
                tx -= 4;
            } else {
                cx = 0x94;
                tx = mBalls[idx].x >> 12;
                cy = 0x22;
                ty = mBalls[idx].y >> 12;
                if (ty >= 0x22)
                    ty += 4;
                else
                    ty -= 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0)
                adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0)
                ady = -dy;

            if (adx >= ady) {
                mGuideStep = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        PlotSubBg2Pixels(cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        mGuideStep += ady;
                        if (mGuideStep > adx) {
                            if (dy >= 0)
                                cy++;
                            else
                                cy--;
                            mGuideStep -= adx;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        mGuideStep += ady;
                        if (mGuideStep > adx) {
                            if (dy >= 0)
                                cy++;
                            else
                                cy--;
                            mGuideStep -= adx;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                mGuideStep = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        PlotSubBg2Pixels(cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        mGuideStep += adx;
                        if (mGuideStep > ady) {
                            if (dx >= 0)
                                cx++;
                            else
                                cx--;
                            mGuideStep -= ady;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        mGuideStep += adx;
                        if (mGuideStep > ady) {
                            if (dx >= 0)
                                cx++;
                            else
                                cx--;
                            mGuideStep -= ady;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cy != ty)
                            continue;
                        break;
                    }
                }
            }
        }
    }
    mBalls[idx].prevX = mBalls[idx].x;
    mBalls[idx].prevY = mBalls[idx].y;
}

void dScMgPachinko2_c::Ball_HitAt(int x, int z, int d) {
    int i;
    for (i = 0; i < 48; i++) {
        if (mBalls[i].active == 0) {
            mBalls[i].active = 1;
            mBalls[i].state = 7;
            mBalls[i].timer = 64;
            mBalls[i].visible = 1;
            mBalls[i].sprite = 0;
            if (d != 0)
                mBalls[i].angle = 0xd000;
            else
                mBalls[i].angle = 0xb000;
            mBalls[i].x = x << 12;
            mBalls[i].y = (z - 0x12) << 12;
            mBalls[i].vx = FX_MUL(FX_CosIdx((u16)mBalls[i].angle >> 4), 0x2000);
            mBalls[i].vy = FX_MUL(FX_SinIdx((u16)mBalls[i].angle >> 4), 0x2000);
            mBalls[i].gravity = 0;
            Sound::PlayBank2_2D(0x199);
            return;
        }
    }
}

/* Bounce the ball at y=168: reverse and quarter vertical velocity, stop
 * horizontal motion, reset animation and play the sound at the ball's x. */
void dScMgPachinko2_c::Ball_Bounce(int i) {
    mBalls[i].state = 4;
    mBalls[i].vy = -mBalls[i].vy >> 2;
    mBalls[i].vx = 0;
    mBalls[i].timer = 0;
    mBalls[i].animStep = 0;
    mBalls[i].y = 0xa8000;
    func_02012718(0x19c, mBalls[i].x);
}

void dScMgPachinko2_c::Ball_ClearAll() {
    for (int i = 0; i < 48; i++) {
        mBalls[i].visible = 0;
        mBalls[i].active = 0;
    }
}

void dScMgPachinko2_c::Ball_DrawAll() {
    int i;
    for (i = 0; i < 48; i++) {
        if (mBalls[i].visible != 0) {
            int x = mBalls[i].x >> 12;
            unsigned char t = mBalls[i].sprite;
            int y = mBalls[i].y >> 12;
            if (t != 0) {
                Mg_RenderSpriteBoth(sPachinko2BallCountSprites[t - 1], x, y, -1, -1);
            } else {
                Mg_RenderSpriteBothScaled(sPachinko2BallSprite, x, y, -1, -1, 0x1000,
                                          mBalls[i].rotation);
            }
        }
    }
}

void dScMgPachinko2_c::Ball_UpdateAll() {
    int i;
    for (i = 0; i < 48; i++) {
        if (mBalls[i].active != 0) {
            unsigned char k = mBalls[i].state;
            (this->*sPachinko2BallStateTable[k].pmf)(i);
        }
    }
}

void dScMgPachinko2_c::StartCountdown() {
    if (mCountdownArmed != 0)
        return;
    mCountdown = 30;
    mCountdownTick = 0;
    mCountdownShown = 1;
    mCountdownArmed++;
}

void dScMgPachinko2_c::Ball_StateWaitGrab(int i) {
    u8 slot;
    int touched;
    int dx, dy;

    touched = 0;
    slot = gActivePlayerSlot;
    if (gTouchInfo[slot].held) {
        if (gTouchInfo[slot].edge)
            touched = 1;
    }
    if (touched != 0) {
        dx = (mBalls[i].x >> 12) - gTouchInfo[slot].x;
        dy = (mBalls[i].y >> 12) - gTouchInfo[slot].y;
        mBalls[i].state = 1;
        mBalls[i].px = dx << 12;
        mBalls[i].py = dy << 12;
        mBalls[i].spawnClear = 0;
    }
    DrawAimLine();
}

/* dScMgPachinko2_c ball i under a live pen record: while the pen is down the
   ball follows the pen (x/y = pen + held offset, clamped to the field, offset
   re-derived); on release it aims the ball at (0x80, 0x20) and launches it.
   Touch record reads: the pen x is a volatile byte read and the pen y a
   plain byte read, both as a flat byte view of gTouchInfo strided by 4. This
   is a launder, kept because nothing natural reaches the ROM: plain
   gTouchInfo[idx].x/.y at both sites is 16 words off (r3/r6 colouring at the
   first site, the literal hoisting at the second), a volatile TouchInfo at
   both 165, a volatile &gTouchInfo[idx].x 14; the flat view at only one site
   leaves 11 (first) or 5 (second). Measured under 2004/b56 against the
   lane-symbol form this replaced, which compiled the same bytes.
   Re-measured (#821, 2026-11-16): typed gTouchInfo[idx].x/.y at the first
   site alone is 5 words off (r3/r6 swapped) with y read first, y declared
   first, the shift operands swapped or u8 locals; read inline 9; through a
   TouchInfo reference 0x374. Typed at the second site alone is 11. */
void dScMgPachinko2_c::Ball_StateAim(int i) {
    int idx = gActivePlayerSlot;

    if (gTouchInfo[idx].held != 0) {
        int sx;
        int sy;
        int dx;
        int dy;
        int dist;
        int sq;
        int prev;
        /* Kept (#821 r79): inline TouchX/TouchY(slot) accessors returning int or u8, at both
           sites or either alone, are 16 words off. */
        int tx = ((volatile u8 *)&gTouchInfo->x)[idx * 4];
        int ty = ((u8 *)&gTouchInfo->y)[idx * 4];

        this->mBalls[i].x = this->mBalls[i].px + (tx << 12);
        this->mBalls[i].y = this->mBalls[i].py + (ty << 12);
        sx = this->mBalls[i].x >> 12;
        sy = this->mBalls[i].y >> 12;
        if (sx >= 0xf8) {
            this->mBalls[i].x = 0xf8000;
        }
        if (sx <= 8) {
            this->mBalls[i].x = 0x8000;
        }
        if (sy >= 0xb8) {
            this->mBalls[i].y = 0xb8000;
        }
        if (sy <= 8) {
            this->mBalls[i].y = 0x8000;
        }
        {
            int nx =
                (this->mBalls[i].x >> 12) - ((volatile u8 *)&gTouchInfo->x)[gActivePlayerSlot * 4];
            int ny = (this->mBalls[i].y >> 12) - ((u8 *)&gTouchInfo->y)[gActivePlayerSlot * 4];
            this->mBalls[i].px = nx << 12;
            this->mBalls[i].py = ny << 12;
        }

        dy = 0x80 - (this->mBalls[i].x >> 12);
        dx = 0x20 - (this->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = cstd::sqrt(dist);
        prev = this->mBalls[i].prevDist;
        this->mBalls[i].prevDist = sq;
        if (sq > prev) {
            if (sq > prev + 10) {
                this->mBalls[i].sound = Sound_PlayIfNotActive(this->mBalls[i].sound, 2, 0x17b, 0);
            } else {
                this->mBalls[i].sound = Sound_PlayIfNotActive(this->mBalls[i].sound, 2, 0x17c, 0);
            }
        }
    } else {
        int dy;
        int dx;
        int dist;
        int sq;

        this->mBalls[i].state = 2;
        dy = 0x80 - (this->mBalls[i].x >> 12);
        dx = 0x20 - (this->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = cstd::sqrt(dist);
        if (sq >= 0x10) {
            this->mBalls[i].angle = cstd::atan2(dx, dy);
            this->mBalls[i].speed = cstd::sqrt(dist) << 8;
            this->mBalls[i].speed += this->mBalls[i].speed >> 3;
            this->mBalls[i].speed += 0x1800;
            if (this->mBalls[i].speed >= 0x9400) {
                this->mBalls[i].speed = 0x9400;
            }
            this->mNextBallDelay = this->mBalls[i].speed >> 11;
            this->mNextBallDelay += this->mNextBallDelay >> 1;
            if (this->mNextBallDelay == 0) {
                this->mNextBallDelay = 1;
            }
            {
                fx16 c = FX_CosIdx((u16)this->mBalls[i].angle >> 4);
                this->mBalls[i].vx = FX_MUL(c, this->mBalls[i].speed);
            }
            {
                fx16 s = FX_SinIdx((u16)this->mBalls[i].angle >> 4);
                this->mBalls[i].vy = FX_MUL(s, this->mBalls[i].speed);
            }
            this->mBalls[i].timer = 0;
            this->mBalls[i].spin = this->mBalls[i].vx >> 2;
            if (sq >= 0x40) {
                Sound::PlayBank2_2D(0x17e);
            } else {
                Sound::PlayBank2_2D(0x17d);
            }
        } else {
            this->mBalls[i].x = 0x80000;
            this->mBalls[i].y = 0x28000;
            this->mBalls[i].state = 0;
        }
    }
    DrawAimLine();
}

void dScMgPachinko2_c::Ball_StateFlying(int i) {
    int screenX, screenY;

    if (FX_SinIdx((u16)mBalls[i].angle >> 4) <= 0) {
        {
            fx16 c = FX_CosIdx((u16)mBalls[i].angle >> 4);
            mBalls[i].x += FX_MUL(c, mBalls[i].speed);
        }
        {
            fx16 s = FX_SinIdx((u16)mBalls[i].angle >> 4);
            mBalls[i].y += FX_MUL(s, mBalls[i].speed);
        }

        mBalls[i].gravity += 0x10;

        mBalls[i].speed -= mBalls[i].gravity;

        if (mBalls[i].gravity >= 0x600) {
            mBalls[i].gravity = 0x600;
        }

        if (mBalls[i].speed < 0) {
            mBalls[i].state = 3;
            mBalls[i].speed = 0;
            mBalls[i].gravity = 0;
            mBalls[i].vy = 0;
            mBalls[i].vx = 0;
        }
    } else {
        mBalls[i].x += mBalls[i].vx;
        mBalls[i].y += mBalls[i].vy;
        mBalls[i].gravity += 0x10;
        mBalls[i].vy += mBalls[i].gravity;

        {
            int v = mBalls[i].vx;
            int nv;
            if (v > 0) {
                nv = v - 0x200;
                if (nv <= 0)
                    nv = 0;
            } else if (v < 0) {
                nv = v + 0x200;
                if (nv >= 0)
                    nv = 0;
            } else {
                mBalls[i].state = 3;
                mBalls[i].speed = 0;
                mBalls[i].gravity = 0;
                mBalls[i].vx = 0;
                return;
            }
            mBalls[i].vx = nv;
        }
    }

    screenX = mBalls[i].x >> 12;
    screenY = mBalls[i].y >> 12;
    mBalls[i].rotation += (u16)mBalls[i].spin;

    if (screenY <= -0x140 || screenY >= 0xd0) {
        mBalls[i].state = 6;
    }
    if (screenX >= 0x140 || screenX <= -0x40) {
        mBalls[i].state = 6;
    }

    ClearAimLine();
    if (mHeldBall != 0) {
        DrawAimLine();
    }
}

void dScMgPachinko2_c::Ball_StateTumble(int i) {
    int screenX, screenY;
    mBalls[i].x += mBalls[i].vx;
    mBalls[i].y += mBalls[i].vy;
    mBalls[i].gravity += 0x20;
    mBalls[i].vy += mBalls[i].gravity;
    mBalls[i].rotation += (u16)mBalls[i].spin;
    if (mBalls[i].vy >= 0x8000)
        mBalls[i].vy = 0x8000;
    screenX = mBalls[i].x >> 12;
    screenY = mBalls[i].y >> 12;
    if (screenY <= -0x140)
        mBalls[i].state = 6;
    if (screenX >= 0x140 || screenX <= -0x40)
        mBalls[i].state = 6;
    if (screenY >= 0xa8)
        Ball_Bounce(i);
}

void dScMgPachinko2_c::Ball_StateHop(int i) {
    int prevVy;
    int screenX;
    int screenY;

    mBalls[i].y += mBalls[i].vy;
    prevVy = mBalls[i].vy;
    mBalls[i].vy += 0x200;

    if (prevVy < 0 && mBalls[i].vy >= 0) {
        mBalls[i].rotation = 0;
        screenX = mBalls[i].x >> 12;
        if (screenX > 0x80) {
            mBalls[i].facingRight = 1;
            mBalls[i].sprite = 5;
            mBalls[i].vx = 0x1000;
        } else if (screenX < 0x80) {
            mBalls[i].facingRight = 0;
            mBalls[i].sprite = 1;
            mBalls[i].vx = -0x1000;
        } else {
            if (PACHINKO_RAND(2) != 0) {
                mBalls[i].facingRight = 1;
                mBalls[i].sprite = 5;
                mBalls[i].vx = 0x1000;
            } else {
                mBalls[i].facingRight = 0;
                mBalls[i].sprite = 1;
                mBalls[i].vx = -0x1000;
            }
        }
    }

    screenY = mBalls[i].y >> 12;
    if (screenY >= 0xa8) {
        mBalls[i].y = 0xa8000;
        mBalls[i].state = 5;
    }
}

void dScMgPachinko2_c::Ball_StateWalkOff(int i) {
    mBalls[i].timer++;
    if (mBalls[i].timer >= 4) {
        mBalls[i].timer = 0;
        mBalls[i].animStep++;
        if (mBalls[i].animStep >= 4)
            mBalls[i].animStep = 0;
    }

    mBalls[i].sprite = sPachinko2BallWalkOffSprites[mBalls[i].animStep];
    if (mBalls[i].facingRight != 0)
        mBalls[i].sprite += 4;

    mBalls[i].x += mBalls[i].vx;

    int x = mBalls[i].x >> 12;
    if (x >= 0x110 || x <= -16)
        mBalls[i].state = 6;
}

void dScMgPachinko2_c::Ball_StateRemove(int idx) {
    mBalls[idx].active = 0;
    mBalls[idx].visible = 0;
}

void dScMgPachinko2_c::Ball_StateScored(int i) {
    mBalls[i].x += mBalls[i].vx;
    mBalls[i].y += mBalls[i].vy;
    mBalls[i].gravity += 0x10;
    mBalls[i].vy += mBalls[i].gravity;

    if (mBalls[i].vy >= 0x8000)
        mBalls[i].vy = 0x8000;

    if (mBalls[i].timer != 0) {
        mBalls[i].timer--;
        if (mBalls[i].timer == 32) {
            Popup_Spawn(mBalls[i].x, mBalls[i].y, Mg_GetSubmittedScore() + 1);
            Mg_SubmitScore(Mg_GetSubmittedScore() + 1);
        }
    }

    if ((mBalls[i].y >> 12) < 0xa8)
        return;

    Ball_Bounce(i);
}

void dScMgPachinko2_c::UpdateNextBall() {
    int i;
    if (mNextBallDelay == 0)
        return;
    mNextBallDelay--;
    if ((short)mNextBallDelay > 0)
        return;
    mNextBallDelay = 0;
    for (i = 0; i < 48; i++) {
        if (mBalls[i].active == 0) {
            mBalls[i].active = 1;
            mBalls[i].visible = 1;
            mBalls[i].state = 0;
            mBalls[i].gravity = 0;
            mBalls[i].x = 0x80000;
            mBalls[i].y = 0x28000;
            mBalls[i].px = 0;
            mBalls[i].py = 0;
            mBalls[i].spawnClear = 0;
            mBalls[i].vx = 0;
            mBalls[i].vy = 0;
            mBalls[i].timer = 0;
            mBalls[i].rotation = 0;
            mBalls[i].spin = 0;
            mBalls[i].sprite = 0;
            mBalls[i].animStep = 0;
            mBalls[i].facingRight = 0;
            mBalls[i].sound = 0;
            mBalls[i].prevDist = 4;
            mHeldBall = i + 1;
            if (mBallDealt != 0) {
                Sound::PlayBank2_2D(0x19d);
                return;
            }
            mBallDealt += 1;
            return;
        }
    }
}

void dScMgPachinko2_c::ResetRound() {
    int i;
    int j;
    mPhase = 0;
    for (i = 0; i < 48; i++) {
        mBalls[i].x = 0;
        mBalls[i].y = 0;
        mBalls[i].prevX = 0;
        mBalls[i].prevY = 0;
        mBalls[i].speed = 0;
        mBalls[i].gravity = 0;
        mBalls[i].angle = 0;
        mBalls[i].timer = 0;
        mBalls[i].active = 0;
        mBalls[i].state = 0;
        mBalls[i].visible = 0;
    }
    for (j = 0; j < 16; j++) {
        mHitFx[j].active = 0;
        mHitFx[j].visible = 0;
    }
    mEndTimer = 0;
    unk_5672 = 0;
    mResult = 0;
    mCountdownArmed = 0;
    mCountdownTick = 0;
    mCountdownShown = 0;
    mBallDealt = 0;
    Mg_SubmitScore(0);
    Paddle_ClearAll();
    Cup_HideAll();
}

/* Slot 18, overriding dScMgBase_c::StartRound(int). That name is a
   placeholder borrowed from dActor_c by slot index. */
void dScMgPachinko2_c::StartRound(int n) {
    /* Reading through `ro` keeps mwcc from holding &mLevel in a register,
       which costs a word: plain mLevel++ and mLevel = mLevel + 1 both build
       add r2, r4, #0xbc and come out 4 bytes long. The twin
       (d_s_mg_pachinko.cpp) keeps the same read. */
    const dScMgPachinko2_c *ro = this;

    mPhase = 0;
    if (n == 0x10) {
        mLevel = ro->mLevel + 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        mLevel = 0;
        /* dead clamp, kept: dropping it changes the code */
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    }
    FreeGfxSlotsById(0x1d);
    ResetRound();
}

/* Slot 9. */
s32 dScMgPachinko2_c::Render() {
    DrawTopRecord();
    DrawScore();
    DrawCountdown();
    Spark_DrawAll();
    Popup_DrawAll();
    Ball_DrawAll();
    Paddle_DrawAll();
    CupSlot_DrawAll();
    return 1;
}

/* Slot 6. mPhase selects which helpers run this frame. */
s32 dScMgPachinko2_c::Behavior() {
    switch (mPhase) {
    case 0:
        ChooseLaneConfig();
        Paddle_SpawnForLanes();
        Cup_ShowAll();
        mNextBallDelay = 16;
        mPhase = 1;
        break;
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        UpdateCountdown();
        StartCountdown();
        UpdateNextBall();
        Ball_UpdateAll();
        Paddle_UpdateAll();
        Cup_DrawAll();
        Spark_UpdateAll();
        CheckCountdownEnd();
        break;
    case 2:
        Ball_UpdateAll();
        Paddle_UpdateAll();
        Cup_DrawAll();
        Spark_UpdateAll();
        CheckRoundOver();
        Popup_UpdateAll();
        break;
    case 3:
        Ball_UpdateAll();
        Paddle_UpdateAll();
        Cup_DrawAll();
        Popup_UpdateAll();
        if (mEndTimer != 0) {
            mEndTimer--;
            if ((s16)mEndTimer <= 0) {
                Mg_SetSharedState(MG_STATE_RESULTS);
                mPromptEnabled = 0;
            }
        }
        break;
    }
    return 1;
}

/* Slot 0. Loads both screens' backgrounds and OBJ graphics, then runs the
   three helpers and the two stores of Behavior's case 0. */
s32 dScMgPachinko2_c::InitResources() {
    char *b;
    char *dst;
    /* volatile: the ROM stores each fill value to the stack and reloads it */
    volatile u16 spC;
    volatile u16 spE;
    void *objChar;
    void *objPltt;
    void *f;
    int n;
    int y;
    int x;

    gMainScreenLayers |= DISPLAY_LAYER_BG3; /* main BG3 on */
    G2_SetBG3Priority(1);
    G2_BG3Mosaic(0);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_UP_NCG_BIN);
    DecompressLZ16(f, G2_GetBG3CharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_UP_NCL_BIN);
    GX_LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_UP_NSC_BIN);
    GX_LoadBG3Scr(f, 0, 0x800);
    Deallocate(f);

    objChar = LoadFile(ASSET_D_2D_MG_PACHINKO_NCG_BIN);
    objPltt = LoadFile(ASSET_D_2D_MG_PACHINKO_NCL_BIN);
    DecompressLZ16(objChar, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(objPltt, 0, 0x100);

    gSubScreenLayers |= 0xd; /* sub BG0, BG2 and BG3 on */
    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(0);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x18000);

    b = (char *)G2S_GetBG2CharPtr();
    spC = 0;
    MI_CpuFill16(b, spC, 0x6000);

    n = 0;
    for (y = 0; y < 24; y++) {
        for (x = 0; x < 32; x++) {
            dst = (char *)((u16 *)G2S_GetBG2ScrPtr() + x + y * 0x20);
            spE = n;
            MI_CpuFill16(dst, spE, 2);
            n++;
        }
    }

    InitBrush(0, 2, 0);

    G2S_SetBG3Priority(2);
    G2S_BG3Mosaic(0);
    G2S_SetBG3Offset(0, 0);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x08000);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCG_BIN);
    DecompressLZ16(f, G2S_GetBG3CharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCL_BIN);
    GXS_LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_DOWN_NSC_BIN);
    GXS_LoadBG3Scr(f, 0, 0x800);
    Deallocate(f);

    G2S_SetBG0Priority(1);
    G2S_BG0Mosaic(0);
    G2S_SetBG0Offset(0, 0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);

    f = LoadFile(ASSET_D_2D_HEI_TOP_DOWN_NSC_BIN);
    GXS_LoadBG0Scr(f, 0, 0x800);
    Deallocate(f);

    DecompressLZ16(objChar, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(objPltt, 0, 0x100);
    Deallocate(objChar);
    Deallocate(objPltt);

    ResetRound();
    ChooseLaneConfig();
    Paddle_SpawnForLanes();
    Cup_ShowAll();
    Mg_SetTopScreenYOffset(0x20);

    mRunDuringFade = 1;
    mNextBallDelay = 16;
    mPhase = 1;
    return 1;
}

/* `new` inlines the constructor: fBase_c::operator new for the 0x567c
   bytes, dScMgBase_c's C2, then this class's vtable. The classInit
   spelling follows later EAD lineage; the historical alias is
   MgLakituLaunch_Spawn. */
dScMgPachinko2_c *dScMgPachinko2_c_classInit(void) {
    return new dScMgPachinko2_c;
}

BaseSpawnInfo<dScMgPachinko2_c> g_profile_MG_TAMAIRE = {dScMgPachinko2_c_classInit, 0x171, 0x171};

/* The three state tables, defined in retail copy order: the 8-row ball
   table, the 15-row paddle table, then the 5-row cup table. mwcc emits
   __sinit_d_s_mg_pachinko2.cpp from these definitions, copying the ROM
   .data records. */
Entry sPachinko2BallStateTable[8] = {
    {&dScMgPachinko2_c::Ball_StateWaitGrab}, {&dScMgPachinko2_c::Ball_StateAim},
    {&dScMgPachinko2_c::Ball_StateFlying},   {&dScMgPachinko2_c::Ball_StateTumble},
    {&dScMgPachinko2_c::Ball_StateHop},      {&dScMgPachinko2_c::Ball_StateWalkOff},
    {&dScMgPachinko2_c::Ball_StateRemove},   {&dScMgPachinko2_c::Ball_StateScored},
};
PMF sPachinko2PaddleStateTable[15] = {
    &dScMgPachinko2_c::Paddle_StateSpawn,       &dScMgPachinko2_c::Paddle_StateDescend,
    &dScMgPachinko2_c::Paddle_StateSweep,       &dScMgPachinko2_c::Paddle_StateBoxLoop,
    &dScMgPachinko2_c::Paddle_StateOrbit,       &dScMgPachinko2_c::Paddle_StateWeave,
    &dScMgPachinko2_c::Paddle_StateCross,       &dScMgPachinko2_c::Paddle_StatePause,
    &dScMgPachinko2_c::Paddle_StateKnockedDown, &dScMgPachinko2_c::Paddle_StateFallToGround,
    &dScMgPachinko2_c::Paddle_StateSwing,       &dScMgPachinko2_c::Paddle_StateRiseHome,
    &dScMgPachinko2_c::Paddle_StateGather,      &dScMgPachinko2_c::Paddle_StateDropBalls,
    &dScMgPachinko2_c::Paddle_StateFlyAway,
};
Entry sPachinko2CupStateTable[5] = {
    {&dScMgPachinko2_c::Cup_StateInit},    {&dScMgPachinko2_c::Cup_StateWait},
    {&dScMgPachinko2_c::Cup_StateHopLeft}, {&dScMgPachinko2_c::Cup_StateHopRight},
    {&dScMgPachinko2_c::Cup_StateDecide},
};
