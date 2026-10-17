//cpp
/* Memory Master (MG_MEMORY_J). Deal the cards, move the markers, judge pairs.
 *
 * Functions are in ROM order; do not reorder. The local pragmas keep their
 * emission. Earlier probes are in
 * notes/experiments/pr2875-source-repair-0920.json.
 *
 * leftovers:
 * - DrawCards, RoundWaitDeal, RoundReadyCards, RoundHideCards,
 *   RoundShowCards, ResultFinish, ResetGame and InitPlayers step char* by
 *   0x18 or 0x14 from this. A card or player pointer changes those loops.
 * - CardFlyAway keeps the Ctx view. mCards[idx] misses.
 * - CheckFinished calls inline_fn(this+0xa8) and this+0xb4.
 *   mCredits and mHudScore directly miss. unk_5407 is only cleared.
 * - ResultWait decrements mResultTimer as (int)this+0x53e4.
 * - Camera_InitDealerView sets the camera at this+0x4660 and tail-calls
 *   Camera_UpdateMatrices. Render and InitResources call that wrapper.
 * - Mg_DrawScoreTally, Mg_SaveRecordPair and Dealer_Load
 *   stay the linker names. Their bodies are not in this file.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgMemory2_c_AssetId {
    ASSET_D_2D_MG_CARD_NCL_BIN = 0x00bb
};

#include "dScMgMemory2_c.h"
#include "Sound.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "decl_common.h"
#include "PlayerInput.h"

/* Car, Ctx, B, Slot and Work are views over the scene that three functions
 * still need to match. They are not the original class layouts. */

typedef void (dScMgMemory2_c::*dScMgMemory2_cState)();
typedef void (dScMgMemory2_c::*dScMgMemory2_cCardState)(int);
extern "C" dScMgMemory2_cCardState sMemory2CardTable[];

typedef struct Car {
    /* 0x00 */ s32 x;
    /* 0x04 */ u8 unk04[0x12 - 0x04];
    /* 0x12 */ u8 b12;
    /* 0x13 */ u8 b13;
    /* 0x14 */ u8 b14;
    /* 0x15 */ u8 b15;
    /* 0x16 */ u8 b16;
    /* 0x17 */ u8 b17;
} Car;

typedef struct Ctx {
    /* 0x0000 */ Car raw[1];
    /* 0x0018 */ u8 unk18[0x51a8 - 0x18];
    /* 0x51a8 */ Car cars[25];
    /* 0x5400 */ u8 unk5400[0xa];
    /* 0x540a */ u8 mode;
} Ctx;

struct B {
    char pad[0xb4];
    int b4;
    int b8;
};

typedef struct {
    char _pad0[0x13];
    u8 done;     /* +0x13 */
    char _pad1[4];
} Slot; /* 0x18 */

typedef struct {
    char _pad0[0x51a8];
    Slot slots[23];   /* 0x51a8 */
    char _pad1[8];
    int state;        /* 0x53d8 */
    char _pad2[0xa];
    short ready;      /* 0x53e6 */
    short count;      /* 0x53e8 */
    u16 total;        /* 0x53ea */
    char _pad3[0x1e];
    u8 mode;          /* 0x540a */
} Work;

extern "C" dScMgMemory2_cState sMemory2ResultTable[];
extern "C" dScMgMemory2_cState sMemory2StateTable[];
extern "C" dScMgMemory2_cState sMemory2RoundTable[];

namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern "C" {
extern void Mg_DrawScoreTally(void *thiz, int a, int b, int c);
extern void Mg_DrawBlinkSprite(void *c, int arg1, int arg2);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
extern "C" unsigned int func_02012790(unsigned int x);
extern "C" void MgCoins_StartLoseOneMore(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 sMemory2CardFrameSprites[];
extern void *sMemory2CardSprites[];
extern s32 sMemory2FlyAwayLimitPerMode[];
extern unsigned short sMemory2FlipFrameTimes[];
extern void Mg_SaveRecordPair(int a, int b);
extern u16 *sMemory2CardPositionsPerDifficulty[];
extern int sMemory2ResultRowsPerDifficulty[];
extern int sMemory2ResultColsPerDifficulty[];
extern void MgCoins_StartPayout(char* p);
extern short gMgCoinsStatus;
extern void Mg_SetPerfectBanner(void);
extern void Mg_SetSharedState(int);
extern void Mg_SubmitScore(int self);
extern void Dealer_PlayWin(void *c);
extern void Dealer_PlayLose(void *c);
extern void Dealer_StartShuffle(char *c, int unused, short count, int ready);
extern int sMemory2FourthDealsPerDifficulty[];
extern int sMemory2HardDealsPerMode[];
extern int sMemory2NormalDealsPerDifficulty[];
extern int sMemory2EasyDealsPerDifficulty[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int sMemory2PreviewCountPerDifficulty[];
extern int sMemory2PreviewTimePerDifficulty[];
extern void FreeGfxSlotsById(int x);
extern void Dealer_PlayIdle(char *p);
extern int Dealer_IsIdle(int* p);
extern void MgCoins_Reset(void);
void Camera_InitDealerView(char *);
void Mg_DrawCredits(char *, int, int, int);
void Dealer_Draw(char *);
extern "C" void MgCoins_Update(void);
extern "C" void Dealer_Update(char *p);
void LoadCard2SubBg3(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int Dealer_Load(void *c);
int Mg_GetSavedRecord0(void);
int Mg_GetSavedRecord1(void);
void Mg_SetTopScreenYOffset(int v);
void Mg_PlaceBannersOnTopScreen(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

#pragma defer_codegen off

// @symbol _ZN14dScMgMemory2_cD1Ev
/* Destroys mShared, then the inherited scene. CodeWarrior emits both D1 and
 * D0 from this one definition. */
dScMgMemory2_c::~dScMgMemory2_c()
{
}
// @symbol _ZN14dScMgMemory2_cD0Ev
/* Deleting variant, emitted from the destructor above. */

void dScMgMemory2_c::DrawMessage()
{
    if (mMessageVisible == 0) return;
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
}

void dScMgMemory2_c::DrawCursor() {
    if (mCursor.enabled == 0) return;
    Mg_DrawBlinkSprite(this, 0xe0, 0xa0);
}

void dScMgMemory2_c::UpdateCursor() {
  if (mCursor.visible == 0) return;
  {
    unsigned short* timer = (unsigned short *)&mCursor.blinkTimer;
    *timer = *timer + 1;
    if (*timer < 0x14) return;
    *timer = 0;
  }
  {
    unsigned char* frame = &mCursor.frame;
    *frame = *frame + 1;
    *frame = *frame & 1;
  }
}

void dScMgMemory2_c::ShowCursor()
{
    mCursor.visible = 1;
    mCursor.enabled = 1;
    mCursor.x = 966656;
    mCursor.y = 688128;
    mCursor.blinkTimer = 0;
    mCursor.frame = 0;
}

void dScMgMemory2_c::HideCursor()
{
    mCursor.visible = 0;
    mCursor.enabled = 0;
}

void dScMgMemory2_c::PlayerWait(int /* player */)
{
}

void dScMgMemory2_c::PlayerMove(int player)
{
  int tx = (player << 4) + 0xc;
  int tz = -0x2c;
  u16 *angle;
  int dx;
  int dz;
  dx = tx - (mPlayers[player].x >> 12);
  dz = tz - (mPlayers[player].y >> 12);
  mPlayers[player].angle = _ZN4cstd5atan2E5Fix12IiES1_(dz, dx);
  mPlayers[player].speed = mPlayers[player].speed + 0x200;
  angle = (u16 *)&mPlayers[player].angle;
  mPlayers[player].x =
      mPlayers[player].x + FX_MUL(data_02082214[((*angle >> 4) << 1) + 1], mPlayers[player].speed);
  mPlayers[player].y =
      mPlayers[player].y + FX_MUL(data_02082214[(*angle >> 4) << 1], mPlayers[player].speed);
  dx = tx - (mPlayers[player].x >> 12);
  dz = tz - (mPlayers[player].y >> 12);
  if (dx < -3 || dx > 3)
  {
    return;
  }
  if (dz < -3 || dz > 3)
  {
    return;
  }
  mPlayers[player].x = tx << 12;
  mPlayers[player].y = tz << 12;
  mPlayers[player].state = 2;
}

void dScMgMemory2_c::PlayerDrop(int player) {
    short *delay = &mPlayers[player].delay;
    if (*(unsigned short*)delay != 0) {
        *delay = *(unsigned short*)delay - 1;
        if (*delay < 0) *delay = 0;
        return;
    }
    mPlayers[player].x = 0xc000;
    mPlayers[player].y = 0xc000;
    mPlayers[player].unk_11 = 1;
    mPlayers[player].state = 1;
    mPlayers[player].speed = 0xc00;
}

void dScMgMemory2_c::InitPlayers()
{
    char *raw = (char *)this;
    int count;
    int i;
    int delay;
    char *entry;

    if (gMgScene != 0)
        count = gMgScene->mCredits;
    else
        count = 0;
    if (count >= 5)
        count = 5;

    entry = raw;
    i = 0;
    mMaxMisses = (unsigned char)count;
    if (count <= 0)
        return;

    delay = 0;
    do {
        *(unsigned char *)(entry + 0x5398) = 1;
        *(unsigned char *)(entry + 0x539a) = 0;
        *(short *)(entry + 0x5396) = delay;
        i++;
        entry += 0x14;
        delay += 0x10;
    } while (i < count);
}

/* inline_fn stays: reading mCredits and mHudScore directly changes the code. */
void Mg_SaveRecordPair(int r0arg, int r1arg);
inline int *inline_fn(char *arg0)
{
  return (int *) arg0;
}

void dScMgMemory2_c::CheckFinished()
{
  char *raw = (char *)this;
  if (mPairsFound >= mTargetPairs)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    Mg_SaveRecordPair(((5 - mMisses) * 5) + (*inline_fn(raw + 0xa8)), (*inline_fn(raw + 0xb4)) + 1);
  }
  if (mMisses >= mMaxMisses)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    Mg_SaveRecordPair(*inline_fn(raw + 0xa8), (*inline_fn(raw + 0xb4)) - 1);
  }
}

void dScMgMemory2_c::JudgePair()
{
    int first, second;
    unsigned char *firstState, *secondState;
    if (mSelectedCount < 2) return;
    first = mSelectedCards[0];
    firstState = &mCards[first].state;
    second = mSelectedCards[1];
    if (*firstState != 4) return;
    secondState = &mCards[second].state;
    if (*secondState != 4) return;
    if (mCards[first].value == mCards[second].value) {
        mCards[first].active = 0;
        mCards[first].visible = 0;
        mCards[second].active = 0;
        mCards[second].visible = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        mPairsFound += 1;
        mSelectedCount = 0;
    } else {
        func_02012790(0xe);
        Sound::PlayBank2_2D(0x13e);
        mMisses += 1;
        MgCoins_StartLoseOneMore();
        if (mMisses < mMaxMisses) {
            *firstState = 5;
            *secondState = 5;
            mSelectedCount = 0;
        }
    }
}

// opt_propagation off keeps the card loop's constant live, as in the ROM.
#pragma push
#pragma opt_propagation off
void dScMgMemory2_c::DrawCards()
{
    char *raw = (char *)this;
    int i;
    int sprite, dim;
    char *p;

    /* p walks the 20 cards. sMemory2CardFrameSprites is indexed [value][frame]. */
    p = raw;
    i = 0;
    do {
        if (*(u8 *)(p + 0x51ba) != 0) {
            dim = 0;
            if (mState != 2)
                dim = 1;
            sprite = sMemory2CardFrameSprites[*(u8 *)(p + 0x51b8) * 5 + *(u8 *)(p + 0x51bd)];
            Hud_RenderSprite(sMemory2CardSprites[sprite], *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12, -1, dim);
        }
        i += 1;
        p += 0x18;
    } while (i < 20);
}
#pragma pop

void dScMgMemory2_c::UpdateCards()
{
    int i;
    for (i = 0; i < 0x14; i++) {
        if (mCards[i].active)
            (this->*sMemory2CardTable[mCards[i].state])(i);
    }
}

void dScMgMemory2_c::CardFlyAway(int idx)
{
    Ctx *ctx = (Ctx *)this;
    s32 lim;
    s32 rem;
    s32 k;
    s32 *px;
    s32 n;
    u8 *pb;

    ctx->cars[idx].x -= 0x10000;
    if (ctx->cars[idx].b16 == 0) {
        lim = sMemory2FlyAwayLimitPerMode[ctx->mode];
        rem = idx;
        if (idx >= lim) {
            n = lim;
            do {
                rem -= n;
            } while (rem >= n);
        }
        if (rem == 0) {
            ctx->cars[idx].b16 = 1;
            return;
        }
        n = rem + 1;
        k = 1;
        if (n > 1) {
            do {
                px = (s32 *)((u8 *)&ctx->raw[idx] + 0x51a8);
                pb = (u8 *)&ctx->raw[idx] + 0x51be;
                if (ctx->cars[idx - k].b13 != 0) {
                    if (((*px - ctx->cars[idx - k].x) >> 12) <= 4) {
                        *pb = 1;
                        ctx->cars[idx - k].b14 = 6;
                    }
                }
                k++;
            } while (k < n);
        }
    }
    if ((ctx->cars[idx].x >> 12) > -0x18) {
        return;
    }
    *((u8 *)ctx + idx * 24 + 0x51bb) = 0;
    *((u8 *)ctx + idx * 24 + 0x51ba) = 0;
}

void dScMgMemory2_c::CardFlipDown(int i){
    unsigned short cnt;
    mCards[i].animTimer += 1;
    cnt = mCards[i].animTimer;
    if (cnt < (sMemory2FlipFrameTimes[mCards[i].frame] & 0xff))
        return;
    mCards[i].animTimer = 0;
    mCards[i].frame -= 1;
    if(mCards[i].frame == 0)
        mCards[i].state = 2;
}

void dScMgMemory2_c::CardWait(int /* card */)
{
}

void dScMgMemory2_c::CardFlipUp(int idx){
  unsigned short* timer = &mCards[idx].animTimer;
  unsigned char* frame = &mCards[idx].frame;
  *timer = *timer + 1;
  if (*timer < (sMemory2FlipFrameTimes[*frame] & 0xff))
      return;
  *timer = 0;
  *frame = *frame + 1;
  if (*frame > 4) {
    *frame = 4;
    mCards[idx].state = 4;
  }
}

void dScMgMemory2_c::CardSelect(int idx)
{
    unsigned int count;
    unsigned int touch;
    int touching;
    int dx, dy;

    count = mSelectedCount;
    if (count >= 2) return;

    touch = gActivePlayerSlot;
    touching = 0;
    if (gTouchHeld[touch * 4] != 0) {
        if (gTouchEdge[touch * 4] != 0) {
            touching = 1;
        }
    }
    if (touching == 0) return;

    dx = gTouchX[gActivePlayerSlot * 4] - (mCards[idx].x >> 12);
    dy = gTouchY[gActivePlayerSlot * 4] - (mCards[idx].y >> 12);

    if (dx < -0x10) return;
    if (dx > 0x10) return;
    if (dy < -0x16) return;
    if (dy > 0x16) return;

    mSelectedValues[count] = mCards[idx].value;
    mSelectedCards[mSelectedCount] = (u8)idx;
    {
        u8 *pc = &mSelectedCount;
        *pc = *pc + 1;
    }
    mCards[idx].state = 3;
    func_02012718(0x143, mCards[idx].x);

    if (mInputSeen != 0) return;
    {
        u8 *pd = &mInputSeen;
        *pd = *pd + 1;
    }
    Mg_SaveRecordPair(mCredits, mHudScore);
}

void dScMgMemory2_c::CardIdle(int /* card */)
{
}

void dScMgMemory2_c::CardMove(int i)
{
    int i2 = i * 2;
    u16 *row = sMemory2CardPositionsPerDifficulty[mDifficulty];
    int dx, dy, a, b;
    a = row[i2];
    dx = a - (mCards[i].x >> 12);
    b = row[i2 + 1];
    dy = b - (mCards[i].y >> 12);

    mCards[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 tv = data_02082214[((static_cast<u16>(mCards[i].angle) >> 4) << 1) + 1];
        mCards[i].x += FX_MUL(tv, mCards[i].speed);
    }
    {
        s16 tv = data_02082214[(static_cast<u16>(mCards[i].angle) >> 4) << 1];
        mCards[i].y += FX_MUL(tv, mCards[i].speed);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    mCards[i].x = a << 12;
    mCards[i].y = b << 12;
    *&mCards[i].state = 1;
}

#pragma push
#pragma opt_loop_invariants off
void dScMgMemory2_c::ResultFinish()
{
    char *raw = (char *)this;
    int i;
    int best;
    int j;
    int n;
    int touch;
    int touching;
    unsigned short *timer;

    UpdateCards();
    UpdateCursor();

    if (mCardTimer != 0) {
        timer = &mCardTimer;
        *timer = *timer - 1;
        return;
    }

    touching = 0;
    touch = gActivePlayerSlot;
    if (gTouchHeld[touch * 4] != 0) {
        if (gTouchEdge[touch * 4] != 0) {
            touching = 1;
        }
    }
    if (touching == 0) {
        return;
    }

    func_02012790(0x62);

    if (mCredits == 0) {
        for (i = 0; i < sMemory2ResultRowsPerDifficulty[mDifficulty]; i++) {
            best = 0xff;
            j = 0;
            n = sMemory2ResultColsPerDifficulty[mDifficulty];
            for (; j < sMemory2ResultColsPerDifficulty[mDifficulty]; j++) {
                if (*(unsigned char *)(raw + 0x51bb + (i * n + j) * 0x18) != 0) {
                    best = j;
                    best = best + i * n;
                }
            }
            if (best != 0xff) {
                mCards[best].state = 6;
            }
        }
        mMessageVisible = 0;
    }

    HideCursor();
    mState = 4;
}
#pragma pop

void dScMgMemory2_c::ResultTurnCards()
{
    if (mCardTimer) {
        unsigned short* q = &mCardTimer;
        *q = *q - 1;
        return;
    }
    if (mPairsFound >= mTargetPairs) {
        mState = 4;
        mCardTimer = 0;
        return;
    }
    {
        int count = 0;
        int i = 0;
        for (; i < 0x14; i++) {
            if (mCards[i].active != 0) {
                if (mCards[i].state == 2) {
                    mCards[i].state = 3;
                    count++;
                }
            }
        }
        if (count <= 2) {
            Sound::PlayBank2_2D(0x145);
        } else {
            Sound::PlayBank2_2D(0x146);
        }
    }
    mSubstate = 3;
    ShowCursor();
    mCardTimer = 0x20;
}

void dScMgMemory2_c::ResultReward() {
    if (mCardTimer != 0) {
        unsigned short *ptr = &mCardTimer;
        *ptr = *ptr - 1;
        if (mCardTimer != 0) {
            return;
        }
        if (mPairsFound < mTargetPairs) {
            return;
        }
        MgCoins_StartPayout((char *)(5 * (5 - mMisses)));
        return;
    }
    if (gMgCoinsStatus == 1) {
        mCardTimer = 0;
        mSubstate = 2;
    }
}

void dScMgMemory2_c::ResultWait()
{
    char *raw = (char *)this;
    struct B *p;

    if (mResultTimer == 0)
        goto zero;

    *(unsigned short *)(((int)raw + 0x53e4)) =
        *(unsigned short *)(((int)raw + 0x53e4)) - 1;
    if (*(short *)(raw + 0x53e4) > 0)
        return;

    mResultTimer = 0;

    if (mPairsFound >= mTargetPairs) {
        if (mMisses == 0)
            Mg_SetPerfectBanner();
        Mg_SetSharedState(MG_STATE_WIN);
        p = (struct B *)gMgScene;
        if (p != 0) {
            if (p->b4 < 0x270f)
                p->b4 += 1;
            if (p->b4 > p->b8)
                p->b8 = p->b4;
        }
        Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
        Dealer_PlayWin(&mShared);
        return;
    }

    Mg_SetSharedState(MG_STATE_LOSE);
    p = (struct B *)gMgScene;
    if (p != 0) {
        if (p->b4 > 0)
            p->b4 -= 1;
    }
    Dealer_PlayLose(&mShared);
    return;

zero:
    mPromptEnabled = 0;
    mCardTimer = 0x10;
}

void dScMgMemory2_c::RoundReveal()
{
  unsigned short *timer;
  if (mCardTimer != 0)
  {
    timer = &mCardTimer;
    *timer = *timer - 1;
    if ((short)mCardTimer > 0)
    {
      return;
    }
    InitPlayers();
    return;
  }
  {
    long i;
    for (i = 0; i < 0x14; i++)
    {
      mCards[i].state = 2;
    }
  }
  mSubstate = 0;
  mState = 2;
}

void *dScMgMemory2_c::RoundWaitDeal(){
  char *raw = (char *)this;
  int pending = 0;
  int i = 0;
  char* p = raw;
  for (; i < 0x14; ) {
    if (*(unsigned char*)(p + 0x51bb) != 0) {
      if (*(unsigned char*)(p + 0x51bc) != 1) {
        pending++;
        break;
      }
    }
    i++;
    p += 0x18;
  }
  if (pending != 0) return raw;
  p = raw;
  i = 0;
  {
    unsigned char v = 2;
    for (; i < 0x14; ) {
      *(unsigned char*)(p + 0x51bc) = v;
      i++;
      p += 0x18;
    }
  }
  {
    char* base = raw + 0x5000;
    mSubstate = 0;
    mState = 2;
    return base;
  }
}

void dScMgMemory2_c::RoundReadyCards(){
    char *raw = (char *)this;
    int pending = 0;
    int i;
    char *p;
    for(i = 0, p = raw; i < 0x14; i++, p += 0x18){
        if(*(unsigned char*)(p + 0x51bb) != 0){
            if(*(unsigned char*)(p + 0x51bc) == 2)
                *(unsigned char*)(p + 0x51bc) = 1;
        }
    }
    for(i = 0, p = raw; i < 0x14; i++, p += 0x18){
        if(*(unsigned char*)(p + 0x51bb) != 0){
            if(*(unsigned char*)(p + 0x51bc) != 1){ pending++; break; }
        }
    }
    if(pending == 0)
        mSubstate = 8;
}

void dScMgMemory2_c::RoundHideCards()
{
  char *raw = (char *)this;
  int count;
  int i;
  unsigned char *card;
  if (mPreviewTimer != 0)
  {
    i = 0x53ec;
    *((unsigned short *) ((((int) raw) + i))) -= 1;
    return;
  }
  card = (unsigned char *) raw;
  for (i = 0; i < 0x14; i++)
  {
    if (card[0x51bb] != 0)
    {
      if (card[0x51bc] == 4)
      {
        card[0x51bc] = 5;
      }
    }
    card += 0x18;
  }

  count = 0;
  i = 0;
  for (card = (unsigned char *) raw; i < 0x14; i++)
  {
    if (card[0x51bb] != 0)
    {
      unsigned char st = card[0x51bc];
      if ((st != 1) && (st != 5))
      {
        count++;
        break;
      }
    }
    card = card + 0x18;
  }

  if (count == 0)
  {
    mSubstate = 7;
  }
}

void dScMgMemory2_c::RoundShowCards()
{
    char *raw = (char *)this;
    unsigned int off_bb;
    int queued = 0;
    int shown = 0;
    int i;
    int ff = 0xff;
    int three = 3;
    int off_ids = 0x53fd;
    int off_bc = 0x51bc;
    int stride = 0x18;

    for (i = 0; i < 8; i++) {
        unsigned char *slot = (unsigned char *)((raw + i) + off_ids);
        unsigned char id = *slot;
        if (id == 0xff)
            continue;
        {
            int base = (int)raw + id * stride;
            off_bb = 0x51bb;
            if (*(unsigned char *)(base + off_bb) != 0) {
                unsigned char *fp = (unsigned char *)(base + off_bc);
                if (*fp == 1) {
                    *fp = (unsigned char)three;
                    *slot = (unsigned char)ff;
                    shown++;
                }
            } else {
                *slot = (unsigned char)ff;
            }
            queued++;
        }
    }

    if (shown == 1)
        Sound::PlayBank2_2D(0x148);
    else if (shown == 2)
        Sound::PlayBank2_2D(0x145);
    else if (shown >= 3)
        Sound::PlayBank2_2D(0x146);
    if (queued != 0)
        return;
    mSubstate = 6;
    mPreviewTimer = mDifficulty * 20 + 0x50;
}

void dScMgMemory2_c::RoundDealFourth()
{
  int idx;

  if (mReadyCount < 4)
  {
    return;
  }
  idx = mTargetPairs * 2 - 1 - mDealCount;
  mCards[idx].active = 1;
  mDealCount++;
  if (mDealCount >= sMemory2FourthDealsPerDifficulty[mDifficulty]) {
      mSubstate = 5;
  }
}

/* Once three cards are ready, deal the next one and count it. At the
 * difficulty's limit (sMemory2HardDealsPerMode) move to substate 5 on difficulty 1, 4
 * otherwise. */
void dScMgMemory2_c::RoundDealHard()
{
    Work* w = (Work*)this;
    u8 mode;
    if (w->ready < 3)
        return;
    w->slots[w->total * 2 - 1 - w->count].done = 1;
    mDealCount++;
    mode = w->mode;
    if (w->count < sMemory2HardDealsPerMode[mode])
        return;
    if (mode == 1)
        w->state = 5;
    else
        w->state = 4;
}

void dScMgMemory2_c::RoundDealNormal()
{
    int idx;
    if (mReadyCount < 2)
    {
        return;
    }
    idx = mTargetPairs * 2 - 1 - mDealCount;
    mCards[idx].active = 1;
    mDealCount++;
    if (mDealCount >= sMemory2NormalDealsPerDifficulty[mDifficulty]) {
        mSubstate = 3;
    }
}

void dScMgMemory2_c::RoundDealEasy()
{
  int idx;
  if (mReadyCount < 1)
  {
    return;
  }
  idx = mTargetPairs * 2 - 1 - mDealCount;
  mCards[idx].active = 1;
  mDealCount++;
  if (mDealCount >= sMemory2EasyDealsPerDifficulty[mDifficulty]) {
      mSubstate = 2;
  }
}

void dScMgMemory2_c::RoundStart(){
    mReadyCount = 0;
    mDealCount = 0;
    if (mDifficulty == 1)
        Dealer_StartShuffle((char *)&mShared, 4, 3, (int)&mReadyCount);
    else
        Dealer_StartShuffle((char *)&mShared, 4, 4, (int)&mReadyCount);
    mShared.ready = 1;
    mSubstate = 1;
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

void dScMgMemory2_c::ShuffleCards()
{
    char *raw = (char *)this;
    for (int i = 0; i < 11; i++)
        mValueCounts[i] = 0;
    for (int i = 0; i < 8; i++)
        mPreviewCards[i] = 0xff;

    u8 difficulty = mDifficulty;
    if (difficulty == 0) {
        int i = 0;
        char *p = raw;
        for (; i < 16; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
            u8 *entry;
        check0:
            entry = (u8 *)(raw + idx + 0x53f2);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
                goto check0;
            }
        }
        int j = 0;
        char *q = raw;
        for (; j < 16; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    } else if (difficulty == 1) {
        int i = 0;
        char *p = raw;
        for (; i < 18; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
            u8 *entry;
        check1:
            entry = (u8 *)(raw + idx + 0x53f2);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
                goto check1;
            }
        }
        int j = 0;
        char *q = raw;
        for (; j < 18; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    } else {
        int i = 0;
        char *p = raw;
        for (; i < 20; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
            u8 *entry;
        check2:
            entry = (u8 *)(raw + idx + 0x53f2);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
                goto check2;
            }
        }
        int j = 0;
        char *q = raw;
        for (; j < 20; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    }
}

void dScMgMemory2_c::ChoosePreviewCards()
{
    int difficulty = mDifficulty;
    int n = sMemory2PreviewCountPerDifficulty[difficulty];
    int mul = sMemory2PreviewTimePerDifficulty[difficulty];
    int i;
    int zero = 0;
    for (i = 0; i < n; i++) {
        int pick;
        pick = (mul * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
        if (i != 0) {
            int found;
            int j;
            do {
                found = zero;
                for (j = zero; j < i; j++) {
                    if (pick == mPreviewCards[j]) {
                        found++;
                        break;
                    }
                }
                if (found == 0) break;
                pick = (mul * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
            } while (1);
        }
        mPreviewCards[i] = (unsigned char)pick;
    }
}

void dScMgMemory2_c::ResetGame()
{
    char *c = (char *)this;
    int i; char* p; int j;
    p = c;
    i = 0;
    do {
        *(int*)(p + 0x51a8) = 0;
        *(int*)(p + 0x51ac) = 0;
        *(int*)(p + 0x51b0) = 0;
        *(short*)(p + 0x51b4) = 0;
        *(short*)(p + 0x51b6) = 0;
        *(char*)(p + 0x51b8) = 0;
        *(char*)(p + 0x51b9) = 0;
        *(char*)(p + 0x51ba) = 0;
        *(char*)(p + 0x51bb) = 0;
        *(char*)(p + 0x51bc) = 0;
        *(char*)(p + 0x51bd) = 0;
        i++;
        *(char*)(p + 0x51be) = 0;
        p += 0x18;
    } while (i < 0x14);
    p = c;
    j = 0;
    do {
        *(int*)(p + 0x5388) = 0;
        *(int*)(p + 0x538c) = 0;
        *(int*)(p + 0x5390) = 0;
        *(short*)(p + 0x5396) = 0;
        *(short*)(p + 0x5394) = 0;
        *(char*)(p + 0x5398) = 0;
        *(char*)(p + 0x5399) = 0;
        j++;
        *(char*)(p + 0x539a) = 0;
        p += 0x14;
    } while (j < 3);
    p = c + 0x5000;
    *(char*)(p + 0x3d0) = 0;
    *(char*)(p + 0x3d2) = 0;
    *(char*)(p + 0x3d1) = 0;
    *(int*)(p + 0x3d4) = 0;
    *(int*)(p + 0x3d8) = 0;
    *(short*)(c + 0x53e0) = 0;
    *(short*)(c + 0x53e2) = 0;
    *(short*)(c + 0x53e4) = 0;
    *(char*)(p + 0x405) = 0;
    *(char*)(p + 0x406) = 0;
    *(char*)(p + 0x407) = 0;
    *(char*)(p + 0x408) = 0;
    i = 0;
    do {
        p = c + i;
        p += 0x5000;
        *(char*)(p + 0x3ee) = 0;
        i++;
        *(char*)(p + 0x3f0) = 0;
    } while (i < 2);
    j = 0;
    do {
        p = (c + j) + 0x5000;
        j++;
        *(char*)(p + 0x3f2) = 0;
    } while (j < 0xb);
    c += 0x5000;
    *(char*)(c + 0x40b) = 1;
    *(char*)(c + 0x40c) = 0;
}

void dScMgMemory2_c::StateExit()
{
    UpdateCards();
    if (mCardTimer)
        mCardTimer -= 1;
}

void dScMgMemory2_c::StateResult(){
    (this->*sMemory2ResultTable[mSubstate])();
}

void dScMgMemory2_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

void dScMgMemory2_c::StatePlay() {
    int j = mSubstate;
    (this->*sMemory2RoundTable[j])();
    UpdateCards();
}

void dScMgMemory2_c::StateSetup(){
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    Dealer_PlayIdle((char *)&mShared);
    mState = 1;
}

void dScMgMemory2_c::SetupDifficulty() {
    mDifficulty = 0;
    mTargetPairs = 8;
    int x = mHudScore;
    if (x >= 0xa) {
        mDifficulty = 2;
        mTargetPairs = 0xa;
        return;
    }
    if (x >= 5) {
        mDifficulty = 1;
        mTargetPairs = 9;
    }
}

// @symbol _ZN14dScMgMemory2_c10OnDealDoneEv
void dScMgMemory2_c::OnDealDone()
{
    MgCoins_PlaceBet(mMaxMisses);
}

// @symbol _ZN14dScMgMemory2_c11CanEndStateEi
int dScMgMemory2_c::CanEndState(int /* mode */)
{
    if (mState == 3 && mSubstate == 0) {
        if (Dealer_IsIdle((int *)&mShared) == 0) return 0;
        mSubstate = 1;
        FreeGfxSlotsById(0x1d);
    }
    return mState == 4;
}

// @symbol _ZN14dScMgMemory2_c10StartRoundEi
/* Clear the board, go back to StateSetup and reload the miss limit. */
void dScMgMemory2_c::StartRound(int /* arg */)
{
    dScMgBase_c *o;
    int v;
    ResetGame();
    mState = 0;
    o = gMgScene;
    v = 0;
    if (o != 0) v = o->mCredits;
    if (v >= 5) v = 5;
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mMaxMisses = (unsigned char)v;
}

// @symbol _ZN14dScMgMemory2_c6RenderEv
/* Vtable slot 9: two unnamed scene passes, then the message, cursor, cards
 * and the shared dMeter_c. */
s32 dScMgMemory2_c::Render()
{
    /* pad_4660 is the minigame camera, in dScMgSingle3DBase_c's padding. */
    Camera_InitDealerView((char *)pad_4660);
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    MgCoins_Draw();
    DrawMessage();
    DrawCursor();
    DrawCards();
    Dealer_Draw((char *)&mShared);
    return 1;
}

// @symbol _ZN14dScMgMemory2_c8BehaviorEv
/* Vtable slot 6: run the current state, then the two per-frame calls every
 * minigame in this family makes. __sinit_ov006_021314e4 fills the state table
 * with StateSetup, StatePlay, StateJudge, StateResult and StateExit. */
s32 dScMgMemory2_c::Behavior()
{
    (this->*sMemory2StateTable[mState])();
    MgCoins_Update();
    Dealer_Update((char *)&mShared);
    return 1;
}

// @symbol _ZN14dScMgMemory2_c13InitResourcesEv
/* Vtable slot 0: load the language's card art into both OBJ banks with the
 * shared palette, reset the board, then set up the timer and the dMeter_c.
 * LoadCard2SubBg3 is dScMgSingle3DBase_c's unnamed slot 26, so it stays a
 * plain call. */
s32 dScMgMemory2_c::InitResources()
{
    void *art;
    void *palette;

    data_0209d45c = 0x11;
    LoadCard2SubBg3();
    art = LoadFile(sMemory2CardNcgByLanguage[GetGameLanguage()]);
    palette = LoadFile(ASSET_D_2D_MG_CARD_NCL_BIN);
    DecompressLZ16(art, (void *)0x6400000);
    GX::LoadOBJPltt(palette, 0, 0x100);
    DecompressLZ16(art, (void *)0x6600000);
    GXS::LoadOBJPltt(palette, 0, 0x100);
    Deallocate(art);
    Deallocate(palette);
    data_0209d454 = 0x18;
    ResetGame();
    /* 0x4660..0x471c is dScMgSingle3DBase_c's minigame camera. */
    Camera_InitDealerView((char *)this + 0x4660);
    if (Dealer_Load(&mShared) == 0) return 0;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    mHudScore = Mg_GetSavedRecord1();
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mMaxMisses = 5;
    Mg_SetTopScreenYOffset(0x20);
    Mg_PlaceBannersOnTopScreen();
    return 1;
}

// @symbol dScMgMemory2_c_classInit
extern "C" void *dScMgMemory2_c_classInit()
{
    return new dScMgMemory2_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_021314e4 shard).
 * The sprite-pointer table plus the five PMF state tables plain-copied
 * from the ROM constants, in retail copy order: the permuted sprite
 * pointers, the five main states, the ten round states, the four result
 * states, the seven card states, then the three player states. */
extern "C" int sMgCardSprites[];
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_StateSetup;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_StatePlay;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_StateJudge;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_StateResult;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_StateExit;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundStart;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundDealEasy;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundDealNormal;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundDealHard;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundDealFourth;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundShowCards;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundHideCards;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundReadyCards;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundWaitDeal;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_RoundReveal;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_ResultWait;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_ResultReward;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_ResultTurnCards;
extern "C" dScMgMemory2_cState pmf_dScMgMemory2_c_ResultFinish;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardMove;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardIdle;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardSelect;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardFlipUp;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardWait;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardFlipDown;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_CardFlyAway;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_PlayerDrop;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_PlayerMove;
extern "C" dScMgMemory2_cCardState pmf_dScMgMemory2_c_PlayerWait;

void *sMemory2CardSprites[23] = {
    (void *)sMgCardSprites[0],  (void *)sMgCardSprites[1],  (void *)sMgCardSprites[2],
    (void *)sMgCardSprites[11], (void *)sMgCardSprites[12], (void *)sMgCardSprites[13],
    (void *)sMgCardSprites[14], (void *)sMgCardSprites[3],  (void *)sMgCardSprites[4],
    (void *)sMgCardSprites[5],  (void *)sMgCardSprites[6],  (void *)sMgCardSprites[9],
    (void *)sMgCardSprites[10], (void *)sMgCardSprites[7],  (void *)sMgCardSprites[8],
    (void *)sMgCardSprites[15], (void *)sMgCardSprites[16], (void *)sMgCardSprites[17],
    (void *)sMgCardSprites[18], (void *)sMgCardSprites[19], (void *)sMgCardSprites[20],
    (void *)sMgCardSprites[21], (void *)sMgCardSprites[22],
};
dScMgMemory2_cState sMemory2StateTable[5] = {
    pmf_dScMgMemory2_c_StateSetup,  pmf_dScMgMemory2_c_StatePlay, pmf_dScMgMemory2_c_StateJudge,
    pmf_dScMgMemory2_c_StateResult, pmf_dScMgMemory2_c_StateExit,
};
dScMgMemory2_cState sMemory2RoundTable[10] = {
    pmf_dScMgMemory2_c_RoundStart,      pmf_dScMgMemory2_c_RoundDealEasy,
    pmf_dScMgMemory2_c_RoundDealNormal, pmf_dScMgMemory2_c_RoundDealHard,
    pmf_dScMgMemory2_c_RoundDealFourth, pmf_dScMgMemory2_c_RoundShowCards,
    pmf_dScMgMemory2_c_RoundHideCards,  pmf_dScMgMemory2_c_RoundReadyCards,
    pmf_dScMgMemory2_c_RoundWaitDeal,   pmf_dScMgMemory2_c_RoundReveal,
};
dScMgMemory2_cState sMemory2ResultTable[4] = {
    pmf_dScMgMemory2_c_ResultWait,
    pmf_dScMgMemory2_c_ResultReward,
    pmf_dScMgMemory2_c_ResultTurnCards,
    pmf_dScMgMemory2_c_ResultFinish,
};
dScMgMemory2_cCardState sMemory2CardTable[7] = {
    pmf_dScMgMemory2_c_CardMove,    pmf_dScMgMemory2_c_CardIdle, pmf_dScMgMemory2_c_CardSelect,
    pmf_dScMgMemory2_c_CardFlipUp,  pmf_dScMgMemory2_c_CardWait, pmf_dScMgMemory2_c_CardFlipDown,
    pmf_dScMgMemory2_c_CardFlyAway,
};
dScMgMemory2_cCardState sMemory2PlayerTable[3] = {
    pmf_dScMgMemory2_c_PlayerDrop,
    pmf_dScMgMemory2_c_PlayerMove,
    pmf_dScMgMemory2_c_PlayerWait,
};
