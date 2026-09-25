//cpp
/* ov006/dScMgBSC_c -- the Lucky Stars card minigame, 20 functions.
 *
 * `#pragma opt_strength_reduction off` is file-wide and needed:
 * StateDealCards and StateRevealCards depend on it, a bracket around them
 * does not bind (last setting wins), and the other functions do not care.
 * Functions run in REVERSE of ROM order; do not reorder.
 *
 * Still raw: the func_ov004 and func_ov006 helpers and most data_ globals
 * are unnamed in symbols.txt, and unk_0a8 and unk_0b8 are unnamed in the
 * header. The factory builds the object by hand because the class has no
 * constructor declared yet.
 */
#pragma opt_strength_reduction off

#include "dScMgBSC_c.h"
#include "dScMgCard_c.h"
#include "types.h"
#include "decl_common.h"

/* The state table: one pointer-to-member per state, indexed by mState.
 * Declared outside extern "C" it still links by its plain name. */
typedef void (dScMgBSC_c::*StateHandler)();
extern StateHandler data_ov006_02142f94[];

extern "C" {
extern short data_ov004_020bf9e4;
extern int Math_Function_0203b14c(int *ptr, int target, int rate, int limit, int step);
extern void func_ov004_020b0a38(void);
extern int func_ov006_020c1718(int* r0);
extern void func_ov004_020b56c8(int x);
extern void func_ov004_020b5ed0(void);
extern int data_ov004_020bfa18;
extern void func_ov006_020c0c80(void* c);
/* decl_common.h already declares this with a void pointer, so the
 * definition's char pointer cannot be repeated here. */
extern void func_ov006_020c0d68(void* c);
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b0a54(int v);
extern void func_ov004_020ad79c(int a, int b);
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern int data_ov006_0213fd44[];
extern int data_ov004_020bf9ec;
extern int RandomIntInternal(int* seed);
extern void func_ov006_020c1604(char* c, int unused, short a2, int a3);
extern int data_0209e650;
extern "C" void FreeGfxSlotsById(int arg);
extern "C" void func_ov004_020b66d4(void);
void func_ov006_020c0aa8(void *camera);
void Hud_RenderSprite(void *sprite, int x, int y, int a3, int a4);
void func_ov004_020b1bc8(void *scene, int a1, int a2, int a3);
void func_ov004_020b1e34(void *scene, int a1, int a2, int a3);
void func_ov006_020c1804(void *table);
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(char *c);
extern unsigned char data_0209d45c[];
extern int data_0208ee44[];
extern void func_ov006_0210a534(void);
extern s32 GetGameLanguage(void);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern int LoadFile(int handle);
extern int func_ov006_020c1a88(char *c);
void _ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
void __cxa_vec_ctor(void*, int, int, void*, void*);
void func_0203d738(void);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV10dScMgBSC_c[];
}

/* Real C++ declarations for the engine calls below; the namespaces and the
 * reference parameter are what give the ROM's mangled names. */
namespace Sound {
void PlayBank2_2D(u32 id);
}

namespace GXS {
void LoadOBJPltt(const void *plt, u32 base, u32 size);
}

/* Moves value toward target by step; nonzero once it gets there. */
int ApproachLinear(int& value, int target, int step);

// @symbol dScMgBSC_c_classInit
extern "C" void* dScMgBSC_c_classInit()
{
    char* scene = (char*)_ZN7fBase_cnwEj(sizeof(dScMgBSC_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int*)scene = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(int*)scene = (int)&_ZTV10dScMgBSC_c[2];
        func_ov006_020c1d80(scene + 0x4f38);
        __cxa_vec_ctor(scene + 0x51a8, 2, 8,
                     (void*)func_0203d738,
                     (void*)NullDestructor_0203d47c);
    }
    return scene;
}

// @symbol _ZN10dScMgBSC_c13InitResourcesEv
/* Slot 0, and the key function: the destructor is inline in the header, so
 * this is the first virtual defined out of line, and defining it here emits
 * the vtable and the D1 and D0 pair. The last call dispatches virtually
 * (mwcc does not devirtualize it). pad_4660 is the head of the camera
 * block, untyped in the header. */
