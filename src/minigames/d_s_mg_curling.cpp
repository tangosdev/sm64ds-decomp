//cpp
/* One-player curling scene: the whole ov006 unit 0x020e0638..0x020e3854,
 * 47 functions in ROM order, from the destructor through InitResources to
 * the registry factory dScMgCurling_c_classInit. The destructor is out of
 * line and comes first, which makes this file the key function's home, so
 * the vtable and RTTI are emitted here too.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; do not
 * reorder. The helpers are all dScMgCurling_c members: the state tables
 * call them through pointers to member, the rest with the scene as r0.
 *
 * Leftovers, each kept because the plain spelling changes codegen:
 * - Stone_Slide keeps its angle/speed/position pointer temporaries; the
 *   angle and speed ones stay `&mStone[0].f + offset`, since
 *   &mStone[idx] moves their address math.
 * - ResolveStoneCollision indexes the sine table by hand; FX_SinIdx and
 *   FX_CosIdx there reallocate registers across the function.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgCurling_c_AssetId {
    ASSET_D_2D_MG_BG_CURLING1_LEFT_NSC_BIN = 0x002d,
    ASSET_D_2D_MG_BG_CURLING1_NCL_BIN = 0x002f,
    ASSET_D_2D_MG_BG_CURLING1_NSC_BIN = 0x0030,
    ASSET_D_2D_MG_BG_CURLING1_RIGHT_NSC_BIN = 0x0031,
    ASSET_D_2D_MG_BG_CURLING2_NCL_BIN = 0x0033,
    ASSET_D_2D_MG_BG_CURLING2_NSC_BIN = 0x0034,
    ASSET_D_2D_MG_CURLING_NCG_BIN = 0x00c7,
    ASSET_D_2D_MG_CURLING_NCL_BIN = 0x00c8
};

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "dScMgCurling_c.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/g2.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
namespace s20b2444 {
void Mg_DrawBigNumber(int, int, int, int, int, int, int);
}
using s20b2444::Mg_DrawBigNumber;
namespace s20b2220 {
void Mg_DrawNumberRotated(int, int, int, int, int, int, u16);
}
using s20b2220::Mg_DrawNumberRotated;
namespace s20b19f0 {
void Mg_DrawTopLeftCounter(int);
}
using s20b19f0::Mg_DrawTopLeftCounter;
namespace s20b0a54 {
void Mg_SetSharedState(s32);
}
using s20b0a54::Mg_SetSharedState;
namespace s20b04d0 {
void Mg_SetTopScreenYOffset(int);
}
using s20b04d0::Mg_SetTopScreenYOffset;
namespace s20b0104 {
void RenderOamBothScreens(void *attr, int, int, int, int, void *mtx);
}
using s20b0104::RenderOamBothScreens;
namespace s20afdd0 {
void Mg_RenderSpriteBoth(void *a0, int, int, int, int);
}
using s20afdd0::Mg_RenderSpriteBoth;
struct OamAttr;
struct Matrix2x2;
namespace s20afcf8 {
void DrawOamSprite(OamAttr *attr, int, int, Matrix2x2 *mtx);
}
using s20afcf8::DrawOamSprite;
namespace s20af948 {
void Mg_RenderSubSpriteMtx(void *a, int, int, void *m);
}
using s20af948::Mg_RenderSubSpriteMtx;
namespace s20adc74 {
void *Ov004_LoadFileByPath(const char *path);
}
using s20adc74::Ov004_LoadFileByPath;
namespace s20adc5c {
void Ov004_Deallocate(void *ptr);
}
using s20adc5c::Ov004_Deallocate;
namespace s20adbe0 {
int Mg_IsNewRecord(void);
}
using s20adbe0::Mg_IsNewRecord;
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

/* `C` is the receiver of the pointer-to-member state tables. It must stay
 * incomplete: mwccarm picks the pointer-to-member layout from whether the
 * class is complete, and completing it changes all four dispatchers. */
struct C;
typedef void (C::*PMF)(int);
struct Entry {
    PMF pmf;
};
typedef void (C::*PMF0)();

/* B4 is a 4-byte-record view of a touch lane, used by the stone-drag
 * reader; the other readers index the lanes flat. Measured: reading all
 * three through ((TouchInfo *)gTouchHeld)[slot] changes Stone_Drag's size,
 * and TouchInfo for held with flat gTouchX/gTouchY reads differs by 20+
 * words. */
struct B4 {
    unsigned char v;
    unsigned char pad[3];
};

/* A random integer in [0, n): the top 15 bits of the minigame generator,
   scaled. */
#define CURL_RAND(n) (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * (n)) >> 15)

/* Everything this file calls or reads. Class members cannot go inside an
 * `extern "C"` block, so the C-linkage names are collected here in one. */
extern "C" {}

extern void func_02012718(int a, int b);

struct OamAttr;

struct Matrix2x2;
extern "C" {}

extern int func_020126e8(int a);

extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);

extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern "C" {

extern void DecompressLZ16(const void *src, void *dst);

extern void *LoadFile(int handle);
}

extern void Deallocate(void *ptr);
extern "C" {}

extern unsigned char sCurlingBitAStates[];

extern unsigned char sCurlingBitBStates[];

extern u8 sCurlingScoreDelays[];

extern int sCurlingBitKinds[];

extern int sCurlingStonesLeftX[];

extern OamAttr *sCurlingBitSprites[];

extern int sCurlingStoneLeftSprite;

extern int sCurlingStoneLeftShadowSprite;

extern int sCurlingAimMarkerSprites[];

extern unsigned char gMainScreenLayers;

extern unsigned char gSubScreenLayers;

extern char sCurlingMgDirPath;

extern char sCurlingBgCurling2NcgPath;

extern int sCurlingSlideZoneEdges[];

extern int sCurlingSlideShiftLeft[];

extern int sCurlingSlideShiftRight[];

extern int sCurlingSlideMinLeft[];

extern int sCurlingSlideMinRight[];

extern int sCurlingHouseSpots[];

/* The six state tables, filled at startup by the ov006 static init. */
extern PMF0 sCurlingAimTable[];

extern PMF sCurlingBitKindTable[];

extern PMF sCurlingBitStateTable[];

extern Entry sCurlingBitBStateTable[];

extern PMF sCurlingStoneTable[];

extern PMF sCurlingBitAStateTable[];

extern PMF0 sCurlingSceneTable[];

/* The ROM PMF constants the seven state tables copy from (unlicensed
 * .data), grouped by destination table in retail copy order. */
extern PMF0 pmf_dScMgCurling_c_Scene_StartRound;

extern PMF0 pmf_dScMgCurling_c_Scene_Play;

extern PMF0 pmf_dScMgCurling_c_Scene_Score;

extern PMF0 pmf_dScMgCurling_c_Scene_End;

extern PMF0 pmf_dScMgCurling_c_Scene_Idle;

extern PMF pmf_dScMgCurling_c_Stone_WaitRelease;

extern PMF pmf_dScMgCurling_c_Stone_Slide;

extern PMF pmf_dScMgCurling_c_Stone_Collide;

extern PMF pmf_dScMgCurling_c_Stone_Drag;

extern PMF0 pmf_dScMgCurling_c_Aim_WaitTouch;

extern PMF0 pmf_dScMgCurling_c_Aim_Swing;

extern PMF pmf_dScMgCurling_c_Bit_PickKind;

extern PMF pmf_dScMgCurling_c_Bit_RunKindB;

