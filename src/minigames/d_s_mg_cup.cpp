//cpp
/* Shell game (MG_CUP). Three cups shuffle on the sub screen; touch the cup
 * containing the requested item.
 */

// leftovers:
// StateShuffle's fake-out angle bump keeps the full assignment; += changes its address calculation.
// mFx and mCup are still built and torn down through spelled __cxa_vec_ctor /
// __cxa_vec_cleanup calls: CupFx and CupPos have no constructor or destructor
// of their own yet (Cup_Nothing, CupFx_ElemDtor, func_0203d738,
// NullDestructor_0203d47c).

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgCup_c_AssetId {
    ASSET_D_2D_MG_BG_CUP2_1_NSC_BIN = 0x0029,
    ASSET_D_2D_MG_BG_CUP2_2_NSC_BIN = 0x002a,
    ASSET_D_2D_MG_BG_CUP_NCG_BIN = 0x002b,
    ASSET_D_2D_MG_BG_CUP_NCL_BIN = 0x002c,
    ASSET_D_2D_MG_CUP_NCG_BIN = 0x00c5,
    ASSET_D_2D_MG_CUP_NCL_BIN = 0x00c6
};

#include "dScMgCup_c.h"
#include "nitro/hw/registers.h"
#include "types.h"
#include "decl_common.h"
#include "OamAttr.h"
#include "nitro/gx/gxcommon.h"
#include "SpawnInfo.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Sound.h"
#include "nitro/fx/fx_cp.h"

/* sCupRoundTable: swap count, then two speed-row nibbles.
   StateSetup reads .swaps; SetupSwap reads .lo / .hi. */
typedef struct {
    u8 swaps;
    u8 lo : 4;
    u8 hi : 4;
} RoundRow;

/* The three rest positions, in Q12 screen coordinates. */
struct RestPos {
    s32 x, y;
};

/* The three possible pairs of slots to swap. */
struct IdxPair {
    s32 a, b;
};

/* The eight-entry Cup PMF table at 0x02141870, defined at the end of this
 * file (folded sinit 0x021303d0). */
typedef void (dScMgCup_c::*PMF)();

/* One step of sCupAnimTable. list is an OamAttr run ending at attr3
   0xffff (DrawOamList). ticks == 0 means Behavior does not advance. */
struct AnimStep {
    OamAttr *list;
    int ticks;
};

struct OamAttr;
#include "OAM.h"
#include "LevelRng.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
#include "math/CalcLib.h"

extern "C" {
int CupFx_FindFree(CupFx *fx, int kind, int x, int y);
struct OamAttr;
extern int sCupFxScales[];
extern unsigned char sCupFxFrameSprites[];
extern OamAttr *sCupFxSprites[];
extern int OamRender8_d_s_mg_cup(int show, struct OamAttr *attr, int a, int b, int c, int d, int e,
                                 int f);
extern int Mg_RenderSubSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5,
                                    unsigned short a6);
extern RoundRow sCupRoundTable[];
extern s16 sCupShuffleSpeeds[];
extern IdxPair sCupSwapFirst[];
extern RestPos sCupHomePositions[];
extern void Mg_SetSharedState(int c);
extern void Mg_SubCredits(void *c, int val);
extern void Mg_SubmitScore(int self);
extern void FreeGfxSlotsById(int arg);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
void CupFx_MoveForCup(CupFx *fx, int cup, int dx, int dy);
int CupFx_Spawn(CupFx *fx, int kind, int x, int y, signed char cup);
struct CupShuffleOffset {
    s8 x, y;
};
extern CupShuffleOffset sCupShuffleOffsets[];
void CupFx_DrawForCup(CupFx *fx, int cup);
void CupFx_UpdateAll(CupFx *fx);
void CupFx_ClearAll(CupFx *fx);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void Mg_DrawStockCounter(int arg0, int arg1);
void Mg_DrawScoreTally(void *a, int b, int c, int d);
extern char data_ov006_02139df4[];
extern PMF sCupStateTable[];
extern AnimStep *sCupAnimTable[];
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern int data_0208ee44;
extern unsigned int LoadCompressedFileAt(int, void *);
extern void *LoadFile(int);
void Cup_Nothing(void);
void func_0203d738(void);
}

