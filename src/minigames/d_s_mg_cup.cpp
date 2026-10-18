//cpp
/* Shell game (MG_CUP). Three cups shuffle on the sub screen; touch the cup
 * containing the requested item.
 */

// leftovers:
// CupFx_Draw calls the mangled Fix12 OAM::Render. include/OAM.h will not declare that overload.
// CupFx_Update keeps two spellings of frame. One shared spelling reuses the address and the body shrinks.
// Cup_UpdateAnim keeps the 0x5000 bases. mRevealRequested[i] / mAnim[i] come out 0xa0 bytes instead of 0xa4.
// StatePrepareShuffle retains a byte-pointer decrement of mSparkleShuffles.
// StateShuffle's fake-out angle bump keeps the full assignment; += changes its address calculation.
// Render's cup x/y arguments retain two different spellings.
// the factory is still _ZN11dScMgBase_cC2Ev plus SysTracker C1. There is
// no dScMgCup_c constructor.

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
#include "types.h"
#include "decl_common.h"
#include "OamAttr.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"

/* sCupRoundTable: swap count, then two speed-row nibbles.
   StateSetup reads byte [kind * 2]. df024 reads .lo / .hi. */
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
   0xffff (Cup_DrawOamList). ticks == 0 means Behavior does not advance. */
struct AnimStep {
    OamAttr *list;
    int ticks;
};

namespace Sound {
void PlayBank2_2D(unsigned int id);
}

namespace G2S {
int GetBG2CharPtr();
int GetBG0ScrPtr();
int GetBG2ScrPtr();
}

namespace GXS {
void LoadBGPltt(const void *data, unsigned int offset, unsigned int size);
void LoadOBJPltt(const void *data, unsigned int offset, unsigned int size);
}

extern "C" {
extern void CupFx_Move(char *p, int a, int b);
extern int CupFx_FindFree(char *p, int a, int b, int c);
extern void CupFx_Init(char *p, int a, int b, int c, signed char e);
extern void CupFx_Draw(char *a);
extern void CupFx_Update(int p);
struct OamAttr;
extern int sCupFxScales[];
extern unsigned char sCupFxFrameSprites[];
extern int sCupFxSprites[];
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int show, struct OamAttr *attr, int a, int b,
                                                   int c, int d, int e, int f);
extern unsigned char DecIfAbove0_Byte(unsigned char *counter);
extern int func_02053200(int x);
extern int Mg_RenderSubSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5,
                                    unsigned short a6);
extern int data_0209e650;
extern RoundRow sCupRoundTable[];
extern s16 sCupShuffleSpeeds[];
extern IdxPair sCupSwapFirst[];
extern RestPos sCupHomePositions[];
extern int RandomIntInternal(int *seed);
extern void Mg_SetSharedState(int c);
extern void Cup_UpdateAnim(char *self, int i);
extern void Presenter_PlayCheer(void *c);
extern void Mg_SubCredits(void *c, int val);
extern void Presenter_PlayMiss(char *c);
extern void Presenter_PlayLose(char *c);
extern void func_02012790(int x);
extern void Mg_SubmitScore(int self);
extern void FreeGfxSlotsById(int arg);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void CupFx_MoveForCup(char *p, int key, int a, int b);
extern int CupFx_Spawn(char *c, int a, int b, int d, signed char e);
extern short data_02082214[];
extern signed char sCupShuffleOffsets[];
extern signed char sCupShuffleOffsetsY[];
extern void Cup_PickShuffleSpeed(char *o);
extern void Presenter_ResetPose(char *c);
/* Render spells cup y as this view. mCup[cup].y changes that call. */
struct P8 {
    int a;
    int b;
};
void CupFx_DrawForCup(char *a, int b);
void Cup_DrawOamList(int a0, void *a1, int a2, int a3, int a4, int a5);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void Mg_DrawStockCounter(int arg0, int arg1);
void Mg_DrawScoreTally(void *a, int b, int c, int d);
extern char data_ov006_02139df4[];
extern PMF sCupStateTable[];
/* The ROM PMF constants the table copies from (unlicensed .data). */
extern PMF pmf_dScMgCup_c_StateSetup;
extern PMF pmf_dScMgCup_c_StatePrepareShuffle;
extern PMF pmf_dScMgCup_c_StateShuffle;
extern PMF pmf_dScMgCup_c_StateWaitForInput;
extern PMF pmf_dScMgCup_c_StateSelect;
extern PMF pmf_dScMgCup_c_StateResult;
extern PMF pmf_dScMgCup_c_StateFinish;
extern PMF pmf_dScMgCup_c_StateIdle;
extern AnimStep *sCupAnimTable[];
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;
extern void Camera_InitPresenterView(char *);
extern int Presenter_Load(char *);
extern void LoadCompressedFileAt(int, int);
extern void *LoadFile(int);
void _ZN8Particle10SysTrackerC1Ev(void *tracker);
void Presenter_Construct(void *table);
void __cxa_vec_ctor(void *base, int count, int stride, void *ctor, void *dtor);
void Cup_Nothing(void);
void func_0203d738(void);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *_ZTV10dScMgCup_c[];
}