s32 dScMgBSC_c::InitResources()
{
    int fh;
    data_0209d45c[0] = 0x11;
    func_ov006_0210a534();
    LoadCompressedFileAt(data_ov006_0213fe78[GetGameLanguage()], (void *)0x6600000);
    fh = LoadFile(0xb3);
    GXS::LoadOBJPltt((void *)fh, 0, 0x100);
    Deallocate((void *)fh);
    func_ov006_020c0aa8(pad_4660);
    if (func_ov006_020c1a88((char *)&mShared) == 0)
        return 0;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    data_0208ee44[0] = 1;
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    this->OnYoshiTryEat(3);
    return 1;
}

// @symbol _ZN10dScMgBSC_c8BehaviorEv
/* Slot 6: runs the current state from the table. */
s32 dScMgBSC_c::Behavior()
{
    (this->*data_ov006_02142f94[mState])();
    func_ov004_020b65e4();
    func_ov006_020c19d0((char *)&mShared);
    return 1;
}

// @symbol _ZN10dScMgBSC_c6RenderEv
/* Slot 9. Card positions are 20.12 fixed point. Sprite frames 0 to 2 are
 * the flip; from frame 3 the card's value picks the face. */
s32 dScMgBSC_c::Render()
{
    int i;

    func_ov006_020c0aa8(pad_4660);
    if (mState >= 1) {
        for (i = 0; i < 2; i++) {
            if (mCardFrame[i] < 3) {
                Hud_RenderSprite(data_ov006_0213fe8c[mCardFrame[i]],
                                    mCardPos[i].x >> 12,
                                    mCardPos[i].y >> 12, -1, -1);
            } else {
                Hud_RenderSprite(
                    data_ov006_0213fe8c[mCardFrame[i] + mCardValue[i] * 2],
                    mCardPos[i].x >> 12,
                    mCardPos[i].y >> 12, -1, -1);
            }
        }
    }
    func_ov004_020b6430();
    func_ov004_020b1bc8(this, 0xc, 0xc, 0);
    func_ov004_020b1e34(this, 0xe0, 0x14, 1);
    func_ov006_020c1804(&mShared);
    return 1;
}

// @symbol _ZN10dScMgBSC_c13OnYoshiTryEatEi
/* Slot 18. Mode 3 reads the meter score into mHudScore and posts it; every
   mode then restarts the state machine. The redundant `!= 4 && != 5`
   compares are in the ROM and must stay. */
void dScMgBSC_c::OnYoshiTryEat(int mode)
{
    if (mode != 4 && mode != 5 && mode == 3) {
        mHudScore = func_ov004_020ad878();
        func_ov004_020adb1c(mHudScore);
    }
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    mState = 0;
}

// @symbol _ZN10dScMgBSC_c13OnTurnIntoEggEi
/* Slot 19. In state 13, once the table's reaction has finished, mode 4
   moves to state 9 and mode 5 to state 10. When the round has settled
   (state 12, with data_ov004_020bf9e4 set) the final scores go to the
   meter. */
s32 dScMgBSC_c::OnTurnIntoEgg(int mode)
{
    int state;
    short gate;
    if (mState == 0xd) {
        if (func_ov006_020c1718((int *)&mShared) != 0) {
            if (mode == 4) {
                FreeGfxSlotsById(0x1d);
                mState = 9;
            } else if (mode == 5) {
                FreeGfxSlotsById(0x1d);
                mState = 0xa;
            }
        } else {
            return 0;
        }
    }
    state = mState;
    if (state >= 0xc) {
        gate = data_ov004_020bf9e4;
        if (gate == 1) goto settle;
    }
    return 0;
settle:
    if (state == 0xc && gate == 1) {
        func_ov004_020ad79c(unk_0a8, mHudScore);
    }
    return 1;
}

// @symbol _ZN10dScMgBSC_c15OnGroundPoundedEv
/* Slot 21: picks one of four reactions from the score, mHudScore / 5
   capped at 3. */
void dScMgBSC_c::OnGroundPounded()
{
    int score = mHudScore;
    int tier = score / 5;
    if ((unsigned int)tier > 3) {
        tier = 3;
    }
    func_ov004_020b6324(tier + 1);
}

// @symbol _ZN10dScMgBSC_c14StateDealCardsEv
/* State 0: deals two cards with distinct random values out of five
 * (flags marks the values taken), starts the deal animation and the
 * prompt, and moves to StateWaitDeal. */