#pragma defer_codegen off
/* Emit each function as it is read, so D1 lands ahead of D0. */

/* Reverse of the factory: cups, effects, then the model, which the
   compiler destroys as a member. The tracker and the base chain are the
   inlined dScMgSingle3DBase_c destructor. */
dScMgCup_c::~dScMgCup_c() {
    __cxa_vec_cleanup(mCup, 3, 8, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mFx, 0x20, 0x18, (dtor_t)CupFx_ElemDtor);
}

extern "C" void CupFx_ElemDtor(void) {}

extern "C" int CupFx_FindFree(CupFx *fx, int, int, int) {
    int slot;
    for (slot = 0; slot < 0x20; slot++) {
        if (fx[slot].active == 0)
            break;
    }
    return slot;
}

extern "C" void CupFx_MoveForCup(CupFx *fx, int cup, int dx, int dy) {
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx[i].active != 0 && cup == fx[i].cup)
            fx[i].Move(dx, dy);
    }
}

extern "C" int CupFx_Spawn(CupFx *fx, int kind, int x, int y, signed char cup) {
    int slot = CupFx_FindFree(fx, kind, x, y);
    if (slot >= 0 && slot < 0x20) {
        fx[slot].Init(kind, x, y, cup);
        return slot;
    }
    return -1;
}

extern "C" void CupFx_DrawForCup(CupFx *fx, int cup) {
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx[i].active != 0 && cup == fx[i].cup)
            fx[i].Draw();
    }
}

extern "C" void CupFx_UpdateAll(CupFx *fx) {
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx[i].active)
            fx[i].Update();
    }
}

extern "C" void CupFx_ClearAll(CupFx *fx) {
    int i;
    for (i = 0; i < 0x20; i++)
        fx[i].active = 0;
}

void CupFx::Move(int dx, int dy) {
    if (kind != 1)
        return;
    x += dx;
    y += dy;
}

void CupFx::Draw() {
    int frameIndex = frame + row * 6;
    OAM::Render(1, sCupFxSprites[sCupFxFrameSprites[frameIndex]], x >> 12, y >> 12, -1, -1,
                sCupFxScales[frameIndex], 0);
}

void CupFx::Update() {
    x += vx;
    y += vy;
    if (row != 0)
        y -= 6;
    if (DecIfAbove0_Byte(&delay))
        return;
    delay = 3;
    frame -= 1;
    if (frame < 0)
        active = 0;
}

void CupFx::Init(int kind, int x, int y, s8 cup) {
    if (kind == 2) {
        this->kind = 0;
        row = 1;
    } else {
        this->kind = (unsigned char)kind;
        row = 0;
    }
    this->cup = cup;
    active = 1;
    this->x = x;
    this->y = y;
    vx = 0;
    vy = 0;
    if (row != 0)
        frame = 7;
    else
        frame = 5;
    delay = 3;
}

void dScMgCup_c::PlayAnimSound(int anim, int frame, int cup) {
    if (anim == 6) {
        if (frame == 4) {
            func_02012718(0x1cf, mCup[cup].x);
        } else if (frame == 5 || frame == 0xb) {
            func_02012718(0x1d0, mCup[cup].x);
        }
    }
    if (anim == 5 && frame == 6 && cup == mRevealCup) {
        func_02012718(0x1ce, mCup[cup].x);
    }
    if (anim != 1 && anim != 4)
        return;
    if (frame != 4)
        return;
    Sound::PlayBank2_2D(0x1cc);
}