#pragma defer_codegen off
/* Emit each function as it is read, so D1 lands ahead of D0. */

// @symbol _ZN10dScMgCup_cD1Ev
/* Reverse of the factory: cups, effects, then the model. The tracker and
   the base chain are the inlined dScMgSingle3DBase_c destructor. */
dScMgCup_c::~dScMgCup_c() {
    __cxa_vec_cleanup(mCup, 3, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mFx, 0x20, 0x18, (void *)CupFx_ElemDtor);
    Presenter_Destruct((char *)&mModel);
}

extern "C" void CupFx_ElemDtor(void) {}

extern "C" int CupFx_FindFree(char *sprite, int, int, int) {
    CupFx *fx = (CupFx *)sprite;
    int slot = 0;
    do {
        if (fx->active == 0)
            break;
        slot++;
        fx++;
    } while (slot < 0x20);
    return slot;
}

extern "C" void CupFx_MoveForCup(char *sprite, int cup, int dx, int dy) {
    CupFx *fx = (CupFx *)sprite;
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx->active != 0) {
            if (cup == fx->cup) {
                CupFx_Move((char *)fx, dx, dy);
            }
        }
        fx++;
    }
}

extern "C" int CupFx_Spawn(char *sprites, int kind, int x, int y, signed char cup) {
    int slot = CupFx_FindFree(sprites, kind, x, y);
    if (slot >= 0 && slot < 0x20) {
        CupFx_Init((char *)((CupFx *)sprites + slot), kind, x, y, cup);
        return slot;
    }
    return -1;
}

extern "C" void CupFx_DrawForCup(char *sprite, int cup) {
    CupFx *fx = (CupFx *)sprite;
    int i = 0;
    do {
        if (fx->active != 0 && cup == fx->cup)
            CupFx_Draw((char *)fx);
        i++;
        fx++;
    } while (i < 0x20);
}

extern "C" void CupFx_UpdateAll(char *sprites) {
    CupFx *fx = (CupFx *)sprites;
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx->active)
            CupFx_Update((int)fx);
        fx++;
    }
}

extern "C" void CupFx_ClearAll(char *sprite) {
    CupFx *fx = (CupFx *)sprite;
    int i;
    for (i = 0; i < 0x20; i++) {
        fx->active = 0;
        fx++;
    }
}

extern "C" void CupFx_Move(char *sprite, int dx, int dy) {
    CupFx *fx = (CupFx *)sprite;
    if (fx->kind != 1)
        return;
    fx->x += dx;
    fx->y += dy;
}

extern "C" void CupFx_Draw(char *sprite) {
    CupFx *fx = (CupFx *)sprite;
    int frameIndex = fx->frame + fx->row * 6;
    /* Fix12 overload. OAM.h does not declare it: a by-value Fix12 homes
       r0-r3 on the stack. */
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
        1, (struct OamAttr *)sCupFxSprites[sCupFxFrameSprites[frameIndex]], fx->x >> 12,
        fx->y >> 12, -1, -1, sCupFxScales[frameIndex], 0);
}

