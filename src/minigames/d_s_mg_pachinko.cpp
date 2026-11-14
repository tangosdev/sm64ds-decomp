//cpp
/* Bob-omb Squad. The player aims a Bob-omb with the stylus and fires it
 * up the board; a shot that reaches a target knocks the balls loose.
 * 76 functions (.text 0x020fa75c..0x020ff420), the class's whole linker
 * unit, closing with its factory dScMgPachinko_c_classInit. Below it is the
 * pair-match card game (d_s_mg_m_carlo2.cpp); above it is the next class's
 * destructor.
 *
 * _ZTS15dScMgPachinko_c 0x0213d960. _ZTI15dScMgPachinko_c 0x0213d948.
 * _ZTV15dScMgPachinko_c 0x0213d9cc. The destructor is the key function:
 * D1 at 0x020fa75c, D0 at 0x020fa780, no D2.
 *
 * The 30 functions from 0x020fc8c0 up were folded in from one-function
 * sources. The ball drawer at 0x020fc8c0 was the last to match: its
 * products go through FX_Mul, under opt_prelinearize off. 0x020fcb4c,
 * 0x020fdaf0 and 0x020fe394 depend on spellings that look interchangeable
 * (declaration order, which of two locals is read first); each one is
 * noted at its function.
 *
 * Functions run in ROM order here, lowest address first, under `#pragma
 * defer_codegen off`. Do not reorder.
 *
 * Everything between D0 and StartRound is a dScMgPachinko_c member: every
 * caller passes the scene in r0. The five state tables are pointer-to-member
 * tables on the scene ({code, adjustment} pairs), copied at static init from
 * the pmf_dScMgPachinko_c_* records in ov006 .data: balls (7 states), Bob-ombs
 * (5), pipes (4), the flyer (5) and the background (4).
 */

// leftovers:
// - The func_ and data_ helpers are unnamed in symbols.txt.
// - Ball_AimAtTarget and BobOmb_StateAim keep the field-base addressing
//   above; natural indexing misses in each (AimAtTarget's note gives the
//   sizes; StateAim's note gives its two measurements, 2026-11-13).
// - The BG2 character fills (ClearAimLine, _CheckTargetsLeft,
//   _DrawAimLine, InitResources) go through a volatile zero; in
//   ClearAimLine a by-reference fill wrapper is 7 words off
//   (measured 2026-11-06).

#pragma defer_codegen off

#include "types.h"
#include "PlayerInput.h"
#include "nitro/hw/registers.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "dScMgPachinko_c.h"
#include "LevelRng.h"
#include "math/CalcLib.h"

/* The five state tables are pointer-to-member tables on the scene. */
typedef void (dScMgPachinko_c::*PMF)(int);

struct Entry {
    PMF pmf;
};

/* The sprite record Mg_RenderSpriteBothScaled draws; only handled by pointer here. */
struct OamAttr;

/* The 2x2 rotate-and-scale matrix Ball_DrawAll hands the sprite
   drawer. */
#include "Matrix2x2.h"

/* The two functions that keep the ROM's field-base addressing
 * (Ball_AimAtTarget, BobOmb_StateAim) reach a record field as the scene
 * plus the field's whole-scene offset, then add the pre-scaled index
 * i * 0x38; each one's note gives what mBall[i].f / mShot[i].f measured.
 * BALL views one record already stepped by i * 0x38. */
#define SHOT_BASE ((int)&((dScMgPachinko_c *)0)->mShot)
#define BALL_BASE ((int)&((dScMgPachinko_c *)0)->mBall)
#define BALL(p) ((dScMgPachinko_ball *)((p) + BALL_BASE))
#define SHOT_OFF(member) (SHOT_BASE + (int)&((dScMgPachinko_shot *)0)->member)
#define BALL_OFF(member) (BALL_BASE + (int)&((dScMgPachinko_ball *)0)->member)

/* A random integer in [0, n): the top 15 bits of the shared generator, scaled. */
#define PACHINKO_RAND(n) (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * (n)) >> 15)

namespace cstd {
s16 atan2(s32, s32);
int sqrt(unsigned long long val);
}

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

namespace s20b2444 {
void Mg_DrawBigNumber(int, int, int, int, int, int, int);
}
using s20b2444::Mg_DrawBigNumber;
namespace s20b1a5c {
void Mg_DrawTopRightCounter(int, int);
}
using s20b1a5c::Mg_DrawTopRightCounter;
namespace s20b19f0 {
void Mg_DrawTopLeftCounter(int);
}
using s20b19f0::Mg_DrawTopLeftCounter;
namespace s20b0a54 {
void Mg_SetSharedState(int);
}
using s20b0a54::Mg_SetSharedState;
namespace s20b0aa0 {
void FreeGfxSlotsById(int);
}
using s20b0aa0::FreeGfxSlotsById;
namespace s20b04d0 {
void Mg_SetTopScreenYOffset(int);
}
using s20b04d0::Mg_SetTopScreenYOffset;
namespace s20b023c {
void Mg_RenderSpriteBothPalMtx(void *a0, int, int, int, void *a4);
}
using s20b023c::Mg_RenderSpriteBothPalMtx;
namespace s20aff38 {
struct OamAttr;
int Mg_RenderSpriteBothScaled(OamAttr *, int, int, int, int, int, int);
}
using s20aff38::Mg_RenderSpriteBothScaled;
namespace s20afdd0 {
void Mg_RenderSpriteBoth(void *a0, int, int, int, int);
}
using s20afdd0::Mg_RenderSpriteBoth;
namespace s20af2f8 {
void Mg_InitBrush(char *, char, int, int);
}
using s20af2f8::Mg_InitBrush;
namespace s20adb1c {
void Mg_SubmitScore(int);
}
using s20adb1c::Mg_SubmitScore;
namespace s20adc1c {
int Mg_GetTopRecord(void);
}
using s20adc1c::Mg_GetTopRecord;
namespace s20adbc0 {
int Mg_GetSubmittedScore(void);
}
using s20adbc0::Mg_GetSubmittedScore;

extern "C" {

/* --- shared ov004, main and library helpers --- */
}

extern int  func_020126e8(int a);

extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern "C" {

extern void SetBg2Offset(int a, int b);
}

struct OamAttr;
#include "OAM.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_load.h"
#include "nitro/mi/memory.h"
extern "C" {}

extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);

extern void func_02012718(int a, int b);

extern int Sound_PlayIfNotActive(int handle, int player, int id, short unused);
extern "C" {

extern s16 atan2_hold_d_s_mg_pachinko_1(int y, int x);

extern void *LoadFile(int handle);

extern void DecompressLZ16(void *src, void *dst);
}

extern void Deallocate(void *p);

/* local extern: nitro/gx/gx_load.h leaves GX_LoadBG3Scr out until
 * decl_common.h's non-const spelling of it goes. */
extern void GX_LoadBG3Scr(const void *dst, u32 offset, u32 len);

/* --- ROM data this TU reads --- */
extern unsigned char gMainScreenLayers;

extern u8 gSubScreenLayers;

extern void *sPachinkoDecoSprite;

extern unsigned char sPachinkoPipeRiseStartFrames[];

extern unsigned char sPachinkoPipeSinkStartFrames[];

extern unsigned char sPachinkoPipeRiseFrames[];

extern unsigned char sPachinkoPipeSinkFrames[];

extern int   data_ov006_0212eb24[];

extern int sPachinkoPipeKinds[];

extern u8 sPachinkoPipeIdleFrames[];

extern u8 sPachinkoPipeIdleTimes[];

extern s32   data_ov006_0212eb44[];

extern int sPachinkoPipeRiseStops[];

extern s32 sPachinkoPipeSinkStops[];

extern int sPachinkoTargetX[];

extern int sPachinkoFxSpritesA[];

extern int sPachinkoFxSpritesB[];

extern int sPachinkoPipeSprites[];

extern int sPachinkoTargetSprites[];

extern int sPachinkoFlyerSprites[];

extern int sPachinkoWaveColumnX[];

extern int sPachinkoPopSfx[];

extern int sPachinkoBallSprites[];

extern int sPachinkoBobOmbSprites;

extern u16 sPachinkoPopScores[];

extern u8 sPachinkoWaveSlots[];

/* The five state tables. Static initialisers fill them from the {code
   pointer, adjustment} records in ov006 .data. */
extern Entry sPachinkoBgStateTable[];

extern Entry sPachinkoPipeStateTable[];

extern Entry sPachinkoBobOmbStateTable[];

extern PMF sPachinkoFlyerStateTable[];

extern Entry sPachinkoBallStateTable[];

/* The ROM {code pointer, adjustment} records the five tables copy from
   (unlicensed .data), grouped by destination table in retail copy order. */
extern PMF pmf_dScMgPachinko_c_BobOmb_StateWaitGrab;

extern PMF pmf_dScMgPachinko_c_BobOmb_StateAim;

extern PMF pmf_dScMgPachinko_c_BobOmb_StateFlying;

extern PMF pmf_dScMgPachinko_c_BobOmb_StateFallAway;

extern PMF pmf_dScMgPachinko_c_BobOmb_StateRemove;

extern PMF pmf_dScMgPachinko_c_Ball_StateWaiting;

extern PMF pmf_dScMgPachinko_c_Ball_StateFall;

extern PMF pmf_dScMgPachinko_c_Ball_StateDrift;

extern PMF pmf_dScMgPachinko_c_Ball_StateDive;

extern PMF pmf_dScMgPachinko_c_Ball_StateGlide;

extern PMF pmf_dScMgPachinko_c_Ball_StateSpinAway;

extern PMF pmf_dScMgPachinko_c_Ball_StatePop;

extern PMF pmf_dScMgPachinko_c_Pipe_StatePick;

extern PMF pmf_dScMgPachinko_c_Pipe_StateRise;

extern PMF pmf_dScMgPachinko_c_Pipe_StateSink;

extern PMF pmf_dScMgPachinko_c_Pipe_StateIdle;

extern PMF pmf_dScMgPachinko_c_Flyer_StateWaiting;

extern PMF pmf_dScMgPachinko_c_Flyer_StateDescend;

extern PMF pmf_dScMgPachinko_c_Flyer_StateCross;