extern PMF pmf_dScMgCurling_c_Bit_RunKindA;

extern PMF pmf_dScMgCurling_c_BitB_StatePause;

extern PMF pmf_dScMgCurling_c_BitB_StateDriftRight;

extern PMF pmf_dScMgCurling_c_BitB_StateDriftLeft;

extern PMF pmf_dScMgCurling_c_BitB_StateDriftStop;

extern PMF pmf_dScMgCurling_c_BitA_StatePause;

extern PMF pmf_dScMgCurling_c_BitA_StateDriftRight;

extern PMF pmf_dScMgCurling_c_BitA_StateDriftLeft;

extern PMF pmf_dScMgCurling_c_BitA_StateDriftStop;

extern PMF pmf_dScMgCurling_c_Bit_StateFallStart;

extern PMF pmf_dScMgCurling_c_Bit_StateFallSpeedUp;

extern PMF pmf_dScMgCurling_c_Bit_StateFallSlowDown;
/* extern "C" */

namespace cstd {
s16 atan2(s32, s32);
int sqrt(u64 value);
}
namespace Sound {
u32 PlayBank2_2D(u32 id);
}

dScMgCurling_c::~dScMgCurling_c() {}
/* Draws the falling bits. The functions after this one are the bits'
 * states: each field is 0x24 * index past its base offset. */
void dScMgCurling_c::Bit_DrawAll() {
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mBit[i].visible) {
            int x = mBit[i].x >> 0xc;
            int y = mBit[i].y >> 0xc;
            Mg_RenderSubSpriteMtx(sCurlingBitSprites[mBit[i].spriteA], x, y, 0);
            DrawOamSprite(sCurlingBitSprites[mBit[i].spriteB], x, y, 0);
        }
    }
}

void dScMgCurling_c::Bit_StateFallSlowDown(int index) {
    if (mBit[index].fallTimer != 0) {
        mBit[index].fallTimer--;
        if ((s16)mBit[index].fallTimer < 0)
            mBit[index].fallTimer = 0;
        return;
    }
    if (mBit[index].vy > 0x100) {
        mBit[index].vy -= 0x10;
        if ((s16)mBit[index].vy < 0x100)
            mBit[index].vy = 0x100;
        return;
    }
    mBit[index].fallState = 0;
}

void dScMgCurling_c::Bit_StateFallSpeedUp(int index) {
    if (mBit[index].fallSpeed > mBit[index].vy) {
        mBit[index].vy += 0x10;
        if (mBit[index].fallSpeed > mBit[index].vy)
            mBit[index].vy = mBit[index].fallSpeed;
    }
    if (mBit[index].fallTimer != 0) {
        mBit[index].fallTimer--;
        if ((s16)mBit[index].fallTimer < 0)
            mBit[index].fallTimer = 0;
        return;
    }
    mBit[index].fallState = 2;
    mBit[index].fallTimer = (u8)(CURL_RAND(0x20) + 0x20);
}

void dScMgCurling_c::Bit_StateFallStart(int index) {
    mBit[index].vy = 0;
    mBit[index].fallSpeed = (CURL_RAND(0x10) << 4) + 0x300;
    mBit[index].fallState = 1;
    mBit[index].fallTimer = (u8)(CURL_RAND(0x20) + 0x20);
}

void dScMgCurling_c::BitA_StateDriftStop(int index) {
    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;
    if (mBit[index].holdTimer != 0) {
        mBit[index].driftTimer--;
        if ((s16)mBit[index].driftTimer < 0)
            mBit[index].driftTimer = 0;
        return;
    }
    if (mBit[index].vx > 0) {
        mBit[index].vx -= 8;
        if ((s16)mBit[index].vx < 0)
            mBit[index].vx = 0;
        return;
    }
    if (mBit[index].vx < 0) {
        mBit[index].vx += 8;
        if (mBit[index].vx > 0)
            mBit[index].vx = 0;
        return;
    }
    mBit[index].driftState = 0;
}

void dScMgCurling_c::BitA_StateDriftLeft(int index) {
    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;
    if (mBit[index].holdTimer != 0) {
        mBit[index].holdTimer--;
        if ((s16)mBit[index].holdTimer < 0)
            mBit[index].holdTimer = 0;
        return;
    }
    if (mBit[index].vx > -0x300) {
        mBit[index].vx -= 8;
        if (mBit[index].vx <= -0x300)
            mBit[index].vx = 0x300;
    }
    if (mBit[index].driftTimer != 0) {
        mBit[index].driftTimer--;
        if ((s16)mBit[index].driftTimer < 0)
            mBit[index].driftTimer = 0;
        return;
    }
    mBit[index].driftState = 3;
    mBit[index].driftTimer = (u8)(CURL_RAND(0x20) + 0x20);
}

void dScMgCurling_c::BitA_StateDriftRight(int index) {
    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;
    if (mBit[index].holdTimer != 0) {
        mBit[index].holdTimer--;
        if ((s16)mBit[index].holdTimer < 0)
            mBit[index].holdTimer = 0;
        return;
    }
    if (mBit[index].vx < 0x300) {
        mBit[index].vx += 8;
        if (mBit[index].vx >= 0x300)
            mBit[index].vx = 0x300;
    }
    if (mBit[index].driftTimer != 0) {
        mBit[index].driftTimer--;
        if ((s16)mBit[index].driftTimer < 0)
            mBit[index].driftTimer = 0;
        return;
    }
    mBit[index].driftState = 3;
    mBit[index].driftTimer = (u8)(CURL_RAND(0x20) + 0x20);
}

void dScMgCurling_c::BitA_StatePause(int index) {
    if (mBit[index].holdTimer != 0) {
        mBit[index].holdTimer--;
        if ((s16)mBit[index].holdTimer < 0)
            mBit[index].holdTimer = 0;
        return;
    }
    mBit[index].vx = 0;
    mBit[index].driftState = sCurlingBitAStates[CURL_RAND(2)];
    mBit[index].holdTimer = (u8)CURL_RAND(0x10);
    mBit[index].driftTimer = (u8)(CURL_RAND(0x40) + 0x60);
}

void dScMgCurling_c::Bit_RunKindA(int index) {
    (((C *)this)->*sCurlingBitAStateTable[mBit[index].driftState])(index);
    (((C *)this)->*sCurlingBitStateTable[mBit[index].fallState])(index);
}

void dScMgCurling_c::BitB_StateDriftStop(int index) {
    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;
    if (mBit[index].vx > 0) {
        mBit[index].vx -= 0x20;
        if ((s16)mBit[index].vx < 0)
            mBit[index].vx = 0;
    } else if (mBit[index].vx < 0) {
        mBit[index].vx += 0x20;
        if (mBit[index].vx > 0)
            mBit[index].vx = 0;
    } else {
        mBit[index].driftState = 0;
    }
}

void dScMgCurling_c::BitB_StateDriftLeft(int index) {
    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;
    if (mBit[index].holdTimer != 0) {
        mBit[index].holdTimer--;
        if ((s16)mBit[index].holdTimer < 0)
            mBit[index].holdTimer = 0;
        return;
    }
    if (mBit[index].vx > -0x400) {
        mBit[index].vx -= 0x20;
        if (mBit[index].vx <= -0x400)
            mBit[index].vx = 0x400;
    }
    if (mBit[index].driftTimer != 0) {
        mBit[index].driftTimer--;
        if ((s16)mBit[index].driftTimer < 0)
            mBit[index].driftTimer = 0;
        return;
    }
    mBit[index].driftState = 3;
}

