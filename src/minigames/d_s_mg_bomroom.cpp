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
 *
 * Each pragma bracket below carries the file-global pragma of the shard
 * that function came from; every bracket moves bytes, and they bind only
 * under defer_codegen off.
 */

// leftovers: a few bomb fields are reached through a bomb pointer stepped
// by i * 0x40 and cast back to the scene, ((dScMgBomroom_c *)bomb)->
// mBombs[0].field, because indexing mBombs[i] directly changes the code:
// OnBombExploded's pen, state and color (&mBombs[i].state is 4 bytes
// shorter or 8 longer; .color and &mBombs[picked].pen swap registers),
// CheckColorComplete's lineSlot and SpawnBombs' color (8 bytes longer each).
// SpawnBombs also adds the offset of mBombs[0].active to this + j * 0x40.
// CountSortedBombs bumps mLevel through a volatile store. Bomb_StateInPen
// keeps the sine-table value as the first factor (inline reads flip smull
// order).

#include "dScMgBomroom_c.h"
#include "types.h"
#include "GX.h"
#include "cstd.h"
#include "decl_common.h"
#include "Sound.h"
#include "G2x.h"
#include "nitro/fx/fx.h"
#include "G2.h"

extern "C" {
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern int sBomroomPenGateSprites[];
extern int sBomroomPenGateX[];
extern int sBomroomPenGateY[];
extern int sBomroomHatchPanelSprites[];
extern int sBomroomTopDoorSprites[];
extern int sBomroomHatchPanelX[];
extern int sBomroomHatchPanelY[];
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern unsigned char *sBomroomBombFrameTimes[];
extern unsigned char sBomroomBombFrameCounts[];
extern s16 data_02082214[];
extern s16 atan2_hold_d_s_mg_bomroom_1(int y, int x);
extern void Mg_RenderSpriteBoth(void* a0, int a1, int a2, int a3, int a4);
extern unsigned char *sBomroomBombSprites[];
extern unsigned char *sBomroomBombLitSprites[];
extern void *sBomroomBombsSprites[];
extern void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern void *sBomroomRoundOverSprites;
extern void *sBomroomMarkerPlainSprites[];
extern void *sBomroomMarkerLitSprites[];
extern int sBomroomMarkerY[];
extern int Mg_GetSubmittedScore(void);
extern int RandomIntInternal(int *seed);
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern u8 gTouchX[];
// local extern: see above.
extern u8 gTouchY[];
extern u16 sBomroomSpawnDelays[];
extern int sBomroomRoundEndTable[];
extern int LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);

extern void MultiStore16(u16 val, char *dst, int nbytes);

extern void func_02056554(const void *src, int offset, int count);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

#pragma defer_codegen off

/* No member needs explicit destruction: the empty body is the compiler's
 * own vtable store and base-destructor call. The deleting variant reaches
 * dScMgBase_c's operator delete, its immediate base's. */
// @symbol _ZN14dScMgBomroom_cD1Ev
// @symbol _ZN14dScMgBomroom_cD0Ev
dScMgBomroom_c::~dScMgBomroom_c()
{
}

// @symbol _ZN14dScMgBomroom_c13DrawRoundOverEv
/* Draws the round-over sprite once mRoundOver is set. */

void dScMgBomroom_c::DrawRoundOver() {
    if (mRoundOver == 0)
        return;
    RenderOamMainScreen((int)sBomroomRoundOverSprites, 0x80, 0xc0, -1, 1);
}


// @symbol _ZN14dScMgBomroom_c12SetRoundOverEv

void dScMgBomroom_c::SetRoundOver() {
    mRoundOver = 1;
}

// @symbol _ZN14dScMgBomroom_c13UpdateBg2LiftEv
/* Steps the BG2 y offset mBg2Y (Fix12): state 1 raises it by 3 a
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

// @symbol _ZN14dScMgBomroom_c12ResetBg2LiftEv

void dScMgBomroom_c::ResetBg2Lift() {
    int zero = 0;
    mBg2Y = zero;
    mBg2State = zero;
    mBg2Hold = zero;
    SetBg2Offset(zero, zero);
}

// @symbol _ZN14dScMgBomroom_c14Marker_DrawAllEv
/* Draws the two mMarkers sprite records that are active. */

void dScMgBomroom_c::Marker_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mMarkers[i].gate != 0) {
            int xv = mMarkers[i].x;
            int yv = mMarkers[i].y;
            int idx = mMarkers[i].idx;
            int a1 = xv >> 12;
            int a2 = yv >> 12;
            Hud_RenderSprite((i != 0) ? sBomroomMarkerLitSprites[idx]
                                      : sBomroomMarkerPlainSprites[idx],
                             a1, a2, -1, 1);
        }
    }
}


// @symbol _ZN14dScMgBomroom_c16Marker_UpdateAllEv
#pragma push
#pragma opt_strength_reduction off

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

#pragma pop

// @symbol _ZN14dScMgBomroom_c11Marker_ShowEi

void dScMgBomroom_c::Marker_Show(int idx) {
    mMarkers[idx].state = 1;
    mMarkers[idx].gate = 1;
    mMarkers[idx].x = 0x80000;
    mMarkers[idx].y = sBomroomMarkerY[idx] << 12;
    mMarkers[idx].timer = 0;
    mMarkers[idx].idx = 0;
}

/* The scene keeps its per-slot UI state in the two-entry Slot arrays
 * mHatchPanels, mPenGates and mMarkers. The loops over them match only
 * with strength reduction off, so each carries its own bracket. */
// @symbol _ZN14dScMgBomroom_c14Marker_HideAllEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::Marker_HideAll() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mMarkers[slot].state = 0;
        mMarkers[slot].gate = 0;
    }
}

#pragma pop

// @symbol Bomroom_DrawScore
extern "C" {
void Bomroom_DrawScore(void)
{
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 4);
}
}

// @symbol _ZN14dScMgBomroom_c13TopDoor_CloseEv

void dScMgBomroom_c::TopDoor_Close() {
    mTopDoor.state = 3;
    Sound::PlayBank2_2D(0x1e3);
}