extern PMF pmf_dScMgPachinko_c_Flyer_StatePause;

extern PMF pmf_dScMgPachinko_c_Flyer_StateLeave;

extern PMF pmf_dScMgPachinko_c_Bg_StateWaiting;

extern PMF pmf_dScMgPachinko_c_Bg_StateRiseIn;

extern PMF pmf_dScMgPachinko_c_Bg_StateScroll;

extern PMF pmf_dScMgPachinko_c_Bg_StateSinkOut;
/* extern "C" */

/* Both D1 and D0 come from this one definition. */
dScMgPachinko_c::~dScMgPachinko_c()
{
}

void dScMgPachinko_c::Deco_DrawAll() {
    for (int i = 0; i < 3; i++) {
        if (mRow[i].visible != 0) {
            OAM::Render(false, (OamAttr *)sPachinkoDecoSprite, mRow[i].x >> 12, mRow[i].y >> 12, -1,
                        -1, 0x1000, 0x1000, mRow[i].angle, -1);
        }
    }
}

void dScMgPachinko_c::Deco_UpdateAll() {
    int i;

    for (i = 0; i < 3; i++) {
        mRow[i].x = (data_ov006_0212eb44[i] << 12) - mBg[0].x;
        mRow[i].y = -mBg[0].y;
        if (mRow[i].dir != 0) {
            mRow[i].angle -= 0x40;
            if ((s16)mRow[i].angle <= -0x1000) {
                mRow[i].angle = -0x1000;
                mRow[i].dir = 0;
            }
        } else {
            mRow[i].angle += 0x40;
            if ((s16)mRow[i].angle >= 0x1000) {
                mRow[i].angle = 0x1000;
                mRow[i].dir++;
            }
        }
    }
}

void dScMgPachinko_c::Deco_ShowAll() {
    int i;
    for (i = 0; i < 3; i++) {
        if (mRow[i].active == 0) {
            mRow[i].active = 1;
            mRow[i].visible = 1;
            mRow[i].x = (data_ov006_0212eb44[i] << 12) - mBg[0].x;
            mRow[i].y = -mBg[0].y;
            mRow[i].unk12 = 0;
            mRow[i].angle = 0;
            mRow[i].dir = 0;
        }
    }
}

void dScMgPachinko_c::Deco_HideAll() {
    int i;
    for (i = 0; i < 3; i++) {
        mRow[i].active = 0;
        mRow[i].visible = 0;
    }
}

void dScMgPachinko_c::Bg_StateSinkOut(int i) {
    int y;
    mBg[i].x += mBg[i].vx;
    mBg[i].y += mBg[i].vy;
    if (mBg[i].vx <= -0x1000)
        mBg[i].vx += 0x40;
    if (mBg[i].vy >= 0x3000)
        mBg[i].vy += 0x100;
    y = mBg[i].y >> 12;
    SetBg2Offset(mBg[i].x >> 12, y);
    if (y < 0x40) return;
    mBg[i].timer = 0x258;
    mBg[i].state = 0;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    Deco_HideAll();
}

void dScMgPachinko_c::Bg_StateScroll(int i) {
    int x;

    mBg[i].x += mBg[i].vx;
    if (mBg[i].vx <= -0x2000)
        mBg[i].vx -= 0x100;

    x = mBg[i].x >> 12;
    SetBg2Offset(x, mBg[i].y >> 12);

    if (x > -0x90) return;

    mBg[i].vy = 0x800;
    mBg[i].state = 3;
}

void dScMgPachinko_c::Bg_StateRiseIn(int i) {
    int y;
    int x;

    mBg[i].x += mBg[i].vx;
    mBg[i].y += mBg[i].vy;
    if (mBg[i].vx <= -0x2000)
        mBg[i].vx -= 0x100;
    if (mBg[i].vy <= -0xc00)
        mBg[i].vy += 0x40;
    x = mBg[i].x >> 12;
    y = mBg[i].y >> 12;
    if (y <= 0) {
        y = 0;
        mBg[i].y = 0;
        mBg[i].vy = 0;
        mBg[i].state = 2;
    }
    SetBg2Offset(x, y);
}

void dScMgPachinko_c::Bg_StateWaiting(int idx) {
    int x, y;

    if (mPhase == 2)
        return;
    if (mBg[idx].timer != 0) {
        mBg[idx].timer--;
        return;
    }
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    mBg[idx].x = 0x100000;
    mBg[idx].y = 0x40000;
    mBg[idx].vx = -0x1000;
    mBg[idx].vy = -0x1000;
    x = mBg[idx].x >> 12;
    y = mBg[idx].y >> 12;
    mBg[idx].state = 1;
    G2_SetBG2Offset(x, y);
    Deco_ShowAll();
}

/* Dispatches through sPachinkoBgStateTable (0x02142604). */
void dScMgPachinko_c::Bg_Update() {
    if (mBg[0].active) {
        (this->*(sPachinkoBgStateTable[mBg[0].state].pmf))(0);
    }
    Deco_UpdateAll();
}

void dScMgPachinko_c::Bg_Start() {
    mBg[0].active = 1;
    mBg[0].x = 0x100000;
    mBg[0].y = 0x40000;
    mBg[0].vx = 0;
    mBg[0].vy = 0;
    mBg[0].state = 0;
    mBg[0].timer = 0;
    G2_SetBG2Offset(mBg[0].x >> 12, mBg[0].y >> 12);
}

void dScMgPachinko_c::Bg_Stop() {
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    mBg[0].active = 0;
}

void dScMgPachinko_c::Target_DrawAll() {
    int i;
    for (i = 0; i < 4; i++) {
        if (mTarget[i].visible != 0) {
            Mg_RenderSpriteBoth((void *)sPachinkoTargetSprites[mTarget[i].sprite],
                                mTarget[i].x >> 0xc, mTarget[i].y >> 0xc, -1, 1);
        }
    }
}

void dScMgPachinko_c::Target_ShowAll() {
    int i;
    for (i = 0; i < 4; i++) {
        mTarget[i].active = 1;
        mTarget[i].visible = 1;
        mTarget[i].x = sPachinkoTargetX[i] << 12;
        mTarget[i].y = 0xa0000;
        mTarget[i].sprite = 0;
    }
}

void dScMgPachinko_c::Target_HideAll() {
    int i;
    for (i = 0; i < 4; i++) {
        mTarget[i].active = 0;
        mTarget[i].visible = 0;
    }
}

void dScMgPachinko_c::Flyer_Draw() {
    if (mFlyer[0].visible == 0)
        return;
    int x = mFlyer[0].x;
    int y = mFlyer[0].y;
    Mg_RenderSpriteBoth((void *)sPachinkoFlyerSprites[mFlyer[0].facing], x >> 0xc, y >> 0xc, -1,
                        -1);
}

void dScMgPachinko_c::Flyer_CheckHits(int idx) {
    int i;
    int j;
    int dx;
    int dy;
    u8 state;

    if (mFlyer[idx].state == 0)
        return;

    for (i = 0; i < 0x30; i++) {
        if (mShot[i].active == 0)
            continue;
        if (mShot[i].state < 2)
            continue;
        dx = (mFlyer[idx].x - mShot[i].x) >> 12;
        dy = (mFlyer[idx].y - mShot[i].y) >> 12;
        if (cstd::sqrt(dx * dx + dy * dy) > 0x18)
            continue;

        mHitPause = 0x20;
        mShot[i].active = 0;
        mShot[i].visible = 0;
        Fx_Spawn(mFlyer[idx].x, mFlyer[idx].y, 2, 0, 0);
        for (j = 0; j < 0x1e; j++) {
            if (mBall[j].active == 0)
                continue;
            state = mBall[j].state;
            if (state == 0)
                continue;
            if (state < 5) {
                mBall[j].state = 6;
                mBall[j].timer = 0x10;
            }
        }
        mFlyer[idx].visible = 0;
        mFlyer[idx].active = 0;
        mFlyerOut = 0;
        Sound::PlayBank2_2D(0x18a);
        return;
    }
}

void dScMgPachinko_c::Flyer_StateLeave(int i) {
    mFlyer[i].sound =
        func_02012468(mFlyer[i].sound, 2, 0x189, 4, 0, 0, func_020126e8(mFlyer[i].x), 0);
    mFlyer[i].x += mFlyer[i].vx;
    if ((mFlyer[i].x >> 12) >= 0x110 || (mFlyer[i].x >> 12) <= -0x10) {
        mFlyer[i].active = 0;
        mFlyer[i].visible = 0;
        mFlyerOut = 0;
    }
}

void dScMgPachinko_c::Flyer_StatePause(int i) {
    if (mFlyer[i].delay != 0) {
        mFlyer[i].delay--;
        if ((s16)mFlyer[i].delay < 0)
            mFlyer[i].delay = 0;
        return;
    }
    mFlyer[i].state = 2;
    mFlyer[i].passes--;
}

void dScMgPachinko_c::Flyer_StateCross(int i) {
    int x;
    u8 flag;
    mFlyer[i].sound =
        func_02012468(mFlyer[i].sound, 2, 0x189, 4, 0, 0, func_020126e8(mFlyer[i].x), 0);
    mFlyer[i].x += mFlyer[i].vx;
    flag = mFlyer[i].facing;
    x = mFlyer[i].x >> 12;
    if (flag != 0) {
        if (x >= 0xc0)
            mFlyer[i].vx -= 0x80;
        else if (mFlyer[i].vx <= 0x3000)
            mFlyer[i].vx += 0x200;
        if (x >= 0xf0) {
            if (mFlyer[i].passes == 0) {
                mFlyer[i].state = 4;
                return;
            }
            mFlyer[i].x = 0xf0000;
            mFlyer[i].vx = 0;
            mFlyer[i].facing ^= 1;
            mFlyer[i].delay = PACHINKO_RAND(8) * 2 + 0x10;
            mFlyer[i].state = 3;
            return;
        }
    } else {
        if (x <= 0x40)
            mFlyer[i].vx += 0x80;
        else if (mFlyer[i].vx >= -0x3000)
            mFlyer[i].vx -= 0x200;
        if (x <= 0x10) {
            if (mFlyer[i].passes == 0) {
                mFlyer[i].state = 4;
                return;
            }
            mFlyer[i].x = 0x10000;
            mFlyer[i].vx = 0;
            mFlyer[i].facing ^= 1;
            mFlyer[i].delay = PACHINKO_RAND(8) * 2 + 0x10;
            mFlyer[i].state = 3;
            return;
        }
    }
}