void dScMgBSC_c::StateDealCards()
{
    unsigned char flags[5];
    unsigned char* p;
    int j;
    int i;
    unsigned char idx;

    p = flags;
    for (j = 0; j < 5; j++)
        *p++ = 0;

    for (i = 0; i < 2; i++) {
        mCardPos[i].x = 0x80000;
        mCardPos[i].y = -0x40000;
        idx = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) % 5u;
        for (;;) {
            if (flags[idx] == 0) {
                mCardValue[i] = idx;
                flags[idx] = 1;
                break;
            }
            idx++;
            idx %= 5;
        }
        mCardFrame[i] = 0;
    }

    mOtherCard = 0;
    mChosenCard = 1;
    mDealCount = 0;
    func_ov006_020c1604((char *)&mShared, 3, 2, (int)&mDealCount);

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }

    mState += 1;
}

// @symbol _ZN10dScMgBSC_c13StateWaitDealEv
/* State 1: slides each card to its lane (x toward
 * data_ov006_0213fd44, y toward 0x60000). mDealCount is re-read every pass
 * because the animation helper counts it down. Moves on once both cards
 * are in place. */
void dScMgBSC_c::StateWaitDeal()
{
  int i;
  int arrived;
  int xArrived;
  char *xp;
  char *yp;
  short count;

  arrived = 0;
  i = 0;
  count = mDealCount;
  if (count <= 0)
    goto after_loop;

  xp = (char *)&mCardPos[0].x;
  yp = (char *)&mCardPos[0].y;
  do {
    if (Math_Function_0203b14c((int *)xp, data_ov006_0213fd44[i], 0x800, 0x10000, 0x200) == 0)
      xArrived = 1;
    else
      xArrived = 0;

    if (Math_Function_0203b14c((int *)yp, 0x60000, 0x800, 0x10000, 0x200) == 0)
    {
      if (xArrived != 0)
        arrived++;
    }
    xp += 8;
    yp += 8;
    i++;
    count = mDealCount;
  } while (i < count);

after_loop:
  if (arrived < 2)
    return;
  mState++;
}

// @symbol _ZN10dScMgBSC_c15StateChooseCardEv
/* State 2: waits for a touch within 16 pixels of a lane's x and 24 of y
 * 0x60. The touched card becomes mChosenCard and the machine moves on. */
void dScMgBSC_c::StateChooseCard()
{
    unsigned int sample = data_020a0e40;
    int touched = 0;
    int lane;
    if (data_020a0de8[sample * 4] != 0) {
        if (data_020a0de9[sample * 4] != 0) touched = 1;
    }
    if (touched == 0) return;
    for (lane = 0; lane < 2; lane++) {
        unsigned char idx = *(volatile unsigned char *)&data_020a0e40;
        unsigned char *touch = &data_020a0de8[(unsigned int)idx * 4];
        int dx = (int)touch[2] - (data_ov006_0213fd44[lane] >> 12);
        int dy = (int)touch[3] - 0x60;
        if (dx < -0x10) continue;
        if (dx > 0x10) continue;
        if (dy < -0x18) continue;
        if (dy > 0x18) continue;
        mChosenCard = lane;
        mOtherCard = lane ^ 1;
        mRevealStep = 0;
        mState += 1;
        data_ov004_020bf9ec = 1;
        Sound::PlayBank2_2D(0x153);
    }
}

// @symbol _ZN10dScMgBSC_c14StateMoveCardsEv
/* State 3: moves the chosen card down and the other up, then primes the
 * 30-frame timer. */
void dScMgBSC_c::StateMoveCards()
{
    int chosenArrived = ApproachLinear(mCardPos[mChosenCard].y, 0x90000, 0x4000) & 1;
    int otherArrived = ApproachLinear(mCardPos[mOtherCard].y, 0x30000, 0x4000) & 1;

    if (chosenArrived == 0) return;
    if (otherArrived == 0) return;

    mStateTimer = 0x1e;
    mState += 1;
}

// @symbol _ZN10dScMgBSC_c13StateWaitMoveEv
/* State 4: waits out the timer, then plays the move sound. */
void dScMgBSC_c::StateWaitMove()
{
    mStateTimer = mStateTimer - 1;
    if (mStateTimer > 0)
        return;
    Sound::PlayBank2_2D(0x148);
    mState = mState + 1;
}

// @symbol _ZN10dScMgBSC_c16StateRevealCardsEv
/* State 5: flips both cards, one frame every four ticks, until the other
 * card is face up. Then settles the stakes: the higher card wins, which
 * adds twice data_ov004_020bfa18 to unk_0a8 and one to the score; a loss
 * takes one away. */