extern "C" void CupFx_Update(int sprite) {
    CupFx *fx = (CupFx *)sprite;
    fx->x += fx->vx;
    fx->y += fx->vy;
    if (fx->row != 0)
        fx->y -= 6;
    if (DecIfAbove0_Byte(&fx->delay))
        return;
    fx->delay = 3;
    /* Keep the frame reload through the byte view: using fx->frame again
       reuses the decrement address and changes the emitted instructions. */
    fx->frame -= 1;
    if (*((signed char *)sprite + 0x12) < 0)
        fx->active = 0;
}

extern "C" void CupFx_Init(char *sprite, int kind, int x, int y, signed char cup) {
    CupFx *fx = (CupFx *)sprite;
    if (kind == 2) {
        fx->kind = 0;
        fx->row = 1;
    } else {
        fx->kind = (unsigned char)kind;
        fx->row = 0;
    }
    fx->cup = cup;
    fx->active = 1;
    fx->x = x;
    fx->y = y;
    fx->vx = 0;
    fx->vy = 0;
    if (fx->row != 0)
        fx->frame = 7;
    else
        fx->frame = 5;
    fx->delay = 3;
}

extern "C" void Cup_PlayAnimSound(char *raw, int anim, int frame, int cup) {
    dScMgCup_c *self = (dScMgCup_c *)raw;
    if (anim == 6) {
        if (frame == 4) {
            func_02012718(0x1cf, self->mCup[cup].x);
        } else if (frame == 5 || frame == 0xb) {
            func_02012718(0x1d0, self->mCup[cup].x);
        }
    }
    if (anim == 5 && frame == 6 && cup == self->mRevealCup) {
        func_02012718(0x1ce, self->mCup[cup].x);
    }
    if (anim != 1 && anim != 4)
        return;
    if (frame != 4)
        return;
    Sound::PlayBank2_2D(0x1cc);
}

extern "C" void Cup_DrawOamList(int scene, void *list, int x, int y, int scale, int cupContent) {
    OamAttr *entry;
    int content;
    int screenX;
    int screenY;

    entry = (OamAttr *)list;
    screenX = x >> 12;
    screenY = y >> 12;
    content = cupContent;
    scale = func_02053200(scale);

    for (;;) {
        int spriteEntry = Mg_RenderSubSpriteScaled((int)entry, screenX, screenY, -1, -1, scale, 0);
        if (spriteEntry != 0) {
            if (content == 2) {
                /* Loaded as one word; the shifts keep attr2 bits 12..15, the palette. */
                if ((unsigned)(*(int *)((char *)entry + 4) << 0x10) >> 0x1c == 3) {
                    int *attributes = (int *)(spriteEntry + 4);
                    *attributes = (*attributes & ~0xf000) | 0x4000;
                }
            }
        }
        if (entry->attr3 == 0xffff)
            break;
        entry++;
    }
}

extern "C" void Cup_UpdateAnim(char *raw, int cupIndex) {
    unsigned char flag;
    char *row = raw + cupIndex;
    int resetCupIndex;
    row += 0x5000;
    flag = *((unsigned char *)(row + 0x465));
    if (flag == 0) {
        flag = *((unsigned char *)(row + 0x462));
        if (flag != 0) {
            char *out = raw + (cupIndex << 2);
            out += 0x5000;
            *((int *)(out + 0x434)) = 4;
        } else {
            char *out = raw + (cupIndex << 2);
            out += 0x5000;
            *((int *)(out + 0x434)) = 1;
        }
        resetCupIndex = cupIndex;
        goto epilogue;
    }
    flag = *((unsigned char *)(row + 0x462));
    if (flag != 0) {
        char *out = raw + (cupIndex << 2);
        out += 0x5000;
        *((int *)(out + 0x434)) = 5;
        if ((*((unsigned char *)((raw + 0x5000) + 0x46d))) == 0xff) {
            if ((*((unsigned char *)((raw + 0x5000) + 0x469))) != 1) {
                *((unsigned char *)((raw + 0x5000) + 0x46d)) = (unsigned char)cupIndex;
            }
        }
        resetCupIndex = cupIndex;
        goto epilogue;
    }
    {
        char *out = raw + (cupIndex << 2);
        out += 0x5000;
        *((int *)(out + 0x434)) = 2;
    }
    resetCupIndex = cupIndex;

    /* Every path assigns resetCupIndex before the shared epilogue; using cupIndex there, or
       assigning resetCupIndex once at the top, emits a 0xa0-byte body. */
epilogue: {
    char *out = raw + (resetCupIndex << 2);
    out += 0x5000;
    *((int *)(out + 0x440)) = 0;
    *((int *)(out + 0x44c)) = 0;
}
}

