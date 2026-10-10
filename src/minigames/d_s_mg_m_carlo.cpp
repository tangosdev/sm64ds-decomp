//cpp
/* ov006/dScMgMCarlo_c: the Monte Carlo card minigame. Pairs of matching
 * faces on neighbouring board slots are picked with the stylus and removed,
 * and the board deals itself back in behind them.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder.
 *
 * Still raw: the func_ov004_ helpers and the data_ov006_ board globals are
 * unnamed in symbols.txt, and the unk_ fields have no evidence for a name
 * yet. The card methods and the static board helpers have coined names;
 * the ROM has only addresses for them. The factory writes the allocation
 * and vptr stores out by hand because the ROM has no dScMgMCarlo_c C1.
 */

#include "dScMgMCarlo_c.h"
#include "types.h"
#include "decl_common.h"
#include "dScMgCard_c.h"
#include "PlayerInput.h"

/* Coined from the card transitions in DealIn and Update. The byte-sized
 * mState field retains its existing layout. */
enum CardState {
    CARD_WAITING = 0,
    CARD_ENTERING = 1,
    CARD_IDLE = 2,
    CARD_SELECTED = 3,
    CARD_MOVING = 4,
    CARD_LEAVING = 5
};

/* The TUBUILD CONFLICT notes below are the merged legacy files' own
 * spellings of the card and scene structs; the manifest records them, so
 * they stay. The code uses dMgMCarloCardObj_c from the header. */

/* Engine helpers; the parameter types are the ones the mangled names
 * encode. */
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }
namespace Sound { void PlayBank2_2D(u32 id); }
int  ApproachLinear(int &value, int target, int step);
int  ApproachLinear2(s16 &value, s16 target, s16 step);

/* TUBUILD CONFLICT -- alternate body of struct 'Node', from the legacy file for func_ov006_020f7994, NOT applied:
struct Node { char pad[8]; struct Node* next; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Node', from the legacy file for func_ov006_020f7b90, NOT applied:
struct Node { char pad[8]; struct Node *next; char pad2[0x1e]; short key; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_020f8224, NOT applied:
struct Obj
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_020f82d0, NOT applied:
struct Obj { char pad[1]; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_020f8320, NOT applied:
struct Obj {
    char pad_00[0xc];
    int f_0c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    char pad_28[2];
    short f_2a;
    unsigned char f_2c;
    unsigned char f_2d;
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Node', from the legacy file for _ZN13dScMgMCarlo_c6RenderEv, NOT applied:
struct Node {
    virtual void f0();
    Node* next;
    char pad[0x18];
    int field20;
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN13dScMgMCarlo_c13InitResourcesEv, NOT applied:
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(int x);
};
*/