void dScMgCurling_c::BitB_StateDriftRight(int index) {
    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;
    if (mBit[index].holdTimer != 0) {
        mBit[index].holdTimer--;
        if ((s16)mBit[index].holdTimer < 0)
            mBit[index].holdTimer = 0;
        return;
    }
    if (mBit[index].vx < 0x400) {
        mBit[index].vx += 0x20;
        if (mBit[index].vx >= 0x400)
            mBit[index].vx = 0x400;
    }
    if (mBit[index].driftTimer != 0) {
        mBit[index].driftTimer--;
        if ((s16)mBit[index].driftTimer < 0)
            mBit[index].driftTimer = 0;
        return;
    }
    mBit[index].driftState = 3;
}

void dScMgCurling_c::BitB_StatePause(int idx) {
    if (mBit[idx].holdTimer != 0) {
        mBit[idx].holdTimer--;
        if ((s16)mBit[idx].holdTimer < 0)
            mBit[idx].holdTimer = 0;
        return;
    }
    mBit[idx].vx = 0;
    mBit[idx].vy = (CURL_RAND(0x20) << 4) + 0x600;
    mBit[idx].driftState = sCurlingBitBStates[CURL_RAND(2)];
    mBit[idx].holdTimer = (u8)CURL_RAND(0x10);
    mBit[idx].driftTimer = (u8)(CURL_RAND(0x30) + 0x30);
}

void dScMgCurling_c::Bit_RunKindB(int idx) {
    (((C *)this)->*sCurlingBitBStateTable[mBit[idx].driftState].pmf)(idx);
}

void dScMgCurling_c::Bit_PickKind(int idx) {
    unsigned pick = CURL_RAND(8);
    int k = 0;
    if (pick == 5)
        k = 1;
    mBit[idx].kind = sCurlingBitKinds[k];
    mBit[idx].driftState = 0;
}

void dScMgCurling_c::Bit_UpdateAll() {
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mBit[i].active != 0) {
            (((C *)this)->*sCurlingBitKindTable[mBit[i].kind])(i);
            if ((mBit[i].y >> 0xc) >= 0xc8) {
                mBit[i].x = CURL_RAND(0x20) << 0xf;
                mBit[i].y = -0x8000;
                mBit[i].driftState = 0;
                mBit[i].kind = 0;
                mBit[i].fallState = 0;
            }
        }
    }
}

/* Clear every bit, then seed it: active, a random x, two sprite indices
 * (the second is the first plus one to four, modulo five) and a y spread
 * over the screen. The first x, the y at -0x8000 and the first holdTimer
 * store are overwritten a few lines later; those dead stores are the ROM's. */
void dScMgCurling_c::Bit_InitAll() {
    int i;
    unsigned int v;

    for (i = 0; i < 0x32; i++) {
        mBit[i].x = 0;
        mBit[i].y = 0;
        mBit[i].vx = 0;
        mBit[i].vy = 0;
        mBit[i].holdTimer = 0;
        mBit[i].driftTimer = 0;
        mBit[i].fallTimer = 0;
        mBit[i].active = 0;
        mBit[i].kind = 0;
        mBit[i].driftState = 0;
        mBit[i].fallState = 0;
        mBit[i].visible = 0;
        mBit[i].spriteA = 0;
        mBit[i].spriteB = 1;
    }

    for (i = 0; i < 0x32; i++) {
        mBit[i].x = CURL_RAND(0x20) << 0xf;
        mBit[i].y = -0x8000;
        mBit[i].active = 1;
        mBit[i].visible = 1;
        mBit[i].kind = 0;
        mBit[i].driftState = 0;

        mBit[i].spriteA = CURL_RAND(5);

        v = mBit[i].spriteA + CURL_RAND(4) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        mBit[i].spriteB = (char)v;

        mBit[i].holdTimer = (short)(((i & 7) << 6) + CURL_RAND(0x30));

        mBit[i].x = CURL_RAND(0x20) << 0xf;

        mBit[i].y = ((CURL_RAND(0x1a) << 3) - 8) << 0xc;
        mBit[i].holdTimer = 0;
    }
}

void dScMgCurling_c::Popup_DrawAll() {
    int i;
    for (i = 0; i < 5; i++) {
        if (mPopup[i].shown != 0) {
            Mg_DrawBigNumber(mPopup[i].x >> 12, mPopup[i].y >> 12, mPopup[i].points, -1, -1, 0, 0);
        }
    }
    if (mRoundOver != 0) {
        int r = Mg_GetSubmittedScore();
        Mg_DrawNumberRotated(0x80, 0x60, r, 1, 0, 0x800, 0);
    }
}

void dScMgCurling_c::Popup_UpdateAll() {
    int i;
    for (i = 0; i < 5; i++) {
        if (mPopup[i].live == 0)
            continue;
        if (mPopup[i].countdown == 0)
            continue;
        mPopup[i].countdown--;
        if (mPopup[i].countdown != 0)
            continue;
        mPopup[i].shown = 1;
        Sound::PlayBank2_2D(0x1bc);
    }
}

void dScMgCurling_c::ScoreStones() {
    int slot = 0;
    int i, j;

    for (j = 0; j < 5; j++) {
        if (mPopup[j].live == 0) {
            slot = j;
            break;
        }
    }

    for (i = 0; i < 5; i++) {
        int dx, dz, dist, top;

        if (mStone[i].active == 0)
            continue;
        dx = mStone[i].x - mHouseX;
        dz = mStone[i].y - mHouseY;
        {
            int ax = dx >> 12;
            int az = dz >> 12;
            dist = cstd::sqrt((ax * ax + az * az));
        }
        mPopup[slot].live = 1;
        mPopup[slot].x = mStone[i].x;
        mPopup[slot].y = mStone[i].y + 0x1000;
        mPopup[slot].countdown = sCurlingScoreDelays[slot];
        if (dist <= 8)
            mPopup[slot].points = 1000;
        else if (dist <= 0x18)
            mPopup[slot].points = 500;
        else if (dist <= 0x28)
            mPopup[slot].points = 300;
        else if (dist <= 0x38)
            mPopup[slot].points = 100;
        else
            mPopup[slot].points = 0;
        top = mPopup[slot].y >> 12;
        if (top >= -32 && top <= 8)
            mPopup[slot].y = -0x28000;
        slot++;
    }
}

/* Draws a sprite at the aimed stone's position (mAimX/mAimY) once the
 * stylus handler has set mAimShown. */
void dScMgCurling_c::DrawAimMarker() {
    if (mAimShown == 0)
        return;
    int x = mAimX;
    int y = mAimY;
    Mg_RenderSpriteBoth(sCurlingAimMarkerSprites, (x >> 12) - 0x20, (y >> 12) - 8, -1, 0);
}

/* Stylus handler for the stone being aimed. While the stylus is down, the
 * touch point plus the grab offset becomes the new position, clamped to the
 * 0x20000..0xe0000 by 0x94000..0xb8000 box; a move under two units is undone.
 * Otherwise it updates the swing tracking (mSwingState), turns the move into
 * an angle at mSwingAng clamped to the lower half turn and averaged with the last
 * one, and folds the move length into mSwingSpeed. With the stylus
 * up, two flags are reset.
 *
 * Shapes that must stay: the touch records are read through a `u8 *` cast;
 * dx is computed before dy with no temporaries; the tail's index `j` is
 * wider than a byte; and dx is halved in place before atan2. Spelled
 * `dx >> 1` at both uses, the two shifts are merged into one, which the
 * ROM does not do. */
