//cpp
/* Memory Match. Twelve cards sit on the touch screen; two equal values
 * are a pair, and too many misses ends the round.
 *
 * leftovers:
 *   - func_ and data_ helpers are still the linker names.
 *   - CheckFinished reads mCredits through WordAt. As a plain member the
 *     multiply-add takes its registers the other way round.
 *   - ShuffleCards: mValueCounts[slot] hoists the base; adding the slot
 *     to 0x5330 matches.
 *   - cstd::atan2 stays the mangled symbol (Fix12<int> by value).
 *   - the factory builds the object by hand because the class has no
 *     constructor declared yet.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_memory_AssetId {
    ASSET_D_2D_MG_CARD_NCL_BIN = 0x00bb
};

#include "dScMgMemory_c.h"
#include "common.h"
#include "types.h"
#include "PlayerInput.h"
#include "decl_common.h"
#include "Sound.h"
#include "nitro/fx/fx.h"

/* State numbers follow the card PMF table initialized by
 * __sinit_ov006_021311c8. These descriptive names are reconstructed. */
enum MemoryCardState {
    CARD_MOVING = 0,
    CARD_IDLE = 1,
    CARD_SELECTABLE = 2,
    CARD_FLIPPING_UP = 3,
    CARD_WAITING = 4,
    CARD_FLIPPING_DOWN = 5,
    CARD_FLYING_AWAY = 6
};

typedef void (dScMgMemory_c::*dScMgMemory_cState)();
typedef void (dScMgMemory_c::*dScMgMemory_cCardState)(int);
extern "C" dScMgMemory_cState sMemoryResultTable[];
extern "C" dScMgMemory_cState sMemoryStateTable[];
extern dScMgMemory_cState sMemoryRoundTable[];
extern dScMgMemory_cCardState sMemoryCardTable[];
/* The player-state table filled by the same initializer; no enrolled
 * consumer names it yet, so it keeps the shard's grouping with the
 * no-arg tables. */
extern "C" dScMgMemory_cState sMemoryPlayerTable[];

/* A walk that advances the scene pointer. The card sits at 0x51a8 so the
 * address stays add #0x5000 plus the field. mCards[i] did not match in
 * DrawCards, RoundWaitDeal, or ShuffleCards. */
struct dMgMemoryCardCur {
    char pad[0x51a8];
    dMgMemoryCard_c card;
};

namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern "C" {
extern void Mg_DrawScoreTally(void* c, int a, int b, int d);
extern void Mg_DrawBlinkSprite(void *c, int arg1, int arg2);
/* cstd::atan2 takes Fix12<int> by value, which mwccarm passes differently
 * from these plain ints, so it keeps its mangled name. */
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short data_02082214[];
void func_02012790(int a);
void MgCoins_StartLoseOneMore(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 sMemoryCardFrameSprites[];
extern void *sMemoryCardSprites[];
extern unsigned short sMemoryFlipFrameTimes[];
extern void Mg_SaveRecordPair(int a, int b);
extern s16 data_02082214[];
extern u16 *sMemoryCardPositionsPerDifficulty[];
extern int MgCoins_StartPayout(int);
extern short gMgCoinsStatus;
extern void Mg_SetPerfectBanner(void);
extern void Mg_SetSharedState(int c);
extern void Mg_SubmitScore(int self);
extern void Dealer_PlayWin(void *c);
extern void Dealer_StartShuffle(char *c, int unused, short count, int ready);
extern int data_0209d4b8;
extern int RandomIntInternal(int *seed);
extern void FreeGfxSlotsById(int x);
void Camera_InitDealerView(char *p);
void Mg_DrawCredits(char *c, int a, int b, int d);
void Dealer_Draw(char *p);
void MgCoins_Update(void);
int Dealer_Update(void *p);
void LoadCard2SubBg3(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int Dealer_Load(void *c);
int Mg_GetSavedRecord0(void);
int Mg_GetSavedRecord1(void);
void MgCoins_Reset(void);
int Dealer_IsIdle(int *p);
void Mg_SetTopScreenYOffset(int v);
void Mg_PlaceBannersOnTopScreen(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
void *_ZN11dScMgBase_cC2Ev(void *scene);
void _ZN8Particle10SysTrackerC1Ev(void *tracker);
/* Only the sibling scene headers (dScMgCard_c.h, dScMgMemory2_c.h) declare
   it, and this TU includes neither. */
void Dealer_Construct(void *sharedState);  /* local extern: see above */
extern void *_ZTV19dScMgSingle3DBase_c[];
}

/* Load-bearing: source order, and the opt pragmas on DrawCards and
 * CardFlyAway. Under deferred emission the last pragma setting wins for the
 * whole file; defer_codegen off lets push/pop scope them to one function. */
#pragma defer_codegen off

// @symbol _ZN13dScMgMemory_cD1Ev
// @symbol _ZN13dScMgMemory_cD0Ev
/* Both D1 and D0 come from this one definition. */
dScMgMemory_c::~dScMgMemory_c()
{
    /* mShared is destroyed automatically before the inherited scene. */
}

void dScMgMemory_c::DrawMessage(){
  if (mMessageVisible == 0) return;
  Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
}

void dScMgMemory_c::DrawCursor() {
    if (mCursor.enabled == 0) return;
    Mg_DrawBlinkSprite(this, 0xe0, 0xa0);
}

void dScMgMemory_c::UpdateCursor()
{
    if (mCursor.visible == 0)
        return;

    {
        unsigned short* e = (unsigned short*)&mCursor.angle;
        *e = *e + 1;
        if (*e < 0x14)
            return;
        *e = 0;
    }
    {
        unsigned char* p = &mCursor.frame;
        *p = *p + 1;
        *p = *p & 1;
    }
}

void dScMgMemory_c::ShowCursor()
{
    mCursor.visible = 1;
    mCursor.enabled = 1;
    mCursor.x = 0x80000;
    mCursor.y = 0xa8000;
    mCursor.angle = 0;
    mCursor.frame = 0;
}

void dScMgMemory_c::HideCursor()
{
    mCursor.visible = 0;
    mCursor.enabled = 0;
}

void dScMgMemory_c::PlayerWait(int /* player */)
{
}

void dScMgMemory_c::PlayerMove(int player)
{
    int targetX = player * 16 + 12;
    int dx, dy, angleIndex;

    dx = targetX - (mPlayers[player].x >> 12);
    dy = -44 - (mPlayers[player].y >> 12);

    mPlayers[player].angle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    mPlayers[player].speed += 0x200;

    angleIndex = (u16)mPlayers[player].angle >> 4;
    {
        short trigValue = data_02082214[angleIndex * 2 + 1];
        int speed = mPlayers[player].speed;
        mPlayers[player].x += FX_MUL(trigValue, speed);
    }

    angleIndex = (u16)mPlayers[player].angle >> 4;
    {
        short trigValue = data_02082214[angleIndex * 2];
        int speed = mPlayers[player].speed;
        mPlayers[player].y += FX_MUL(trigValue, speed);
    }

    dx = targetX - (mPlayers[player].x >> 12);
    dy = -44 - (mPlayers[player].y >> 12);

    if (dx < -3) return;
    if (dx > 3) return;
    if (dy < -3) return;
    if (dy > 3) return;

    mPlayers[player].x = targetX << 12;
    mPlayers[player].y = -0x2c000;
    mPlayers[player].state = 2;
}

void dScMgMemory_c::PlayerDrop(int i){
    short *e = &mPlayers[i].delay;
    if(*(unsigned short*)e != 0){
        *e = *(unsigned short*)e - 1;
        if(*e < 0) *e = 0;
        return;
    }
    mPlayers[i].x = 0xc000;
    mPlayers[i].y = 0xc000;
    mPlayers[i].unk_11 = 1;
    mPlayers[i].state = 1;
    mPlayers[i].speed = 0xc00;
}

void dScMgMemory_c::InitPlayers()
{
    int count;
    int i;
    int stagger;

    if (gMgScene != 0)
        count = gMgScene->mCredits;
    else
        count = 0;
    if (count >= 3)
        count = 3;

    i = 0;
    mMaxMisses = (unsigned char)count;
    if (count <= 0)
        return;

    stagger = 0;
    do {
        mPlayers[i].active = 1;
        mPlayers[i].state = 0;
        mPlayers[i].delay = stagger;
        i++;
        stagger += 0x10;
    } while (i < count);
}

inline int *WordAt(char *p)
{
  return (int *)p;
}

void dScMgMemory_c::CheckFinished()
{
  if (mPairsFound >= mTargetPairs)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    /* Read mCredits through WordAt: as a plain member the multiply-add takes
       its registers the other way round. */
    Mg_SaveRecordPair(*WordAt((char *)&mCredits) + (3 - mMisses) * 6, mHudScore + 1);
  }
  if (mMisses >= mMaxMisses)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    Mg_SaveRecordPair(mCredits, mHudScore - 1);
  }
}

void dScMgMemory_c::JudgePair()
{
    int first, second;
    unsigned char *firstState, *secondState;
    if (mSelectedCount < 2) return;
    first = mSelectedCards[0];
    second = mSelectedCards[1];
    firstState = &mCards[first].state;
    if (*firstState != CARD_WAITING) return;
    secondState = &mCards[second].state;
    if (*secondState != CARD_WAITING) return;
    if (mCards[first].value == mCards[second].value) {
        mCards[first].active = 0;
        mCards[first].visible = 0;
        mCards[second].active = 0;
        mCards[second].visible = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        mPairsFound++;
        mSelectedCount = 0;
    } else {
        func_02012790(0xe);
        mMisses++;
        MgCoins_StartLoseOneMore();
        Sound::PlayBank2_2D(0x13e);
        if (mMisses < mMaxMisses) {
            *firstState = CARD_FLIPPING_DOWN;
            *secondState = CARD_FLIPPING_DOWN;
            mSelectedCount = 0;
        }
    }
}

#pragma push
#pragma opt_propagation off
void dScMgMemory_c::DrawCards()
{
    char *raw = (char *)this;
    int i;
    int spriteIndex, dim;
    char *p;

    /* sMemoryCardFrameSprites is indexed as [value][frame], five frames per value. */
    p = raw;
    i = 0;
    do {
        dMgMemoryCardCur *card = (dMgMemoryCardCur *)p;
        if (card->card.visible != 0) {
            dim = 0;
            if (mState != 2)
                dim = 1;
            spriteIndex = sMemoryCardFrameSprites[card->card.value * 5 + card->card.frame];
            Hud_RenderSprite(sMemoryCardSprites[spriteIndex], card->card.x >> 12,
                             card->card.y >> 12, -1, dim);
        }
        i += 1;
        p += 0x18;
    } while (i < 12);
}

#pragma pop

void dScMgMemory_c::UpdateCards()
{
    for (int i = 0; i < 12; i++) {
        if (mCards[i].active)
            (this->*sMemoryCardTable[mCards[i].state])(i);
    }
}

#pragma push
#pragma opt_lifetimes off
void dScMgMemory_c::CardFlyAway(int i)
{
    int j;
    int n;
    unsigned char *started;
    int *cardX;

    cardX = &mCards[i].x;
    started = &mCards[i].flyAwayStarted;
    *cardX -= 0x10000;
    if (*started == 0)
    {
        if (mDifficulty == 1)
        {
            if (i == 6 || i == 0xb)
            {
                *started = 1;
            }
            n = (i >= 7) ? 0xc - i : 7 - i;
            for (j = 1; j < n; j++)
            {
                if (mCards[i + j].active != 0)
                {
                    if (((mCards[i].x - mCards[i + j].x) >> 12) <= 4)
                    {
                        *started = 1;
                        mCards[i + j].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        }
        else
        {
            if ((i & 3) == 3)
            {
                *started = 1;
            }
            j = 4 - (i & 3);
            for (n = 1; n < j; n++)
            {
                if (mCards[i + n].active != 0)
                {
                    if (((mCards[i].x - mCards[i + n].x) >> 12) <= 4)
                    {
                        *started = 1;
                        mCards[i + n].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        }
    }
    if ((*cardX >> 12) > -0x18)
    {
        return;
    }
    mCards[i].active = 0;
    mCards[i].visible = 0;
}

#pragma pop

void dScMgMemory_c::CardFlipDown(int i){
    unsigned short timer;
    mCards[i].animTimer += 1;
    timer = mCards[i].animTimer;
    if (timer < (sMemoryFlipFrameTimes[mCards[i].frame] & 0xff))
        return;
    mCards[i].animTimer = 0;
    mCards[i].frame -= 1;
    if (mCards[i].frame == 0)
        mCards[i].state = CARD_SELECTABLE;
}

void dScMgMemory_c::CardWait(int /* card */)
{
}

void dScMgMemory_c::CardFlipUp(int card)
{
    ++mCards[card].animTimer;
    if (mCards[card].animTimer < (sMemoryFlipFrameTimes[mCards[card].frame] & 0xff))
        return;
    mCards[card].animTimer = 0;
    ++mCards[card].frame;
    if (mCards[card].frame > 4) {
        mCards[card].frame = 4;
        mCards[card].state = CARD_WAITING;
    }
}

void dScMgMemory_c::CardSelect(int card)
{
    unsigned int count = mSelectedCount;
    unsigned int touchSample;
    int touchActive;
    int touchDeltaX;
    int touchDeltaY;
    if (count >= 2)
    {
        return;
    }
    touchSample = gActivePlayerSlot;
    touchActive = 0;
    if (gTouchHeld[gActivePlayerSlot * 4] != 0)
    {
        if (gTouchEdge[gActivePlayerSlot * 4] != 0)
        {
            touchActive = 1;
        }
    }
    if (touchActive == 0)
    {
        return;
    }
    touchDeltaX = gTouchX[touchSample * 4] - (mCards[card].x >> 12);
    touchDeltaY = gTouchY[touchSample * 4] - (mCards[card].y >> 12);
    if (touchDeltaX < (-0x10))
    {
        return;
    }
    if (touchDeltaX > 0x10)
    {
        return;
    }
    if (touchDeltaY < (-0x16))
    {
        return;
    }
    if (touchDeltaY > 0x16)
    {
        return;
    }
    mSelectedValues[count] = mCards[card].value;
    mSelectedCards[mSelectedCount] = (u8) card;
    ++mSelectedCount;
    mCards[card].state = CARD_FLIPPING_UP;
    func_02012718(0x143, mCards[card].x);
    if (mInputSeen != 0)
    {
        return;
    }
    ++mInputSeen;
    Mg_SaveRecordPair(mCredits, mHudScore);
}

void dScMgMemory_c::CardIdle(int /* card */)
{
}

void dScMgMemory_c::CardMove(int card)
{
    int positionIndex = card * 2;
    u16 *positions = sMemoryCardPositionsPerDifficulty[mDifficulty];
    int dx, dy, targetX, targetY;
    targetX = positions[positionIndex];
    dx = targetX - (mCards[card].x >> 12);
    targetY = positions[positionIndex + 1];
    dy = targetY - (mCards[card].y >> 12);

    mCards[card].angle = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 trigValue = data_02082214[(((u16)mCards[card].angle >> 4) << 1) + 1];
        mCards[card].x += (int)(((s64)trigValue * mCards[card].speed + 0x800) >> 0xc);
    }
    {
        s16 trigValue = data_02082214[((u16)mCards[card].angle >> 4) << 1];
        mCards[card].y += (int)(((s64)trigValue * mCards[card].speed + 0x800) >> 0xc);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    mCards[card].x = targetX << 12;
    mCards[card].y = targetY << 12;
    mCards[card].state = CARD_IDLE;
}

void dScMgMemory_c::ResultFinish()
{
    s32 i;
    s32 j;
    s32 k;
    s32 j2;
    s32 k2;
    s32 touched;
    u16 *p;

    UpdateCards();
    UpdateCursor();

    if (mCardTimer != 0)
    {
        p = &mCardTimer;
        *p = (u16)(*p - 1);
        return;
    }

    i = gActivePlayerSlot;
    touched = 0;
    if (gTouchHeld[i * 4] != 0)
    {
        if (gTouchEdge[i * 4] != 0)
        {
            touched = 1;
        }
    }
    if (touched == 0)
    {
        return;
    }

    func_02012790(0x62);

    if (mCredits == 0)
    {
        if (mDifficulty == 1)
        {
            for (j = 0; j < 2; j++)
            {
                for (k = 0; k < 5; k++)
                {
                    if (mCards[j * 5 + k + 2].active != 0)
                    {
                        mCards[j * 5 + k + 2].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        }
        else
        {
            for (j2 = 0; j2 < 3; j2++)
            {
                for (k2 = 0; k2 < 4; k2++)
                {
                    if (mCards[j2 * 4 + k2].active != 0)
                    {
                        mCards[j2 * 4 + k2].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        }
        mMessageVisible = 0;
    }

    HideCursor();
    mState = 4;
}

void dScMgMemory_c::ResultTurnCards()
{
    int flipped;
    int i;
    if (mCardTimer != 0) {
        unsigned short *q = &mCardTimer;
        *q = *q - 1;
        return;
    }
    if (mPairsFound >= mTargetPairs) {
        mState = 4;
        mCardTimer = 0;
        return;
    }
    flipped = 0;
    for (i = 0; i < 0xc; i++) {
        if (mCards[i].active != 0) {
            if (mCards[i].state == CARD_SELECTABLE) {
                mCards[i].state = CARD_FLIPPING_UP;
                flipped++;
            }
        }
    }
    if (flipped <= 2)
        Sound::PlayBank2_2D(0x145);
    else
        Sound::PlayBank2_2D(0x146);
    mSubstate = 3;
    ShowCursor();
    mCardTimer = 0x20;
}

void dScMgMemory_c::ResultReward()
{
    if (mCardTimer != 0) {
        --mCardTimer;
        if (mCardTimer != 0)
            return;
        if (mPairsFound < mTargetPairs)
            return;
        MgCoins_StartPayout(6 * (3 - mMisses));
        return;
    }
    if (gMgCoinsStatus == 1) {
        mCardTimer = 0;
        mSubstate = 2;
    }
}

void dScMgMemory_c::ResultWait()
{
    if (mResultTimer != 0) {
        mResultTimer -= 1;
        if ((short)mResultTimer <= 0) {
            mResultTimer = 0;
            if (mPairsFound >= mTargetPairs) {
                if (mMisses == 0)
                    Mg_SetPerfectBanner();
                Mg_SetSharedState(MG_STATE_WIN);
                if (mHudScore < 0x270f) {
                    int *score = &mHudScore;
                    *score += 1;
                }
                if (mHudScore > mBestHudScore)
                    mBestHudScore = mHudScore;
                Mg_SubmitScore(gMgScene ? gMgScene->mHudScore : 0);
                Dealer_PlayWin(&mShared);
            } else {
                Mg_SetSharedState(MG_STATE_LOSE);
                if (mHudScore > 0) {
                    int *score = &mHudScore;
                    *score -= 1;
                }
                Dealer_PlayLose(&mShared);
            }
        }
    } else {
        mPromptEnabled = 0;
        mCardTimer = 0x10;
    }
}

void dScMgMemory_c::RoundReveal()
{
    int i;
    unsigned short *t;
    if (mCardTimer != 0) {
        t = &mCardTimer;
        *t = *t - 1;
        if ((short)mCardTimer > 0)
            return;
        InitPlayers();
        return;
    }
    for (i = 0; i < 0xc; i++) {
        mCards[i].state = CARD_SELECTABLE;
    }
    mSubstate = 0;
    mState = 2;
}

/* Nothing reads the result. The void* return leaves this + 0x5000 in r0. */
void* dScMgMemory_c::RoundWaitDeal(){
  char* raw = (char*)this;
  int waiting = 0;
  int i = 0;
  char* p = raw;
  for (; i < 0xc; ) {
    dMgMemoryCardCur *card = (dMgMemoryCardCur *)p;
    if (card->card.active != 0) {
      if (card->card.state != CARD_IDLE) {
        waiting++;
        break;
      }
    }
    i++;
    p += 0x18;
  }
  if (waiting != 0) return raw;
  p = raw;
  i = 0;
  {
    unsigned char v = CARD_SELECTABLE;
    for (; i < 0xc; ) {
      ((dMgMemoryCardCur *)p)->card.state = v;
      i++;
      p += 0x18;
    }
  }
  {
    mSubstate = 0;
    mState = 2;
    return raw + 0x5000;
  }
}

void dScMgMemory_c::RoundDealHard()
{
    if (mReadyCount < 3)
        return;
    mCards[11 - mDealCount].active = 1;
    ++mDealCount;
    if (mDealCount >= 12)
        mSubstate = 4;
}

void dScMgMemory_c::RoundDealNormal()
{
    u8 mode;
    if (mReadyCount < 2)
        return;
    mCards[0xb - mDealCount].active = 1;
    mDealCount++;
    mode = mDifficulty;
    if (mode == 1) {
        if (mDealCount >= 10)
            mSubstate = 4;
        return;
    }
    if (mDealCount < 8)
        return;
    if (mode != 0)
        mSubstate = 3;
    else
        mSubstate = 4;
}

void dScMgMemory_c::RoundDealEasy()
{
    if (mReadyCount < 1)
        return;
    mCards[11 - mDealCount].active = 1;
    ++mDealCount;
    if (mDifficulty == 1) {
        if (mDealCount >= 5)
            mSubstate = 2;
    } else {
        if (mDealCount >= 4)
            mSubstate = 2;
    }
}

void dScMgMemory_c::RoundStart(){
    mReadyCount = 0;
    mDealCount = 0;
    if (mDifficulty >= 2)
        Dealer_StartShuffle((char *)&mShared, 4, 3, (int)&mReadyCount);
    else
        Dealer_StartShuffle((char *)&mShared, 4, 2, (int)&mReadyCount);
    mShared.ready = 1;
    mSubstate = 1;
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

void dScMgMemory_c::ShuffleCards()
{
    char *raw = (char *)this;
    int slot;
    int k;
    u8 *count;
    int i;
    char *p;

    for (i = 0; i < 7; i++)
        mValueCounts[i] = 0;

    if (mDifficulty == 0) {
        p = raw + 0x60;
        for (k = 4; k < 12; k++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            for (;;) {
                /* mValueCounts[slot] hoists the base. Same spelling below. */
                count = (u8 *)(raw + slot + 0x5330);
                if (*count < 2) {
                    ((dMgMemoryCardCur *)p)->card.value = slot;
                    *count += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            }
            p += 0x18;
        }
        p = raw + 0x60;
        for (i = 4; i < 12; i++) {
            ((dMgMemoryCardCur *)p)->card.x = 0x80000;
            ((dMgMemoryCardCur *)p)->card.y = -0x80000;
            ((dMgMemoryCardCur *)p)->card.speed = 0x8000;
            ((dMgMemoryCardCur *)p)->card.visible = 1;
            ((dMgMemoryCardCur *)p)->card.state = CARD_MOVING;
            p += 0x18;
        }
    } else if (mDifficulty == 1) {
        p = raw + 0x30;
        for (i = 2; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            for (;;) {
                count = (u8 *)(raw + slot + 0x5330);
                if (*count < 2) {
                    ((dMgMemoryCardCur *)p)->card.value = slot;
                    *count += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            }
            p += 0x18;
        }
        p = raw + 0x30;
        for (i = 2; i < 12; i++) {
            ((dMgMemoryCardCur *)p)->card.x = 0x80000;
            ((dMgMemoryCardCur *)p)->card.y = -0x80000;
            ((dMgMemoryCardCur *)p)->card.speed = 0x8000;
            ((dMgMemoryCardCur *)p)->card.visible = 1;
            ((dMgMemoryCardCur *)p)->card.state = CARD_MOVING;
            p += 0x18;
        }
    } else {
        p = raw;
        for (i = 0; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            for (;;) {
                count = (u8 *)(raw + slot + 0x5330);
                if (*count < 2) {
                    ((dMgMemoryCardCur *)p)->card.value = slot;
                    *count += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            }
            p += 0x18;
        }
        p = raw;
        for (i = 0; i < 12; i++) {
            ((dMgMemoryCardCur *)p)->card.x = 0x80000;
            ((dMgMemoryCardCur *)p)->card.y = -0x80000;
            ((dMgMemoryCardCur *)p)->card.speed = 0x8000;
            ((dMgMemoryCardCur *)p)->card.visible = 1;
            ((dMgMemoryCardCur *)p)->card.state = CARD_MOVING;
            p += 0x18;
        }
    }
}

void dScMgMemory_c::ResetGame()
{
    int i;
    int j;
    i = 0;
    do {
        mCards[i].x = 0;
        mCards[i].y = 0;
        mCards[i].speed = 0;
        mCards[i].angle = 0;
        mCards[i].animTimer = 0;
        mCards[i].value = 0;
        mCards[i].unk_11 = 0;
        mCards[i].visible = 0;
        mCards[i].active = 0;
        mCards[i].state = CARD_MOVING;
        mCards[i].frame = 0;
        mCards[i].flyAwayStarted = 0;
        i++;
    } while (i < 12);
    j = 0;
    do {
        mPlayers[j].x = 0;
        mPlayers[j].y = 0;
        mPlayers[j].speed = 0;
        mPlayers[j].delay = 0;
        mPlayers[j].angle = 0;
        mPlayers[j].active = 0;
        mPlayers[j].unk_11 = 0;
        mPlayers[j].state = 0;
        j++;
    } while (j < 3);
    mCursor.visible = 0;
    mCursor.enabled = 0;
    mCursor.frame = 0;
    mState = 0;
    mSubstate = 0;
    mRoundTimer = 0;
    mCardTimer = 0;
    mResultTimer = 0;
    mPairsFound = 0;
    mSelectedCount = 0;
    unk_5339 = 0;
    mMisses = 0;
    i = 0;
    do {
        mSelectedValues[i] = 0;
        mSelectedCards[i] = 0;
        i++;
    } while (i < 2);
    j = 0;
    do {
        mValueCounts[j] = 0;
        j++;
    } while (j < 7);
    mMessageVisible = 1;
    mInputSeen = 0;
}

void dScMgMemory_c::StateExit()
{
    UpdateCards();
    if (mCardTimer)
        mCardTimer -= 1;
}

void dScMgMemory_c::StateResult(){
    (this->*sMemoryResultTable[mSubstate])();
}

void dScMgMemory_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

void dScMgMemory_c::StatePlay() {
    (this->*sMemoryRoundTable[mSubstate])();
    UpdateCards();
}

void dScMgMemory_c::StateSetup(){
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    Dealer_PlayIdle((char *)&mShared);
    mState = 1;
}

void dScMgMemory_c::SetupDifficulty() {
    mDifficulty = 0;
    mTargetPairs = 4;
    int score = mHudScore;
    if (score >= 0xa) {
        mDifficulty = 2;
        mTargetPairs = 6;
        return;
    }
    if (score >= 5) {
        mDifficulty = 1;
        mTargetPairs = 5;
    }
}

// @symbol _ZN13dScMgMemory_c10OnDealDoneEv
void dScMgMemory_c::OnDealDone()
{
    MgCoins_PlaceBet(mMaxMisses);
}

// @symbol _ZN13dScMgMemory_c11CanEndStateEi
int dScMgMemory_c::CanEndState(int /* mode */)
{
    if (mState == 3 && mSubstate == 0) {
        if (Dealer_IsIdle((int*)&mShared) == 0) return 0;
        mSubstate = 1;
        FreeGfxSlotsById(0x1d);
    }
    return mState == 4;
}

// @symbol _ZN13dScMgMemory_c10StartRoundEi
/* Slot 18: restarts the game, with up to three misses allowed from the meter. */

void dScMgMemory_c::StartRound(int /* arg */)
{
    dScMgBase_c *meter;
    int lives;
    ResetGame();
    mState = 0;
    meter = gMgScene;
    lives = 0;
    if (meter != 0) lives = meter->mCredits;
    if (lives >= 3) lives = 3;
    ((void (*)(void *))MgCoins_Reset)(meter);
    gMgCoinsShowDelta = 1;
    mMaxMisses = (unsigned char)lives;
}

// @symbol _ZN13dScMgMemory_c6RenderEv
/* Slot 9. */
s32 dScMgMemory_c::Render()
{
    Camera_InitDealerView((char *)pad_4660);
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    MgCoins_Draw();
    DrawMessage();
    DrawCursor();
    DrawCards();
    Dealer_Draw((char *)&mShared);
    return 1;
}

// @symbol _ZN13dScMgMemory_c8BehaviorEv
/* Slot 6: runs the current state from the table, indexed by mState. */
s32 dScMgMemory_c::Behavior()
{
    (this->*sMemoryStateTable[mState])();
    MgCoins_Update();
    Dealer_Update(&mShared);
    return 1;
}

// @symbol _ZN13dScMgMemory_c13InitResourcesEv
/* Slot 0. */
s32 dScMgMemory_c::InitResources()
{
    void *tiles;
    void *palette;

    data_0209d45c = 0x11;
    LoadCard2SubBg3();
    tiles = LoadFile(sMemoryCardNcgByLanguage[GetGameLanguage()]);
    palette = LoadFile(ASSET_D_2D_MG_CARD_NCL_BIN);
    DecompressLZ16(tiles, (void *)0x6400000);
    GX::LoadOBJPltt(palette, 0, 0x100);
    DecompressLZ16(tiles, (void *)0x6600000);
    GXS::LoadOBJPltt(palette, 0, 0x100);
    Deallocate(tiles);
    Deallocate(palette);
    data_0209d454 = 0x18;
    ResetGame();
    Camera_InitDealerView((char *)pad_4660);
    if (Dealer_Load(&mShared) == 0)
        return 0;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    mHudScore = Mg_GetSavedRecord1();
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mMaxMisses = 3;
    Mg_SetTopScreenYOffset(0x20);
    Mg_PlaceBannersOnTopScreen();
    return 1;
}

// @symbol dScMgMemory_c_classInit
/* Builds the scene by hand (operator new, base constructor, vtables,
 * mShared) because the class has no constructor declared yet. */
extern "C" void *dScMgMemory_c_classInit()
{
    char *scene = (char *)fBase_c::operator new(sizeof(dScMgMemory_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(void **)scene = &_ZTV13dScMgMemory_c[2];
        Dealer_Construct(scene + 0x4f38);
    }
    return scene;
}

/* Static-init globals (was the handwritten __sinit_ov006_021311c8 shard).
 * The sprite-pointer table plus the five PMF state tables plain-copied
 * from the ROM constants, in retail copy order: the permuted sprite
 * pointers, the five main states, the six round states, the four result
 * states, the seven card states, then the three player states. */
extern "C" int sMgCardSprites[];
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_StateSetup;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_StatePlay;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_StateJudge;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_StateResult;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_StateExit;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_RoundStart;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_RoundDealEasy;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_RoundDealNormal;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_RoundDealHard;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_RoundWaitDeal;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_RoundReveal;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_ResultWait;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_ResultReward;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_ResultTurnCards;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_ResultFinish;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardMove;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardIdle;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardSelect;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardFlipUp;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardWait;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardFlipDown;
extern "C" dScMgMemory_cCardState pmf_dScMgMemory_c_CardFlyAway;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_PlayerDrop;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_PlayerMove;
extern "C" dScMgMemory_cState pmf_dScMgMemory_c_PlayerWait;

void *sMemoryCardSprites[15] = {
    (void *)sMgCardSprites[0],  (void *)sMgCardSprites[1],  (void *)sMgCardSprites[2],
    (void *)sMgCardSprites[11], (void *)sMgCardSprites[12], (void *)sMgCardSprites[13],
    (void *)sMgCardSprites[14], (void *)sMgCardSprites[3],  (void *)sMgCardSprites[4],
    (void *)sMgCardSprites[5],  (void *)sMgCardSprites[6],  (void *)sMgCardSprites[9],
    (void *)sMgCardSprites[10], (void *)sMgCardSprites[7],  (void *)sMgCardSprites[8],
};
dScMgMemory_cState sMemoryStateTable[5] = {
    pmf_dScMgMemory_c_StateSetup,  pmf_dScMgMemory_c_StatePlay, pmf_dScMgMemory_c_StateJudge,
    pmf_dScMgMemory_c_StateResult, pmf_dScMgMemory_c_StateExit,
};
dScMgMemory_cState sMemoryRoundTable[6] = {
    pmf_dScMgMemory_c_RoundStart,      pmf_dScMgMemory_c_RoundDealEasy,
    pmf_dScMgMemory_c_RoundDealNormal, pmf_dScMgMemory_c_RoundDealHard,
    pmf_dScMgMemory_c_RoundWaitDeal,   pmf_dScMgMemory_c_RoundReveal,
};
dScMgMemory_cState sMemoryResultTable[4] = {
    pmf_dScMgMemory_c_ResultWait,
    pmf_dScMgMemory_c_ResultReward,
    pmf_dScMgMemory_c_ResultTurnCards,
    pmf_dScMgMemory_c_ResultFinish,
};
dScMgMemory_cCardState sMemoryCardTable[7] = {
    pmf_dScMgMemory_c_CardMove,    pmf_dScMgMemory_c_CardIdle, pmf_dScMgMemory_c_CardSelect,
    pmf_dScMgMemory_c_CardFlipUp,  pmf_dScMgMemory_c_CardWait, pmf_dScMgMemory_c_CardFlipDown,
    pmf_dScMgMemory_c_CardFlyAway,
};
dScMgMemory_cState sMemoryPlayerTable[3] = {
    pmf_dScMgMemory_c_PlayerDrop,
    pmf_dScMgMemory_c_PlayerMove,
    pmf_dScMgMemory_c_PlayerWait,
};