extern "C" void Cup_PickShuffleSpeed(char *raw) {
    dScMgCup_c *self = (dScMgCup_c *)raw;
    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
    u32 bits = rnd >> 0x10;
    int speedIndex;
    int swapPair;

    if (bits & 1) {
        speedIndex = sCupRoundTable[self->mRoundRow].hi;
    } else {
        speedIndex = sCupRoundTable[self->mRoundRow].lo;
    }

    speedIndex = speedIndex * 2;
    if (bits & 2)
        speedIndex += 1;

    self->mShuffleSpeed = sCupShuffleSpeeds[speedIndex];

    swapPair = (rnd >> 0x18) % 3;
    self->mSwapSlotA = sCupSwapFirst[swapPair].a;
    self->mSwapSlotB = sCupSwapFirst[swapPair].b;

    self->mMidX =
        (sCupHomePositions[self->mSwapSlotB].x + sCupHomePositions[self->mSwapSlotA].x) / 2;
    self->mMidY =
        (sCupHomePositions[self->mSwapSlotB].y + sCupHomePositions[self->mSwapSlotA].y) / 2;
    self->mHalfX = self->mMidX - sCupHomePositions[self->mSwapSlotA].x;
    self->mShuffleAngle = 0;

    {
        u8 roundRow = self->mRoundRow;
        if (roundRow == 7) {
            if (self->mSwapsRemaining == self->mFakeOutAt) {
                goto set1;
            }
        }
        if (roundRow < 7) {
            goto set0;
        }
        {
            u32 fakeOutRoll = (u32)RandomIntInternal(&data_0209e650);
            if (((fakeOutRoll >> 0x10) & 7) != 0) {
                goto set0;
            }
        }
    set1:
        self->mFakeOut = 1;
        return;
    set0:
        self->mFakeOut = 0;
        return;
    }
}

// @symbol _ZN10dScMgCup_c9StateIdleEv
void dScMgCup_c::StateIdle() {}

// @symbol _ZN10dScMgCup_c11StateFinishEv
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

// @symbol _ZN10dScMgCup_c11StateResultEv
#pragma push
#pragma opt_strength_reduction off
void dScMgCup_c::StateResult() {
    int score;
    int i;

    mTimer = mTimer - 1;
    if (mTimer > 0)
        return;
    for (i = 0; i < 3; i++) {
        if (mRevealRequested[i] == 0) {
            mRevealRequested[i] = 1;
            Cup_UpdateAnim((char *)this, i);
        }
    }
    score = mHudScore;
    if (mCorrect != 0) {
        mTimer = 0x3c;
        Presenter_PlayCheer(&mModel);
        score++;
    } else {
        Mg_SubCredits(this, 1);
        if (mCredits > 0) {
            mTimer = 0x96;
            Presenter_PlayMiss((char *)&mModel);
        } else {
            mTimer = 0x1e;
            Presenter_PlayLose((char *)&mModel);
        }
        func_02012790(0x12f);
    }
    Mg_SubmitScore(score);
    mState = 6;
    mPromptEnabled = 0;
}
#pragma pop