/* Moves flyer i down: y += vy. While y is still below -0xc0 it slows
   (vy -= 0x60); once it reaches -0xc0 it snaps to -0xc0000 and stops.
   Then hands on to Flyer_StateCross. */
void dScMgPachinko_c::Flyer_StateDescend(int i) {
    mFlyer[i].y += mFlyer[i].vy;
    if (mFlyer[i].y >> 12 >= -0xc0) {
        mFlyer[i].y = -0xc0000;
        mFlyer[i].vy = 0;
    } else {
        mFlyer[i].vy -= 0x60;
    }
    Flyer_StateCross(i);
}

void dScMgPachinko_c::Flyer_StateWaiting(int idx) {
    unsigned int side;
    if (mFlyer[idx].delay != 0) {
        mFlyer[idx].delay--;
        if ((s16)mFlyer[idx].delay < 0)
            mFlyer[idx].delay = 0;
        return;
    }
    Sound::PlayBank2_2D(0x188);
    mFlyer[idx].state = 1;
    side = PACHINKO_RAND(2);
    mFlyer[idx].facing = side;
    if (side != 0) {
        mFlyer[idx].visible = 1;
        mFlyer[idx].x = -0x10000;
        mFlyer[idx].vx = 0x400;
    } else {
        mFlyer[idx].visible = 1;
        mFlyer[idx].x = 0x110000;
        mFlyer[idx].vx = -0x400;
    }
    mFlyer[idx].y = -0xf8000;
    mFlyer[idx].passes = PACHINKO_RAND(3) + 2;
    mFlyer[idx].vy = 0x2000;
}

/* Dispatches through sPachinkoFlyerStateTable (0x0214266c). */
void dScMgPachinko_c::Flyer_Update() {
    if (mFlyer[0].active == 0)
        return;
    (this->*sPachinkoFlyerStateTable[mFlyer[0].state])(0);
    Flyer_CheckHits(0);
}

void dScMgPachinko_c::Target_Choose() {
    int targets;
    int i;
    if (mFlyerOut != 0) {
        return;
    }
    mFlyer[0].active = 1;
    mFlyer[0].state = 0;
    mFlyer[0].sound = 0;
    mFlyer[0].delay = (PACHINKO_RAND(0x20) << 3) + 0x200;
    targets = 0;
    for (i = 0; i < 4; i++) {
        if (mTarget[i].active != 0) {
            targets++;
        }
    }

    if (targets == 1) {
        mFlyer[0].delay = (PACHINKO_RAND(0x20) << 2) + 0x200;
    }
    mFlyerOut++;
}

void dScMgPachinko_c::Fx_DrawAll() {
    int i;
    for (i = 0; i < 30; i++) {
        if (mFx[i].visible) {
            int type = mFx[i].kind;
            int x = mFx[i].x >> 12;
            int y = mFx[i].y >> 12;
            int idx = mFx[i].frame;
            if (type != 2) {
                Mg_RenderSpriteBoth((void *)sPachinkoFxSpritesA[idx], x, y, -1, -1);
            } else {
                Mg_RenderSpriteBoth((void *)sPachinkoFxSpritesB[idx], x, y, -1, -1);
            }
        }
    }
}

void dScMgPachinko_c::Fx_UpdateAll() {
    int i;
    int lastFrame;
    u8 target;

    for (i = 0; i < 0x1e; i++) {
        if (mFx[i].active == 0)
            continue;
        mFx[i].timer++;
        if (mFx[i].timer < 6)
            continue;
        mFx[i].timer = 0;
        mFx[i].frame++;
        lastFrame = 3;
        if (mFx[i].kind == 2)
            lastFrame = 5;
        target = mFx[i].target;
        if (target != 0 && lastFrame - 2 == mFx[i].frame) {
            mTarget[target - 1].active = 0;
            mTarget[target - 1].sprite = 1;
        }
        if (mFx[i].frame >= lastFrame) {
            mFx[i].visible = 0;
            mFx[i].active = 0;
            if (mFx[i].points != 0)
                Popup_SpawnScore(i, mFx[i].points);
        }
    }
}

void dScMgPachinko_c::Fx_Spawn(int x, int y, int kind, int points, int target) {
    int i;
    for (i = 0; i < 0x1e; i++) {
        if (mFx[i].active == 0) {
            mFx[i].x = x;
            mFx[i].y = y;
            mFx[i].active = 1;
            mFx[i].visible = 1;
            mFx[i].timer = 0;
            mFx[i].frame = 0;
            mFx[i].kind = kind;
            mFx[i].points = points;
            mFx[i].target = target;
            return;
        }
    }
}

void dScMgPachinko_c::CheckTargetsLeft() {
    volatile unsigned short zero;
    int count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (mTarget[i].active != 0) {
            count++;
            break;
        }
    }
    if (count != 0)
        return;
    mPhase = 2;
    mEndTimer = 0x40;
    {
        unsigned chars = (unsigned)G2S_GetBG2CharPtr();
        zero = 0;
        MI_CpuFill16((void *)chars, zero, 0x6000);
    }
    mUnused5c30 = 0;
    BobOmb_ClearAll();
    Pipe_SetAll(1);
}

/* Does not read `this`. */
void dScMgPachinko_c::DrawScore() {
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
}

/* Draws the high score: the stored best comes back from Mg_GetTopRecord
   as a full word and goes straight to the HUD's number drawer. Does not
   read `this`. */
void dScMgPachinko_c::DrawTopRecord() {
    Mg_DrawTopLeftCounter(Mg_GetTopRecord());
}

void dScMgPachinko_c::Popup_DrawAll() {
    int i;
    for (i = 0; i < 0x1e; i++) {
        if (mPopup[i].timer != 0)
            Mg_DrawBigNumber(mPopup[i].x >> 12, mPopup[i].y >> 12, mPopup[i].points, -1, -1, 0, 0);
    }
}

void dScMgPachinko_c::Popup_UpdateAll() {
    int i;
    for (i = 0; i < 0x1e; i++) {
        if (mPopup[i].timer != 0) {
            mPopup[i].timer--;
            if ((s16)mPopup[i].timer < 0)
                mPopup[i].timer = 0;
        }
    }
}

void dScMgPachinko_c::Popup_SpawnScore(int idx, unsigned short points) {
    int i;
    for (i = 0x1d; i >= 0; i--) {
        if (mPopup[i].timer != 0)
            continue;
        mPopup[i].x = mFx[idx].x;
        mPopup[i].y = mFx[idx].y;
        mPopup[i].timer = 0x30;
        mPopup[i].points = points;
        Mg_SubmitScore(mPopup[i].points + Mg_GetSubmittedScore());
        if ((unsigned int)Mg_GetSubmittedScore() < 0xbb8) return;
        if (mPromptEnabled != 0)
            mPromptEnabled = 0;
        return;
    }
}

void dScMgPachinko_c::ClearAimLine() {
    volatile unsigned short zero;
    int shotNo = mAimShot;
    int dx, dy;

    if (shotNo == 0) return;

    dx = 0x80 - (mShot[shotNo - 1].x >> 12);
    dy = 0x20 - (mShot[shotNo - 1].y >> 12);

    if (dx < -6) return;
    if (dx > 6) return;
    if (dy < -6) return;
    if (dy > 6) return;
    if (mShot[shotNo - 1].state != 2)
        return;

    mAimShot = 0;
    {
        char *dst = (char *)G2S_GetBG2CharPtr();
        zero = 0;
        MI_CpuFill16(dst, zero, 0x6000);
    }
}

#pragma push
#pragma opt_loop_invariants off
/* Sets the 2x2 block of pixels centred on (x, y) in the sub BG2 character
   data, 4bpp tiles laid out 32 to a row: y picks the tile row and the
   4-byte line inside the tile, x the tile column and the nibble. It does
   not read `this`.
   opt_loop_invariants stays off: the ROM keeps j on the stack and every spelling without these staged locals is a word short (0x7c of 0x80). */
void dScMgPachinko_c::PlotSubBg2Pixels(int x, int y, int colour) {
    int i, j, py, line, tileY, lineOfs, rowTile, xb, x0;
    u32 *bp;
    x0 = x;
    for (i = 0; i < 2; i++)
    {
        py = y - 1 + i;
        line = py & 7;
        tileY = py >> 3;
        lineOfs = line * 4;
        for (j = 0; j < 2; j++)
        {
            rowTile = tileY * 32;
            xb = x0 - 1;
            x = xb + j;
            bp = (u32 *)G2S_GetBG2CharPtr();
            *(u32 *)((char *)(bp + (rowTile + (x >> 3)) * 8) + lineOfs) |= colour << ((x & 7) * 4);
        }
    }
}
#pragma pop

void dScMgPachinko_c::DrawAimLine() {
    int i;
    int idx;
    int tx;
    int ty;
    int cx;
    int cy;
    int dx;
    int adx;
    int dy;
    int ady;
    volatile u16 fill;

    if (mAimShot == 0)
        return;
    idx = mAimShot - 1;
    if (mShot[idx].x != mShot[idx].prevX || mShot[idx].y != mShot[idx].prevY) {
        char *buf = (char *)G2S_GetBG2CharPtr();
        fill = 0;
        MI_CpuFill16(buf, fill, 0x6000);
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cx = 0x6c;
                tx = mShot[idx].x >> 12;
                cy = 0x22;
                ty = mShot[idx].y >> 12;
                if (ty >= 0x22)
                    ty += 4;
                else
                    ty -= 4;
                tx -= 4;
            } else {
                cx = 0x94;
                tx = mShot[idx].x >> 12;
                cy = 0x22;
                ty = mShot[idx].y >> 12;
                if (ty >= 0x22)
                    ty += 4;
                else
                    ty -= 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;
            if (adx >= ady) {
                mLineErr = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        PlotSubBg2Pixels(cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        mLineErr += ady;
                        if (mLineErr > adx) {
                            if (dy >= 0) cy++; else cy--;
                            mLineErr -= adx;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        mLineErr += ady;
                        if (mLineErr > adx) {
                            if (dy >= 0) cy++; else cy--;
                            mLineErr -= adx;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                mLineErr = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        PlotSubBg2Pixels(cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        mLineErr += adx;
                        if (mLineErr > ady) {
                            if (dx >= 0) cx++; else cx--;
                            mLineErr -= ady;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        mLineErr += adx;
                        if (mLineErr > ady) {
                            if (dx >= 0) cx++; else cx--;
                            mLineErr -= ady;
                        }
                        PlotSubBg2Pixels(cx, cy, 2);
                        if (cy == ty)
                            break;
                    }
                }
            }
        }
    }
    mShot[idx].prevX = mShot[idx].x;
    mShot[idx].prevY = mShot[idx].y;
}

void dScMgPachinko_c::Pipe_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mPipe[i].visible != 0)
            Mg_RenderSpriteBoth((void *)sPachinkoPipeSprites[mPipe[i].sprite], mPipe[i].x >> 12,
                                mPipe[i].y >> 12, -1, 2);
    }
}