void dScMgBSC_c::StateRevealCards()
{
    int i;
    mRevealStep += 1;
    if (mRevealStep < 4)
        return;
    mRevealStep = 0;
    for (i = 0; i < 2; i++)
        mCardFrame[i] += 1;
    if (mCardFrame[mOtherCard] < 4)
        return;
    mStateTimer = 0x1e;
    mState += 1;
    mPromptEnabled = 0;
    {
        u8 chosen = mCardValue[mChosenCard];
        u8 other = mCardValue[mOtherCard];
        int base = unk_0a8;
        int score = mHudScore;
        if (chosen > other) {
            base += data_ov004_020bfa18 * 2;
            score = score + 1;
        } else {
            score = score - 1;
        }
        func_ov004_020ad79c(base, score);
    }
}

// @symbol _ZN10dScMgBSC_c15StateJudgeCardsEv
/* State 6: 30 frames after the flip, bumps the score (capped at 9999) and
 * the best score on a win, plays jingle 4 or 5, queues the table's win or
 * loss reaction, and parks in state 13. The bump goes through a pointer on
 * purpose: the ROM takes the member's address first. */
void dScMgBSC_c::StateJudgeCards()
{
    int *score;

    mStateTimer = mStateTimer - 1;
    if (mStateTimer > 0)
        return;

    if (mCardValue[mChosenCard] > mCardValue[mOtherCard])
    {
        if (mHudScore < 0x270f)
        {
            score = &mHudScore;
            *score = *score + 1;
        }
        if (mHudScore > unk_0b8)
            unk_0b8 = mHudScore;
        func_ov004_020adb1c(mHudScore);
        func_ov004_020b0a54(4);
        func_ov006_020c0c80(&mShared);
        mState = 0xd;
    }
    else
    {
        if (mHudScore > 0)
        {
            score = &mHudScore;
            *score = *score - 1;
        }
        func_ov004_020b0a54(5);
        func_ov006_020c0d68(&mShared);
        mState = 0xd;
    }
}

// @symbol _ZN10dScMgBSC_c18StateTableReactionEv
/* States 7 and 8: queue the table's win or loss reaction. Nothing in this
 * file sets state 7 or 8; StateJudgeCards does the same work inline. */
void dScMgBSC_c::StateTableReaction()
{
    if (mState == 7) {
        func_ov006_020c0c80(&mShared);
        mState = 9;
    } else {
        func_ov006_020c0d68(&mShared);
        mState = 0xa;
    }
}

// @symbol _ZN10dScMgBSC_c14StateWaitTableEv
/* States 9 and 10: wait for the table's reaction to finish, then pay out
 * a win (state 12) or play the loss sting and gather the cards (state 11). */
void dScMgBSC_c::StateWaitTable()
{
    if (func_ov006_020c1718((int *)&mShared) == 0)
        return;
    if (mState == 9) {
        func_ov004_020b56c8(data_ov004_020bfa18 << 1);
        mState = 0xc;
    } else {
        func_ov004_020b5ed0();
        mState = 0xb;
    }
}

// @symbol _ZN10dScMgBSC_c16StateReturnCardsEv
/* State 11, while data_ov004_020bf9e4 is set: if the base score is spent,
 * first walks both cards back to the deck (mDealCount re-read every pass,
 * as in StateWaitDeal). Then refreshes the meter and settles in state 12. */
void dScMgBSC_c::StateReturnCards()
{
    int done;
    int i;
    CardPos *pos;

    if (data_ov004_020bf9e4 != 1)
    {
        return;
    }
    if (unk_0a8 <= 0)
    {
        done = 0;
        i = 0;
        if (mDealCount > 0)
        {
            pos = &mCardPos[0];
            do
            {
                if (Math_Function_0203b14c((int *)pos, -0x40000, 0x800, 0xc000, 0x200) == 0)
                {
                    done++;
                }
                i++;
                pos++;
            } while (i < mDealCount);
        }
        if (done < 2)
        {
            return;
        }
    }
    func_ov004_020b0a38();
    mState = 0xc;
}

// @symbol _ZN10dScMgBSC_c9StateDoneEv
/* States 12 and 13: at rest. */
void dScMgBSC_c::StateDone()
{
}

/* D1 and D0 are not written out: the vtable InitResources emits pulls them
 * in, in ROM order. Written out of line, D0 would come out first. */