// @symbol _ZN14dScMgBomroom_c12TopDoor_OpenEv

void dScMgBomroom_c::TopDoor_Open() {
    mTopDoor.state = 1;
    Sound::PlayBank2_2D(0x1e2);
}

// @symbol _ZN14dScMgBomroom_c12TopDoor_DrawEv

void dScMgBomroom_c::TopDoor_Draw() {
    if (mTopDoor.level == 0)
        return;
    RenderOamMainScreen(sBomroomTopDoorSprites[mTopDoor.idx], mTopDoor.x >> 0xc, mTopDoor.y >> 0xc,
                        -1, 2);
}

// @symbol _ZN14dScMgBomroom_c20TopDoor_StateClosingEi
// The top door is a single record, but its state handlers keep the index the
// shared PMF signature passes (TopDoor_Update always passes 0), indexing off
// mTopDoor the way the two-slot mPenGates handlers index their array.

void dScMgBomroom_c::TopDoor_StateClosing(int index) {
    if ((&mTopDoor)[index].idx != 0) {
        (&mTopDoor)[index].timer++;
        if ((&mTopDoor)[index].timer < 3)
            return;
        (&mTopDoor)[index].timer = 0;
        (&mTopDoor)[index].idx--;
    } else {
        (&mTopDoor)[index].state = 0;
    }
}

// @symbol _ZN14dScMgBomroom_c17TopDoor_StateOpenEv

void dScMgBomroom_c::TopDoor_StateOpen() {}

// @symbol _ZN14dScMgBomroom_c20TopDoor_StateOpeningEi

void dScMgBomroom_c::TopDoor_StateOpening(int index) {
    if ((&mTopDoor)[index].idx >= 4) {
        (&mTopDoor)[index].idx = 4;
        (&mTopDoor)[index].state = 2;
        return;
    }
    (&mTopDoor)[index].timer++;
    if ((&mTopDoor)[index].timer < 3) {
        return;
    }
    (&mTopDoor)[index].timer = 0;
    (&mTopDoor)[index].idx++;
    if ((&mTopDoor)[index].idx >= 4) {
        (&mTopDoor)[index].idx = 4;
        (&mTopDoor)[index].state = 2;
    }
}

// @symbol _ZN14dScMgBomroom_c17TopDoor_StateShutEi

void dScMgBomroom_c::TopDoor_StateShut(int index) {
    (&mTopDoor)[index].timer = 0;
}

// @symbol _ZN14dScMgBomroom_c14TopDoor_UpdateEv

typedef void (dScMgBomroom_c::*PMF)(int);
struct PMFEntry { PMF pmf; };
extern "C" PMFEntry sBomroomTopDoorStateTable[];
void dScMgBomroom_c::TopDoor_Update() {
    if (!mTopDoor.gate)
        return;
    (this->*(sBomroomTopDoorStateTable[mTopDoor.state].pmf))(0);
}

// @symbol _ZN14dScMgBomroom_c12TopDoor_ShowEv

void dScMgBomroom_c::TopDoor_Show() {
    mTopDoor.gate = 1;
    mTopDoor.state = 0;
    mTopDoor.idx = 0;
    mTopDoor.level = 1;
    mTopDoor.timer = 0;
    mTopDoor.x = 0x80000;
    mTopDoor.y = 0x8000;
}

// @symbol _ZN14dScMgBomroom_c12TopDoor_HideEv

void dScMgBomroom_c::TopDoor_Hide() {
    mTopDoor.gate = 0;
    mTopDoor.level = 0;
}

// @symbol _ZN14dScMgBomroom_c15PenGate_DrawAllEv

void dScMgBomroom_c::PenGate_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        int xv = mPenGates[i].x >> 12;
        int yv = mPenGates[i].y >> 12;
        int k = mPenGates[i].level;
        if (i != 0) k += 5;
        Hud_RenderSprite((void *)sBomroomPenGateSprites[k], xv, yv, -1, -1);
    }
}

// @symbol _ZN14dScMgBomroom_c20PenGate_StateClosingEi

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

// @symbol _ZN14dScMgBomroom_c17PenGate_StateHoldEi

void dScMgBomroom_c::PenGate_StateHold(int index) {
    if (mPenGates[index].timer != 0) {
        mPenGates[index].timer--;
        if ((s16)mPenGates[index].timer < 0)
            mPenGates[index].timer = 0;
        return;
    }

    mPenGates[index].state = 3;
}

// @symbol _ZN14dScMgBomroom_c20PenGate_StateOpeningEi

void dScMgBomroom_c::PenGate_StateOpening(int index) {
    mPenGates[index].timer++;
    if (mPenGates[index].timer < 5)
        return;
    mPenGates[index].timer = 0;
    mPenGates[index].level++;
    if (mPenGates[index].level >= 4) {
        mPenGates[index].state = 2;
        mPenGates[index].timer = 0x20;
    }
}

// @symbol _ZN14dScMgBomroom_c17PenGate_StateShutEi

void dScMgBomroom_c::PenGate_StateShut(int index) {
    mPenGates[index].timer = 0;
}

// @symbol _ZN14dScMgBomroom_c17PenGate_UpdateAllEv
#pragma push
#pragma opt_strength_reduction off