void dScMgPachinko_c::Pipe_SetAll(int phase) {
    int i;
    for (i = 0; i < 2; i++) {
        if (mPipe[i].active) {
            mPipe[i].state = 3;
            mPipe[i].laps = 0;
            mPipe[i].phase = phase;
        }
    }
}

int dScMgPachinko_c::Pipe_StateIdle(int idx) {
    u16 *counter = &mPipe[idx].timer;
    u8 *frame = &mPipe[idx].frame;
    u16 c = *counter;
    *counter = c + 1;
    u8 oldFrame = *frame;
    if (*counter >= sPachinkoPipeIdleTimes[oldFrame]) {
        *counter = 0;
        *frame = *frame + 1;
        if (*frame >= 8) {
            *frame = 0;
            mPipe[idx].laps = mPipe[idx].laps + 1;
        }
        mPipe[idx].sprite = sPachinkoPipeIdleFrames[*frame];
    }
    {
        if (mPipe[idx].phase != 0) {
            mPipe[idx].laps = 0;
            return 0;
        }
    }
    {
        u8 c3 = mPipe[idx].laps;
        if (c3 >= 2) {
            c3 = 0;
            mPipe[idx].state = c3;
            mPipe[idx].laps = c3;
        }
        return c3;
    }
}

void dScMgPachinko_c::Pipe_StateSink(int i) {
    int x;

    if (mPipe[i].phase != 0) {
        mPipe[i].timer++;
        if (mPipe[i].timer < 4)
            return;
        mPipe[i].timer = 0;
        mPipe[i].frame++;
        if (mPipe[i].frame >= 3) {
            mPipe[i].frame = 0;
            mPipe[i].state = 1;
            mPipe[i].phase = 0;
            return;
        }
        mPipe[i].sprite = sPachinkoPipeSinkStartFrames[mPipe[i].frame];
        return;
    }

    mPipe[i].timer++;
    if (mPipe[i].timer >= 4) {
        mPipe[i].timer = 0;
        mPipe[i].frame++;
        if (mPipe[i].frame >= 6)
            mPipe[i].frame = 0;
        mPipe[i].sprite = sPachinkoPipeSinkFrames[mPipe[i].frame];
    }

    mPipe[i].x += mPipe[i].vel;
    x = mPipe[i].x >> 12;
    if (x >= sPachinkoPipeSinkStops[i + 2]) {
        if (mPipe[i].vel >= 0x800)
            mPipe[i].vel -= 0x80;
    } else {
        mPipe[i].vel += 0x80;
    }
    if (x < sPachinkoPipeSinkStops[i])
        return;

    mPipe[i].x = sPachinkoPipeSinkStops[i] << 12;
    mPipe[i].vel = -0x1000;
    mPipe[i].phase++;
    mPipe[i].timer = 0;
    mPipe[i].frame = 0;
    mPipe[i].sprite = sPachinkoPipeSinkStartFrames[0];
}

void dScMgPachinko_c::Pipe_StateRise(int i) {
    int x;

    if (mPipe[i].phase != 0) {
        (mPipe[i].timer)++;
        if (mPipe[i].timer < 4) {
            return;
        }
        mPipe[i].timer = 0;
        (mPipe[i].frame)++;
        if (mPipe[i].frame >= 3) {
            mPipe[i].frame = 0;
            mPipe[i].state = 2;
            mPipe[i].phase = 0;
            return;
        }
        mPipe[i].sprite = sPachinkoPipeRiseStartFrames[mPipe[i].frame];
        return;
    }

    (mPipe[i].timer)++;
    if (mPipe[i].timer >= 4) {
        mPipe[i].timer = 0;
        (mPipe[i].frame)++;
        if (mPipe[i].frame >= 6) {
            mPipe[i].frame = 0;
        }
        mPipe[i].sprite = sPachinkoPipeRiseFrames[mPipe[i].frame];
    }

    mPipe[i].x = mPipe[i].x + mPipe[i].vel;
    x = mPipe[i].x >> 0xc;
    if (x <= sPachinkoPipeRiseStops[i + 2]) {
        if (mPipe[i].vel <= -0x800) {
            mPipe[i].vel = mPipe[i].vel + 0x80;
        }
    } else {
        mPipe[i].vel = mPipe[i].vel - 0x80;
    }

    if (x > sPachinkoPipeRiseStops[i]) {
        return;
    }
    mPipe[i].x = sPachinkoPipeRiseStops[i] << 0xc;
    mPipe[i].vel = 0x1000;
    (mPipe[i].phase)++;
    mPipe[i].timer = 0;
    mPipe[i].frame = 0;
    mPipe[i].sprite = sPachinkoPipeRiseStartFrames[0];
}

/* Rolls a random kind for pipe n from sPachinkoPipeKinds and resets it to
   rise (or, for kind 2, sink). */
void dScMgPachinko_c::Pipe_StatePick(int n) {
    int rnd = RandomIntInternal(&data_0209d4b8.value);
    int kind = sPachinkoPipeKinds[((((unsigned)rnd >> 16) & 0x7fff) * 2) >> 15];
    mPipe[n].state = kind;
    mPipe[n].phase = 0;
    if (mPipe[n].state == 2) {
        mPipe[n].sinking = 1;
        mPipe[n].vel = 0x1000;
    } else {
        mPipe[n].sinking = 0;
        mPipe[n].vel = -0x1000;
    }
    mPipe[n].timer = 0;
    mPipe[n].frame = 0;
    mPipe[n].visible = 1;
}

/* Dispatches through sPachinkoPipeStateTable (0x02142624). */
void dScMgPachinko_c::Pipe_UpdateAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mPipe[i].active != 0) {
            (this->*sPachinkoPipeStateTable[mPipe[i].state].pmf)(i);
        }
    }
}

void dScMgPachinko_c::Pipe_ResetAll() {
    int i;
    for (i = 0; i < 2; i++) {
        mPipe[i].active = 1;
        mPipe[i].visible = 0;
        mPipe[i].state = 0;
        mPipe[i].phase = 0;
        mPipe[i].sprite = 0;
        mPipe[i].timer = 0;
        mPipe[i].x = data_ov006_0212eb24[i] << 12;
        mPipe[i].y = 0x68000;
        mPipe[i].vel = 0;
        mPipe[i].velY = 0;
        mPipe[i].sinking = 0;
        mPipe[i].laps = 0;
    }
}

#pragma push
#pragma opt_prelinearize off
static inline int FX_Mul(int v1, int v2)
{
    s64 t = (s64)v1 * v2;
    return (int)((t + 0x800) >> 12);
}

/* Draws the 30 balls. A ball with `visible` set is drawn with the sprite
 * `sprite` picks, at its position plus offX/offY, through a 2x2
 * rotate-and-scale matrix: `rotation` is the angle (>> 4 indexes the shared
 * sine/cosine table) and the 20.12 `scale` is the scale. The products go through FX_Mul; spelled out in the body
 * they put the scale in the first smull's Rm instead of the table value. With
 * opt_prelinearize on, every callee-saved register in the loop rotates by one
 * place. */
void dScMgPachinko_c::Ball_DrawAll() {
    int i;
    for (i = 0; i < 30; i++) {
        if (mBall[i].visible != 0) {
            int scale = mBall[i].scale;
            int sprite = mBall[i].sprite;
            Matrix2x2 m;
            int cosv = FX_Mul(FX_CosIdx(mBall[i].rotation >> 4), scale);
            int sinv = FX_Mul(FX_SinIdx(mBall[i].rotation >> 4), scale);
            int x = (mBall[i].x + mBall[i].offX) >> 12;
            int y = (mBall[i].y + mBall[i].offY) >> 12;
            m.m00 = cosv;
            m.m01 = sinv;
            m.m10 = -sinv;
            m.m11 = cosv;
            Mg_RenderSpriteBothPalMtx((void *)sPachinkoBallSprites[sprite], x, y, -1, &m);
        }
    }
}
#pragma pop

void dScMgPachinko_c::Ball_CountNearby(int i) {
    int x, y, cnt;

    if (mBall[i].state == 5)
        return;
    x = mBall[i].x >> 12;
    y = mBall[i].y >> 12;
    cnt = 0;
    if (y >= 0xd0) cnt++;
    if (x >= 0x120 || x <= -0x20) cnt++;
    if (cnt != 0) {
        mBall[i].active = 0;
        mBall[i].visible = 0;
    }
}

void dScMgPachinko_c::Ball_CheckPipes(int idx) {
    int i;
    int dx;
    int dy;

    if (mBall[idx].state == 5)
        return;

    for (i = 0; i < 4; i++) {
        if (mTarget[i].active == 0)
            continue;

        dx = (mTarget[i].x - mBall[idx].x) >> 12;
        dy = (mTarget[i].y - mBall[idx].y) >> 12;
        if (dx < -0x10 || dx > 0x18 || dy < -0x10 || dy > 0x18)
            continue;

        mBall[idx].active = 0;
        mBall[idx].visible = 0;
        Fx_Spawn(mTarget[i].x, mTarget[i].y, 2, 0, i + 1);
        Pipe_SetAll(0);
        func_02012718(0x18c, mBall[idx].x);
        return;
    }
}