// @symbol _ZN10dScMgCup_c11StateSelectEv
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
void dScMgCup_c::StateSelect() {
    char *raw = (char *)this;
    int touchPressed = 0;
    unsigned int playerSlot = gActivePlayerSlot;
    int cup;
    unsigned char touchX;
    unsigned char touchY;

    if (gTouchHeld[playerSlot * 4] != 0 && gTouchEdge[playerSlot * 4] != 0)
        touchPressed = 1;
    if (touchPressed == 0)
        return;

    {
        unsigned char *touch = &gTouchHeld[playerSlot * 4];
        touchX = touch[2];
        touchY = touch[3];
    }

    for (cup = 0; cup < 3; cup++) {
        int dx = touchX - (mCup[cup].x >> 12);
        int dy = touchY - (mCup[cup].y >> 12);
        if (dx > 0x10 || dx < -0x28 || dy > 0x18 || dy < -0x20)
            continue;

        mRevealRequested[cup] = 1;
        Cup_UpdateAnim(raw, cup);

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
            mTouchLock = 1;
            mTouchX = lockedTouchX;
            mTouchY = lockedTouchY;
        }
        Sound::PlayBank2_2D(0x1cd);
        return;
    }
}
#pragma pop

// @symbol _ZN10dScMgCup_c17StateWaitForInputEv
void dScMgCup_c::StateWaitForInput() {
    mTimer -= 1;
    if (mTimer > 0)
        return;
    Mg_ShowBanner(0xf, 0x80, 0x38, 0, -1, 0xd);
    mState = 4;
}

// @symbol _ZN10dScMgCup_c12StateShuffleEv
#pragma push
#pragma opt_common_subs off
void dScMgCup_c::StateShuffle() {
    char *raw = (char *)this;
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
        int cosine = data_02082214[(mShuffleAngle >> 4) * 2 + 1];
        oldX = mCup[slot].x;
        oldY = mCup[slot].y;
        mCup[slot].x = mMidX - FX_MUL(cosine, mHalfX);
    }
    {
        int slot = mCupAtSlot[mSwapSlotA];
        mCup[slot].y =
            mMidY - (int)((data_02082214[(mShuffleAngle >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = mCupAtSlot[mSwapSlotA];
        mCupScale[slot] = ((mCup[slot].y - mMidY) >> 7) + 0x1000;
    }
    {
        int slot = mCupAtSlot[mSwapSlotA];
        CupFx_MoveForCup((char *)mFx, (u8)(s8)slot, mCup[slot].x - oldX, mCup[slot].y - oldY);
    }

    {
        int slot = mCupAtSlot[mSwapSlotB];
        int cosine = data_02082214[(mShuffleAngle >> 4) * 2 + 1];
        oldX = mCup[slot].x;
        oldY = mCup[slot].y;
        mCup[slot].x = mMidX + FX_MUL(cosine, mHalfX);
    }
    {
        int slot = mCupAtSlot[mSwapSlotB];
        mCup[slot].y =
            mMidY + (int)((data_02082214[(mShuffleAngle >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = mCupAtSlot[mSwapSlotB];
        mCupScale[slot] = ((mCup[slot].y - mMidY) >> 7) + 0x1000;
    }
    {
        int slot = mCupAtSlot[mSwapSlotB];
        CupFx_MoveForCup((char *)mFx, (u8)(s8)slot, mCup[slot].x - oldX, mCup[slot].y - oldY);
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
                u32 delayRoll = RandomIntInternal(&data_0209e650);
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
                CupFx_Spawn((char *)mFx, 2,
                            mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex * 2] << 12),
                            mCup[slot].y + (sCupShuffleOffsetsY[effectOffsetIndex * 2] << 12),
                            slot);
            }
            {
                int slot = mCupAtSlot[mSwapSlotB];
                CupFx_Spawn((char *)mFx, 2,
                            mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex * 2] << 12),
                            mCup[slot].y + (sCupShuffleOffsetsY[effectOffsetIndex * 2] << 12),
                            slot);
            }
        }
    } else {
        int effectTick = mTimer;
        if (effectTick & 1) {
            int effectOffsetIndex;
            {
                int slot = mCupAtSlot[mSwapSlotA];
                effectOffsetIndex = (effectTick >> 1) & 7;
                CupFx_Spawn((char *)mFx, 1,
                            mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex * 2] << 12),
                            mCup[slot].y + (sCupShuffleOffsetsY[effectOffsetIndex * 2] << 12),
                            slot);
            }
            {
                int slot = mCupAtSlot[mSwapSlotB];
                CupFx_Spawn((char *)mFx, 1,
                            mCup[slot].x + (sCupShuffleOffsets[effectOffsetIndex * 2] << 12),
                            mCup[slot].y + (sCupShuffleOffsetsY[effectOffsetIndex * 2] << 12),
                            slot);
            }
        }
        if ((mTimer & 3) == 0) {
            {
                int slot = mCupAtSlot[mSwapSlotA];
                CupFx_Spawn((char *)mFx, 0, mCup[slot].x - 0xe000, mCup[slot].y + 0x1c000, slot);
            }
            {
                int slot = mCupAtSlot[mSwapSlotB];
                CupFx_Spawn((char *)mFx, 0, mCup[slot].x - 0xe000, mCup[slot].y + 0x1c000, slot);
            }
        }
    }

    mTimer += 1;
}
#pragma pop