void dScMgCup_c::DrawOamList(OamAttr *list, int x, int y, int scale, int cupContent) {
    GXOamAttr *entry;
    int content;
    int screenX;
    int screenY;

    entry = (GXOamAttr *)list;
    screenX = x >> 12;
    screenY = y >> 12;
    content = cupContent;
    scale = FX_Inv(scale);

    for (;;) {
        GXOamAttr *oam =
            (GXOamAttr *)Mg_RenderSubSpriteScaled((int)entry, screenX, screenY, -1, -1, scale, 0);
        if (oam != 0) {
            if (content == 2) {
                if (entry->cParam == 3) {
                    oam->cParam = 4;
                }
            }
        }
        if (entry->attr3 == 0xffff)
            break;
        entry++;
    }
}

void dScMgCup_c::UpdateAnim(int cupIndex) {
    if (mRevealRequested[cupIndex] == 0) {
        if (mCupContents[cupIndex] != 0)
            mAnim[cupIndex] = 4;
        else
            mAnim[cupIndex] = 1;
    } else if (mCupContents[cupIndex] != 0) {
        mAnim[cupIndex] = 5;
        if (mRevealCup == 0xff && mCorrect != 1)
            mRevealCup = cupIndex;
    } else {
        mAnim[cupIndex] = 2;
    }
    mFrame[cupIndex] = 0;
    mTick[cupIndex] = 0;
}

void dScMgCup_c::SetupSwap() {
    u32 rnd = (u32)RandomIntInternal(&data_0209e650.value);
    u32 bits = rnd >> 0x10;
    int speedIndex;
    int swapPair;

    if (bits & 1) {
        speedIndex = sCupRoundTable[mRoundRow].hi;
    } else {
        speedIndex = sCupRoundTable[mRoundRow].lo;
    }

    speedIndex = speedIndex * 2;
    if (bits & 2)
        speedIndex += 1;

    mShuffleSpeed = sCupShuffleSpeeds[speedIndex];

    swapPair = (rnd >> 0x18) % 3;
    mSwapSlotA = sCupSwapFirst[swapPair].a;
    mSwapSlotB = sCupSwapFirst[swapPair].b;

    mMidX = (sCupHomePositions[mSwapSlotB].x + sCupHomePositions[mSwapSlotA].x) / 2;
    mMidY = (sCupHomePositions[mSwapSlotB].y + sCupHomePositions[mSwapSlotA].y) / 2;
    mHalfX = mMidX - sCupHomePositions[mSwapSlotA].x;
    mShuffleAngle = 0;

    {
        u8 roundRow = mRoundRow;
        if (roundRow == 7) {
            if (mSwapsRemaining == mFakeOutAt) {
                goto set1;
            }
        }
        if (roundRow < 7) {
            goto set0;
        }
        {
            u32 fakeOutRoll = (u32)RandomIntInternal(&data_0209e650.value);
            if (((fakeOutRoll >> 0x10) & 7) != 0) {
                goto set0;
            }
        }
    set1:
        mFakeOut = 1;
        return;
    set0:
        mFakeOut = 0;
        return;
    }
}

void dScMgCup_c::StateIdle() {}

void dScMgCup_c::StateFinish() {
    mTimer -= 1;
    if (mTimer > 0)
        return;
    if (mCorrect != 0) {
        if (mHudScore < 0x270f)
            mHudScore += 1;
        if (mHudScore > mBestHudScore)
            mBestHudScore = mHudScore;
        Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
        mState = 7;
    } else {
        if (mCredits > 0) {
            StartRound(-1);
        } else {
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            mState = 7;
        }
    }
}

void dScMgCup_c::StateResult() {
    int score;
    int i;

    mTimer = mTimer - 1;
    if (mTimer > 0)
        return;
    for (i = 0; i < 3; i++) {
        if (mRevealRequested[i] == 0) {
            mRevealRequested[i] = 1;
            UpdateAnim(i);
        }
    }
    score = mHudScore;
    if (mCorrect != 0) {
        mTimer = 0x3c;
        mModel.PlayCheer();
        score++;
    } else {
        Mg_SubCredits(this, 1);
        if (mCredits > 0) {
            mTimer = 0x96;
            mModel.PlayMiss();
        } else {
            mTimer = 0x1e;
            mModel.PlayLose();
        }
        func_02012790(0x12f);
    }
    Mg_SubmitScore(score);
    mState = 6;
    mPromptEnabled = 0;
}