#pragma push
#pragma opt_common_subs off
/* Re-aims ball `i` at a randomly chosen active target. Bails out if no
 * mTarget slot is active, or if the ball has sunk below y=0. Takes the length
 * of the ball's current velocity vector, scales it to 9/10, picks a random
 * active mTarget, and rewrites the velocity as that speed pointed along
 * atan2 towards the chosen target, then fires a sound whose pan comes from
 * the ball's x.
 *
 * The two velocity-component base offsets must be introduced in the order
 * below: the constant pool emits 0x4668 before 0x466c, and the declaration
 * order is what fixes it.
 * opt_common_subs stays off: typed mBall[i] spellings land at 0x1fc-0x260 of 0x240 with or without it, so the field-base forms remain. */
void dScMgPachinko_c::Ball_AimAtTarget(int i) {
    int idx;
    int n;
    int factor;
    int dist;
    int *pY;
    int *pVx;
    int *pVy;
    char *angBase;
    s16 angle;
    int count = 0, j;
    for (j = 0; j < 4; j++) {
        if (mTarget[j].active != 0) {
            count++;
            break;
        }
    }
    if (!count)
        return;
    n = i * 0x38;
    {
        char *yBase = (char *)this + BALL_OFF(y);
        int y = *(int *)(yBase + n);
        pY = (int *)(yBase + n);
        if ((y >> 12) < 0)
            return;
    }
    {
        char *velXBase = (char *)this + BALL_OFF(velX);
        char *velYBase = (char *)this + BALL_OFF(velY);
        int a = *(int *)(velXBase + n);
        s64 distSq = (s64)a * a;
        int b = *(int *)(velYBase + n);
        pVx = (int *)(velXBase + n);
        distSq += (s64)b * b;
        pVy = (int *)(velYBase + n);
        dist = (int)cstd::sqrt((u64)distSq);
    }
    factor = dist * 9 / 10;
    idx = PACHINKO_RAND(4);
    for (;;) {
        if (mTarget[idx].active != 0)
            break;
        idx = PACHINKO_RAND(4);
    }
    {
        char *xBase = (char *)this + BALL_OFF(x);
        int targetY = mTarget[idx].y;
        int yval = *pY;
        int targetX = mTarget[idx].x;
        int entX = *(int *)(xBase + n);
        angBase = (char *)this + BALL_OFF(angle);
        angle = cstd::atan2((targetY - yval) >> 12, (targetX - entX) >> 12);
        *(u16 *)(angBase + n) = (u16)angle;
        u16 angU = *(u16 *)(angBase + n);
        {
            fx16 cosv = FX_CosIdx(angU >> 4);
            *pVx = FX_MUL(cosv, factor);
        }
        {
            u16 angU2 = *(u16 *)(angBase + n);
            fx16 sinv = FX_SinIdx(angU2 >> 4);
            *pVy = FX_MUL(sinv, factor);
        }
        int fieldByte = BALL((char *)this + n)->level;
        int div40 = (fieldByte * 0x180) / 40;
        int e8ret = func_020126e8(*(int *)(xBase + n));
        func_020126ac(0x186, 6, 0, div40 - 0x80, e8ret);
        BALL((char *)this + n)->state = 4;
        BALL((char *)this + n)->target = (u8)idx;
    }
}
#pragma pop

void dScMgPachinko_c::Ball_CheckFall(int idx) {
    if (mBall[idx].fallPlayed != 0)
        return;
    if ((mBall[idx].y >> 0xc) < -0xf2)
        return;
    mBall[idx].fallPlayed++;
    func_02012718(0x184, mBall[idx].x);
}

void dScMgPachinko_c::Ball_StatePop(int i) {
    if (mBall[i].timer != 0) {
        mBall[i].timer--;
        if (mBall[i].timer != 0)
            return;
        Fx_Spawn(mBall[i].x, mBall[i].y, 2, sPachinkoPopScores[0], 0);
        func_02012718(0x18b, mBall[i].x);
        return;
    }
    mBall[i].active = 0;
    mBall[i].visible = 0;
}

void dScMgPachinko_c::Ball_StateSpinAway(int i) {
    int n = i * 0x38;
    int x;
    int y;
    int f;

    mBall[i].x += mBall[i].velX;
    mBall[i].y += mBall[i].velY;
    mBall[i].rotation += 0x800;
    if (mBall[i].spinRadius <= 0x30000) {
        mBall[i].spinRadius += 0x800;
    }
    {
        fx16 c = FX_CosIdx(mBall[i].rotation >> 4);
        mBall[i].offX = FX_MUL(c, mBall[i].spinRadius);
    }
    {
        fx16 s = FX_SinIdx(mBall[i].rotation >> 4);
        mBall[i].offY = FX_MUL(s, mBall[i].spinRadius);
    }

    x = (mBall[i].x + mBall[i].offX) >> 12;
    y = (mBall[i].y + mBall[i].offY) >> 12;
    mBall[i].sound = Sound_PlayIfNotActive(mBall[i].sound, 2, 0x187, 0);
    f = 0;
    if (x >= 0x130 || x <= -0x30) {
        f++;
    }
    if (y >= 0xf0 || y <= -0x110) {
        f++;
    }
    if (f != 0) {
        mBall[i].active = 0;
        mBall[i].visible = 0;
    }
}

void dScMgPachinko_c::Ball_StateGlide(int idx) {
    u16 target;

    mBall[idx].x += mBall[idx].velX;
    mBall[idx].y += mBall[idx].velY;
    target = mBall[idx].angle - 0x4000;
    if ((s16)mBall[idx].rotation > (s16)target) {
        mBall[idx].rotation -= 0x100;
        if ((s16)mBall[idx].rotation <= (s16)target)
            mBall[idx].rotation = target;
    } else if ((s16)mBall[idx].rotation < (s16)target) {
        mBall[idx].rotation += 0x100;
        if ((s16)mBall[idx].rotation >= (s16)target)
            mBall[idx].rotation = target;
    }
    Ball_CheckHitByBobOmb(idx);
    Ball_CheckPipes(idx);
    Ball_CountNearby(idx);
}

void dScMgPachinko_c::Ball_StateDive(int i) {
    unsigned short t;
    int lim;

    if (mBall[i].timer != 0) {
        (mBall[i].timer)--;
        if (mBall[i].timer == 0) {
            func_02012718(0x185, mBall[i].x);
        }
    }

    mBall[i].x += mBall[i].velX;
    mBall[i].y += mBall[i].velY;

    t = mWaveLevel;
    if (t > 12)
    {
        lim = 0xd80 + ((t - 12) << 7);
    }
    else
    {
        lim = 0x600 + t * 0xa0;
    }

    if (mBall[i].velY <= lim) {
        mBall[i].velY += t * 4 + 8;
    }

    Ball_CheckHitByBobOmb(i);
    Ball_CheckPipes(i);
    Ball_CountNearby(i);

    mBall[i].angle = cstd::atan2(mBall[i].velY, mBall[i].velX);
    mBall[i].rotation = mBall[i].angle - 0x4000;

    Ball_AimAtTarget(i);
}

void dScMgPachinko_c::Ball_StateDrift(int i) {
    Ball_CheckFall(i);
    if (mBall[i].started == 0) {
        mBall[i].started++;
        if (mWaveLevel > 0xc)
            mBall[i].velY = ((PACHINKO_RAND(8)) << 7) + 0xb80 + ((mWaveLevel - 0xc) << 7);
        else
            mBall[i].velY = mWaveLevel * 0xa0 + (((PACHINKO_RAND(8)) << 7) + 0x400);
        if (PACHINKO_RAND(2) != 0) {
            mBall[i].velX = (mWaveLevel << 7) + 0x600;
            mBall[i].driftDir = 0;
        } else {
            mBall[i].velX = -((mWaveLevel << 7) + 0x600);
            mBall[i].driftDir = 0;
        }
        mBall[i].timer = (PACHINKO_RAND(0x20) << 3) + 0x20;
    } else {
        int x;
        mBall[i].y += mBall[i].velY;
        mBall[i].x += mBall[i].velX;
        x = mBall[i].x >> 12;
        if (x > 0xf0) {
            mBall[i].velX = -mBall[i].velX;
            if (mBall[i].driftDir != 0) {
                if (mBall[i].driftDir == 1)
                    mBall[i].driftDir = 2;
                else
                    mBall[i].driftDir = 1;
            }
            mBall[i].x = 0xf0000;
        }
        if (x < 0x10) {
            mBall[i].velX = -mBall[i].velX;
            if (mBall[i].driftDir != 0) {
                if (mBall[i].driftDir == 1)
                    mBall[i].driftDir = 2;
                else
                    mBall[i].driftDir = 1;
            }
            mBall[i].x = 0x10000;
        }
        Ball_CheckHitByBobOmb(i);
        Ball_CheckPipes(i);
        Ball_CountNearby(i);
        if (mBall[i].timer != 0) {
            mBall[i].timer--;
            if (mBall[i].velX > 0) {
                mBall[i].rotation -= 0x80;
                if ((s16)mBall[i].rotation <= -0x1800)
                    mBall[i].rotation = -0x1800;
            } else if (mBall[i].velX < 0) {
                mBall[i].rotation += 0x80;
                if ((s16)mBall[i].rotation >= 0x1800)
                    mBall[i].rotation = 0x1800;
            }
            if ((s16)mBall[i].timer < 0)
                mBall[i].timer = 0;
            return;
        }
        if (mBall[i].driftDir == 0) {
            if (mBall[i].velX > 0) {
                mBall[i].velX -= 0x20;
                mBall[i].rotation += 0x80;
                if ((s16)mBall[i].rotation >= 0)
                    mBall[i].rotation = 0;
                if (mBall[i].velX <= 0) {
                    mBall[i].driftDir = PACHINKO_RAND(2) + 1;
                    mBall[i].timer = PACHINKO_RAND(0x10) << 2;
                    mBall[i].rotation = 0;
                }
            } else if (mBall[i].velX < 0) {
                mBall[i].velX += 0x20;
                mBall[i].rotation -= 0x80;
                if ((s16)mBall[i].rotation < 0)
                    mBall[i].rotation = 0;
                if (mBall[i].velX >= 0) {
                    mBall[i].driftDir = PACHINKO_RAND(2) + 1;
                    mBall[i].timer = PACHINKO_RAND(0x10) << 2;
                    mBall[i].rotation = 0;
                }
            }
        } else if (mBall[i].driftDir == 1) {
            mBall[i].velX += 0x20;
            mBall[i].rotation -= 0x80;
            if ((s16)mBall[i].rotation <= -0x1800)
                mBall[i].rotation = -0x1800;
            if (mBall[i].velX >= (mWaveLevel << 7) + 0x600) {
                mBall[i].driftDir = 0;
                mBall[i].velX = (mWaveLevel << 7) + 0x600;
                mBall[i].timer = (PACHINKO_RAND(0x20) << 3) + 0x10;
            }
        } else {
            mBall[i].velX -= 0x20;
            mBall[i].rotation += 0x80;
            if ((s16)mBall[i].rotation >= 0x1800)
                mBall[i].rotation = 0x1800;
            if (mBall[i].velX <= -((mWaveLevel << 7) + 0x600)) {
                mBall[i].driftDir = 0;
                mBall[i].velX = -((mWaveLevel << 7) + 0x600);
                mBall[i].timer = (PACHINKO_RAND(0x20) << 3) + 0x10;
            }
        }
    }
    Ball_AimAtTarget(i);
}