typedef void (dScMgBomroom_c::*PMF)(int);
extern "C" PMF sBomroomPenGateStateTable[];
void dScMgBomroom_c::PenGate_UpdateAll() {
    for (int slot = 0; slot < 2; slot++) {
        if (mPenGates[slot].gate) {
            u8 state = mPenGates[slot].state;
            (this->*sBomroomPenGateStateTable[state])(slot);
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c15PenGate_ShowAllEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::PenGate_ShowAll() {
    void * p = (void *)this;
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

#pragma pop

// @symbol _ZN14dScMgBomroom_c12PenGate_OpenEi

void dScMgBomroom_c::PenGate_Open(int index) {
    unsigned char st = mPenGates[index].state;
    if (st != 0) {
        if (st == 2) {
            mPenGates[index].timer = 0x20;
        } else {
            mPenGates[index].state = 1;
        }
        return;
    }
    mPenGates[index].state = 1;
    SetBg0Offset(0x100, 0);
}

// @symbol _ZN14dScMgBomroom_c15PenGate_HideAllEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::PenGate_HideAll() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mPenGates[slot].gate = 0;
        mPenGates[slot].idx = 0;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c18HatchPanel_DrawAllEv

void dScMgBomroom_c::HatchPanel_DrawAll() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mHatchPanels[i].idx != 0) {
            int xv = mHatchPanels[i].x >> 12;
            int yv = mHatchPanels[i].y >> 12;
            int k = mHatchPanels[i].level;
            int v = (i == 0) ? sBomroomTopDoorSprites[k] : sBomroomHatchPanelSprites[k];
            Hud_RenderSprite((void*)v, xv, yv, -1, 1);
        }
    }
}

// @symbol _ZN14dScMgBomroom_c22HatchPanel_StateRevertEi

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

// @symbol _ZN14dScMgBomroom_c20HatchPanel_StateHeldEv

void dScMgBomroom_c::HatchPanel_StateHeld() {}

// @symbol _ZN14dScMgBomroom_c23HatchPanel_StateAnimateEi
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off

void dScMgBomroom_c::HatchPanel_StateAnimate(int index) {
    mHatchPanels[index].timer += 1;
    if (mHatchPanels[index].timer < 5)
        return;
    mHatchPanels[index].timer = 0;
    mHatchPanels[index].level += 1;
    if (index == 0) {
        u8 level = mHatchPanels[index].level;
        if (level == 2 || level == 4) {
            int *offsetY = &mMarkers[0].y;
            *offsetY += 0x8000;
        }
    }
    if (mHatchPanels[index].level == 3 && index == 1)
        Marker_Show(index);
    if (mHatchPanels[index].level >= 4)
        mHatchPanels[index].state = 2;
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c20HatchPanel_StateIdleEi

void dScMgBomroom_c::HatchPanel_StateIdle(int index) {
    mHatchPanels[index].timer = 0;
}

// @symbol _ZN14dScMgBomroom_c20HatchPanel_UpdateAllEv
#pragma push
#pragma opt_strength_reduction off

typedef void (dScMgBomroom_c::*PMF)(int);
extern "C" PMF sBomroomHatchPanelStateTable[];
void dScMgBomroom_c::HatchPanel_UpdateAll() {
    for (int slot = 0; slot < 2; slot++) {
        if (mHatchPanels[slot].gate) {
            u8 state = mHatchPanels[slot].state;
            (this->*sBomroomHatchPanelStateTable[state])(slot);
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c18HatchPanel_ShowAllEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::HatchPanel_ShowAll() {
    void * p = (void *)this;
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

#pragma pop

// @symbol _ZN14dScMgBomroom_c18HatchPanel_HideAllEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::HatchPanel_HideAll() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mHatchPanels[slot].gate = 0;
        mHatchPanels[slot].idx = 0;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19HatchPanel_ActivateEi

void dScMgBomroom_c::HatchPanel_Activate(int index) {
    if (mHatchPanels[index].state != 0)
        return;
    mHatchPanels[index].state = 1;
    Sound::PlayBank2_2D(0x1e2);
    if (index != 0) return;
    Marker_Show(index);
}

// @symbol _ZN14dScMgBomroom_c13DrawRemainingEv

void dScMgBomroom_c::DrawRemaining() {
    if (mShowTally == 0)
        return;
    Mg_DrawBigNumber(0x80, 0xc, mTallyCount, 1, -1, 0, 0);
}


// @symbol _ZN14dScMgBomroom_c16CountSortedBombsEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::CountSortedBombs() {
    int plain, colored, i;
    colored = 0;
    plain = 0;
    for (i = 0; i < 0x70; i++) {
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
        /* Both stores go through volatile: a plain mLevel store makes the
           function 8 bytes longer. */
        *(volatile u32 *)&mLevel = mLevel + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
        mFusesFrozen = 1;
        Sound::PlayBank2_2D(0x1e6);
    } else if (colored >= 0x28) {
        mSceneState = 3;
        mSubState = 0;
        mBlastPen = 0;
        mWinColor = 1;
        *(volatile u32 *)&mLevel = mLevel + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
        mFusesFrozen = 1;
        Sound::PlayBank2_2D(0x1e6);
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c14OnBombExplodedEi
/* Bomb `picked` has gone off. Its pen byte is 0 when it
 * was loose: every other live bomb not yet sorted (state 5) is knocked into
 * state 3 for 0x20 frames. Otherwise only the bombs of that pen's color and
 * the idle ones are. The first blast also records its pen in mBlastPen and
 * sets mSceneState to 3. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::OnBombExploded(int picked) {
    char * raw = (char *)this;
    int i;
    u8 *cur = &((dScMgBomroom_c *)(raw + picked * 0x40))->mBombs[0].pen;
    for (i = 0; i < 0x70; i++) {
        char *bomb = raw + i * 0x40;
        if (mBombs[i].active != 0) {
            if (i != picked) {
                u8 pen = *cur;
                if (pen == 0) {
                    u8 *state = &((dScMgBomroom_c *)bomb)->mBombs[0].state;
                    if (*state != 5) {
                        *state = 3;
                        mBombs[i].marchDelay = 0x20;
                    }
                } else {
                    u8 *state = &((dScMgBomroom_c *)bomb)->mBombs[0].state;
                    if (*state == 5) {
                        if (((dScMgBomroom_c *)bomb)->mBombs[0].color + 1 == pen) {
                            *state = 3;
                            mBombs[i].marchDelay = 0x20;
                        }
                    }
                    if (*state <= 1) {
                        *state = 3;
                        mBombs[i].marchDelay = 0x20;
                    }
                }
            }
        }
    }
    if (mBlastPen != 0)
        return;
    mBlastPen = mBombs[picked].pen + 1;
    mStateTimer = 0x60;
    mSceneState = 3;
    mSubState = 0;
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c12Bomb_AnimateEi
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
                    func_02012718((int)0x1d9, mBombs[index].x);
                } else {
                    func_02012718((int)0x1da, mBombs[index].x);
                }
            }
        }
        if (type == 3 && frame == 1) {
            func_02012718((int)0x1e1, mBombs[index].x);
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


// @symbol _ZN14dScMgBomroom_c16Bomb_CheckSortedEi

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
        func_02012718((int)0x1dc, mBombs[index].x);
    } else {
        if (x >= 0x40) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        mBombs[index].state = 5;
        mBombs[index].type = 0;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].speed = 0x999;
        func_02012718((int)0x1dc, mBombs[index].x);
    }
}

// @symbol _ZN14dScMgBomroom_c18Bomb_CheckWrongPenEi

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

// @symbol _ZN14dScMgBomroom_c14Bomb_CheckGrabEi
/* Picks up a bomb under the stylus: a touch within 12 units across and 15
 * down of it, while nothing is held (mHeldColor is 0xff). grabX/grabY keep the
 * stylus-minus-bomb offset. */
#pragma push
#pragma opt_propagation off


typedef struct { u8 f0, f1, f2, f3; } Tab;
// local extern: see above.
extern u8 gActivePlayerSlot;
// local extern: see above.
extern Tab gTouchHeld[];

void dScMgBomroom_c::Bomb_CheckGrab(int index) {
    u8 touch = gActivePlayerSlot;
    int touching = 0;
    int dx, dy;
    if (gTouchHeld[touch].f0 != 0) {
        if (gTouchHeld[touch].f1 != 0) touching = 1;
    }
    if (touching == 0) return;
    if (mHeldColor != 0xff) return;
    dx = gTouchHeld[touch].f2 - (mBombs[index].x >> 0xc);
    dy = gTouchHeld[touch].f3 - (mBombs[index].y >> 0xc);
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
    func_02012718((int)0x1d2, mBombs[index].x);
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c14Bomb_PushApartEi
#pragma push
#pragma opt_common_subs off

void dScMgBomroom_c::Bomb_PushApart(int index) {
    char* raw = (char*)this;
    int x, y, py, dx, dy1, ang, i2, a, dy2, sy, px;

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
            cstd::sqrt((s64)(sy * sy + dy2 * dy2));
            ang = (u16)cstd::atan2(dy2, sy);
            i2 = (ang >> 4) * 2;
            mBombs[index].x = (0xC0 - FX_MUL(data_02082214[i2 + 1], 0xF)) << 12;
            if (py >= 0x60) {
                mBombs[index].y = (0x80 - FX_MUL(data_02082214[i2], 0xF)) << 12;
            } else {
                mBombs[index].y = (0x40 - FX_MUL(data_02082214[i2], 0xF)) << 12;
            }
            a = mBombs[index].angle;
            if (data_02082214[(a >> 4) * 2 + 1] > 0) {
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
    cstd::sqrt((s64)(dy2 * dy2 + sy * sy));
    ang = (u16)cstd::atan2(sy, dy2);
    i2 = (ang >> 4) * 2;
    mBombs[index].x = (0x40 - FX_MUL(data_02082214[i2 + 1], 0xF)) << 12;
    if (py >= 0x60) {
        mBombs[index].y = (0x80 - FX_MUL(data_02082214[i2], 0xF)) << 12;
    } else {
        mBombs[index].y = (0x40 - FX_MUL(data_02082214[i2], 0xF)) << 12;
    }
    a = mBombs[index].angle;
    if (data_02082214[(a >> 4) * 2 + 1] < 0) {
        mBombs[index].angle = 0x8000 - a;
    } else {
        mBombs[index].angle = 0 - a;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c9DrawBombsEv

void dScMgBomroom_c::DrawBombs() {
    int i;
    for (i = 0; i < 0x70; i++) {
        if (mBombs[i].visible == 0)
            continue;
        {
            int b694 = mBombs[i].type;
            int palette = 1;
            int b695, b696;
            unsigned char sprite;
            int b697;
            int f664;
            int x, y;
            if (b694 == 3) palette = 0;
            b696 = mBombs[i].color;
            b695 = mBombs[i].frame;
            if (b696 != 0) {
                sprite = sBomroomBombLitSprites[b694][b695];
            } else {
                sprite = sBomroomBombSprites[b694][b695];
            }
            b697 = mBombs[i].state;
            x = mBombs[i].x >> 12;
            f664 = mBombs[i].y;
            y = f664 >> 12;
            if (b697 == 6) {
                if (mBombs[i].substate == 4) { y = (f664 - mBg2Y) >> 12; }
            }
            Mg_RenderSpriteBoth(sBomroomBombsSprites[sprite], x, y, -1, palette);
        }
    }
}


// @symbol _ZN14dScMgBomroom_c18CheckColorCompleteEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::CheckColorComplete() {
    void * raw = (void *)this;
    unsigned char *bytes = (unsigned char *)raw;
    int ca = 0;
    int cb = 0;
    int cc = 0;
    int i;

    for (i = 0; i < 0x70; i++) {
        unsigned char *bomb = bytes + i * 64;
        if (mBombs[i].active != 0) {
            if (mBlastPen == 1) {
                if (mBombs[i].color != 0) {
                    int v;
                    int q;
                    mBombs[i].substate = 3;
                    v = ca;
                    q = 0;
                    mBombs[i].marchDelay = (unsigned short)(cc * 8);
                    while (v >= 5) { v -= 5; q++; }
                    mBombs[i].lineSlot = (unsigned char)(q * 10 + v);
                    ca++;
                    cc++;
                } else {
                    int v;
                    int q;
                    unsigned char *d = &((dScMgBomroom_c *)bomb)->mBombs[0].lineSlot;
                    *d = (unsigned char)cb;
                    mBombs[i].substate = 3;
                    v = cb;
                    mBombs[i].marchDelay = (unsigned short)(cc * 8);
                    q = 0;
                    while (v >= 5) { v -= 5; q++; }
                    *d = (unsigned char)(q * 10 + v + 5);
                    cb++;
                    cc++;
                }
            } else if (mBlastPen != 0) {
                mBombs[i].lineSlot = (unsigned char)cc;
                mBombs[i].substate = 3;
                mBombs[i].marchDelay = (unsigned short)(cc * 8);
                cc++;
            } else {
                if (mWinColor == mBombs[i].color && mBombs[i].state == 6) {
                    mBombs[i].lineSlot = (unsigned char)cc;
                    mBombs[i].substate = 3;
                    mBombs[i].marchDelay = (unsigned short)(cc * 8);
                    cc++;
                }
            }
        }
    }
    TopDoor_Open();
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c17BombMarch_LinedUpEv

void dScMgBomroom_c::BombMarch_LinedUp() {}

// @symbol _ZN14dScMgBomroom_c16BombMarch_ToLineEi
#pragma push
#pragma opt_common_subs off

void dScMgBomroom_c::BombMarch_ToLine(int index) {
    int off = index << 6;
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
        off = row * 20;
        targetX = slot * 16 + 0x38;
        targetY = off - 0xC0;
        curX = mBombs[index].x >> 12;
        curY = mBombs[index].y >> 12;
        if (targetX == curX && targetY == curY) {
            mBombs[index].substate = 4;
            if (mBombs[index].color != 0)
                func_02012718((int)0x1d9, mBombs[index].x);
            else
                func_02012718((int)0x1da, mBombs[index].x);
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

#pragma pop

// @symbol _ZN14dScMgBomroom_c19BombMarch_OffscreenEv

void dScMgBomroom_c::BombMarch_Offscreen() {}

// @symbol _ZN14dScMgBomroom_c14BombMarch_ExitEi
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::BombMarch_Exit(int index) {
    int color, x;
    {
        s16 sine = data_02082214[((mBombs[index].angle) >> 4) * 2 + 1];
        mBombs[index].x += FX_MUL(sine, mBombs[index].speed);
    }
    {
        s16 sine = data_02082214[((mBombs[index].angle) >> 4) * 2];
        mBombs[index].y += FX_MUL(sine, mBombs[index].speed);
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

#pragma pop

// @symbol _ZN14dScMgBomroom_c16BombMarch_ToGateEi
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::BombMarch_ToGate(int index) {
    int color, x, z, tx, ty;

    tx = (mBombs[index].color != 0) ? 0x100 : 0;
    ty = 0x60;
    tx -= mBombs[index].x >> 12;
    ty -= mBombs[index].y >> 12;

    mBombs[index].angle = cstd::atan2(ty, tx);

    {
        s16 sine = data_02082214[
            ((mBombs[index].angle >> 4) * 2) + 1];

        mBombs[index].x +=
            (int)(((s64)sine *
                   mBombs[index].speed + 0x800) >> 12);
    }

    {
        s16 sine = data_02082214[
            (mBombs[index].angle >> 4) * 2];

        mBombs[index].y +=
            (int)(((s64)sine *
                   mBombs[index].speed + 0x800) >> 12);
    }

    color = mBombs[index].color;
    x = mBombs[index].x >> 12;
    z = mBombs[index].y >> 12;
    tx = color ? 0x100 : 0;

    if (x > tx + 2)
        goto done;
    if (x < tx - 2)
        goto done;
    if (z > 0x62)
        goto done;
    if (z < 0x5e)
        goto done;

    mBombs[index].substate = 1;
    if (mBombs[index].color != 0)
        mBombs[index].angle = 0;
    else
        mBombs[index].angle = 0x8000;

done:
    PenGate_Open(mBombs[index].color);
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c15Bomb_StateMarchEi
/* Runs the bomb's current handler: its substate indexes the
 * pointer-to-member table sBomroomBombMarchTable. */

extern "C" PMFEntry sBomroomBombMarchTable[];
void dScMgBomroom_c::Bomb_StateMarch(int index) {
    unsigned char state = mBombs[index].substate;
    (this->*(sBomroomBombMarchTable[state].pmf))(index);
}

// @symbol _ZN14dScMgBomroom_c15Bomb_StateInPenEi
/* Roaming step: advance x/y along the angle at speed (sine table, Fix12),
 * then bounce off the box selected by color (0xc0..0x100 x 0x40..0x80 for
 * color 1, 0..0x40 x 0x40..0x80 for color 0): a vertical wall mirrors the
 * angle (0x8000 - a), a horizontal one negates it, and the coordinate is
 * pinned just inside the wall. */

void dScMgBomroom_c::Bomb_StateInPen(int idx) {
    dScMgBomroom_c * self = (dScMgBomroom_c *)this;
    int xv;
    int yv;
    s16 sinv;
    s16 cosv;

    sinv = data_02082214[(self->mBombs[idx].angle >> 4) * 2 + 1];
    self->mBombs[idx].x += FX_MUL(sinv, self->mBombs[idx].speed);
    cosv = data_02082214[(self->mBombs[idx].angle >> 4) * 2];
    self->mBombs[idx].y += FX_MUL(cosv, self->mBombs[idx].speed);
    xv = self->mBombs[idx].x >> 12;
    yv = self->mBombs[idx].y >> 12;
    if (self->mBombs[idx].color != 0) {
        if (xv + 0xc > 0x100) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xf4000;
        } else if (xv - 0xc < 0xc0) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xcc000;
        }
        if (yv + 0xc > 0x80) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x4c000;
        }
    } else {
        if (xv + 0xc > 0x40) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0x34000;
        } else if (xv - 0xc < 0) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xc000;
        }
        if (yv + 0xc > 0x80) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x4c000;
        }
    }
}

// @symbol _ZN14dScMgBomroom_c19Bomb_StateExplodingEi

void dScMgBomroom_c::Bomb_StateExploding(int i) {
    if (mBombs[i].exploded == 0)
        return;
    if (mBombs[i].frame != 0) return;
    if (mBombs[i].counter != 1) return;
    Sound::PlayBank2_2D(0x1e0);
    mDispatchDelay = 0x60;
}

// @symbol _ZN14dScMgBomroom_c20Bomb_StateChainDelayEi

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

// @symbol _ZN14dScMgBomroom_c14Bomb_StateHeldEi
/* A held bomb follows the stylus less its grab offset, and overPen records
 * whether it is over its own pen. With nothing held (mHeldColor is 0xff) the
 * bomb goes back to state 1 instead. */
#pragma push
#pragma opt_common_subs off

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

        mBombs[idx].x = (gTouchX[t << 2] << 12) - mBombs[idx].grabX;
        mBombs[idx].y = (gTouchY[t << 2] << 12) - mBombs[idx].grabY;
        cx = old_x >> 12;
        cy = old_y >> 12;

        Bomb_PushApart(idx);

        {
            int t2 = gActivePlayerSlot;
            mBombs[idx].x = (gTouchX[t2 << 2] << 12) - mBombs[idx].grabX;
            mBombs[idx].y = (gTouchY[t2 << 2] << 12) - mBombs[idx].grabY;
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

#pragma pop

// @symbol _ZN14dScMgBomroom_c14Bomb_StateWalkEi
/* While the bomb's fuse runs it counts down (unless mFusesFrozen is
 * set) and the bomb moves along its angle; once it reaches 0 the bomb goes
 * to state 4 and OnBombExploded takes over. */

void dScMgBomroom_c::Bomb_StateWalk(int idx) {
    int t;

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

    t = data_02082214[((int)mBombs[idx].angle >> 4) * 2 + 1];
    mBombs[idx].x += FX_MUL(t, mBombs[idx].speed);
    t = data_02082214[((int)mBombs[idx].angle >> 4) * 2];
    mBombs[idx].y += FX_MUL(t, mBombs[idx].speed);
    Bomb_PushApart(idx);
    Bomb_CheckGrab(idx);
    mBombs[idx].prevX = mBombs[idx].x;
    mBombs[idx].prevY = mBombs[idx].y;
}

// @symbol _ZN14dScMgBomroom_c17Bomb_StateWaitingEi

void dScMgBomroom_c::Bomb_StateWaiting(int i) {
    if (mBombs[i].marchDelay != 0) {
        unsigned short *h = &mBombs[i].marchDelay;
        *h = *h - 1;
    } else {
        mBombs[i].visible = 1;
        mBombs[i].type = 1;
        mBombs[i].state = 1;
    }
}

// @symbol _ZN14dScMgBomroom_c11UpdateBombsEv
/* Per-frame bomb pass: with no touch, nothing is held (mHeldColor = 0xff);
 * then every live bomb runs its state handler from the pointer-to-member
 * table sBomroomBombStateTable, indexed by its state, and Bomb_Animate. */
#pragma push
#pragma opt_strength_reduction off

typedef void (dScMgBomroom_c::*PMF)(int);
extern PMF sBomroomBombStateTable[];
void dScMgBomroom_c::UpdateBombs() {
    if (*(u8 *)(gTouchHeld + gActivePlayerSlot) == 0)
        mHeldColor = 0xff;
    int i;
    for (i = 0; i < 0x70; i++) {
        u8 flag;
        flag = mBombs[i].active;
        if (flag != 0) {
            (this->*sBomroomBombStateTable[mBombs[i].state])(i);
            Bomb_Animate(i);
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c10SpawnBombsEv
/* Spawner: once mSpawnDelay runs out, picks a step from the spawn
 * count mSpawnCount, drops that many new bombs into free records with a
 * random color and angle, and reloads the delay from
 * sBomroomSpawnDelays. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_loop_invariants off

void dScMgBomroom_c::SpawnBombs()
{
    char * c = (char *)this;
  s32 sb;
  s32 step;
  s32 ang;
  s32 i;
  s32 one;
  s32 off;
  s32 t0;
  u32 rnd;
  s32 cnt;
  s32 flag;
  s32 j;
  s32 z0;
  s32 z1;
  s32 z2;
  s32 z3;
  s32 v80000;
  s32 new_var;
  s32 v4;
  s32 v200;
  s32 vB8000;
  s32 j0;
  if (mSpawnDelay != 0)
  {
      mSpawnDelay--;
      if ((s16)mSpawnDelay < 0) {
          mSpawnDelay = 0;
      }
    return;
  }
  t0 = mSpawnCount;
  sb = 0;
  if (t0 >= 0x12c)
  {
    sb = 0xc;
  }
  else
    if (t0 >= 0xc6)
  {
    sb = 0xb;
  }
  else
    if (t0 >= 0x9f)
  {
    sb = 0xa;
  }
  else
    if (t0 >= 0x84)
  {
    sb = 9;
  }
  else
    if (t0 >= 0x5c)
  {
    sb = 8;
  }
  else
    if (t0 >= 0x39)
  {
    sb = 7;
  }
  else
    if (t0 >= 0x21)
  {
    sb = 6;
  }
  else
    if (t0 >= 0x1b)
  {
    sb = 5;
  }
  else
    if (t0 >= 0x15)
  {
    sb = 4;
  }
  else
    if (t0 >= 0xf)
  {
    sb = 3;
  }
  else
    if (t0 >= 9)
  {
    sb = 2;
  }
  else
    if (t0 >= 3)
  {
    sb = 1;
  }
  cnt = 1;
  step = 0;
  if (sb >= 5)
  {
    cnt = 2;
  }
  if (sb == 9)
  {
    cnt = 1;
  }
  flag = 0;
  if (sb == 7)
  {
      unsigned char *pf = &mSpawnHatchFlip;
      s32 bit = mSpawnHatchFlip;
      cnt = 2;
      flag = (bit & 1) + 1;
      *pf ^= 1;
      step = 0x3000;
  }
  new_var = sb;
  if (new_var == 8)
  {
      s32 bit = mSpawnHatchFlip & 1;
      unsigned char *pf = &mSpawnHatchFlip;
      s32 x = *pf;
      cnt = bit + 2;
      *pf = x ^ 1;
      flag = bit + 1;
      if (cnt == 2) {
          step = 0x3000;
      } else {
          step = 0x1800;
      }
  }
  if (new_var == 10)
  {
    cnt = 4;
    step = 0x3000;
  }
  if (sb == 11)
  {
      unsigned char *pf = &mSpawnHatchFlip;
      s32 bit = mSpawnHatchFlip;
      cnt = 3;
      flag = (bit & 1) + 1;
      *pf ^= 1;
      step = 0x1800;
  }
  if (sb >= 12)
  {
    cnt = 6;
    step = 0x1800;
  }
  i = 0;
  if (cnt > 0)
  {
    v80000 = 0x80000;
    v4 = 4;
    v200 = 0x200;
    vB8000 = 0xb8000;
    /* Offset of mBombs[0].active, kept in a register and added to
       this + j*0x40 below: &mBombs[j].active shrinks the function by 0xc. */
    off = (int)&((dScMgBomroom_c *)0)->mBombs[0].active;
    ang = 0;
    z0 = 0;
    z1 = 0;
    z2 = 0;
    z3 = 0;
    one = 1;
    j0 = 0;
    do
    {
      s32 z = z0;
      j = j0;
      while (1) {
          unsigned char *slot = (unsigned char *)((c + (j << 6)) + off);
          if ((*slot) == 0) {
              unsigned char *ptype;
              *slot = (unsigned char)one;
              mBombs[j].state = (char)z;
              mBombs[j].substate = (char)z;
              mBombs[j].lineSlot = (char)z;
              mBombs[j].exploded = (char)z;
              mBombs[j].x = v80000;
              mBombs[j].marchDelay = (s16)v4;
              mBombs[j].fuse = (s16)v200;
              rnd = (u32)RandomIntInternal(&data_0209d4b8);
              ptype = &((dScMgBomroom_c *)(c + (j << 6)))->mBombs[0].color;
              *ptype = (((rnd >> 16) & 0x7fff) << 1) >> 15;
              mBombs[j].speed = 0x999;
              mBombs[j].dragSound = z1;
              if (sb == 0) {
                  *ptype = (unsigned char)one;
                  HatchPanel_Activate(z1);
              }
              if (sb <= 1) {
                  rnd = (u32)RandomIntInternal(&data_0209d4b8);
                  *(s16 *)&mBombs[j].angle = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
                  mBombs[j].y = z2;
              } else if (flag != 0) {
                  if (flag == 1) {
                      *(s16 *)&mBombs[j].angle = ang + 0x2000;
                      mBombs[j].y = z2;
                      HatchPanel_Activate(z2);
                  } else {
                      *(s16 *)&mBombs[j].angle = ang + 0xa000;
                      mBombs[j].y = vB8000;
                      HatchPanel_Activate(one);
                  }
              } else if ((mSpawnCount & 1) != 0) {
                  if (step != 0) {
                      *(s16 *)&mBombs[j].angle = (step * (i >> 1)) + 0x2800;
                  } else {
                      rnd = (u32)RandomIntInternal(&data_0209d4b8);
                      *(s16 *)&mBombs[j].angle =
                          (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
                  }
                  mBombs[j].y = z3;
                  HatchPanel_Activate(z3);
              } else {
                  if (step != 0) {
                      *(s16 *)&mBombs[j].angle = (step * (i >> 1)) + 0xa800;
                  } else {
                      rnd = (u32)RandomIntInternal(&data_0209d4b8);
                      *(s16 *)&mBombs[j].angle =
                          (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0xa000;
                  }
                  mBombs[j].y = vB8000;
                  HatchPanel_Activate(one);
              }
              mSpawnCount++;
              break;
          }
          j++;
          if (j >= 0x70) {
              break;
          }
      }

      ang += step;
      i++;
    }
    while (i < cnt);
  }
  mSpawnDelay = sBomroomSpawnDelays[sb];
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c17HideExplodedBombsEv

void dScMgBomroom_c::HideExplodedBombs() {
    int i;
    for (i = 0; i < 0x70; i++) {
        if (mBombs[i].active != 0) {
            if (mBombs[i].state == 6)
                mBombs[i].visible = 0;
        }
    }
}

// @symbol _ZN14dScMgBomroom_c10ClearBombsEv
/* Zeroes all 0x70 mBombs records: 11 words, 4
 * halfwords and 10 bytes each. */

typedef struct {
    u32 w0;      /* +0x00 */
    u32 w1;
    u32 w2;
    u32 w3;
    u32 w4;
    u32 w5;
    u32 w6;
    u32 w7;
    u32 w8;
    u32 w9;
    u32 w10;     /* +0x28 */
    u16 h0;      /* +0x2c */
    u16 h1;
    u16 h2;
    u16 h3;      /* +0x32 */
    u8 b0;       /* +0x34 */
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;       /* +0x38 */
    u8 b5;       /* +0x39 */
    u8 b6;       /* +0x3a */
    u8 b7;       /* +0x3b */
    u8 b8;       /* +0x3c */
    u8 b9;       /* +0x3d */
    char _pad[2];
} Entry_893c; /* 0x40 */

void dScMgBomroom_c::ClearBombs() {
    int i;
    for (i = 0; i < 0x70; i++) {
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

// @symbol _ZN14dScMgBomroom_c15RoundEnd_FinishEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::RoundEnd_Finish() {
    int j;

    UpdateBombs();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
        return;
    }

    for (j = 0; j < 0x70; j++) {
        if (mBombs[j].active == 2) {
            if (mBombs[j].state == 6
             && mBombs[j].substate == 4) {
                mBombs[j].active = 0;
                mBombs[j].visible = 0;
            }
        }
    }

    if (mBlastPen != 0) {
        mStateTimer = 0x60;
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

#pragma pop

// @symbol _ZN14dScMgBomroom_c14RoundEnd_TallyEv

void dScMgBomroom_c::RoundEnd_Tally() {
    char * self = (char *)this;
    int found;
    int j;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;

    UpdateBombs();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer > 0)
            return;
        mStateTimer = 0;
        mTallyDelay = 0x10;
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
    } while (j < 0x70);

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
        mStateTimer = 0x40;
        mSubState = 3;
    }
}

// @symbol _ZN14dScMgBomroom_c15AnyBombMarchingEv

int dScMgBomroom_c::AnyBombMarching() {
    char * c = (char *)this;
    int i;
    unsigned char (*arr)[0x40];
    i = 0;
    arr = (unsigned char (*)[0x40])c;
    do {
        if (mBombs[i].active == 1 && mBombs[i].state == 6)
            return 1;
        i++;
    } while (i < 0x70);
    return 0;
}

// @symbol _ZN14dScMgBomroom_c20RoundEnd_WaitLinedUpEv
#pragma push
#pragma opt_strength_reduction off

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
    } while (i < 0x70);
    if (c2 == 0 && mBg2State == 0) {
        CheckColorComplete();
    }
    if (c4 != 0) return;
    TopDoor_Close();
    mStateTimer = 0x10;
    mSubState = 2;
    mShowTally = 1;
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19RoundEnd_StartMarchEv

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
        } while (i < 0x70);
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
        } while (i < 0x70);
        Sound::PlayBank2_2D(0x1e6);
        PenGate_Open(mWinColor);
    }
    mSubState = 1;
}

// @symbol _ZN14dScMgBomroom_c19Round_StateGameOverEv

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

// @symbol _ZN14dScMgBomroom_c14Round_StateEndEv
/* Runs the mSubState sub-state through the pointer-to-member table
 * sBomroomRoundEndTable, decoded by hand (a function word, then a
 * this-adjust whose low bit marks a virtual call), then the three
 * per-frame passes. */

void dScMgBomroom_c::Round_StateEnd() {
    unsigned char * c = (unsigned char *)this;
    int idx = mSubState;
    int *e = &sBomroomRoundEndTable[idx * 2];
    int off = e[1];
    void *obj = c + (off >> 1);
    void (*f)(void *);
    if (off & 1) f = (void (*)(void *))(*(int *)(*(int *)obj + e[0]));
    else f = (void (*)(void *))e[0];
    f(obj);
    HatchPanel_UpdateAll();
    PenGate_UpdateAll();
    TopDoor_Update();
}

// @symbol _ZN14dScMgBomroom_c15Round_StatePlayEv

void dScMgBomroom_c::Round_StatePlay() {
    void * c = (void *)this;
    HatchPanel_UpdateAll();
    SpawnBombs();
    UpdateBombs();
    CountSortedBombs();
}

// @symbol _ZN14dScMgBomroom_c16Round_StateStartEv

void dScMgBomroom_c::Round_StateStart() {
    if (mPromptBlinkCount == 0) { mPromptEnabled = 1; mPromptBlinkCount = 1; mPromptBlinkTimer = 0; }
    mSceneState = 2;
}

// @symbol _ZN14dScMgBomroom_c16Round_StateSetupEv

void dScMgBomroom_c::Round_StateSetup() {
    HatchPanel_ShowAll();
    PenGate_ShowAll();
    TopDoor_Show();
    mSceneState = 1;
}

// @symbol _ZN14dScMgBomroom_c10ResetRoundEv
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
// @symbol _ZN14dScMgBomroom_c10StartRoundEi
void dScMgBomroom_c::StartRound(int /* arg */) {
    ResetRound();
    mTallyCount = 0;
    mSceneState = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 1, 0x1c, 4, 3);
    SetBg0Offset(0, 0);
}

// @symbol _ZN14dScMgBomroom_c6RenderEv
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

// @symbol _ZN14dScMgBomroom_c8BehaviorEv
/* Waits out mDispatchDelay, then runs the round state mSceneState from
 * the pointer-to-member table sBomroomRoundStateTable and steps the sprite
 * records and the BG2 offset. */
typedef void (dScMgBomroom_c::*PMFv)();
extern "C" PMFv sBomroomRoundStateTable[];
s32 dScMgBomroom_c::Behavior()
{
    if (mDispatchDelay != 0) {
        unsigned short *t = &mDispatchDelay;
        *t = *t - 1;
        if ((s16)mDispatchDelay <= 0)
            mDispatchDelay = 0;
    } else {
        (this->*sBomroomRoundStateTable[mSceneState])();
        Marker_UpdateAll();
        UpdateBg2Lift();
    }
    return 1;
}

// @symbol _ZN14dScMgBomroom_c13InitResourcesEv
/* Loads the main- and sub-screen backgrounds, palettes and object
 * graphics, sets the blend, then resets the round into state 1. */
s32 dScMgBomroom_c::InitResources() {
    char *b;
    volatile u16 fill;
    int f;
    int arc;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 2;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x23);
    DecompressLZ16((void *)f, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)f);

    f = LoadFile(0x24);
    GX::LoadBGPltt((const void *)f, 0x180, 0x80);
    Deallocate((void *)f);

    f = LoadFile(0x25);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9410;

    f = LoadFile(3);
    b = (char *)G2::GetBG2ScrPtr();
    fill = 0xf23f;
    MultiStore16(fill, b, 0x1000);
    func_020563d4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    data_0209d45c |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3);
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x5610;

    f = LoadFile(2);
    func_02056554((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    f = LoadFile(7);
    func_02056554((const void *)f, 0x800, 0x800);
    Deallocate((void *)f);

    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 1, 0x1c, 4, 3);

    arc = LoadFile(0xb5);
    f = LoadFile(0xb6);
    DecompressLZ16((void *)arc, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)f, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x210;

    {
        int f7 = LoadFile(0x23);
        DecompressLZ16((void *)f7, (void *)((char *)G2S::GetBG3CharPtr() + 0x4000));
        Deallocate((void *)f7);

        f7 = LoadFile(0x24);
        GXS::LoadBGPltt((const void *)f7, 0x180, 0x80);
        Deallocate((void *)f7);

        f7 = LoadFile(0x22);
        func_020562b4((const void *)f7, 0, 0x800);
        Deallocate((void *)f7);

        DecompressLZ16((void *)arc, (void *)0x6600000);
        GXS::LoadOBJPltt((const void *)f, 0, 0x100);
        Deallocate((void *)arc);
        Deallocate((void *)f);
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
// @symbol dScMgBomroom_c_classInit
extern "C" void *dScMgBomroom_c_classInit()
{
    return new dScMgBomroom_c;
}