void dScMgCup_c::StateSelect() {
    int touchPressed = 0;
    unsigned int playerSlot = gActivePlayerSlot;
    int cup;
    unsigned char touchX;
    unsigned char touchY;

    if (gTouchHeld[playerSlot * 4] != 0 && gTouchEdge[playerSlot * 4] != 0)
        touchPressed = 1;
    if (touchPressed == 0)
        return;

    touchX = ((TouchInfo *)gTouchHeld)[playerSlot].x;
    touchY = ((TouchInfo *)gTouchHeld)[playerSlot].y;

    for (cup = 0; cup < 3; cup++) {
        int dx = touchX - (mCup[cup].x >> 12);
        int dy = touchY - (mCup[cup].y >> 12);
        if (dx > 0x10 || dx < -0x28 || dy > 0x18 || dy < -0x20)
            continue;

        mRevealRequested[cup] = 1;
        UpdateAnim(cup);

        if (mTargetContent == mCupContents[cup]) {
            mCorrect = 1;
            mAnim[cup] = 6;
        } else {
            mCorrect = 0;
        }

        mState = 5;
        mTimer = 0x3c;
        FreeGfxSlotsById(0x1d);

        {
            unsigned int currentPlayerSlot = gActivePlayerSlot;
            unsigned char lockedTouchY = *(volatile unsigned char *)&gTouchY[currentPlayerSlot * 4];
            unsigned char lockedTouchX = *(volatile unsigned char *)&gTouchX[currentPlayerSlot * 4];
            mModel.mTouchLocked = 1;
            mModel.mTouchX = lockedTouchX;
            mModel.mTouchY = lockedTouchY;
        }
        Sound::PlayBank2_2D(0x1cd);
        return;
    }
}

void dScMgCup_c::StateWaitForInput() {
    mTimer -= 1;
    if (mTimer > 0)
        return;
    Mg_ShowBanner(0xf, 0x80, 0x38, 0, -1, 0xd);
    mState = 4;
}