void dScMgPachinko_c::Ball_StateFall(int i) {
    if (mBall[i].started == 0) {
        mBall[i].started++;
        if (mWaveLevel > 0xc) {
            mBall[i].velY = (PACHINKO_RAND(8) << 7) + 0xb80 + ((mWaveLevel - 0xc) << 7);
        } else {
            mBall[i].velY = mWaveLevel * 0xa0 + ((PACHINKO_RAND(8) << 7) + 0x400);
        }
    } else {
        mBall[i].y += mBall[i].velY;
    }
    Ball_CheckFall(i);
    Ball_CheckHitByBobOmb(i);
    Ball_CheckPipes(i);
    Ball_CountNearby(i);
    Ball_AimAtTarget(i);
}

void dScMgPachinko_c::Ball_StateWaiting(int idx) {
    if (mBall[idx].timer != 0) {
        mBall[idx].timer--;
        return;
    }
    mBall[idx].state = PACHINKO_RAND(2) + 1;
    mBall[idx].started = 0;
    mBall[idx].visible = 1;
    mBall[idx].sprite = 0;
    mBall[idx].rotation = 0;
    mBall[idx].spinRadius = 0;
    mBall[idx].offX = 0;
    mBall[idx].offY = 0;
    mBall[idx].scale = 0x1000;
    mBall[idx].fallPlayed = 0;
}

void dScMgPachinko_c::Ball_UpdateAll() {
    int i;
    for (i = 0; i < 30; i++) {
        if (mBall[i].active != 0) {
            (this->*sPachinkoBallStateTable[mBall[i].state].pmf)(i);
        }
    }
}

/* Ball `i` against every live Bob-omb: the first shot that has left the
 * launcher (state 2 or later) and lies within 0x18 pixels pops the ball. The
 * ball takes the shot's heading as its spin rotation, starts spinning away
 * (state 5) at 0x2000 along it, plays the pop sound and spawns the burst for
 * the shot's combo (capped at 5), and the shot's combo goes up by one.
 *
 * Two spellings are load-bearing, measured 2026-11-13:
 * - dx is taken before dy, and every operand of the sum of squares carries its
 *   own (s64) cast: (s64)dx * dx moves the loop counter and record base into
 *   other registers (16 words), and dy first emits the y field's offset to the
 *   literal pool ahead of x (5 words).
 * - the cosine and sine of the offset pair go through named fx16 locals;
 *   inline, the spin radius and the table read swap registers (16 words).
 */

void dScMgPachinko_c::Ball_CheckHitByBobOmb(int i) {
    int j;

    for (j = 0; j < 0x30; j++) {
        int dx;
        int dy;
        s64 distsq;

        if (mShot[j].active == 0)
            continue;
        if (mShot[j].state < 2)
            continue;
        dx = (mBall[i].x - mShot[j].x) >> 12;
        dy = (mBall[i].y - mShot[j].y) >> 12;
        distsq = (s64)dx * (s64)dx + (s64)dy * (s64)dy;
        if (cstd::sqrt(distsq) <= 0x18) {
            fx16 cosv;
            fx16 sinv;
            int cc;

            mBall[i].state = 5;
            mBall[i].rotation = mShot[j].angle;
            mBall[i].velX = FX_MUL(FX_CosIdx(mBall[i].rotation >> 4), 0x2000);
            mBall[i].velY = FX_MUL(FX_SinIdx(mBall[i].rotation >> 4), 0x2000);
            mBall[i].spinRadius = 0;
            cosv = FX_CosIdx(mBall[i].rotation >> 4);
            mBall[i].offX = FX_MUL(cosv, mBall[i].spinRadius);
            sinv = FX_SinIdx(mBall[i].rotation >> 4);
            mBall[i].offY = FX_MUL(sinv, mBall[i].spinRadius);
            cc = mShot[j].combo;
            if (cc >= 5)
                cc = 5;
            func_02012718(sPachinkoPopSfx[cc], mBall[i].x);
            Fx_Spawn(mBall[i].x, mBall[i].y, 2, sPachinkoPopScores[cc], 0);
            mShot[j].combo++;
            return;
        }
    }
}

void dScMgPachinko_c::SpawnWave() {
    int i;
    int ones;
    int tens;

    if (mWaveTimer != 0) {
        mWaveTimer--;
        if ((s16)mWaveTimer < 0)
            mWaveTimer = 0;
        return;
    }
    mWaveTimer = 0x50;
    for (i = 0; i < 0x1e; i++) {
        int ra;
        int rb;
        if (mBall[i].active != 0)
            continue;
        mBall[i].active = 1;
        ra = RandomIntInternal(&data_0209d4b8.value);
        rb = RandomIntInternal(&data_0209d4b8.value);
        mBall[i].x = (sPachinkoWaveColumnX[(((u32)rb >> 16) & 0x7fff) * 5 >> 15] +
                      ((((u32)ra >> 16) & 0x7fff) * 4 >> 15) * 8)
                     << 12;
        mBall[i].y = -0x100000;
        mBall[i].velX = 0;
        mBall[i].velY = 0;
        mBall[i].state = 0;
        mBall[i].started = 0;
        ra = RandomIntInternal(&data_0209d4b8.value);
        mBall[i].timer = ((((u32)ra >> 16) & 0x7fff) * 8 >> 15) << 4;
        mBall[i].rotation = 0;
        mBall[i].sound = 0;
        mBall[i].level = mWaveLevel;
        mWaveSpawns++;
        if (mWaveLevel == 0)
            break;
        {
            int r = PACHINKO_RAND(16);
            int lvl = mWaveLevel >> 1;
            if (lvl >= 4) lvl = 4;
            if (sPachinkoWaveSlots[r + lvl * 4] == 0)
                break;
        }
        if (PACHINKO_RAND(2)) {
            mBall[i].x = 0x100000;
            mBall[i].angle = 0xc000 - ((PACHINKO_RAND(12) + 1) << 9);
        } else {
            mBall[i].x = 0;
            mBall[i].angle = ((PACHINKO_RAND(12) + 1) << 9) + 0xc000;
        }
        mBall[i].spinRadius = 0;
        mBall[i].offX = 0;
        mBall[i].offY = 0;
        mBall[i].rotation = mBall[i].angle - 0x4000;
        mBall[i].y = -0x60000;
        mBall[i].velX = FX_MUL(FX_CosIdx(mBall[i].angle >> 4), 0xe80);
        mBall[i].velY = FX_MUL(FX_SinIdx(mBall[i].angle >> 4), 0xe80);
        mBall[i].state = 3;
        mBall[i].visible = 1;
        mBall[i].sprite = 0;
        mBall[i].scale = 0x1000;
        mBall[i].timer = 0x10;
        break;
    }
    ones = mWaveSpawns;
    for (tens = 0; ones >= 0xa; tens++) ones -= 0xa;
    if (tens != 0 && ones == 0)
        mWaveLevel++;
    if (mWaveLevel > 0x28)
        mWaveLevel = 0x27;
    mWaveTimer -= mWaveLevel << 2;
    if ((s16)mWaveTimer <= 0x20)
        mWaveTimer = 0x20;
}

void dScMgPachinko_c::BobOmb_ClearAll() {
    int i;
    for (i = 0; i < 0x30; i++) {
        mShot[i].active = 0;
        mShot[i].visible = 0;
    }
}

void dScMgPachinko_c::BobOmb_DrawAll() {
    int i;
    for (i = 0; i < 0x30; i++) {
        if (mShot[i].visible == 0)
            continue;
        Mg_RenderSpriteBothScaled((s20aff38::OamAttr *)sPachinkoBobOmbSprites, mShot[i].x >> 12,
                                  mShot[i].y >> 12, -1, -1, 0x1000, 0);
    }
}

void dScMgPachinko_c::BobOmb_UpdateAll() {
    int i;
    for (i = 0; i < 48; i++) {
        if (mShot[i].active != 0) {
            (this->*sPachinkoBobOmbStateTable[mShot[i].state].pmf)(i);
        }
    }
}

void dScMgPachinko_c::CountLaunch() {
    if (mPlayStarted == 0)
        mPlayStarted += 1;
}

void dScMgPachinko_c::BobOmb_StateWaitGrab(int i) {
    unsigned int slot = gActivePlayerSlot;
    int grabbed = 0;
    int dx, dy;

    if (((u8(*)[4])gTouchHeld)[slot][0] != 0)
        grabbed = ((u8(*)[4])gTouchEdge)[slot][0] != 0;
    if (grabbed != 0) {
        dx = (mShot[i].x >> 12) - ((u8(*)[4])gTouchX)[slot][0];
        dy = (mShot[i].y >> 12) - ((u8(*)[4])gTouchY)[slot][0];
        mShot[i].state = 1;
        mShot[i].offX = dx << 12;
        mShot[i].offY = dy << 12;
        mShot[i].combo = 0;
    }
    DrawAimLine();
}