// @symbol _ZN10dScMgCup_c19StatePrepareShuffleEv
void dScMgCup_c::StatePrepareShuffle() {
    char *raw = (char *)this;
    int *countdown = &mTimer;
    *countdown = *countdown - 1;
    if (mTimer > 0)
        return;
    Cup_PickShuffleSpeed(raw);
    if (mSparkleShuffles != 0) {
        unsigned char *pending = (unsigned char *)(raw + 0x546b);
        *pending = *pending - 1;
    }
    mState = 2;
}

// @symbol _ZN10dScMgCup_c10StateSetupEv
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
        Cup_UpdateAnim((char *)this, cupIndex);
    }

    score = mHudScore;
    if (score < 0xa) {
        mRoundRow = (unsigned char)score;
    } else {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        randomUpper = rnd >> 16;
        roundRow = (randomUpper % 5) + 5;
        mRoundRow = (unsigned char)roundRow;
    }

    mSwapsRemaining = ((unsigned char *)sCupRoundTable)[mRoundRow * 2];

    if (score > 3) {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650);
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

    rnd = (unsigned int)RandomIntInternal(&data_0209e650);
    fakeOutSwap = (rnd % 10) + 1;
    mFakeOutAt = (unsigned char)fakeOutSwap;

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

// @symbol _ZN10dScMgCup_c11OnNewRecordEv
/* Slot 20: the win animation on mModel. */
void dScMgCup_c::OnNewRecord() {
    Presenter_PlayCheer(&mModel);
}

#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
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
    mTouchLock = 0;
    Presenter_ResetPose((char *)&mModel);

    mLevel = mHudScore;
    if (mLevel > 0x270e) {
        mLevel = 0x270e;
    }
    Mg_ShowBanner(0xd, 0x80, 0xa0, 1, -1, 0xd);
}
#pragma pop

// @symbol _ZN10dScMgCup_c6RenderEv
/* Slot 9: submits cups in descending screen-y order. The animation frame
 * index follows draw order, while position/content follow the cup index. */
s32 dScMgCup_c::Render() {
    char *raw = (char *)this;
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
        CupFx_DrawForCup((char *)mFx, (char)cup);
        Cup_DrawOamList((int)raw, (void *)sCupAnimTable[mAnim[cup]][mFrame[drawIndex]].list,
                        mCup[cup].x, ((struct P8 *)raw + cup)[0xa7d].b, mCupScale[cup],
                        mCupContents[cup]);
    }

    if (mState == 4 || mState == 5) {
        Hud_RenderSprite(data_ov006_02139df4, 0x92, 0x20, (mTargetContent == 2) ? 4 : -1, -1);
    }

    Mg_DrawStockCounter(mCredits, 1);
    Mg_DrawScoreTally(raw, 0xe0, 0x14, 1);
    Presenter_Draw((char *)&mModel);
    return 1;
}