void dScMgCup_c::StateShuffle() {
    int oldX, oldY;

    {
        int absoluteSpeed = mShuffleSpeed;
        int volume;
        if (absoluteSpeed < 0)
            absoluteSpeed = -absoluteSpeed;
        volume = (absoluteSpeed * 0x1f4) / 0x1000;
        if (volume >= 0x1f4)
            volume = 0x1f4;
        mShuffleSound = func_02012468(mShuffleSound, 2, 0x1cb, 2, 0, volume, 0, 0);
    }

    mShuffleAngle += mShuffleSpeed;

    {
        s16 speed = mShuffleSpeed;
        if ((speed >= 0 && mShuffleAngle >= 0x8000u) || (speed < 0 && mShuffleAngle <= 0x8000u)) {
            mShuffleAngle = 0x8000;
        }
    }

    {
        int slot = mCupAtSlot[mSwapSlotA];
        int cosine = FX_CosIdx(mShuffleAngle >> 4);
        oldX = mCup[slot].x;
        oldY = mCup[slot].y;
        mCup[slot].x = mMidX - FX_MUL(cosine, mHalfX);
    }
    {
        int slot = mCupAtSlot[mSwapSlotA];
        mCup[slot].y = mMidY - FX_MUL(FX_SinIdx(mShuffleAngle >> 4), 0x14000);
    }
    {
        int slot = mCupAtSlot[mSwapSlotA];
        mCupScale[slot] = ((mCup[slot].y - mMidY) >> 7) + 0x1000;
    }
    {
        int slot = mCupAtSlot[mSwapSlotA];
        CupFx_MoveForCup(mFx, (u8)(s8)slot, mCup[slot].x - oldX, mCup[slot].y - oldY);
    }

    {
        int slot = mCupAtSlot[mSwapSlotB];
        int cosine = FX_CosIdx(mShuffleAngle >> 4);
        oldX = mCup[slot].x;
        oldY = mCup[slot].y;
        mCup[slot].x = mMidX + FX_MUL(cosine, mHalfX);
    }
    {
        int slot = mCupAtSlot[mSwapSlotB];
        mCup[slot].y = mMidY + FX_MUL(FX_SinIdx(mShuffleAngle >> 4), 0x14000);
    }
    {
        int slot = mCupAtSlot[mSwapSlotB];
        mCupScale[slot] = ((mCup[slot].y - mMidY) >> 7) + 0x1000;
    }
    {
        int slot = mCupAtSlot[mSwapSlotB];
        CupFx_MoveForCup(mFx, (u8)(s8)slot, mCup[slot].x - oldX, mCup[slot].y - oldY);
    }

    {
        u16 angle = mShuffleAngle;
        if (angle == 0x8000) {
            int cupB = mCupAtSlot[mSwapSlotB];
            int cupA = mCupAtSlot[mSwapSlotA];
            mCupAtSlot[mSwapSlotA] = cupB;
            mCupAtSlot[mSwapSlotB] = cupA;
            mSwapsRemaining -= 1;
            if (mSwapsRemaining == 0) {
                mTimer = 0x1e;
                mState = 3;
            } else {
                u32 delayRoll = RandomIntInternal(&data_0209e650.value);
                mTimer = (delayRoll >> 8) % 0x18 + 1;
                mState = 1;
            }
        } else if (mFakeOut != 0) {
            s16 speed = mShuffleSpeed;
            if ((speed >= 0 && angle >= 0x5555u) || (speed < 0 && angle <= 0xaaabu)) {
                /* Keep the angle bump as a full assignment: += changes
                   the address calculation in this path. */
                speed = -speed;
                mShuffleSpeed = speed;
                mShuffleAngle = mShuffleAngle + 0x8000;
                {
                    int cupB = mCupAtSlot[mSwapSlotB];
                    int cupA = mCupAtSlot[mSwapSlotA];
                    mCupAtSlot[mSwapSlotA] = cupB;
                    mCupAtSlot[mSwapSlotB] = cupA;
                }
                mFakeOut = 0;
            }
        }
    }

    if (mSparkleShuffles != 0) {
        int effectTick = mTimer;
        if ((effectTick & 3) == 0) {
            int effectOffsetIndex;
            {
                int slot = mCupAtSlot[mSwapSlotA];
                effectOffsetIndex = (effectTick >> 2) & 7;
                CupFx_Spawn(mFx, 2, mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex].x << 12),
                            mCup[slot].y + (sCupShuffleOffsets[effectOffsetIndex].y << 12), slot);
            }
            {
                int slot = mCupAtSlot[mSwapSlotB];
                CupFx_Spawn(mFx, 2, mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex].x << 12),
                            mCup[slot].y + (sCupShuffleOffsets[effectOffsetIndex].y << 12), slot);
            }
        }
    } else {
        int effectTick = mTimer;
        if (effectTick & 1) {
            int effectOffsetIndex;
            {
                int slot = mCupAtSlot[mSwapSlotA];
                effectOffsetIndex = (effectTick >> 1) & 7;
                CupFx_Spawn(mFx, 1, mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex].x << 12),
                            mCup[slot].y + (sCupShuffleOffsets[effectOffsetIndex].y << 12), slot);
            }
            {
                int slot = mCupAtSlot[mSwapSlotB];
                CupFx_Spawn(mFx, 1, mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex].x << 12),
                            mCup[slot].y + (sCupShuffleOffsets[effectOffsetIndex].y << 12), slot);
            }
        }
        if ((mTimer & 3) == 0) {
            {
                int slot = mCupAtSlot[mSwapSlotA];
                CupFx_Spawn(mFx, 0, mCup[slot].x - 0xe000, mCup[slot].y + 0x1c000, slot);
            }
            {
                int slot = mCupAtSlot[mSwapSlotB];
                CupFx_Spawn(mFx, 0, mCup[slot].x - 0xe000, mCup[slot].y + 0x1c000, slot);
            }
        }
    }

    mTimer += 1;
}