#pragma push
#pragma opt_propagation off
/* One Bob-omb's stylus frame (mShot[i]). While the stylus is down the shot
 * follows the touch point through its grab offset, clamped into the 8..0xf8 x
 * 8..0xb8 box, the offset is re-derived, and the drag-speed sound is re-armed
 * at 0x17b or 0x17c depending on whether the distance from the launcher grew
 * by more than ten. On release the launch fires: the pull-back vector becomes
 * the shot's angle and speed (the square root scaled by 1.953, clamped to
 * 0xa000), that speed drives the sine table into velX/velY, and the shot is
 * re-seated at 0x80000, 0x28000 if the pull was too short.
 *
 * Four spellings only move registers, and each was measured:
 * - dy is taken before dx in both distance blocks;
 * - the second sine read goes through a named `const s16 *` (non-const changes
 *   the size; doing the same to the first read is worse), which keeps the two
 *   reads of FX_SinCosTable_ from sharing one table base;
 * - the last y clamp stores through `py`, assigned between the two x clamps;
 * - the first velocity word's table index is a block-scope local, not inline
 *   and not at function scope.
 * `opt_propagation off` was measured against deleting it outright.
 * The field-base form stays (measured 2026-11-13): typed mShot[i] under
 * opt_propagation off recomputes i * 0x38 after each call and branch (0x3e8
 * bytes of 0x3bc); typed without the pragma is the right size but schedules
 * the touch loads, the x/y reloads and the clamps differently (74 words,
 * unmoved by declaration order, idx type, vx/vy order, `py`, inline touch
 * reads, `+=` or else-if clamps).
 * The two velocity reads stay hand-indexed into FX_SinCosTable_: through
 * FX_CosIdx / FX_SinIdx the products take swapped registers (17-22 DIFF in
 * eleven spellings: inline, index local, fx16 local, speed read first).
 */
void dScMgPachinko_c::BobOmb_StateAim(int i) {
    char *c = (char *)this;
    s32 cx;
    s32 *py;
    s32 cy;
    u32 idx = gActivePlayerSlot;

    if (((u8(*)[4])gTouchHeld)[idx][0] != 0) {
        int off = i * 0x38;
        s32 vy, vx;

        cx = ((u8(*)[4])gTouchX)[idx][0];
        cy = ((u8(*)[4])gTouchY)[idx][0];
        *(s32 *)(c + SHOT_OFF(x) + off) = (cx << 12) + *(s32 *)(c + SHOT_OFF(offX) + off);
        *(s32 *)(c + SHOT_OFF(y) + off) = (cy << 12) + *(s32 *)(c + SHOT_OFF(offY) + off);

        vy = *(s32 *)(c + SHOT_OFF(y) + off) >> 12;
        vx = *(s32 *)(c + SHOT_OFF(x) + off) >> 12;
        if (vx >= 0xf8)
            *(s32 *)(c + SHOT_OFF(x) + off) = 0xf8000;
        py = (s32 *)(c + SHOT_OFF(y) + off);
        if (vx <= 8)
            *(s32 *)(c + SHOT_OFF(x) + off) = 0x8000;
        if (vy >= 0xb8)
            *py = 0xb8000;
        if (vy <= 8)
            *py = 0x8000;

        {
            s32 nx =
                (*(s32 *)(c + SHOT_OFF(x) + off) >> 12) - ((u8(*)[4])gTouchX)[gActivePlayerSlot][0];
            s32 ny =
                (*(s32 *)(c + SHOT_OFF(y) + off) >> 12) - ((u8(*)[4])gTouchY)[gActivePlayerSlot][0];

            *(s32 *)(c + SHOT_OFF(offX) + off) = nx << 12;
            *(s32 *)(c + SHOT_OFF(offY) + off) = ny << 12;
        }

        {
            s32 dy = 0x80 - (*(s32 *)(c + SHOT_OFF(x) + off) >> 12);
            s32 dx = 0x20 - (*(s32 *)(c + SHOT_OFF(y) + off) >> 12);
            s32 dist = cstd::sqrt((dy * dy + dx * dx));
            s32 prev = *(s32 *)(c + SHOT_OFF(dist) + off);

            *(s32 *)(c + SHOT_OFF(dist) + off) = dist;
            if (dist > prev) {
                if (dist > prev + 10)
                    *(s32 *)(c + SHOT_OFF(sound) + off) =
                        Sound_PlayIfNotActive(*(s32 *)(c + SHOT_OFF(sound) + off), 2, 0x17b, 0);
                else
                    *(s32 *)(c + SHOT_OFF(sound) + off) =
                        Sound_PlayIfNotActive(*(s32 *)(c + SHOT_OFF(sound) + off), 2, 0x17c, 0);
            }
        }
    } else {
        int off = i * 0x38;
        s32 dy, dx, dist;

        *(u8 *)(c + SHOT_OFF(state) + off) = 2;
        dy = 0x80 - (*(s32 *)(c + SHOT_OFF(x) + off) >> 12);
        dx = 0x20 - (*(s32 *)(c + SHOT_OFF(y) + off) >> 12);
        dist = cstd::sqrt((dy * dy + dx * dx));
        if (dist >= 0x10) {
            {
                s16 ang = cstd::atan2(dx, dy);
                char *p = c + off;

                *(s16 *)(p + SHOT_OFF(angle)) = ang;
            }
            *(s32 *)(c + SHOT_OFF(speed) + off) = cstd::sqrt((dy * dy + dx * dx)) << 8;
            *(s32 *)(c + SHOT_OFF(speed) + off) += *(s32 *)(c + SHOT_OFF(speed) + off) >> 2;
            *(s32 *)(c + SHOT_OFF(speed) + off) += *(s32 *)(c + SHOT_OFF(speed) + off) >> 2;
            *(s32 *)(c + SHOT_OFF(speed) + off) += *(s32 *)(c + SHOT_OFF(speed) + off) >> 2;
            if (*(s32 *)(c + SHOT_OFF(speed) + off) >= 0xa000)
                *(s32 *)(c + SHOT_OFF(speed) + off) = 0xa000;
            mNextBallDelay = (u16)(*(s32 *)(c + SHOT_OFF(speed) + off) >> 10);
            if (mNextBallDelay == 0)
                mNextBallDelay = 1;
            mUnused5c31 = 0;
            {
                int ai = (*(u16 *)(c + SHOT_OFF(angle) + off) >> 4) * 2 + 1;
                *(s32 *)(c + SHOT_OFF(velX) + off) =
                    FX_MUL(FX_SinCosTable_[ai], *(s32 *)(c + SHOT_OFF(speed) + off));
            }
            {
                const s16 *sp = &FX_SinCosTable_[(*(u16 *)(c + SHOT_OFF(angle) + off) >> 4) * 2];
                *(s32 *)(c + SHOT_OFF(velY) + off) =
                    FX_MUL(*sp, *(s32 *)(c + SHOT_OFF(speed) + off));
            }
            *(s16 *)(c + SHOT_OFF(timer) + off) = 0;
            if (dist >= 0x40)
                Sound::PlayBank2_2D(0x17e);
            else
                Sound::PlayBank2_2D(0x17d);
        } else {
            *(s32 *)(c + SHOT_OFF(x) + off) = 0x80000;
            *(s32 *)(c + SHOT_OFF(y) + off) = 0x28000;
            *(u8 *)(c + SHOT_OFF(state) + off) = 0;
        }
    }
    DrawAimLine();
}
#pragma pop

void dScMgPachinko_c::BobOmb_StateFlying(int idx) {
    int old;
    int x, y;

    mShot[idx].x += mShot[idx].velX;
    mShot[idx].y += mShot[idx].velY;
    mShot[idx].gravity += 0x10;
    old = mShot[idx].velY;
    mShot[idx].velY = old + mShot[idx].gravity;
    if (mShot[idx].velY >= 0x8000)
        mShot[idx].velY = 0x8000;

    mShot[idx].angle = cstd::atan2(mShot[idx].velY >> 12, mShot[idx].velX >> 12);

    if (old < 0 && mShot[idx].velY >= 0)
        mShot[idx].timer = 0x40;

    if (mShot[idx].timer != 0) {
        mShot[idx].timer--;
        if ((s16)mShot[idx].timer <= 0)
            mShot[idx].state = 4;
    }

    x = mShot[idx].x >> 12;
    y = mShot[idx].y >> 12;
    if (y <= -0x120)
        mShot[idx].state = 4;
    if (y >= 0xd0)
        mShot[idx].state = 4;
    if (x >= 0x140 || x <= -0x40)
        mShot[idx].state = 4;

    ClearAimLine();
    if (mAimShot != 0)
        DrawAimLine();
}

void dScMgPachinko_c::BobOmb_StateFallAway(int i) {
    int n = i * 0x38;
    u16 v;
    mShot[i].speed = mShot[i].speed + mShot[i].gravity;
    mShot[i].gravity = mShot[i].gravity + 0x10;
    mShot[i].x += FX_MUL(FX_CosIdx(mShot[i].angle >> 4), 0x800);
    {
        fx16 s = FX_SinIdx(mShot[i].angle >> 4);
        mShot[i].y += FX_MUL(s, mShot[i].speed);
    }
    v = mShot[i].timer;
    if (v != 0) {
        mShot[i].timer = v - 1;
        if ((s16)mShot[i].timer < 0)
            mShot[i].timer = 0;
    } else {
        mShot[i].state = 4;
    }
}

void dScMgPachinko_c::BobOmb_StateRemove(int idx) {
    mShot[idx].active = 0;
    mShot[idx].visible = 0;
}

void dScMgPachinko_c::UpdateNextBall() {
    int i;

    if (mNextBallDelay == 0)
        return;
    mNextBallDelay--;
    if ((s16)mNextBallDelay > 0)
        return;
    mNextBallDelay = 0;

    for (i = 0; i < 0x30; i++) {
        if (mShot[i].active == 0) {
            mShot[i].active = 1;
            mShot[i].visible = 1;
            mShot[i].state = 0;
            mShot[i].gravity = 0;
            mShot[i].x = 0x80000;
            mShot[i].y = 0x28000;
            mShot[i].offX = 0;
            mShot[i].offY = 0;
            mShot[i].combo = 0;
            mShot[i].velX = 0;
            mShot[i].velY = 0;
            mShot[i].timer = 0;
            mShot[i].sound = 0;
            mShot[i].dist = 4;
            mAimShot = i + 1;
            if (mBallsServed != 0) {
                Sound::PlayBank2_2D(0x18d);
                return;
            }
            mBallsServed++;
            return;
        }
    }
}