void dScMgCurling_c::Aim_Swing() {
    u8 idx;
    int dx, oldx, oldy;
    int diff;
    u16 ang;

    idx = gActivePlayerSlot;
    if (gTouchHeld[idx * 4] != 0) {
        int dy2, dy;

        int i4 = idx * 4;
        /* gTouchX is already u8[]; dropping these two casts changes 12
           words of Aim_Swing. */
        u8 *pa = (u8 *)gTouchX;
        u8 *pb = (u8 *)gTouchY;
        u8 bx = pa[i4];
        u8 by = pb[i4];

        oldx = mAimX;
        oldy = mAimY;
        mAimX = (bx << 12) + mGrabX;
        mAimY = mGrabY + (by << 12);

        if (mAimY <= 0x94000)
            mAimY = 0x94000;
        if (mAimX <= 0x20000)
            mAimX = 0x20000;
        if (mAimX >= 0xe0000)
            mAimX = 0xe0000;
        if (mAimY >= 0xb8000)
            mAimY = 0xb8000;

        dx = (mAimX - oldx) >> 12;
        dy = (mAimY - oldy) >> 12;
        dy2 = dy * dy;

        if (cstd::sqrt((dx * dx + dy2)) <= 1) {
            mAimX = oldx;
            mAimY = oldy;
            return;
        }

        diff = (mAimY - mAimHomeY) >> 12;
        if (mSwingState == 0) {
            func_02012718(0x1d6, mAimX);
            mSwingState = 2;
            mLastSwingDeltaY = (mAimY - mAimHomeY) >> 12;
            mAimHomeY = mAimY;
        } else if (mSwingState == 1) {
            if (mLastSwingDeltaY * diff > 0) {
                if (diff < 0)
                    diff = -diff;
                if (diff >= 0xa)
                    mSwingState = 0;
            } else {
                mLastSwingDeltaY = diff;
                mAimHomeY = mAimY;
            }
        } else {
            if (mLastSwingDeltaY * diff < 0)
                mSwingState = 1;
            mLastSwingDeltaY = (mAimY - mAimHomeY) >> 12;
            mAimHomeY = mAimY;
        }

        ang = mSwingAng;
        dx >>= 1;
        mSwingAng = cstd::atan2(dy, dx);
        {
            u16 a = mSwingAng;
            if (a <= 0x8000 && a >= 0x4000)
                mSwingAng = 0x8000;
            else if (a <= 0x4000)
                mSwingAng = 0;
        }

        {
            int mag;
            mSwingAng = (u16)((mSwingAng + ang) >> 1);
            mag = cstd::sqrt((dx * dx + dy2)) * 9;
            mag = (mag << 12) >> 4;
            if (mag >= 0xc000)
                mag = 0xc000;
            if (mag > mSwingSpeed)
                mSwingSpeed = mag;
            {
                int cur = mSwingSpeed;
                if (cur > mag) {
                    mSwingSpeed = mSwingSpeed - ((cur - mag) >> 1);
                }
            }
        }

        {
            int px = mAimX;
            int py = mAimY;
            int j = gActivePlayerSlot;
            u8 jx = gTouchX[j * 4];
            int ax = (px >> 12) - jx;
            u8 jy = gTouchY[j * 4];
            int ay = (py >> 12) - jy;
            mGrabX = ax << 12;
            mGrabY = ay << 12;
        }

        return;
    }

    mStylusHeld = 0;
    mAimShown = 1;
}

void dScMgCurling_c::Aim_WaitTouch() {
    int idx;
    int touching = 0;
    int x;
    int y;
    idx = gActivePlayerSlot;
    /* The held test reads the slot's whole TouchInfo record, the edge test
       the gTouchEdge lane; with both spelled `lane[idx * 4]` the index is
       shared and 20 words differ, and both as records grow the function. */
    if (((TouchInfo *)gTouchHeld)[idx].held != 0) {
        if (gTouchEdge[idx * 4] != 0) {
            touching = 1;
        }
    }
    if (touching == 0) {
        return;
    }
    x = (mAimX >> 0xc) - gTouchX[idx * 4];
    y = (mAimY >> 0xc) - gTouchY[idx * 4];
    mGrabX = x << 0xc;
    mGrabY = y << 0xc;
    mStylusHeld = 1;
    mSwingAng = 0xc000;
    if (mSwingTimer == 0) {
        func_02012718(0x1d2, mAimX);
        mSwingTimer = 6;
    }
    unk_4ecc = 0;
    unk_4ed0 = 0;
    mAimHomeX = mAimX + mGrabX;
    mAimHomeY = mAimY + mGrabY;
    mLastSwingDeltaY = 0xff;
    mSwingState = 0;
}

void dScMgCurling_c::DrawStonesLeft() {
    int x, y;
    int i;
    int remaining;
    int j;
    for (i = 0; i < 5; i++) {
        if (mStone[i].active == 0)
            continue;
        if (mStone[i].dealt == 0)
            continue;
        x = mStone[i].x >> 12;
        y = mStone[i].y >> 12;
        RenderOamBothScreens(&sCurlingStoneLeftSprite, x, y, -1, 1, 0);
        RenderOamBothScreens(&sCurlingStoneLeftShadowSprite, x, y + 8, -1, 2, 0);
    }
    remaining = 5 - mThrown;
    if (remaining < 0)
        remaining = 0;
    j = 0;
    if (remaining > 0) {
        for (; j < remaining; j++) {
            int slotX = sCurlingStonesLeftX[j];
            RenderOamBothScreens(&sCurlingStoneLeftSprite, slotX, 0xb0, -1, 1, 0);
            RenderOamBothScreens(&sCurlingStoneLeftShadowSprite, slotX, 0xb8, -1, 2, 0);
        }
    }
}

/* Stone separation. Stone idx has just moved: the first other active stone
 * within 24 units is pushed out to 26 units along the line between them,
 * then that stone gets the same check once against the rest (with a bump
 * sound). Only one push each way per call.
 *
 * The rotated offsets need separate temporaries, and the inner scan needs
 * its own displacement, distance and angle; sharing the outer ones changes the registers. */