void dScMgCup_c::StatePrepareShuffle() {
    mTimer--;
    if (mTimer > 0)
        return;
    SetupSwap();
    if (mSparkleShuffles != 0)
        mSparkleShuffles--;
    mState = 2;
}

void dScMgCup_c::StateSetup() {
    int cupIndex;
    int score;
    unsigned int rnd;
    unsigned int randomUpper;
    unsigned int roundRow;
    unsigned int fakeOutSwap;

    mTimer--;
    if (mTimer > 0) {
        return;
    }

    for (cupIndex = 0; cupIndex < 3; cupIndex++) {
        mRevealRequested[cupIndex] = 0;
        UpdateAnim(cupIndex);
    }

    score = mHudScore;
    if (score < 0xa) {
        mRoundRow = (unsigned char)score;
    } else {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650.value);
        randomUpper = rnd >> 16;
        roundRow = (randomUpper % 5) + 5;
        mRoundRow = (unsigned char)roundRow;
    }

    mSwapsRemaining = sCupRoundTable[mRoundRow].swaps;

    if (score > 3) {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650.value);
        if (rnd & 1) {
            mTargetContent = 1;
        } else {
            mTargetContent = 2;
        }
    } else if (score == 3) {
        mTargetContent = 2;
    }

    mSparkleShuffles = 0;
    mCorrect = 0;
    mState = 1;
    mTimer = 0x1e;
    mRevealCup = 0xff;
    mShuffleSound = 0;
    FreeGfxSlotsById(0x1d);

    rnd = (unsigned int)RandomIntInternal(&data_0209e650.value);
    fakeOutSwap = (rnd % 10) + 1;
    mFakeOutAt = (unsigned char)fakeOutSwap;

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

/* Slot 20: the win animation on mModel. */
void dScMgCup_c::OnNewRecord() {
    mModel.PlayCheer();
}

void dScMgCup_c::StartRound(int msg) {
    int i;

    if (msg == 3 || msg == 0x12) {
        if (msg == 3) {
            mHudScore = 0;
        } else {
            mHudScore = 0;
            Mg_SubmitScore(mHudScore);
        }
        mCredits = 2;
        mCreditsShown = mCredits;
        mCupContents[0] = 1;
        mCupContents[1] = 0;
        if (mHudScore >= 3) {
            mCupContents[2] = 2;
        } else {
            mCupContents[2] = 0;
        }
        mTargetContent = 1;
    } else if (msg == 0) {
        mCredits = 2;
        mCreditsShown = mCredits;
        if (mHudScore == 3) {
            mCupContents[0] = 1;
            mCupContents[1] = 0;
            mCupContents[2] = 2;
        }
    }

    for (i = 0; i < 3; i++) {
        if (mCupContents[i] != 0) {
            mAnim[i] = 3;
        } else {
            mAnim[i] = 0;
        }
        mFrame[i] = 0;
        mTick[i] = 0;
    }

    mTimer = 0x3c;
    mState = 0;
    mModel.mTouchLocked = 0;
    mModel.ResetPose();

    mLevel = mHudScore;
    if (mLevel > 0x270e) {
        mLevel = 0x270e;
    }
    Mg_ShowBanner(0xd, 0x80, 0xa0, 1, -1, 0xd);
}

/* Slot 9: submits cups in descending screen-y order. The animation frame
 * index follows draw order, while position/content follow the cup index. */