void dScMgPachinko_c::ResetRound() {
    int i, j, k, l, m, n;

    mPhase = 0;

    for (i = 0; i < 30; i++) {
        mPopup[i].x = 0;
        mPopup[i].y = 0;
        mPopup[i].points = 0;
        mPopup[i].timer = 0;
    }

    for (j = 0; j < 3; j++) {
        mRow[j].active = 0;
        mRow[j].visible = 0;
    }

    for (k = 0; k < 2; k++) {
        mPipe[k].x = 0;
        mPipe[k].y = 0;
        mPipe[k].vel = 0;
        mPipe[k].velY = 0;
        mPipe[k].timer = 0;
        mPipe[k].sprite = 0;
        mPipe[k].state = 0;
        mPipe[k].pad12 = 0; /* a byte nothing else touches */
        mPipe[k].active = 0;
        mPipe[k].visible = 0;
    }

    for (l = 0; l < 0x30; l++) {
        mShot[l].x = 0;
        mShot[l].y = 0;
        mShot[l].prevX = 0;
        mShot[l].prevY = 0;
        mShot[l].speed = 0;
        mShot[l].gravity = 0;
        mShot[l].angle = 0;
        mShot[l].timer = 0;
        mShot[l].active = 0;
        mShot[l].state = 0;
        mShot[l].visible = 0;
    }

    for (m = 0; m < 30; m++) {
        mBall[m].active = 0;
        mBall[m].visible = 0;
        mBall[m].state = 0;
    }

    for (n = 0; n < 30; n++) {
        mFx[n].active = 0;
        mFx[n].visible = 0;
    }

    mFlyer[0].active = 0;
    mFlyer[0].state = 0;
    mFlyer[0].visible = 0;
    mEndTimer = 0;
    mUnused5c22 = 0;
    mUnused5c1e = 0;
    mUnused5c30 = 0;
    mUnused5c31 = 0;
    mPlayStarted = 0;
    mWaveTimer = 0;
    mFlyerOut = 0;
    mWaveSpawns = 0;
    mWaveLevel = 0;
    mHitPause = 0;
    mBallsServed = 0;

    Mg_SubmitScore(0);
    Target_HideAll();
    Bg_Stop();
    Bg_Start();
}

/* Vtable slot 18, an override of
   dScMgBase_c::StartRound(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgPachinko_c::StartRound(int n)
{
    /* The score increment reads mLevel through a const view of `this`.
       Without it mwcc CSEs the +0xbc field address into its own register
       (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the function grows a
       word; the cartridge re-issues ldr r1,[r4,#0xbc] / str r1,[r4,#0xbc].
       Same lever as dScMgPachinko2_c::StartRound. */
    const dScMgPachinko_c *ro = this;

    mPhase = 0;
    if (n == 9) {
        mLevel = ro->mLevel + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
    } else {
        mLevel = 0;
        if (mLevel > 0x270e) mLevel = 0x270e;
    }
    Bg_Stop();
    Bg_Start();
}

/* Vtable slot 9. */
s32 dScMgPachinko_c::Render()
{
    DrawTopRecord();
    DrawScore();
    Fx_DrawAll();
    Popup_DrawAll();
    BobOmb_DrawAll();
    Flyer_Draw();
    Ball_DrawAll();
    Target_DrawAll();
    Pipe_DrawAll();
    Deco_DrawAll();
    return 1;
}

/* Vtable slot 6. */
s32 dScMgPachinko_c::Behavior()
{
    switch (mPhase) {
    case 0:
        FreeGfxSlotsById(0x1d);
        ResetRound();
        Pipe_ResetAll();
        Target_ShowAll();
        mPhase = 1;
        mNextBallDelay = 0x10;
        mWaveTimer = 0x60;
        break;
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (mHitPause != 0) {
            Fx_UpdateAll();
            mHitPause--;
        } else {
            SpawnWave();
            CountLaunch();
            UpdateNextBall();
            BobOmb_UpdateAll();
            Ball_UpdateAll();
            Target_Choose();
            Flyer_Update();
            Pipe_UpdateAll();
            Popup_UpdateAll();
            CheckTargetsLeft();
            Fx_UpdateAll();
        }
        break;
    case 2:
        if (mEndTimer != 0) {
            mEndTimer--;
            if ((s16)mEndTimer <= 0) {
                Mg_SetSharedState(MG_STATE_RESULTS);
                mPromptEnabled = 0;
            }
        }
        Fx_UpdateAll();
        Ball_UpdateAll();
        Flyer_Update();
        Pipe_UpdateAll();
        Popup_UpdateAll();
        break;
    }
    Bg_Update();
    return 1;
}

/* Vtable slot 0. */
s32 dScMgPachinko_c::InitResources()
{
    char *c = (char *)this;
    char *b;
    char *dst;
    volatile u16 fillScr;
    volatile u16 fillZero;
    volatile u16 fillIdx;
    void *objChar;
    void *objPltt;
    void *f;
    int n;
    int y;
    int x;

    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    G2_SetBG3Priority(1);
    G2_BG3Mosaic(false);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);

    f = LoadFile(0x5f);
    DecompressLZ16(f, (void *)G2_GetBG3CharPtr());
    Deallocate(f);

    f = LoadFile(0x60);
    GX_LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x61);
    GX_LoadBG3Scr(f, 0, 0x800);
    Deallocate(f);

    G2_SetBG2Priority(1);
    G2_BG2Mosaic(false);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                         GX_BG_CHARBASE_0x18000);

    f = LoadFile(0x46);
    DecompressLZ16(f, G2_GetBG2CharPtr());
    Deallocate(f);

    b = (char *)G2_GetBG2ScrPtr();
    fillScr = 0x3073;
    MI_CpuFill16(b, fillScr, 0x1000);

    f = LoadFile(6);
    GX_LoadBG2Scr(f, 0, 0x800);
    Deallocate(f);

    objChar = LoadFile(0xe0);
    objPltt = LoadFile(0xe1);
    DecompressLZ16(objChar, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(objPltt, 0, 0x100);

    gSubScreenLayers |= 0xd;
    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(false);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x18000);

    Mg_InitBrush(c, 0, 2, 0);

    b = (char *)G2S_GetBG2CharPtr();
    fillZero = 0;
    MI_CpuFill16(b, fillZero, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)G2S_GetBG2ScrPtr() + x + y * 0x20);
            fillIdx = n;
            MI_CpuFill16(dst, fillIdx, 2);
            n++;
        }
    }

    G2S_SetBG3Priority(2);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(0, 0);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x08000);

    f = LoadFile(0x5c);
    DecompressLZ16(f, (void *)G2S_GetBG3CharPtr());
    Deallocate(f);

    f = LoadFile(0x5d);
    GXS_LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x5e);
    GXS_LoadBG3Scr(f, 0, 0x800);
    Deallocate(f);

    G2S_SetBG0Priority(1);
    G2S_BG0Mosaic(false);
    G2S_SetBG0Offset(0, 0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);

    f = LoadFile(5);
    GXS_LoadBG0Scr(f, 0, 0x800);
    Deallocate(f);

    DecompressLZ16(objChar, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(objPltt, 0, 0x100);
    Deallocate(objChar);
    Deallocate(objPltt);

    ResetRound();
    Pipe_ResetAll();
    Target_ShowAll();
    Mg_SetTopScreenYOffset(0x20);

    mRunDuringFade = 1;
    mNextBallDelay = 0x10;
    mWaveTimer = 0x60;
    mPhase = 1;
    mRunDuringFade = 1;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgPachinko_c through RTTI,
 * allocation size, vtable identity, and the MG_PACHINKO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgBobOmbSquad_Spawn. */
void *dScMgPachinko_c_classInit() {
    return new dScMgPachinko_c;
}

/* The five state tables, defined in retail copy order: Bob-ombs, balls,
   pipes, the flyer, then the background. mwcc emits __sinit_d_s_mg_pachinko.cpp from
   these definitions, copying the ROM .data records. */
Entry sPachinkoBobOmbStateTable[5] = {
    {pmf_dScMgPachinko_c_BobOmb_StateWaitGrab}, {pmf_dScMgPachinko_c_BobOmb_StateAim},
    {pmf_dScMgPachinko_c_BobOmb_StateFlying},   {pmf_dScMgPachinko_c_BobOmb_StateFallAway},
    {pmf_dScMgPachinko_c_BobOmb_StateRemove},
};
Entry sPachinkoBallStateTable[7] = {
    {pmf_dScMgPachinko_c_Ball_StateWaiting}, {pmf_dScMgPachinko_c_Ball_StateFall},
    {pmf_dScMgPachinko_c_Ball_StateDrift},   {pmf_dScMgPachinko_c_Ball_StateDive},
    {pmf_dScMgPachinko_c_Ball_StateGlide},   {pmf_dScMgPachinko_c_Ball_StateSpinAway},
    {pmf_dScMgPachinko_c_Ball_StatePop},
};
Entry sPachinkoPipeStateTable[4] = {
    {pmf_dScMgPachinko_c_Pipe_StatePick},
    {pmf_dScMgPachinko_c_Pipe_StateRise},
    {pmf_dScMgPachinko_c_Pipe_StateSink},
    {pmf_dScMgPachinko_c_Pipe_StateIdle},
};
PMF sPachinkoFlyerStateTable[5] = {
    pmf_dScMgPachinko_c_Flyer_StateWaiting, pmf_dScMgPachinko_c_Flyer_StateDescend,
    pmf_dScMgPachinko_c_Flyer_StateCross,   pmf_dScMgPachinko_c_Flyer_StatePause,
    pmf_dScMgPachinko_c_Flyer_StateLeave,
};
Entry sPachinkoBgStateTable[4] = {
    {pmf_dScMgPachinko_c_Bg_StateWaiting},
    {pmf_dScMgPachinko_c_Bg_StateRiseIn},
    {pmf_dScMgPachinko_c_Bg_StateScroll},
    {pmf_dScMgPachinko_c_Bg_StateSinkOut},
};