extern "C" {
extern dMgMCarloCardObj_c *data_ov006_021424fc; /* first pick */
extern dMgMCarloCardObj_c *data_ov006_02142500; /* board list head */
extern dMgMCarloCardObj_c *data_ov006_02142504; /* board list tail */
extern dMgMCarloCardObj_c *data_ov006_02142508; /* second pick */
extern short data_ov006_021424ec;
extern int data_ov006_0213d570;
extern int data_ov006_0213d574;
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_021424f4;
extern int data_ov006_021424f8;
extern int data_ov006_021424f0;
extern int data_ov006_0213d568;
extern int data_ov006_0213d56c;
extern int data_ov006_0213d5e0[8]; /* the eight face weights */
extern int data_020a0db0;
extern unsigned short data_ov006_0213d600[];
extern int data_ov006_02133810[];
extern int data_ov006_0214250c[];
extern void Vec2_Sub(int* o, int* a, int* b);
extern void func_0203d630(int *p, int m);
extern int data_ov004_020bf9ec;
extern unsigned int func_02012790(unsigned int a);
extern int func_0203d5dc(void *a, void *b);
extern int data_ov006_02133f18;
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
void func_ov006_020c0aa8(void* p);
void func_ov004_020b1bc8(char* a0, int a1, int a2, int a3);
void func_ov004_020b6430(void);
void func_ov004_020b1e34(char* a0, int a1, int a2, int a3);
void func_ov006_020c1804(void* p);
int func_ov006_020c1718(void* p);
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(void* c);
void func_ov006_020c1604(char* c, int unused, short a2, int a3);
void func_ov004_020b66d4(char* p);
void FreeGfxSlotsById(int arg);
void func_ov004_020b56c8(char* p);
extern unsigned short data_ov004_020bf9e4;
extern void func_ov006_0210a534(void *c);
extern int GetGameLanguage(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern int func_ov006_020c1a88(char *p);
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
void *_ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
void __cxa_vec_ctor(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV13dScMgMCarlo_c[];
void *_ZN18dMgMCarloCardObj_cC1Ev(void*);
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_0213d574, from the legacy file for func_ov006_020f7994, NOT applied: extern Fix12 data_ov006_0213d574; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142500, from the legacy file for func_ov006_020f7994, NOT applied: extern struct Node* data_ov006_02142500; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_021424fc, from the legacy file for func_ov006_020f7a90, NOT applied: extern int data_ov006_021424fc; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142508, from the legacy file for func_ov006_020f7a90, NOT applied: extern int data_ov006_02142508; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_021424fc, from the legacy file for func_ov006_020f7b10, NOT applied: extern int data_ov006_021424fc; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142508, from the legacy file for func_ov006_020f7b10, NOT applied: extern int data_ov006_02142508; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142500, from the legacy file for func_ov006_020f7b90, NOT applied: extern struct Node *data_ov006_02142500; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_021424fc, from the legacy file for func_ov006_020f7c10, NOT applied: extern int data_ov006_021424fc; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142508, from the legacy file for func_ov006_020f7c10, NOT applied: extern int data_ov006_02142508; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142500, from the legacy file for func_ov006_020f7c10, NOT applied: extern char *data_ov006_02142500; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142504, from the legacy file for func_ov006_020f8320, NOT applied: extern int data_ov006_02142504; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142504, from the legacy file for func_ov006_020f84a8, NOT applied: extern void* data_ov006_02142504; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c0aa8, from the legacy file for _ZN13dScMgMCarlo_c13InitResourcesEv, NOT applied: extern void func_ov006_020c0aa8(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov004_020beb68, from the legacy file for _ZN13dScMgMCarlo_c13InitResourcesEv, NOT applied: extern void *data_ov004_020beb68; */
}

// @symbol _ZN18dMgMCarloCardObj_cC1Ev
dMgMCarloCardObj_c::dMgMCarloCardObj_c()
    : mPrev(0), mNext(0)
{
}

// @symbol dScMgMCarlo_c_classInit
extern "C" void* dScMgMCarlo_c_classInit()
{
    char* raw = (char*)_ZN7fBase_cnwEj(0x60b0);
    if (raw) {
        _ZN11dScMgBase_cC2Ev(raw);
        *(int*)raw = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(raw + 0x471c);
        *(int*)raw = (int)&_ZTV13dScMgMCarlo_c[2];
        func_ov006_020c1d80(raw + 0x4f38);
        __cxa_vec_ctor(raw + 0x51a8, 0x50, 0x30,
                     (void*)_ZN18dMgMCarloCardObj_cC1Ev,
                     (void*)_ZN18dMgMCarloCardObj_cD1Ev);
    }
    return raw;
}

// @symbol _ZN13dScMgMCarlo_c13InitResourcesEv
/* Slot 0. Loads the language's board graphics and the shared palette
 * (0xbb), sets the engine mode bytes, parks the 3D matrix at the family
 * camera and arms mShared. The round starts with the virtual
 * OnYoshiTryEat(-1), which deals the first board. */
s32 dScMgMCarlo_c::InitResources()
{
    void *chr;
    void *pal;

    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(this);
    chr = (void *)LoadFile(data_ov006_0213d5b4[GetGameLanguage()]);
    pal = (void *)LoadFile(0xbb);
    DecompressLZ16(chr, (void *)0x6600000);
    GXS::LoadOBJPltt(pal, 0, 0x100);
    Deallocate(chr);
    Deallocate(pal);

    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(unsigned short *)0x4000008 = (*(unsigned short *)0x4000008 & ~3) | 1;
    *(unsigned short *)0x400100a = *(unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8(pad_4660);
    if (func_ov006_020c1a88((char *)&mShared) == 0) return 0;

    func_ov004_020b682c();
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    {
        int score = func_ov004_020ad878();
        if (data_ov004_020beb68 != 0) ((dScMgBase_c *)data_ov004_020beb68)->mHudScore = score;
    }
    OnYoshiTryEat(-1);
    this->unk_60aa = 0;
    return 1;
}

// @symbol _ZN13dScMgMCarlo_c13OnYoshiTryEatEi
/* Slot 18: restart the round from a fresh board. SetupBoard deals a new
 * layout, the round state re-arms (unk_60a8 = 1), and the shared prompt
 * window re-opens watching unk_60ae.
 *
 * func_ov004_020b66d4 never reads its argument, but the ROM still computes
 * this + 0x6000 for it, which is mArray[76].mTargetY. */
void dScMgMCarlo_c::OnYoshiTryEat(int arg)
{
    SetupBoard(mArray);
    data_ov006_0213d564 = 0;
    unk_60ae = 0;
    mShared.unk_1e6 = 1;
    func_ov006_020c1604((char *)&mShared, 4, 4, (int)&unk_60ae);
    mShared.unk_01a = 1;
    unk_60aa = 0;
    unk_60a8 = 1;
    func_ov004_020b66d4((char *)&mArray[76].mTargetY);
    data_ov004_020bc7d4 = 1;
}

// @symbol _ZN13dScMgMCarlo_c13OnTurnIntoEggEi
/* Slot 19: the egg-exit sequence, run on the same round state (unk_60a8)
 * as Behavior. 4 frees the Gfx slots. 5 waits for the shared window, then
 * hands the dealt total on (or plays the wrap-up cue). 6 hides the board
 * one card at a time from the tail. 7 waits for the active scene, then
 * flips every dealt card away. 8 runs the 0x1e timer out. Returns 1 while
 * frames remain. */
int dScMgMCarlo_c::OnTurnIntoEgg(int mode)
{
    switch (unk_60a8) {
    case 4:
        FreeGfxSlotsById(0x1d);
        unk_60a8 += 1;
        break;
    case 5:
        if (func_ov006_020c1718(&mShared) != 0) {
            int dealt = data_ov006_0213d570;
            if (dealt != 0) {
                func_ov004_020b5e40(dealt);
                unk_60aa = -1;
            } else {
                func_ov004_020b56c8((char*)data_ov006_0213d568);
            }
            unk_60ae = 0;
            unk_60a8 += 1;
        }
        break;
    case 6: {
        dMgMCarloCardObj_c *head = data_ov006_02142500;
        if (head != 0 && head->mVisible != 0) {
            if (ApproachLinear2(unk_60aa, 0, 1) != 0) {
                dMgMCarloCardObj_c *card;
                int i;
                int n;
                unk_60aa = 0x10;
                n = unk_60ae;
                card = data_ov006_02142504;
                for (i = 0; i < n; i++) {
                    if (card == 0) break;
                    card = card->mPrev;
                }
                if (card != 0) card->mVisible = 0;
                unk_60ae += 1;
            }
        } else {
            if (data_ov004_020bf9e4 <= 1)
                unk_60a8 += 1;
        }
        break;
    }
    case 7: {
        dScMgBase_c *active = (dScMgBase_c *)data_ov004_020beb68;
        if ((active != 0 ? active->unk_0a8 : 0) != 0)
            return 1;
        dScMgMCarlo_c::FlipDealtCards();
        unk_60aa = 0x1e;
        unk_60a8 += 1;
        break;
    }
    case 8:
        if (ApproachLinear2(unk_60aa, 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}

// @symbol _ZN13dScMgMCarlo_c8BehaviorEv
/* Slot 6: the round, on the s16 state at unk_60a8. Case 1 arms the prompt
 * blink and falls through into case 2 on purpose. data_ov004_020beb68 is
 * the active scene (dScMgBase_c.h declares it void *), so it is cast at
 * each use. */
s32 dScMgMCarlo_c::Behavior()
{
    switch (unk_60a8) {
    case 1:
        unk_60a8++;
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d574 = unk_60ae * 5 << 12;
        if (unk_60ae == 4) {
            if (dScMgMCarlo_c::BoardBusy() == 0) {
                unk_60ae = 0;
                unk_60a8++;
            }
        }
        break;
    case 3:
        if (dScMgMCarlo_c::BoardReady() == 0) {
            int shown;
            if (data_ov006_0213d564 == 1)
                data_ov006_0213d564 = 0;
            shown = data_ov006_0213d574 >> 12;
            if (data_ov006_0213d56c != 0 && shown > 10 && shown <= 18
                && func_ov006_020c1718(&mShared) != 0) {
                unk_60ae = 0;
                mShared.unk_1e6 = 0;
                func_ov006_020c1164(&mShared, 2, &unk_60ae);
                if (data_ov006_0213d56c == 2)
                    mShared.unk_01a = 0;
            } else {
                int lim = unk_60ae + 18;
                if (shown >= lim) {
                    int flag = (data_ov006_021424fc != 0 && data_ov006_02142508 != 0);
                    if (flag == 0)
                        data_ov006_0213d574 = lim << 12;
                }
            }
        } else {
            if (data_ov006_0213d564 == 0) {
                if (dScMgMCarlo_c::HasRemovablePair() != 0) {
                    data_ov006_0213d564 = 1;
                } else {
                    if (data_ov006_0213d570 != 0) {
                        if (mShared.unk_01a == 1) {
                            mShared.unk_01a = 0;
                        } else if (func_ov006_020c16b4(&mShared) != 0) {
                            {
                                dScMgBase_c *active = (dScMgBase_c *)data_ov004_020beb68;
                                if (active != 0) {
                                    if (active->mHudScore > 0)
                                        active->mHudScore = active->mHudScore - 1;
                                }
                            }
                            func_ov006_020c0d68(&mShared);
                            func_ov004_020b0a54(5);
                            {
                                int dealt = data_ov006_0213d570;
                                dScMgBase_c *active = (dScMgBase_c *)data_ov004_020beb68;
                                int left = ((active != 0) ? active->unk_0a8 : 0) - dealt;
                                int score = (active != 0) ? active->mHudScore : 0;
                                func_ov004_020ad79c(left, score);
                            }
                            mPromptEnabled = 0;
                            unk_60a8++;
                        }
                    } else {
                        {
                            dScMgBase_c *active = (dScMgBase_c *)data_ov004_020beb68;
                            if (active != 0) {
                                if (active->mHudScore < 0x270f)
                                    active->mHudScore = active->mHudScore + 1;
                                if (active->mHudScore > active->unk_0b8)
                                    active->unk_0b8 = active->mHudScore;
                            }
                        }
                        func_ov004_020adb1c(mHudScore);
                        func_ov006_020c0c80(&mShared);
                        func_ov004_020b0a54(4);
                        {
                            dScMgBase_c *active = (dScMgBase_c *)data_ov004_020beb68;
                            int base = (active != 0) ? active->unk_0a8 : 0;
                            func_ov004_020ad79c(data_ov006_0213d568 + base,
                                                (active != 0) ? active->mHudScore : 0);
                        }
                        mPromptEnabled = 0;
                        unk_60a8++;
                    }
                    data_ov006_0213d564 = 0;
                }
            }
        }
        break;
    }

    func_ov004_020b65e4();
    func_ov006_020c19d0(&mShared);
    dScMgMCarlo_c::UpdateBoard();
    return 1;
}

// @symbol _ZN13dScMgMCarlo_c6RenderEv
/* Slot 9. The board draws in two passes over the same tail-to-head walk:
 * cards still rising (positive mYStep) first, then the settled ones. */
s32 dScMgMCarlo_c::Render()
{
    func_ov006_020c0aa8(pad_4660);
    func_ov004_020b1bc8((char *)this, 0xc, 0xc, 0);
    func_ov004_020b6430();
    func_ov004_020b1e34((char *)this, 0xe0, 0x14, 1);

    {
        dMgMCarloCardObj_c *n = data_ov006_02142504;
        int i = 0;
        for (;;) {
            if (n == 0) break;
            if (n->mYStep > 0) n->Render();
            i++;
            n = n->mPrev;
            if (i >= 0x14) break;
        }
    }
    {
        int i = 0;
        dMgMCarloCardObj_c *n = data_ov006_02142504;
        for (;;) {
            if (n == 0) break;
            if (n->mYStep == 0) n->Render();
            i++;
            n = n->mPrev;
            if (i >= 0x14) break;
        }
    }
    RenderHud();
    func_ov006_020c1804(&mShared);
    return 1;
}

// @symbol _ZN13dScMgMCarlo_c9RenderHudEv
/* The ROM passes `this` and the body never reads it. Draw the deck
 * gauge while cards remain in the deck: the count -- data_ov006_0213d56c,
 * seeded with the deck size by SetupBoard and drained one card at a time
 * as the deal proceeds -- drawn as a number at (0xe8, 0x28), with the
 * gauge's frame sprite behind it at (0xe8, 0x18). */
void dScMgMCarlo_c::RenderHud(void)
{
    int deck = data_ov006_0213d56c;
    if (deck == 0) return;
    func_ov004_020b1ea4(0xe8, 0x28, deck, -1, -1, 0, 0);
    Hud_RenderSprite((void*)data_ov006_02133f18, 0xe8, 0x18, -1, -1);
}

// @symbol _ZN18dMgMCarloCardObj_c4InitEi
/* Reset an unlinked card. The first twenty slots stagger the deal by
 * column; reserve cards wait one frame. Slot 19 marks the last visible card. */
void dMgMCarloCardObj_c::Init(int slot) {
  mSlot = (short)slot;
  if (slot >= 0x14) {
    mDealDelay = 1;
  } else {
    mDealDelay = (short)((slot % 5) * 2 + 1);
  }
  mLift = 0;
  if (slot == 0x13) data_ov006_02142504 = this;
  mVisible = 1;
  mXStep = 0;
  mYStep = 0;
  mState = CARD_WAITING;
  mNext = 0;
  mPrev = mNext;
}

// @symbol _ZN18dMgMCarloCardObj_c6DealInEi
/* Move toward a board slot. Cards coming from reserve start above the
 * board and become the last visible card; waiting cards use the same start.
 * Other cards move from their current position. The target is the slot's
 * column and row, and the absolute scaled delta supplies the movement step. */
void dMgMCarloCardObj_c::DealIn(int slot)
{
    int delta[3];

    if (slot >= 0x14) {
        return;
    }

    if (mSlot >= 0x14) {
        mX = (0x70 - ((data_ov006_0213d56c >> 2) << 1)) << 12;
        mY = -0x30000;
        data_ov006_02142504 = this;
        mState = CARD_ENTERING;
    } else if (mState == CARD_WAITING) {
        mX = (0x70 - ((data_ov006_0213d56c >> 2) << 1)) << 12;
        mY = -0x30000;
        mState = CARD_MOVING;
        data_ov006_021424f4++;
    } else {
        mState = CARD_MOVING;
        data_ov006_021424f4++;
    }

    mTargetX = ((slot % 5) * 32 + 0x30) << 12;
    mTargetY = ((slot / 5) * 0x30) << 12;
    mSlot = (short)slot;

    Vec2_Sub(delta, &mTargetX, &mX);
    mXStep = delta[0];
    mYStep = delta[1];
    func_0203d630(&mXStep, 0x124);

    if (mXStep < 0) {
        mXStep = -mXStep;
    }
    if (mYStep < 0) {
        mYStep = -mYStep;
    }
}

// @symbol _ZN18dMgMCarloCardObj_c8FlipAwayEi
/* Leave in reverse column order after the round ends. */
void dMgMCarloCardObj_c::FlipAway(int slot) {
    mDealDelay = (short)((4 - slot % 5) * 2);
    mState = CARD_LEAVING;
}

// @symbol _ZN18dMgMCarloCardObj_c10IsPairWithEPS_
/* Can this card and that card be removed
 * together? Same face value, and the two board slots within one step in
 * BOTH axes (slot is col = slot%5, row = slot/5) -- the memory-match
 * adjacency test. Called on the second pick against the first in Update,
 * and across the whole board by HasRemovablePair. */
int dMgMCarloCardObj_c::IsPairWith(dMgMCarloCardObj_c *other)
{
    int slot, otherSlot, columnDistance, rowDistance;
    if (other->mFace != mFace)
        goto fail;
    otherSlot = other->mSlot;
    slot = mSlot;
    columnDistance = slot % 5 - otherSlot % 5;
    rowDistance = slot / 5 - otherSlot / 5;
    if (columnDistance < 0)
        columnDistance = -columnDistance;
    if (columnDistance >= 2)
        goto fail;
    if (rowDistance < 0)
        rowDistance = -rowDistance;
    if (rowDistance >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}

// @symbol _ZN18dMgMCarloCardObj_c7HitTestEv
/* Is this card the one under the stylus?
 * Only when the round is armed (data_ov006_0213d564) and the board is
 * settled (no card still flying or flipping in), the touch panel has to
 * report an active press, and this card's on-screen position (fixed-point
 * >> 12) has to fall in the 0x20-by-0x30 window around the reported touch
 * point -- the same 32x48 slot grid DealIn lays the board out on. */
int dMgMCarloCardObj_c::HitTest(void)
{
    u8 touchSlot;
    int touchOffset;
    int touching;
    int touchXOffset, touchYOffset;

    if (data_ov006_0213d564 == 0) return 0;
    if (dScMgMCarlo_c::BoardBusy() != 0) goto fail;

    touchSlot = gActivePlayerSlot;
    touchOffset = touchSlot * 4;
    touching = 0;
    if (gTouchHeld[touchOffset]) {
        if (gTouchEdge[touchOffset]) touching = 1;
    }
    if (touching == 0) goto fail;

    touchXOffset = gTouchX[touchSlot * 4] - (mX >> 12);
    touchYOffset = gTouchY[touchSlot * 4] - (mY >> 12);
    if (touchXOffset > 7 && touchXOffset < 0x28 && touchYOffset > 0 && touchYOffset < 0x31) return 1;
fail:
    return 0;
}

// @symbol _ZN18dMgMCarloCardObj_c6UpdateEi
/* Advance the card toward its current board slot. Touching an idle card
 * selects it; touching a selected card deselects it. A matching second pick
 * starts the pair's removal delay. Incoming cards settle when their position
 * and lift reach the targets. Leaving cards wait out their stagger, then
 * slide left off the board. */
void dMgMCarloCardObj_c::Update(int slot)
{
    switch (mState) {
    case CARD_WAITING:
        mDealDelay -= 1;
        if (mDealDelay != 0)
            return;
        data_ov006_0213d56c--;
        DealIn(slot);
        return;

    case CARD_IDLE:
        if (mSlot != slot) {
            DealIn(slot);
            return;
        }
        if (HitTest() == 0)
            return;
        if (data_ov006_021424fc == 0) {
            data_ov006_021424fc = this;
            mState = CARD_SELECTED;
            Sound::PlayBank2_2D(0x153);
            return;
        }
        if (IsPairWith(data_ov006_021424fc) != 0) {
            data_ov006_02142508 = this;
            data_ov006_021424ec = 0x20;
            mState = CARD_SELECTED;
            Sound::PlayBank2_2D(0x154);
            if (data_ov004_020bf9ec == 0)
                data_ov004_020bf9ec = 1;
            return;
        }
        func_02012790(0xe);
        {
            dMgMCarloCardObj_c *first = data_ov006_021424fc;
            first->mState = CARD_IDLE;
        }
        data_ov006_021424fc = 0;
        return;

    case CARD_SELECTED:
        if (HitTest() == 0)
            return;
        mState = CARD_IDLE;
        if (data_ov006_021424fc != this)
            return;
        if (data_ov006_02142508 != 0)
            return;
        Sound::PlayBank2_2D(0x155);
        data_ov006_021424fc = 0;
        return;

    case CARD_ENTERING:
    case CARD_MOVING:
        ApproachLinear(mX, mTargetX, mXStep);
        ApproachLinear(mY, mTargetY, mYStep);
        ApproachLinear(mLift, 0x4000, 0x300);
        if (func_0203d5dc(&mX, &mTargetX) != 0)
            return;
        if (mLift != 0x4000)
            return;
        mState = CARD_IDLE;
        ApproachLinear(data_ov006_021424f4, 0, 1);
        mXStep = 0;
        mYStep = 0;
        return;

    case CARD_LEAVING:
        if (ApproachLinear2(mDealDelay, 0, 1) == 0)
            return;
        ApproachLinear(mX, -0x30000, 0x10000);
        return;

    default:
        break;
    }
}

// @symbol _ZN18dMgMCarloCardObj_c6RenderEv
/* Draw this card through the dMeter_c sprite bank.
 * Hidden cards (mVisible 0) and ones still waiting to be dealt (mState 0)
 * draw nothing, and a selected card blinks according to bit 8 of
 * data_020a0db0. The bank index is the face's bank (mFace + 1) of five flip
 * frames -- (mLift >> 12), 0x4000 fully raised = frame 4 -- and the card
 * sits at its fixed-point position shifted to pixels plus a 24-pixel screen
 * offset. Slot 0 of the element vtable; Render is the class's key function,
 * so this TU is what emits _ZTV/_ZTI/_ZTS for the element class. */
void dMgMCarloCardObj_c::Render(void)
{
    unsigned char state;
    if (mVisible == 0) return;
    state = mState;
    if (state == CARD_WAITING) return;
    if (state == CARD_SELECTED) {
        if ((data_020a0db0 & 8) != 0) return;
    }
    {
        int idx = (mFace + 1) * 5
                  + (mLift >> 12);
        unsigned short sprite = data_ov006_0213d600[idx];
        Hud_RenderSprite(
            (void*)data_ov006_0214250c[sprite],
            (mX >> 12) + 24,
            (mY >> 12) + 24,
            -1,
            -1);
    }
}

// @symbol _ZN13dScMgMCarlo_c10SetupBoardEP18dMgMCarloCardObj_c
/* Lay out a fresh round. The active
 * base's star count picks the tier -- under 5 stars, 6 face values at 6
 * cards each (36); under 10, 7 at 8 (56); past that, all 8 at 0xa (80) --
 * and both the deck counter and the dealt total start at faces-times-weight.
 * Then, re-dealing until the opening board is playable: the eight face
 * weights are seeded (the first faces get the weight, the rest zero), every
 * one of the 0x50 card elements is reset (Init), the first dealt-total of
 * them takes a fresh weighted face (DrawCardValue), the round state clears,
 * and the dealt cards are threaded onto the board's doubly-linked list in
 * array order. The do-while repeats the whole deal until the twenty board
 * slots hold at least one removable pair (HasRemovablePair never looks past
 * slot 0x13, so the stock behind the board does not count). */
void dScMgMCarlo_c::SetupBoard(dMgMCarloCardObj_c *cards)
{
    int stars;
    dScMgBase_c *active;
    short face;
    short cardIndex;
    short slot;
    dMgMCarloCardObj_c *card;
    dMgMCarloCardObj_c *previous;
    dMgMCarloCardObj_c *linkTail;
    dMgMCarloCardObj_c *savedNext;

    active = (dScMgBase_c *)data_ov004_020beb68;
    stars = (active != 0) ? active->mHudScore : 0;

    if (stars < 5) {
        data_ov006_021424f8 = 6;
        data_ov006_021424f0 = 6;
        data_ov006_0213d568 = 5;
    } else if (stars < 0xa) {
        data_ov006_021424f8 = 8;
        data_ov006_021424f0 = 7;
        data_ov006_0213d568 = 8;
    } else {
        data_ov006_021424f8 = 0xa;
        data_ov006_021424f0 = 8;
        data_ov006_0213d568 = 0xa;
    }

    data_ov006_0213d56c = data_ov006_021424f0 * data_ov006_021424f8;
    data_ov006_0213d570 = data_ov006_021424f0 * data_ov006_021424f8;

    do
    {
        for (face = 0; face < 8; face++)
        {
            if (face < data_ov006_021424f0)
                data_ov006_0213d5e0[face] = data_ov006_021424f8;
            else
                data_ov006_0213d5e0[face] = 0;
        }

        cardIndex = 0;
        card = cards;
        for (; cardIndex < 0x50; cardIndex++)
        {
            card->Init(cardIndex);
            if (cardIndex < data_ov006_0213d570)
            {
                card->mFace = (unsigned char)dScMgMCarlo_c::DrawCardValue();
            }
            card++;
        }

        slot = 0;
        data_ov006_021424fc = 0;
        data_ov006_02142508 = 0;
        data_ov006_021424ec = 0;
        data_ov006_021424f4 = 0;
        data_ov006_0213d574 = 0;
        data_ov006_02142500 = cards;

        if (data_ov006_0213d570 - 1 > 0)
        {
            previous = cards;
            do
            {
                linkTail = &cards[slot + 1];
                savedNext = previous->mNext;
                previous->mNext = linkTail;
                linkTail->mPrev = previous;
                while (linkTail->mNext != 0)
                {
                    linkTail = linkTail->mNext;
                }
                linkTail->mNext = savedNext;
                previous++;
                slot++;
            } while (slot < data_ov006_0213d570 - 1);
        }
    } while (dScMgMCarlo_c::HasRemovablePair() == 0);
}

// @symbol _ZN13dScMgMCarlo_c16HasRemovablePairEv
/* Does the board still hold two
 * cards that can be removed together (IsPairWith true for some pair)? The
 * walk skips the spare cards past slot 0x13 (they are never on the board),
 * and the inner walk starts one past the outer card so each pair is tested
 * once. SetupBoard re-deals while this is false so a fresh board always has
 * at least one removable pair; Behavior uses it to catch a stuck board. */
int dScMgMCarlo_c::HasRemovablePair(void)
{
    dMgMCarloCardObj_c *first, *second;
    first = data_ov006_02142500;
    while (first != 0 && first->mSlot < 0x14) {
        second = first->mNext;
        while (second != 0 && second->mSlot < 0x14) {
            if (first->IsPairWith(second) != 0) return 1;
            second = second->mNext;
        }
        first = first->mNext;
    }
    return 0;
}

// @symbol _ZN13dScMgMCarlo_c9BoardBusyEv
/* Busy while the deal count is changing, a card is moving, or both
 * picks are waiting for removal. A single selected card does not block input. */
int dScMgMCarlo_c::BoardBusy(void)
{
    int busy = 1;
    short target;
    short shown;
    shown = (short)(data_ov006_0213d574 >> 12);
    target = (short)data_ov006_0213d570;
    if (target > 20)
        target = 20;
    if (shown == target)
    {
        if (data_ov006_021424f4 == 0)
        {
            if (data_ov006_021424fc == 0 || data_ov006_02142508 == 0)
                busy = 0;
        }
    }
    return busy;
}

// @symbol _ZN13dScMgMCarlo_c10BoardReadyEv
/* Ready once the visible count reaches the board size, movement has
 * stopped, and no selected pair is waiting for removal. */
int dScMgMCarlo_c::BoardReady(void)
{
    int ready = 0;
    short target;
    short shown;
    shown = (short)(data_ov006_0213d574 >> 12);
    target = (short)data_ov006_0213d570;
    if (target > 20)
        target = 20;
    if (shown == target)
    {
        if (data_ov006_021424f4 == 0)
        {
            if (data_ov006_021424fc == 0 || data_ov006_02142508 == 0)
                ready = 1;
        }
    }
    return ready;
}

// @symbol _ZN13dScMgMCarlo_c13DrawCardValueEv
/* One weighted draw from the
 * eight-face deck. Roll the seeded RNG scaled by the weight total, then walk
 * the weights subtracting until the running total goes negative -- that face
 * is the draw, and its weight is spent. The same routine dScMgCard_c runs on
 * its six-face deck (src/minigames/d_s_mg_card.cpp); the weights themselves
 * live in this overlay's data and SetupBoard refills them per board size. */
int dScMgMCarlo_c::DrawCardValue(void)
{
    unsigned char drawnFace = 0;
    int total = 0;
    int face;
    int roll;
    for (face = 0; face < 8; face++)
        total += data_ov006_0213d5e0[face];
    roll = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * roll) >> 0xc;
    for (face = 0; face < 8; face++) {
        total -= data_ov006_0213d5e0[face];
        if (total < 0) {
            drawnFace = (unsigned char)face;
            data_ov006_0213d5e0[face]--;
            break;
        }
    }
    return drawnFace;
}

// @symbol _ZN13dScMgMCarlo_c14FlipDealtCardsEv
/* Between rounds, queue every card
 * currently on the board for the flip-out, walking the board list head to
 * tail and numbering the walk so FlipAway's flip stagger mirrors the deal
 * stagger card by card. */
void dScMgMCarlo_c::FlipDealtCards(void) {
    dMgMCarloCardObj_c* node = data_ov006_02142500;
    short i = 0;
    if((data_ov006_0213d574>>12) <= 0) return;
    do {
        if(node == 0) return;
        node->FlipAway(i);
        i = i+1;
        node = node->mNext;
    } while(i < (data_ov006_0213d574>>12));
}

// @symbol _ZN13dScMgMCarlo_c11UpdateBoardEv
/* Resolve a selected pair after its delay, unlink both cards, and restart
 * the deal count at the earlier slot so the remaining cards close the gap.
 * While a pair is selected, update only cards entering from reserve. Otherwise
 * advance the visible-card count and update every card in that prefix. */
void dScMgMCarlo_c::UpdateBoard(void) {
    dMgMCarloCardObj_c* head = data_ov006_02142500;
    dMgMCarloCardObj_c* card;
    short slot;

    if (data_ov006_021424fc != 0) {
        if (data_ov006_02142508 != 0) {
            if (--data_ov006_021424ec == 0) {
                if (data_ov006_021424fc->mSlot > data_ov006_02142508->mSlot)
                    data_ov006_0213d574 = data_ov006_02142508->mSlot << 12;
                else
                    data_ov006_0213d574 = data_ov006_021424fc->mSlot << 12;

                {
                    dMgMCarloCardObj_c* first = data_ov006_021424fc;
                    if (data_ov006_02142500 == first) data_ov006_02142500 = first->mNext;
                    if (data_ov006_02142504 == first) data_ov006_02142504 = first->mPrev;
                    if (first->mPrev != 0) first->mPrev->mNext = first->mNext;
                    if (first->mNext != 0) first->mNext->mPrev = first->mPrev;
                    first->mNext = 0;
                    first->mPrev = first->mNext;
                }
                {
                    dMgMCarloCardObj_c* second = data_ov006_02142508;
                    if (data_ov006_02142500 == second) data_ov006_02142500 = second->mNext;
                    if (data_ov006_02142504 == second) data_ov006_02142504 = second->mPrev;
                    if (second->mPrev != 0) second->mPrev->mNext = second->mNext;
                    if (second->mNext != 0) second->mNext->mPrev = second->mPrev;
                    second->mNext = 0;
                    second->mPrev = second->mNext;
                }

                data_ov006_02142508 = 0;
                data_ov006_021424fc = 0;
                ApproachLinear(data_ov006_0213d570, 0, 2);
            }
            card = head;
            for (slot = 0; slot < (data_ov006_0213d574 >> 12); slot++) {
                if (card == 0) return;
                if (card->mState == CARD_ENTERING) card->Update(slot);
                card = card->mNext;
            }
            return;
        }
    }

    {
        int target = (short)data_ov006_0213d570;
        if (target > 0x14) target = 0x14;
        ApproachLinear(data_ov006_0213d574, target << 12, 0x800);
    }
    card = head;
    for (slot = 0; slot < (data_ov006_0213d574 >> 12); slot++) {
        if (card == 0) return;
        card->Update(slot);
        card = card->mNext;
    }
}

// @symbol _ZN18dMgMCarloCardObj_cD1Ev
/* Empty: the card has no base and nothing to destroy. */
dMgMCarloCardObj_c::~dMgMCarloCardObj_c()
{
}

/* D1 and D0 are not written here. The destructor is inline in the header,
 * which makes InitResources the key function; the vtable it emits names
 * D1 and D0, so the compiler emits both, in ROM order. Writing them out
 * here puts D0 ahead of D1 and adds a D2 the ROM does not have. */

/* The sprite-index remap table. Its initializer reads the retail table, so
 * it is not a constant expression: the compiler emits
 * __sinit_d_s_mg_m_carlo.cpp to copy the 19 words at overlay load. */
int data_ov006_0214250c[19] = {
    data_ov006_02133810[0],
    data_ov006_02133810[1],
    data_ov006_02133810[2],
    data_ov006_02133810[11],
    data_ov006_02133810[12],
    data_ov006_02133810[13],
    data_ov006_02133810[14],
    data_ov006_02133810[3],
    data_ov006_02133810[4],
    data_ov006_02133810[5],
    data_ov006_02133810[6],
    data_ov006_02133810[9],
    data_ov006_02133810[10],
    data_ov006_02133810[7],
    data_ov006_02133810[8],
    data_ov006_02133810[15],
    data_ov006_02133810[16],
    data_ov006_02133810[17],
    data_ov006_02133810[18],
};