void dScMgCurling_c::SeparateStones(int stoneIndex) {
    int hitIndex;
    int nextIndex;
    int dx;
    int dy;
    int distance;
    u16 contactAngle;

    for (hitIndex = 0; hitIndex < 5; hitIndex++) {
        if (mStone[hitIndex].active == 0)
            continue;
        if (stoneIndex == hitIndex)
            continue;
        dx = (mStone[hitIndex].x - mStone[stoneIndex].x) >> 12;
        dy = (mStone[hitIndex].y - mStone[stoneIndex].y) >> 12;
        distance = cstd::sqrt((dx * dx + dy * dy));
        contactAngle = cstd::atan2(dy, dx);
        if (distance > 0x18)
            continue;
        {
            int contactCos;
            int contactSin;
            int separationX;
            int separationY;

            contactCos = FX_CosIdx(contactAngle >> 4);
            separationX = FX_MUL(contactCos, 0x1a);
            mStone[hitIndex].x = mStone[stoneIndex].x + (separationX << 12);
            contactSin = FX_SinIdx(contactAngle >> 4);
            separationY = FX_MUL(contactSin, 0x1a);
            mStone[hitIndex].y = mStone[stoneIndex].y + (separationY << 12);
            for (nextIndex = 0; nextIndex < 5; nextIndex++) {
                int dx2;
                int dy2;
                int nextDistance;
                u16 nextContactAngle;

                if (mStone[nextIndex].active == 0)
                    continue;
                if (hitIndex == nextIndex)
                    continue;
                dx2 = (mStone[nextIndex].x - mStone[hitIndex].x) >> 12;
                dy2 = (mStone[nextIndex].y - mStone[hitIndex].y) >> 12;
                nextDistance = cstd::sqrt((dx2 * dx2 + dy2 * dy2));
                nextContactAngle = cstd::atan2(dy2, dx2);
                if (nextDistance > 0x18)
                    continue;
                {
                    int nextContactCos;
                    int nextContactSin;
                    int nextSeparationX;
                    int nextSeparationY;

                    nextContactCos = FX_CosIdx(nextContactAngle >> 4);
                    nextSeparationX = FX_MUL(nextContactCos, 0x1a);
                    mStone[nextIndex].x = mStone[hitIndex].x + (nextSeparationX << 12);
                    nextContactSin = FX_SinIdx(nextContactAngle >> 4);
                    nextSeparationY = FX_MUL(nextContactSin, 0x1a);
                    mStone[nextIndex].y = mStone[hitIndex].y + (nextSeparationY << 12);
                    func_02012718(0xe8, mStone[stoneIndex].x);
                    return;
                }
            }
            return;
        }
    }
}

/* Stone idx has just moved, so find the first other stone it overlaps and
 * resolve the collision. The two exchange their velocity components along
 * the line between their centres and keep the components across it; the
 * moving stone is then pushed back to one stone width (0x1b000) along that
 * line and clamped to the board, with any overshoot passed on to the stone
 * it hit. Both stones end up moving, the hit stone is flagged fast above
 * 0x3800, and the knock sound plays panned to the x.
 *
 * Shapes that must stay: the contact angle is negated right after atan2,
 * before the velocities are read; the three fields written after a call
 * are reached through pointers taken beside each stone's reads, which puts
 * their addresses in the frame ahead of the temporaries; and the moving
 * stone's new x velocity reuses the outer dx. */
void dScMgCurling_c::ResolveStoneCollision(int stoneIndex) {
    int hitIndex;
    int dx;
    int dy;

    for (hitIndex = 0; hitIndex < 5; hitIndex++) {
        if (mStone[hitIndex].active == 0)
            continue;
        if (stoneIndex == hitIndex)
            continue;
        if (mStone[hitIndex].state == 0)
            continue;
        if (mStone[hitIndex].state == 3)
            continue;
        dx = (mStone[hitIndex].x - mStone[stoneIndex].x) >> 12;
        dy = (mStone[hitIndex].y - mStone[stoneIndex].y) >> 12;
        if (cstd::sqrt((dx * dx + dy * dy)) > 0x18)
            continue;
        {
            u16 *pAngle;
            s32 *pSpeed;
            u16 *pHitAngle;
            int movingTangent;
            int angleIndex;
            int contactCos;
            int movingVelocityY;
            int contactSin;
            u16 contactAngle;
            int hitTangent;
            int inverseContactCos;
            int hitResultX;
            u16 relativeAngle;
            int xi;
            int inverseContactSin;
            int movingNormal;
            int pixelY;
            int movingVelocityX;
            int hitNormal;
            int hitVelocityX;
            int movingResultY;
            int hitResultY;
            int hitVelocityY;

            /* Contact line, from the hit stone to the moving one. */
            dx = mStone[stoneIndex].x - mStone[hitIndex].x;
            dy = mStone[stoneIndex].y - mStone[hitIndex].y;
            contactAngle = cstd::atan2(dy, dx);
            relativeAngle = -contactAngle;
            /* Both velocities as x/y. If the hit stone has zero y velocity,
             * it takes half the moving stone's y velocity before the exchange. */
            pAngle = &mStone[stoneIndex].angle;
            pSpeed = &mStone[stoneIndex].speed;
            /* The index stays by hand here: FX_SinIdx/FX_CosIdx at these eight
             * reads, with or without an index local, reallocate registers
             * across the function (57-99 words). */
            angleIndex = (mStone[stoneIndex].angle >> 4) * 2;
            movingVelocityX = FX_MUL(FX_SinCosTable_[angleIndex + 1], mStone[stoneIndex].speed);
            movingVelocityY = FX_MUL(FX_SinCosTable_[angleIndex], mStone[stoneIndex].speed);
            pHitAngle = &mStone[hitIndex].angle;
            angleIndex = (mStone[hitIndex].angle >> 4) * 2;
            hitVelocityX = FX_MUL(FX_SinCosTable_[angleIndex + 1], mStone[hitIndex].speed);
            hitVelocityY = FX_MUL(FX_SinCosTable_[angleIndex], mStone[hitIndex].speed);
            if (hitVelocityY == 0)
                hitVelocityY = movingVelocityY >> 1;
            /* sin/cos of -contactAngle rotate into the contact frame; sin/cos of
             * +contactAngle rotate back out. */
            angleIndex = (relativeAngle >> 4) * 2;
            inverseContactSin = FX_SinCosTable_[angleIndex];
            inverseContactCos = FX_SinCosTable_[angleIndex + 1];
            relativeAngle = -relativeAngle;
            angleIndex = (relativeAngle >> 4) * 2;
            contactSin = FX_SinCosTable_[angleIndex];
            contactCos = FX_SinCosTable_[angleIndex + 1];

            /* Normal and tangential components of each velocity. */
            movingNormal = FX_MUL(inverseContactCos, movingVelocityX);
            movingNormal -= FX_MUL(inverseContactSin, movingVelocityY);
            movingTangent = FX_MUL(inverseContactSin, movingVelocityX) +
                            FX_MUL(inverseContactCos, movingVelocityY);
            hitNormal =
                FX_MUL(inverseContactCos, hitVelocityX) - FX_MUL(inverseContactSin, hitVelocityY);
            hitTangent =
                FX_MUL(inverseContactSin, hitVelocityX) + FX_MUL(inverseContactCos, hitVelocityY);
            /* Swap the normal components and rotate back: dx/movingResultY for the
             * moving stone, hitResultX/hitResultY for the one it hit. */
            dx = FX_MUL(contactCos, hitNormal) - FX_MUL(contactSin, movingTangent);
            movingResultY = FX_MUL(contactSin, hitNormal) + FX_MUL(contactCos, movingTangent);
            hitResultX = FX_MUL(contactCos, movingNormal) - FX_MUL(contactSin, hitTangent);
            hitResultY = FX_MUL(contactSin, movingNormal) + FX_MUL(contactCos, hitTangent);
            *pAngle = cstd::atan2(movingResultY, dx);
            *pSpeed = cstd::sqrt(((long long)dx * dx + (long long)movingResultY * movingResultY));

            /* Separate the stones along the contact line, then keep the
             * moving one on the board and hand any overshoot to the other. */
            mStone[stoneIndex].x = mStone[hitIndex].x + FX_MUL(contactCos, 0x1b000);
            mStone[stoneIndex].y = mStone[hitIndex].y + FX_MUL(contactSin, 0x1b000);
            xi = mStone[stoneIndex].x >> 12;
            pixelY = mStone[stoneIndex].y >> 12;
            if (xi - 0xc < 0) {
                xi = mStone[stoneIndex].x - 0xc000;
                mStone[hitIndex].x += xi;
                mStone[stoneIndex].x = 0xc000;
            }
            if (xi + 0xc > 0x100) {
                mStone[hitIndex].x += mStone[stoneIndex].x - 0xf4000;
                mStone[stoneIndex].x = 0xf4000;
            }
            if (pixelY - 0xc < -0xe0) {
                mStone[hitIndex].y += mStone[stoneIndex].y + 0xd4000;
                mStone[stoneIndex].y = -0xd4000;
            }

            *pHitAngle = cstd::atan2(hitResultY, hitResultX);
            mStone[hitIndex].speed = cstd::sqrt(
                ((long long)hitResultX * hitResultX + (long long)hitResultY * hitResultY));
            mStone[stoneIndex].state = 1;
            mStone[hitIndex].state = 1;
            if (mStone[hitIndex].speed >= 0x3800) {
                mStone[hitIndex].fast = 1;
            } else {
                mStone[hitIndex].fast = 0;
            }
            func_02012718(0xe8, mStone[stoneIndex].x);
            return;
        }
    }
}