s32 dScMgCup_c::Render() {
    int order[3];
    int initialSlot;
    int limit;
    int sortIndex;
    int currentCup;
    int nextCup;
    int drawIndex;
    int cup;

    for (initialSlot = 0; initialSlot < 3; initialSlot++) {
        order[initialSlot] = initialSlot;
    }

    for (limit = 3; limit > 1; limit--) {
        for (sortIndex = 0; sortIndex < limit - 1; sortIndex++) {
            nextCup = order[sortIndex + 1];
            currentCup = order[sortIndex];
            if (mCup[currentCup].y < mCup[nextCup].y) {
                order[sortIndex] = nextCup;
                order[sortIndex + 1] = currentCup;
            }
        }
    }

    for (drawIndex = 0; drawIndex < 3; drawIndex++) {
        cup = order[drawIndex];
        CupFx_DrawForCup(mFx, (char)cup);
        DrawOamList(sCupAnimTable[mAnim[cup]][mFrame[drawIndex]].list, mCup[cup].x, mCup[cup].y,
                    mCupScale[cup], mCupContents[cup]);
    }

    if (mState == 4 || mState == 5) {
        Hud_RenderSprite(data_ov006_02139df4, 0x92, 0x20, (mTargetContent == 2) ? 4 : -1, -1);
    }

    Mg_DrawStockCounter(mCredits, 1);
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
    mModel.Draw();
    return 1;
}

/* Slot 6: runs the current state, then advances each cup's animation
   according to the duration stored in its current frame. */
s32 dScMgCup_c::Behavior() {
    int cupIndex;
    (this->*sCupStateTable[mState])();
    for (cupIndex = 0; cupIndex < 3; cupIndex++) {
        AnimStep *frame = &sCupAnimTable[mAnim[cupIndex]][mFrame[cupIndex]];
        int frameDuration = frame->ticks;
        if (frameDuration != 0) {
            mTick[cupIndex] += 1;
            if (mTick[cupIndex] >= frameDuration) {
                mTick[cupIndex] = 0;
                mFrame[cupIndex] += 1;
                PlayAnimSound(mAnim[cupIndex], mFrame[cupIndex], cupIndex);
            }
        }
    }
    CupFx_UpdateAll(mFx);
    mModel.Update();
    return 1;
}

/* Slot 0: sets up the sub-screen backgrounds, loads the board graphics,
   places the three cups and starts a game through StartRound(3). */
s32 dScMgCup_c::InitResources() {
    void *file;

    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    Camera_InitPresenterView(&mCamera);
    if (mModel.Load() == 0)
        return 0;

    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                      GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_23);
    G2S_BG0Mosaic(false);
    G2S_SetBG0Priority(0);
    SetSubBg0Offset(0, 0);

    gSubScreenLayers |= 1;
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x14000);
    G2S_BG2Mosaic(false);
    G2S_SetBG2Priority(3);
    SetSubBg2Offset(0, 0);

    gSubScreenLayers |= 4;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_CUP_NCG_BIN, (char *)G2S_GetBG2CharPtr() + 0x4000);
    file = LoadFile(ASSET_D_2D_MG_BG_CUP_NCL_BIN);
    GXS_LoadBGPltt(file, 0x60, 0x1a0);
    Deallocate(file);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_CUP2_2_NSC_BIN, G2S_GetBG0ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_CUP2_1_NSC_BIN, G2S_GetBG2ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_CUP_NCG_BIN, G2S_GetOBJCharPtr());
    file = LoadFile(ASSET_D_2D_MG_CUP_NCL_BIN);
    GXS_LoadOBJPltt(file, 0, 0xa0);
    Deallocate(file);

    data_0208ee44 = 1;
    Mg_ResetRecordBannerPos();
    sMgClearBannerX = 0x80;
    sMgClearBannerY = 0x88;
    sMgPlayAgainX = 0x40;
    sMgPlayAgainY = 0xa0;
    sMgQuitX = 0xc0;
    sMgQuitY = 0xa0;

    {
        int i;
        for (i = 0; i < 3; i++) {
            mCup[i].x = sCupHomePositions[i].x;
            mCup[i].y = sCupHomePositions[i].y;
            mCupScale[i] = 0x1000;
            mCupAtSlot[i] = i;
            mRevealRequested[i] = 1;
        }
    }

    StartRound(3);
    CupFx_ClearAll(mFx);
    return 1;
}
/* The constructor is inline, so the factory below holds it: the base
   constructors, mParticle and mModel are the compiler's, then the two
   arrays of element records. */
