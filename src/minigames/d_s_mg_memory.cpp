//cpp
/* ov006/dScMgMemory_c -- the Memory Match minigame, 47 functions
 * (.text 0x020f3834..0x020f5564).
 *
 * Functions run in ROM order here, not reversed: `#pragma defer_codegen off`
 * below makes the compiler emit each function as it reads it. That is also
 * what lets #pragma push and pop scope DrawCards' opt_propagation and
 * CardFlyAway's opt_lifetimes to those two functions; with deferred emission
 * the last setting wins for the whole file. Removing any of it changes the
 * code.
 *
 * Still raw: the func_ov004 and func_ov006 helpers and most data_ globals
 * are unnamed in symbols.txt, unk_0a8 and unk_0b8 are unnamed in
 * dScMgBase_c.h, and the factory builds the object by hand because the
 * class has no constructor declared yet.
 */

#include "dScMgMemory_c.h"
#include "common.h"
#include "types.h"
#include "decl_common.h"



namespace Sound { void PlayBank2_2D(unsigned int); }

typedef void (dScMgMemory_c::*dScMgMemory_cState)();
typedef void (dScMgMemory_c::*dScMgMemory_cCardState)(int);
extern "C" dScMgMemory_cState data_ov006_021422bc[];
extern "C" dScMgMemory_cState data_ov006_021422dc[];
extern dScMgMemory_cState data_ov006_02142304[];
extern dScMgMemory_cCardState data_ov006_02142334[];

/* Most card and player loops walk a char pointer from `this` in 0x18- and
 * 0x14-byte steps; walking a typed pointer instead changes the code. These
 * name the fields at each step. */
#define CARD_AT(p)   ((dMgMemoryCard_c *)((p) + 0x51a8))
#define PLAYER_AT(p) ((dMgMemoryPlayer_c *)((p) + 0x52c8))

namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern "C" {
extern void func_ov004_020b1e34(void* c, int a, int b, int d);
extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);
/* cstd::atan2 takes Fix12<int> by value, which mwccarm passes differently
 * from these plain ints, so it keeps its mangled name. */
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short data_02082214[];
void func_02012790(int a);
void func_ov004_020b5dd4(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 data_ov006_0213d168[];
extern void *data_ov006_0214236c[];
extern unsigned short data_ov006_0213d0a8[];
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void func_ov004_020ad79c(int a, int b);
extern s16 data_02082214[];
extern u16* data_ov006_0213d09c[];
extern int func_ov004_020b56c8(int);
extern short data_ov004_020bf9e4;
extern void func_ov004_020b67f8(void);
extern void func_ov004_020b0a54(int c);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c0c80(void *c);
extern void func_ov006_020c1604(char *c, int unused, short count, int ready);
extern int data_0209d4b8;
extern int RandomIntInternal(int *seed);
extern void FreeGfxSlotsById(int x);
void func_ov006_020c0aa8(char *p);
void func_ov004_020b1bc8(char *c, int a, int b, int d);
void func_ov006_020c1804(char *p);
void func_ov004_020b65e4(void);
int func_ov006_020c19d0(void *p);
void func_ov006_0210a534(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int func_ov006_020c1a88(void *c);
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
void func_ov004_020b66d4(void);
int func_ov006_020c1718(int *p);
void func_ov004_020b04d0(int v);
void func_ov004_020b682c(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN11dScMgBase_cC2Ev(void *scene);
void _ZN8Particle10SysTrackerC1Ev(void *tracker);
void func_ov006_020c1d80(void *sharedState);
extern void *_ZTV19dScMgSingle3DBase_c[];
}

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
  func_ov004_020b1e34(this, 0xe0, 0x14, 1);
}

void dScMgMemory_c::DrawCursor() {
    if (mCursor.enabled == 0) return;
    func_ov004_020b0d8c(this, 0xe0, 0xa0);
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

void dScMgMemory_c::PlayerMove(int i)
{
    int targetX = i * 16 + 12;
    int dx, dy, idx;

    dx = targetX - (mPlayers[i].x >> 12);
    dy = -44 - (mPlayers[i].y >> 12);

    mPlayers[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    mPlayers[i].speed += 0x200;

    idx = (u16)mPlayers[i].angle >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = mPlayers[i].speed;
        mPlayers[i].x += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    idx = (u16)mPlayers[i].angle >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = mPlayers[i].speed;
        mPlayers[i].y += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    dx = targetX - (mPlayers[i].x >> 12);
    dy = -44 - (mPlayers[i].y >> 12);

    if (dx < -3) return;
    if (dx > 3) return;
    if (dy < -3) return;
    if (dy > 3) return;

    mPlayers[i].x = targetX << 12;
    mPlayers[i].y = -0x2c000;
    mPlayers[i].state = 2;
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
    char *entry;

    if (data_ov004_020beb68 != 0)
        count = *(int *)((char *)data_ov004_020beb68 + 0xa8);
    else
        count = 0;
    if (count >= 3)
        count = 3;

    entry = (char *)this;
    i = 0;
    mMaxMisses = (unsigned char)count;
    if (count <= 0)
        return;

    stagger = 0;
    do {
        PLAYER_AT(entry)->active = 1;
        PLAYER_AT(entry)->state = 0;
        PLAYER_AT(entry)->delay = stagger;
        i++;
        entry += 0x14;
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
    /* Read unk_0a8 through WordAt: as a plain member the multiply-add takes
       its registers the other way round. */
    func_ov004_020ad79c(*WordAt((char *)&unk_0a8) + (3 - mMisses) * 6, mHudScore + 1);
  }
  if (mMisses >= mMaxMisses)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    func_ov004_020ad79c(unk_0a8, mHudScore - 1);
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
        mPairsFound++;
        mSelectedCount = 0;
    } else {
        func_02012790(0xe);
        mMisses++;
        func_ov004_020b5dd4();
        Sound::PlayBank2_2D(0x13e);
        if (mMisses < mMaxMisses) {
            *firstState = 5;
            *secondState = 5;
            mSelectedCount = 0;
        }
    }
}

#pragma push
#pragma opt_propagation off
void dScMgMemory_c::DrawCards()
{
    char *raw = (char*)this;
    int i;
    int spriteIndex, dim;
    char *p;

    /* data_ov006_0213d168 is indexed as [value][frame], five frames per value. */
    p = raw;
    i = 0;
    do {
        if (CARD_AT(p)->visible != 0) {
            dim = 0;
            if (mState != 2)
                dim = 1;
            spriteIndex = data_ov006_0213d168[CARD_AT(p)->value * 5
                                              + CARD_AT(p)->frame];
            Hud_RenderSprite(data_ov006_0214236c[spriteIndex],
                             CARD_AT(p)->x >> 12,
                             CARD_AT(p)->y >> 12, -1, dim);
        }
        i += 1;
        p += 0x18;
    } while (i < 12);
}

#pragma pop

void dScMgMemory_c::UpdateCards() {
    dScMgMemory_c* self = this;
    int i;
    char* card = (char*)self;
    for (i = 0; i < 0xc; i++) {
        if (CARD_AT(card)->active) {
            (self->*data_ov006_02142334[CARD_AT(card)->state])(i);
        }
        card += 0x18;
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
                        mCards[i + j].state = 6;
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
                        mCards[i + n].state = 6;
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
    if (timer < (data_ov006_0213d0a8[mCards[i].frame] & 0xff))
        return;
    mCards[i].animTimer = 0;
    mCards[i].frame -= 1;
    if (mCards[i].frame == 0)
        mCards[i].state = 2;
}

void dScMgMemory_c::CardWait(int /* card */)
{
}

void dScMgMemory_c::CardFlipUp(int idx){
  unsigned short* timer = &mCards[idx].animTimer;
  unsigned char* curFrame = &mCards[idx].frame;
  *timer = *timer + 1;
  if (*timer < (data_ov006_0213d0a8[*curFrame] & 0xff)) return;
  *timer = 0;
  *curFrame = *curFrame + 1;
  if (*curFrame > 4) {
    *curFrame = 4;
    mCards[idx].state = 4;
  }
}

/* Keep the separate cardY base: reading y through CARD_AT here changes the
 * code. */
void dScMgMemory_c::CardSelect(int idx)
{
  char *raw = (char*)this;
  unsigned int count = mSelectedCount;
  unsigned int sample;
  int touched;
  char *cardY;
  int offset;
  int dx;
  int dy;
  if (count >= 2)
  {
    return;
  }
  sample = data_020a0e40;
  touched = 0;
  if (data_020a0de8[data_020a0e40 * 4] != 0)
  {
    if (data_020a0de9[data_020a0e40 * 4] != 0)
    {
      touched = 1;
    }
  }
  if (touched == 0)
  {
    return;
  }
  offset = idx * 0x18;
  dx = data_020a0dea[sample * 4] - (((dMgMemoryCard_c *)(raw + 0x51a8 + offset))->x >> 12);
  cardY = raw + 0x51ac;
  dy = data_020a0deb[sample * 4] - (*(int *)(cardY + offset) >> 12);
  if (dx < (-0x10))
  {
    return;
  }
  if (dx > 0x10)
  {
    return;
  }
  if (dy < (-0x16))
  {
    return;
  }
  if (dy > 0x16)
  {
    return;
  }
  mSelectedValues[count] = mCards[idx].value;
  mSelectedCards[mSelectedCount] = (u8) idx;
  {
    u8 *pc = &mSelectedCount;
    *pc = (*pc) + 1;
  }
  mCards[idx].state = 3;
  func_02012718(0x143, CARD_AT(raw + offset)->x);
  if (mInputSeen != 0)
  {
    return;
  }
  {
    u8 *pd = &mInputSeen;
    *pd = (*pd) + 1;
  }
  func_ov004_020ad79c(unk_0a8, mHudScore);
}

void dScMgMemory_c::CardIdle(int /* card */)
{
}

void dScMgMemory_c::CardMove(int i)
{
    int i2 = i * 2;
    u16* row = data_ov006_0213d09c[mDifficulty];
    int i18 = i * 0x18;
    int dx, dy, a, b;
    a = row[i2];
    dx = a - (mCards[i].x >> 12);
    b = row[i2 + 1];
    dy = b - (mCards[i].y >> 12);

    mCards[i].angle = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 tv = data_02082214[(((u16)mCards[i].angle >> 4) << 1) + 1];
        mCards[i].x += (int)(((s64)tv * mCards[i].speed + 0x800) >> 0xc);
    }
    {
        s16 tv = data_02082214[((u16)mCards[i].angle >> 4) << 1];
        mCards[i].y += (int)(((s64)tv * mCards[i].speed + 0x800) >> 0xc);
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
    mCards[i].state = 1;
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

    i = data_020a0e40;
    touched = 0;
    if (data_020a0de8[i * 4] != 0)
    {
        if (data_020a0de9[i * 4] != 0)
        {
            touched = 1;
        }
    }
    if (touched == 0)
    {
        return;
    }

    func_02012790(0x62);

    if (unk_0a8 == 0)
    {
        if (mDifficulty == 1)
        {
            for (j = 0; j < 2; j++)
            {
                for (k = 0; k < 5; k++)
                {
                    if (mCards[j * 5 + k + 2].active != 0)
                    {
                        mCards[j * 5 + k + 2].state = 6;
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
                        mCards[j2 * 4 + k2].state = 6;
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
    char *raw = (char*)this;
    int flipped;
    int i;
    char *p;
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
    i = 0;
    p = raw;
    for (; i < 0xc; i++) {
        if (CARD_AT(p)->active != 0) {
            if (CARD_AT(p)->state == 2) {
                CARD_AT(p)->state = 3;
                flipped++;
            }
        }
        p += 0x18;
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
  if (mCardTimer != 0)
  {
    unsigned short *ptr = &mCardTimer;
    *ptr = (*ptr) - 1;
    if (mCardTimer != 0)
    {
      return;
    }
    {
      unsigned char b = mPairsFound;
      if (b < mTargetPairs)
      {
        return;
      }
    }
    {
      int val = 6;
      val = (val * (3 - mMisses)) & 0xFFFFFFFFFFFFFFFFu;
      func_ov004_020b56c8(val);
      return;
    }
  }
  if (data_ov004_020bf9e4 == 1)
  {
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
                    func_ov004_020b67f8();
                func_ov004_020b0a54(4);
                if (mHudScore < 0x270f) {
                    int *score = &mHudScore;
                    *score += 1;
                }
                if (mHudScore > unk_0b8)
                    unk_0b8 = mHudScore;
                func_ov004_020adb1c(data_ov004_020beb68 ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0);
                func_ov006_020c0c80(&mShared);
            } else {
                func_ov004_020b0a54(5);
                if (mHudScore > 0) {
                    int *score = &mHudScore;
                    *score -= 1;
                }
                func_ov006_020c0d68(&mShared);
            }
        }
    } else {
        mPromptEnabled = 0;
        mCardTimer = 0x10;
    }
}

void dScMgMemory_c::RoundReveal()
{
    char *raw = (char*)this;
    int i;
    char *e;
    unsigned short *t;
    if (mCardTimer != 0) {
        t = &mCardTimer;
        *t = *t - 1;
        if ((short)mCardTimer > 0)
            return;
        InitPlayers();
        return;
    }
    e = raw;
    for (i = 0; i < 0xc; i++) {
        CARD_AT(e)->state = 2;
        e += 0x18;
    }
    mSubstate = 0;
    mState = 2;
}

/* Called through the void state table, so nothing reads the result, but the
 * ROM leaves this + 0x5000 in r0 and the void* return reproduces that. */
void* dScMgMemory_c::RoundWaitDeal(){
  char* raw = (char*)this;
  int waiting = 0;
  int i = 0;
  char* p = raw;
  for (; i < 0xc; ) {
    if (CARD_AT(p)->active != 0) {
      if (CARD_AT(p)->state != 1) {
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
    unsigned char v = 2;
    for (; i < 0xc; ) {
      CARD_AT(p)->state = v;
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

void dScMgMemory_c::RoundDealHard(){
  short n = mReadyCount;
  short k;
  short* q;
  if (n < 3) return;
  k = mDealCount;
  mCards[0xb - k].active = 1;
  q = &mDealCount;
  *q = *q + 1;
  if (mDealCount >= 0xc) mSubstate = 4;
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

void dScMgMemory_c::RoundDealEasy(){
  short n = mReadyCount;
  short k;
  short* q;
  if (n < 1) return;
  k = mDealCount;
  mCards[0xb - k].active = 1;
  q = &mDealCount;
  *q = *q + 1;
  if (mDifficulty == 1) {
    if (mDealCount >= 5) mSubstate = 2;
  } else {
    if (mDealCount >= 4) mSubstate = 2;
  }
}

void dScMgMemory_c::RoundStart(){
    mReadyCount = 0;
    mDealCount = 0;
    if (mDifficulty >= 2)
        func_ov006_020c1604((char *)&mShared, 4, 3, (int)&mReadyCount);
    else
        func_ov006_020c1604((char *)&mShared, 4, 2, (int)&mReadyCount);
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
    char *raw = (char*)this;
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
                count = (u8 *)(raw + slot + 0x5330);
                if (*count < 2) {
                    CARD_AT(p)->value = slot;
                    *count += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            }
            p += 0x18;
        }
        p = raw + 0x60;
        for (i = 4; i < 12; i++) {
            CARD_AT(p)->x = 0x80000;
            CARD_AT(p)->y = -0x80000;
            CARD_AT(p)->speed = 0x8000;
            CARD_AT(p)->visible = 1;
            CARD_AT(p)->state = 0;
            p += 0x18;
        }
    } else if (mDifficulty == 1) {
        p = raw + 0x30;
        for (i = 2; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            for (;;) {
                count = (u8 *)(raw + slot + 0x5330);
                if (*count < 2) {
                    CARD_AT(p)->value = slot;
                    *count += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            }
            p += 0x18;
        }
        p = raw + 0x30;
        for (i = 2; i < 12; i++) {
            CARD_AT(p)->x = 0x80000;
            CARD_AT(p)->y = -0x80000;
            CARD_AT(p)->speed = 0x8000;
            CARD_AT(p)->visible = 1;
            CARD_AT(p)->state = 0;
            p += 0x18;
        }
    } else {
        p = raw;
        for (i = 0; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            for (;;) {
                count = (u8 *)(raw + slot + 0x5330);
                if (*count < 2) {
                    CARD_AT(p)->value = slot;
                    *count += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            }
            p += 0x18;
        }
        p = raw;
        for (i = 0; i < 12; i++) {
            CARD_AT(p)->x = 0x80000;
            CARD_AT(p)->y = -0x80000;
            CARD_AT(p)->speed = 0x8000;
            CARD_AT(p)->visible = 1;
            CARD_AT(p)->state = 0;
            p += 0x18;
        }
    }
}

void dScMgMemory_c::ResetGame()
{
    char* raw = (char*)this;
    int i; char* p; int j;
    p = raw;
    i = 0;
    do {
        CARD_AT(p)->x = 0;
        CARD_AT(p)->y = 0;
        CARD_AT(p)->speed = 0;
        CARD_AT(p)->angle = 0;
        CARD_AT(p)->animTimer = 0;
        CARD_AT(p)->value = 0;
        CARD_AT(p)->unk_11 = 0;
        CARD_AT(p)->visible = 0;
        CARD_AT(p)->active = 0;
        CARD_AT(p)->state = 0;
        CARD_AT(p)->frame = 0;
        i++;
        CARD_AT(p)->flyAwayStarted = 0;
        p += 0x18;
    } while (i < 12);
    p = raw;
    j = 0;
    do {
        PLAYER_AT(p)->x = 0;
        PLAYER_AT(p)->y = 0;
        PLAYER_AT(p)->speed = 0;
        PLAYER_AT(p)->delay = 0;
        PLAYER_AT(p)->angle = 0;
        PLAYER_AT(p)->active = 0;
        PLAYER_AT(p)->unk_11 = 0;
        j++;
        PLAYER_AT(p)->state = 0;
        p += 0x14;
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
  (this->*data_ov006_021422bc[mSubstate])();
}

void dScMgMemory_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

void dScMgMemory_c::StatePlay() {
    (this->*data_ov006_02142304[mSubstate])();
    UpdateCards();
}

void dScMgMemory_c::StateSetup(){
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    func_ov006_020c1764((char *)&mShared);
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

// @symbol _ZN13dScMgMemory_c15OnGroundPoundedEv
void dScMgMemory_c::OnGroundPounded()
{
    func_ov004_020b63a0(mMaxMisses);
}

// @symbol _ZN13dScMgMemory_c13OnTurnIntoEggEi
int dScMgMemory_c::OnTurnIntoEgg(int /* mode */)
{
    if (mState == 3 && mSubstate == 0) {
        if (func_ov006_020c1718((int*)&mShared) == 0) return 0;
        mSubstate = 1;
        FreeGfxSlotsById(0x1d);
    }
    return mState == 4;
}

// @symbol _ZN13dScMgMemory_c13OnYoshiTryEatEi
/* Slot 18: restarts the game, with up to three misses allowed from the meter. */

void dScMgMemory_c::OnYoshiTryEat(int /* arg */)
{
    char *meter;
    int lives;
    ResetGame();
    mState = 0;
    meter = (char *)data_ov004_020beb68;
    lives = 0;
    if (meter != 0) lives = *(int *)(meter + 0xa8);
    if (lives >= 3) lives = 3;
    ((void (*)(void *))func_ov004_020b66d4)(meter);
    data_ov004_020bc7d4 = 1;
    mMaxMisses = (unsigned char)lives;
}

// @symbol _ZN13dScMgMemory_c6RenderEv
/* Slot 9. */
s32 dScMgMemory_c::Render()
{
    func_ov006_020c0aa8((char *)pad_4660);
    func_ov004_020b1bc8((char *)this, 0xc, 0xc, 0);
    func_ov004_020b6430();
    DrawMessage();
    DrawCursor();
    DrawCards();
    func_ov006_020c1804((char *)&mShared);
    return 1;
}

// @symbol _ZN13dScMgMemory_c8BehaviorEv
/* Slot 6: runs the current state from the table, indexed by mState. */
s32 dScMgMemory_c::Behavior()
{
    (this->*data_ov006_021422dc[mState])();
    func_ov004_020b65e4();
    func_ov006_020c19d0(&mShared);
    return 1;
}

// @symbol _ZN13dScMgMemory_c13InitResourcesEv
/* Slot 0. */
s32 dScMgMemory_c::InitResources()
{
    void *tiles;
    void *palette;

    data_0209d45c = 0x11;
    func_ov006_0210a534();
    tiles = LoadFile(data_ov006_0213d0c4[GetGameLanguage()]);
    palette = LoadFile(0xbb);
    DecompressLZ16(tiles, (void *)0x6400000);
    GX::LoadOBJPltt(palette, 0, 0x100);
    DecompressLZ16(tiles, (void *)0x6600000);
    GXS::LoadOBJPltt(palette, 0, 0x100);
    Deallocate(tiles);
    Deallocate(palette);
    data_0209d454 = 0x18;
    ResetGame();
    func_ov006_020c0aa8((char *)pad_4660);
    if (func_ov006_020c1a88(&mShared) == 0)
        return 0;
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    mHudScore = func_ov004_020ad878();
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    mMaxMisses = 3;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    return 1;
}

// @symbol dScMgMemory_c_classInit
/* Builds the scene by hand (operator new, base constructor, vtables,
 * mShared) because the class has no constructor declared yet. */
extern "C" void *dScMgMemory_c_classInit()
{
    char *scene = (char *)_ZN7fBase_cnwEj(sizeof(dScMgMemory_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(void **)scene = &_ZTV13dScMgMemory_c[2];
        func_ov006_020c1d80(scene + 0x4f38);
    }
    return scene;
}