/* Sets stone i's spin from the x component of its velocity:
 * -(cos(angle) * speed) / 4. */
void dScMgCurling_c::SetStoneSpin(int stoneIndex) {
    int cosHeading = FX_CosIdx(mStone[stoneIndex].angle >> 4);
    int speed = mStone[stoneIndex].speed;
    mStone[stoneIndex].spin = -FX_MUL(cosHeading, speed) >> 2;
}

/* Stone i while it is being dragged. With the stylus down the stone
 * follows the touch point plus the grab offset (grabX/grabY), its x kept
 * between 0xe and 0xf2 units, and the offset is recomputed; with the stylus
 * up the drag state clears and, if the aimed stone (mAimX/mAimY) overlaps it, the
 * aimed stone is moved just below it. */
void dScMgCurling_c::Stone_Drag(int stoneIndex) {
    unsigned char playerSlot = gActivePlayerSlot;
    if (((struct B4 *)gTouchHeld)[playerSlot].v) {
        int stoneXPixels, grabXPixels, grabYPixels;
        mStone[stoneIndex].x =
            mStone[stoneIndex].grabX + (((struct B4 *)gTouchX)[playerSlot].v << 12);
        stoneXPixels = mStone[stoneIndex].x >> 12;
        if (stoneXPixels < 0xe)
            mStone[stoneIndex].x = 0xe000;
        if (stoneXPixels > 0xf2)
            mStone[stoneIndex].x = 0xf2000;
        grabXPixels = (mStone[stoneIndex].x >> 12) - ((struct B4 *)gTouchX)[gActivePlayerSlot].v;
        grabYPixels = (mStone[stoneIndex].y >> 12) - ((struct B4 *)gTouchY)[playerSlot].v;
        mStone[stoneIndex].grabX = grabXPixels << 12;
        mStone[stoneIndex].grabY = grabYPixels << 12;
    } else {
        int dx, dy;
        mStone[stoneIndex].state = 0;
        dx = (mAimX - mStone[stoneIndex].x) >> 12;
        dy = (mAimY - mStone[stoneIndex].y) >> 12;
        if (dx < -0x2e)
            return;
        if (dx > 0x2e)
            return;
        if (dy < -0x14)
            return;
        if (dy <= 0x14)
            mAimY = mStone[stoneIndex].y + 0x15000;
    }
}

/* A tail-call veneer to ResolveStoneCollision. */
void dScMgCurling_c::Stone_Collide(int idx) {
    ResolveStoneCollision(idx);
}

/* Stone idx while it slides. Moves it by its velocity, adds the spin to
 * the angle, bounces it off the four walls (with a sound), then takes
 * friction off the speed: a base of speed/512 (at least 0x1c), more in the
 * five bands below the house from the sCurlingSlideZoneEdges tables, and a
 * steeper rate while the stone still travels upward above -0x20. At zero
 * speed the stone stops (state 2). The collision and spin helpers run, and
 * the slide sound (slideSnd) is retuned to the speed. */
void dScMgCurling_c::Stone_Slide(int stoneIndex) {
    int stoneOffset;
    int zi;
    u16 *pAngle;
    int *pSpeed;
    int *pX;
    int *pY;
    int x;
    int z;
    int xi;
    int v;
    int sbv;
    int i2;
    int s;
    int p;
    int w;
    int *pd;
    int cosAngle;
    int sinAngle;

    stoneOffset = stoneIndex * 0x2c;

    pAngle = (u16 *)((char *)&mStone[0].angle + stoneOffset);
    pSpeed = (int *)((char *)&mStone[0].speed + stoneOffset);
    pX = &mStone[stoneIndex].x;
    pY = &mStone[stoneIndex].y;

    cosAngle = FX_CosIdx(*pAngle >> 4);
    *pX += FX_MUL(cosAngle, *pSpeed);
    sinAngle = FX_SinIdx(*pAngle >> 4);
    *pY += FX_MUL(sinAngle, *pSpeed);
    mStone[stoneIndex].heading += (u16)mStone[stoneIndex].spin;

    x = *pX;
    z = *pY;
    xi = x >> 12;
    zi = z >> 12;

    if (xi + 0xc >= 0x100) {
        *pAngle = 0x8000 - *pAngle;
        *pX = 0xf4000;
        func_02012718(0x1d4, *pX);
    } else if (xi - 0xc < 0) {
        *pAngle = 0x8000 - *pAngle;
        *pX = 0xc000;
        func_02012718(0x1d4, *pX);
    }

    if (zi + 0xc > 0xc0) {
        mStone[stoneIndex].angle = -mStone[stoneIndex].angle;
        *pY = 0xb4000;
        func_02012718(0x1d4, *pX);
    } else if (zi - 0xc < -0xe0) {
        mStone[stoneIndex].angle = -mStone[stoneIndex].angle;
        *pY = -0xd4000;
        func_02012718(0x1d4, *pX);
    }

    zi = *pSpeed;
    v = zi >> 9;
    if (v <= 0x1c)
        v = 0x1c;
    for (i2 = 0, sbv = *pY >> 12; i2 < 5; i2++) {
        if (sbv <= -(sCurlingSlideZoneEdges[i2] + 0x20)) {
            s = FX_SinIdx(*pAngle >> 4);
            if (s < 0) {
                v = zi >> sCurlingSlideShiftLeft[i2];
                if (v < sCurlingSlideMinLeft[i2])
                    v = sCurlingSlideMinLeft[i2];
            } else if (s > 0) {
                v = zi >> sCurlingSlideShiftRight[i2];
                if (v < sCurlingSlideMinRight[i2])
                    v = sCurlingSlideMinRight[i2];
            }
            break;
        }
    }

    if (sbv > -0x20) {
        s = FX_SinIdx(*pAngle >> 4);
        if (s > 0) {
            v = zi >> 3;
            if (v <= 0x180)
                v = 0x180;
        }
    }

    mStone[stoneIndex].speed -= v;
    pd = &mStone[stoneIndex].speed;
    if (mStone[stoneIndex].speed <= 0) {
        *pSpeed = 0;
        mStone[stoneIndex].state = 2;
    }

    ResolveStoneCollision(stoneIndex);
    SetStoneSpin(stoneIndex);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    mStone[stoneIndex].slideSnd =
        func_02012468(mStone[stoneIndex].slideSnd, 2, 0xe7, 7, p, w, func_020126e8(*pX), 0);
}