inline dScMgCup_c::dScMgCup_c() {
    __cxa_vec_ctor(mFx, 0x20, 0x18, (ctor_t)Cup_Nothing, (dtor_t)CupFx_ElemDtor);
    __cxa_vec_ctor(mCup, 3, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);
}

extern "C" void *dScMgCup_c_classInit() {
    return new dScMgCup_c;
}

extern "C" void Cup_Nothing() {}

/* Static-init globals (was the handwritten __sinit_ov006_021303d0 shard).
 * The compiler materializes and copies eight native member-pointer constants;
 * Behavior indexes this table by mState. Definition order is the retail
 * initializer's copy order. */
PMF sCupStateTable[8] = {
    &dScMgCup_c::StateSetup,        &dScMgCup_c::StatePrepareShuffle, &dScMgCup_c::StateShuffle,
    &dScMgCup_c::StateWaitForInput, &dScMgCup_c::StateSelect,         &dScMgCup_c::StateResult,
    &dScMgCup_c::StateFinish,       &dScMgCup_c::StateIdle,
};

extern "C" {
extern AnimStep data_ov006_02139dbc[];
extern AnimStep data_ov006_02139e9c[];
extern AnimStep data_ov006_02139ec4[];
extern AnimStep data_ov006_02139db4[];
extern AnimStep data_ov006_02139e4c[];
extern AnimStep data_ov006_0213a274[];
extern AnimStep data_ov006_0213a1ec[];
}

RoundRow sCupRoundTable[10] = {
    {8, 1, 0},  {10, 4, 3}, {12, 7, 6}, {8, 1, 0},  {10, 2, 1},
    {12, 4, 3}, {14, 6, 5}, {10, 2, 1}, {12, 4, 3}, {16, 7, 6},
};

RestPos sCupHomePositions[3] = {
    {60 << 12, 112 << 12},
    {140 << 12, 112 << 12},
    {220 << 12, 112 << 12},
};

IdxPair sCupSwapFirst[3] = {{0, 1}, {1, 2}, {0, 2}};

AnimStep *sCupAnimTable[7] = {
    data_ov006_02139dbc, data_ov006_02139e9c, data_ov006_02139ec4, data_ov006_02139db4,
    data_ov006_02139e4c, data_ov006_0213a274, data_ov006_0213a1ec,
};

/* Single-sprite effect templates in sCupFxSprites index order. */
OamAttr sCupFxTemplate0 = {0x00f8, 0x41f8, 0x15d2, 0xffff};
OamAttr sCupFxTemplate1 = {0x00f8, 0x41f8, 0x15d4, 0xffff};
OamAttr sCupFxTemplate2 = {0x00fc, 0x01fc, 0x15f6, 0xffff};
OamAttr sCupFxTemplate3 = {0x00fc, 0x01fc, 0x15f7, 0xffff};
OamAttr *sCupFxSprites[4] = {&sCupFxTemplate0, &sCupFxTemplate1, &sCupFxTemplate2,
                             &sCupFxTemplate3};
/* Retail places this byte table on a four-byte boundary. */
u8 sCupFxFrameSprites[14] = {3, 2, 1, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 2};
/* Eight signed pixel offsets used by the shuffle sparkle effects. */
CupShuffleOffset sCupShuffleOffsets[8] = {{-40, 16}, {0, 24},   {15, 9},  {-30, 18},
                                          {5, 19},   {-20, 20}, {10, 14}, {-10, 22}};
s16 sCupShuffleSpeeds[16] = {512,  -512,  1024, -1024, 1536, -1536, 2048, -2048,
                             2560, -2560, 3072, -3072, 3584, -3584, 4096, -4096};
int sCupFxScales[14] = {6144, 4096, 4096, 3584, 4096, 4096, 4096,
                        2048, 2048, 2560, 3072, 3584, 4096, 4096};
extern "C" {
BaseSpawnInfo<void> g_profile_MG_CUP = {dScMgCup_c_classInit, 361, 361};
}