// @symbol _ZN10dScMgCup_c8BehaviorEv
/* Slot 6: runs the current state, then advances each cup's animation
   according to the duration stored in its current frame. */
#pragma push
#pragma opt_strength_reduction off
s32 dScMgCup_c::Behavior() {
    char *raw = (char *)this;
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
                Cup_PlayAnimSound(raw, mAnim[cupIndex], mFrame[cupIndex], cupIndex);
            }
        }
    }
    CupFx_UpdateAll((char *)mFx);
    Presenter_Update((char *)&mModel);
    return 1;
}
#pragma pop

// @symbol _ZN10dScMgCup_c13InitResourcesEv
/* Slot 0: sets up the sub-screen backgrounds, loads the board graphics,
   places the three cups and starts a game through StartRound(3). */
s32 dScMgCup_c::InitResources() {
    void *file;

    data_0209d45c = 0x11;
    Camera_InitPresenterView((char *)pad_4660);
    if (Presenter_Load((char *)&mModel) == 0)
        return 0;

    {
        volatile u16 *bg = (volatile u16 *)0x04001008;
        *bg = (*bg & 0x43) | 0x2214;
        *bg = *bg & ~0x40;
        *bg = *bg & ~3;
        SetSubBg0Offset(0, 0);
    }

    data_0209d454 |= 1;
    {
        volatile u16 *bg = (volatile u16 *)0x0400100c;
        *bg = (*bg & 0x43) | 0x414;
        *bg = *bg & ~0x40;
        *bg = (*bg & ~3) | 3;
        SetSubBg2Offset(0, 0);
    }

    data_0209d454 |= 4;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_CUP_NCG_BIN, G2S::GetBG2CharPtr() + 0x4000);
    file = LoadFile(ASSET_D_2D_MG_BG_CUP_NCL_BIN);
    GXS::LoadBGPltt(file, 0x60, 0x1a0);
    Deallocate(file);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_CUP2_2_NSC_BIN, G2S::GetBG0ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_CUP2_1_NSC_BIN, G2S::GetBG2ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_CUP_NCG_BIN, 0x6600000);
    file = LoadFile(ASSET_D_2D_MG_CUP_NCL_BIN);
    GXS::LoadOBJPltt(file, 0, 0xa0);
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
    CupFx_ClearAll((char *)mFx);
    return 1;
}
// @symbol dScMgCup_c_classInit
/* No dScMgCup_c constructor. Base ctor, intermediate vtable, the tracker,
   then this vtable, the model, the effects, the cups. */
extern "C" void *dScMgCup_c_classInit() {
    dScMgCup_c *scene = (dScMgCup_c *)_ZN7fBase_cnwEj(sizeof(dScMgCup_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(&scene->mSysTracker);
        *(void **)scene = _ZTV10dScMgCup_c + 2;
        Presenter_Construct(&scene->mModel);
        __cxa_vec_ctor(scene->mFx, 0x20, 0x18, (void *)Cup_Nothing, (void *)CupFx_ElemDtor);
        __cxa_vec_ctor(scene->mCup, 3, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    }
    return scene;
}

// @symbol Cup_Nothing
extern "C" void Cup_Nothing() {}

/* Static-init globals (was the handwritten __sinit_ov006_021303d0 shard).
 * The eight Cup state records plain-copied from the ROM PMF constants;
 * Behavior indexes this table by mState. Definition order is the retail
 * initializer's copy order. */
PMF sCupStateTable[8] = {
    pmf_dScMgCup_c_StateSetup,   pmf_dScMgCup_c_StatePrepareShuffle,
    pmf_dScMgCup_c_StateShuffle, pmf_dScMgCup_c_StateWaitForInput,
    pmf_dScMgCup_c_StateSelect,  pmf_dScMgCup_c_StateResult,
    pmf_dScMgCup_c_StateFinish,  pmf_dScMgCup_c_StateIdle,
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