/* Releases stone idx if the aimed stone (mAimX/mAimY) overlaps it: it takes
 * the aim angle (mSwingAng, clamped to the lower half turn) and the swing
 * speed (mSwingSpeed), is flagged fast above 0x3800, and starts sliding. The aim is
 * then put away, the stones are separated, and the throw sound plays at a
 * volume that depends on the fast flag. */
void dScMgCurling_c::Stone_WaitRelease(int idx) {
    int v, w, vol;

    if (mStylusHeld != 1)
        return;

    v = (mAimX - mStone[idx].x) >> 12;
    w = (mAimY - mStone[idx].y) >> 12;
    if (v < -0x2e)
        return;
    if (v > 0x2e)
        return;
    if (w < -0x14)
        return;
    if (w > 0x14)
        return;

    mStone[idx].state = 1;
    mStone[idx].angle = mSwingAng;
    mStone[idx].speed = mSwingSpeed;

    if (mSwingAng < 0x9800u || mSwingAng > 0xe800u) {
        if (mSwingAng >= 0x4000u && mSwingAng <= 0x9800u) {
            mSwingAng = 0x9800;
        } else {
            mSwingAng = 0xe800;
        }
        mStone[idx].angle = mSwingAng;
    }

    if (mSwingSpeed >= 0x3800) {
        mStone[idx].fast = 1;
    } else {
        mStone[idx].fast = 0;
    }

    mNextStone = 1;
    mStoneDelay = 0;
    SeparateStones(idx);

    vol = 0x7f;
    if (mStone[idx].fast == 0)
        vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0, func_020126e8(mStone[idx].x));
}

/* Brings out the next stone once the mStoneDelay delay has run down: it starts
 * active at (0x80000, 0x80000), the stone count mThrown goes up (with a
 * sound from the second stone on), and the aim point resets. */
void dScMgCurling_c::BringOutNextStone() {
    if (mNextStone == 0)
        return;
    if (mStoneDelay != 0) {
        mStoneDelay--;
        if ((short)mStoneDelay <= 0)
            mStoneDelay = 0;
        return;
    }
    mNextStone = 0;
    {
        int idx = mThrown;
        if (idx >= 5)
            return;
        mStone[idx].active = 1;
        mStone[idx].dealt = 1;
        mStone[idx].x = 0x80000;
        mStone[idx].y = 0x80000;
        mStone[idx].timer = 0;
        mStone[idx].fast = 0;
    }
    if (mThrown != 0)
        Sound::PlayBank2_2D(0x1d7);
    mThrown++;
    mAimX = 0x80000;
    mAimY = 0xb0000;
    mStylusHeld = 0;
    mAimShown = 1;
}

/* The scene's idle state. */
void dScMgCurling_c::Scene_Idle() {}

/* The scene's end state. After the mStateDelay delay it asks the base whether
 * the game is over, moves to state 4 with the base's own end state, and
 * clears the stones' visible flags. */
void dScMgCurling_c::Scene_End() {
    int i;
    if (mStateDelay != 0) {
        mStateDelay--;
        if ((s16)mStateDelay <= 0)
            mStateDelay = 0;
        return;
    }
    if (Mg_IsNewRecord() != 0) {
        mRoundOver = 0;
        mState = 4;
        Mg_SetSharedState(MG_STATE_RESULTS);
    } else {
        mState = 4;
        Mg_SetSharedState(MG_STATE_RESULTS);
    }
    mPromptEnabled = 0;
    mAimShown = 0;
    for (i = 0; i < 5; i++) {
        mStone[i].dealt = 0;
    }
}

/* The scoring state. Runs the popup countdowns, then after the mStateDelay
 * delay either starts the next stone (state 1) or, once all five have been
 * thrown, ends the round (state 3) with the popups' points summed into the
 * score. The popups are cleared either way. */
void dScMgCurling_c::Scene_Score() {
    int i;
    int sum;

    Popup_UpdateAll();

    if (mStateDelay != 0) {
        mStateDelay--;
        if ((s16)mStateDelay <= 0)
            mStateDelay = 0;
        return;
    }

    mState = 1;
    if (mThrown >= 5) {
        mState = 3;
        mStateDelay = 0x80;
        mRoundOver = 1;
        sum = 0;
        Sound::PlayBank2_2D(0x1bc);
        for (i = 0; i < 5; i++)
            sum += mPopup[i].points;
        Mg_SubmitScore(sum);
    }

    for (i = 0; i < 5; i++) {
        mPopup[i].x = 0;
        mPopup[i].y = 0;
        mPopup[i].points = 0;
        mPopup[i].countdown = 0;
        mPopup[i].shown = 0;
        mPopup[i].live = 0;
    }

    BringOutNextStone();
}

/* The playing state. After the mStateDelay delay it shows the HUD once, runs
 * the aim state (mStylusHeld) through its table, then every active stone's own
 * state through the other table, remembering the last position of each.
 * When no stone is still moving, the popups are scored and the scene moves
 * to state 2 with a 0xc0 delay. */
void dScMgCurling_c::Scene_Play() {
    if (mStateDelay != 0) {
        mStateDelay--;
        return;
    }
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    if (mSwingTimer != 0) {
        mSwingTimer--;
    }
    (((C *)this)->*sCurlingAimTable[mStylusHeld])();

    {
        int count = 0;
        int i = 0;
        for (; i < 5; i++) {
            if (mStone[i].active != 0) {
                mStone[i].lastX = mStone[i].x;
                mStone[i].lastY = mStone[i].y;
                if (mStone[i].timer != 0)
                    mStone[i].timer--;
                (((C *)this)->*sCurlingStoneTable[mStone[i].state])(i);
                if (mStone[i].state != 2)
                    count++;
            }
        }
        if (count != 0)
            return;
    }
    mState = 2;
    ScoreStones();
    mStateDelay = 0xc0;
}

/* Starts a round: clears the stones, brings out the first one and enters
 * the aiming state. */
void dScMgCurling_c::Scene_StartRound() {
    ClearStones();
    mNextStone = 1;
    mStoneDelay = 0;
    BringOutNextStone();
    mState = 1;
}

/* Picks the house (mHouse): a fresh random one of three that differs from
 * the last, or the first one when the slot still holds its 0xff reset.
 * The house centre comes from sCurlingHouseSpots and its background from
 * one of three files. */
void dScMgCurling_c::PickHouse() {
    int m;
    void *file;
    if (mHouse == 0xff) {
        mHouse = 0;
    } else {
        m = CURL_RAND(3);
        if (mHouse == m) {
            m += CURL_RAND(2) + 1;
            if (m >= 3)
                m -= 3;
        }
        mHouse = m;
    }
    mHouseX = sCurlingHouseSpots[mHouse * 2] << 12;
    mHouseY = (sCurlingHouseSpots[mHouse * 2 + 1] - 0xe0) << 12;
    if (mHouse == 0) {
        file = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NSC_BIN);
    } else if (mHouse == 1) {
        file = LoadFile(ASSET_D_2D_MG_BG_CURLING1_LEFT_NSC_BIN);
    } else if (mHouse == 2) {
        file = LoadFile(ASSET_D_2D_MG_BG_CURLING1_RIGHT_NSC_BIN);
    }
    GX_LoadBG2Scr(file, 0, 0x800);
    Deallocate(file);
}

/* Marks the house as not chosen yet. */
void dScMgCurling_c::ResetHouse() {
    mHouse = 255;
}

/* Zeroes the five stones, the five score popups and the aim fields, and
 * resets the score. */
void dScMgCurling_c::ClearStones() {
    int i;
    for (i = 0; i < 5; i++) {
        mStone[i].x = 0;
        mStone[i].y = 0;
        mStone[i].speed = 0;
        mStone[i].lastX = 0;
        mStone[i].lastY = 0;
        mStone[i].slideSnd = 0;
        mStone[i].spin = 0;
        mStone[i].heading = 0;
        mStone[i].angle = 0;
        mStone[i].state = 0;
        mStone[i].active = 0;
        mStone[i].dealt = 0;
    }
    for (i = 0; i < 5; i++) {
        mPopup[i].x = 0;
        mPopup[i].y = 0;
        mPopup[i].points = 0;
        mPopup[i].countdown = 0;
        mPopup[i].live = 0;
        mPopup[i].shown = 0;
    }
    mStoneDelay = 0;
    mThrown = 0;
    mAimX = 0;
    mAimY = 0;
    mGrabX = 0;
    mGrabY = 0;
    mSwingSpeed = 0;
    mSwingAng = 0;
    mStylusHeld = 0;
    mAimShown = 0;
    mNextStone = 0;
    mRoundOver = 0;
    mStateDelay = 0;
    unk_4edc = 0;
    mSwingTimer = 0;
    Mg_SubmitScore(0);
}

/* Slot 18, one of dScMgBase_c's own undeclared slots; the inherited label
 * is the tree-wide mislabel and this is not a destructor. Restarts the
 * scene: state 0, the stones cleared, a house picked, the blend set up
 * again and the base's handle refreshed. */
void dScMgCurling_c::StartRound(int /* arg */) {
    mState = 0;
    ClearStones();
    PickHouse();
    G2_SetBlendAlpha(0, 0xd, 2, 0x10);
    G2S_SetBlendAlpha(0, 4, 2, 0x10);
    mTopRecord = Mg_GetTopRecord();
}

/* Slot 9. The base's frame, then the popups, the stones, the aim sprite
 * and the falling bits. */
s32 dScMgCurling_c::Render() {
    Mg_DrawTopLeftCounter(Mg_GetTopRecord());
    Popup_DrawAll();
    DrawStonesLeft();
    DrawAimMarker();
    Bit_DrawAll();
    return 1;
}

/* Slot 6. The scene state through its table, then the falling bits. The
 * table's receiver C stays incomplete. */
s32 dScMgCurling_c::Behavior() {
    C *c = (C *)this;
    int j = mState;
    (c->*sCurlingSceneTable[j])();
    Bit_UpdateAll();
    return 1;
}

/* Slot 0. Loads the board and the stone graphics for both screens, the
 * palettes and the house, then starts the first round with a 0x40 delay. */
s32 dScMgCurling_c::InitResources() {
    void *a = Ov004_LoadFileByPath(&sCurlingMgDirPath);
    void *b = Ov004_LoadFileByPath(&sCurlingBgCurling2NcgPath);
    void *f;

    if (a == 0 || b == 0)
        return 0;

    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    G2_SetBG2Priority(2);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x20000);
    DecompressLZ16(a, G2_GetBG2CharPtr());

    f = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NCL_BIN);
    GX_LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NSC_BIN);
    GX_LoadBG2Scr(f, 0, 0x800);
    Deallocate(f);

    {
        void *c7 = LoadFile(ASSET_D_2D_MG_CURLING_NCG_BIN);
        void *c8 = LoadFile(ASSET_D_2D_MG_CURLING_NCL_BIN);
        DecompressLZ16(c7, G2_GetOBJCharPtr());
        GX_LoadOBJPltt(c8, 0, 0x100);

        gSubScreenLayers |= 4;
        G2S_SetBG2Priority(2);
        G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x4000,
                              GX_BG_CHARBASE_0x14000);
        DecompressLZ16(b, (void *)G2S_GetBG2CharPtr());

        f = LoadFile(ASSET_D_2D_MG_BG_CURLING2_NCL_BIN);
        GXS_LoadBGPltt(f, 0x60, 0x1a0);
        Deallocate(f);

        f = LoadFile(ASSET_D_2D_MG_BG_CURLING2_NSC_BIN);
        GXS_LoadBG2Scr(f, 0, 0x800);
        Deallocate(f);

        Ov004_Deallocate(a);
        Ov004_Deallocate(b);

        DecompressLZ16(c7, G2S_GetOBJCharPtr());
        GXS_LoadOBJPltt(c8, 0, 0x100);
        Deallocate(c7);
        Deallocate(c8);
    }

    ClearStones();
    ResetHouse();
    PickHouse();
    mNextStone = 1;
    mStoneDelay = 0;
    BringOutNextStone();
    Bit_InitAll();
    mState = 1;
    Mg_SetTopScreenYOffset(0x20);
    mStateDelay = 0x40;
    mRunDuringFade = 1;
    G2_SetBlendAlpha(0, 0xd, 2, 0x10);
    G2S_SetBlendAlpha(0, 4, 2, 0x10);
    mTopRecord = Mg_GetTopRecord();
    return 1;
}

/* Reconstructed source-style name: SM64DS proves the dScMgCurling_c RTTI
 * identity, MG_CURLING registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: MgShuffleShell_Spawn. */
void *dScMgCurling_c_classInit() {
    return new dScMgCurling_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_021304ac shard).
 * The seven PMF state tables plain-copied from the ROM PMF constants, in
 * retail copy order: the five no-arg scene states, the four per-state
 * tables, the two stylus states, the three kind states, the four Entry
 * states, the four reveal states, then the three fallState states. */
PMF0 sCurlingSceneTable[5] = {
    pmf_dScMgCurling_c_Scene_StartRound, pmf_dScMgCurling_c_Scene_Play,
    pmf_dScMgCurling_c_Scene_Score,      pmf_dScMgCurling_c_Scene_End,
    pmf_dScMgCurling_c_Scene_Idle,
};
PMF sCurlingStoneTable[4] = {
    pmf_dScMgCurling_c_Stone_WaitRelease,
    pmf_dScMgCurling_c_Stone_Slide,
    pmf_dScMgCurling_c_Stone_Collide,
    pmf_dScMgCurling_c_Stone_Drag,
};
PMF0 sCurlingAimTable[2] = {
    pmf_dScMgCurling_c_Aim_WaitTouch,
    pmf_dScMgCurling_c_Aim_Swing,
};
PMF sCurlingBitKindTable[3] = {
    pmf_dScMgCurling_c_Bit_PickKind,
    pmf_dScMgCurling_c_Bit_RunKindB,
    pmf_dScMgCurling_c_Bit_RunKindA,
};
Entry sCurlingBitBStateTable[4] = {
    {pmf_dScMgCurling_c_BitB_StatePause},
    {pmf_dScMgCurling_c_BitB_StateDriftRight},
    {pmf_dScMgCurling_c_BitB_StateDriftLeft},
    {pmf_dScMgCurling_c_BitB_StateDriftStop},
};
PMF sCurlingBitAStateTable[4] = {
    pmf_dScMgCurling_c_BitA_StatePause,
    pmf_dScMgCurling_c_BitA_StateDriftRight,
    pmf_dScMgCurling_c_BitA_StateDriftLeft,
    pmf_dScMgCurling_c_BitA_StateDriftStop,
};
PMF sCurlingBitStateTable[3] = {
    pmf_dScMgCurling_c_Bit_StateFallStart,
    pmf_dScMgCurling_c_Bit_StateFallSpeedUp,
    pmf_dScMgCurling_c_Bit_StateFallSlowDown,
};
