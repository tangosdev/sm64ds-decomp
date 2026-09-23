//cpp
/* Pair-match minigame scene 2: 25 functions. The board is 40 card objects
 * on an intrusive doubly-linked list.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. There is no opt_propagation pragma: it applies to the whole
 * file, and turning it off breaks four other functions.
 *
 * Blocked: the ov004 helpers and the data_ov006 globals are unnamed in
 * symbols.txt; the ROM has no dScMgMCarlo2_c constructor, so the factory
 * builds the scene by hand; and the shared table at 0x4f38 is opaque in
 * dScMgMCarlo2_c.h, so its helpers take its address.
 */


#include "dScMgMCarlo2_c.h"
#include "types.h"
#include "decl_common.h"

/* data_ov006_0214257c is the head of the live card list and _02142568 its
 * tail (SetupBoard builds it); _0214256c heads the list of cleared cards,
 * which UpdateBoard deals back onto the tail. _02142570 and _02142574 are the
 * first and second card picked, and _02142578 is the card in slot 19, the
 * last one on the board, where the draw passes start. */

/* Globals that decl_common.h does not already declare with these types. */
extern "C" {
extern dMgMCarlo2CardObj_c* data_ov006_02142568;
extern dMgMCarlo2CardObj_c* data_ov006_0214256c;
extern dMgMCarlo2CardObj_c* data_ov006_02142570;
extern dMgMCarlo2CardObj_c* data_ov006_02142574;
extern dMgMCarlo2CardObj_c* data_ov006_02142578;
extern dMgMCarlo2CardObj_c* data_ov006_0214257c;
extern s16 data_ov006_02142558;     /* frames left before the pair resolves  */
extern s16 data_ov006_0214255c;     /* pieces still animating                */
extern s16 data_ov006_02142560;     /* difficulty row into data_ov006_0212e97c */
extern int data_ov006_02142580[];   /* per-face weights the picker draws from  */
extern int data_ov006_021425a8[];   /* sprite handles, indexed by 0213d770      */
extern unsigned short data_ov006_0213d770[];
extern int data_ov006_0212e954[];   /* the starting weights                     */
extern int data_ov006_0212e97c[][10]; /* per-difficulty weight top-ups          */
extern int data_0209e650[];
extern int data_020a0db0;
extern unsigned char data_0209d454;
extern unsigned char data_0209d45c;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

int  RandomIntInternal(void* seed);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
int  GetGameLanguage(void);
int  LoadFile(int handle);
void DecompressLZ16(int src, int dst);
void Vec2_Sub(int* out, int* a, int* b);
unsigned int func_02012790(unsigned int id);
int  func_0203d5dc(void* a, void* b);
void func_ov004_020ad90c(void* t);
void func_ov004_020adb1c(int score);
void func_ov006_0210a534(void* c);
void func_ov006_020c0aa8(void* c);
void func_ov006_020c1604(char* t, int unused, short a2, void* a3);
int  func_ov006_020c1718(void* t);
void func_ov006_020c1804(void* t);
void func_ov006_020c19d0(void* t);
int  func_ov006_020c1a88(void* t);

void* _ZN7fBase_cnwEj(unsigned int);
void _ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
void func_ov006_020c1d80(void*);
void __cxa_vec_ctor(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV14dScMgMCarlo2_c[];
void _ZN19dMgMCarlo2CardObj_cC1Ev(void*);
}

namespace GX { void LoadOBJPltt(const void*, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(const void*, unsigned int, unsigned int); }
namespace Sound { void PlayBank2_2D(unsigned int); }

/* C++ linkage: the reference parameters are part of the ROM names. */
int ApproachLinear(int& value, int target, int step);
int ApproachLinear2(s16& value, s16 target, s16 step);

// @symbol _ZN19dMgMCarlo2CardObj_cC1Ev
dMgMCarlo2CardObj_c::dMgMCarlo2CardObj_c()
    : mPrev(0), mNext(0)
{
}

// @symbol dScMgMCarlo2_c_classInit
extern "C" void* dScMgMCarlo2_c_classInit()
{
    char* p = (char*)_ZN7fBase_cnwEj(0x5930);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(int*)p = (int)&_ZTV14dScMgMCarlo2_c[2];
        func_ov006_020c1d80(p + 0x4f38);
        __cxa_vec_ctor(p + 0x51a8, 0x28, 0x30,
                     (void*)_ZN19dMgMCarlo2CardObj_cC1Ev,
                     (void*)_ZN19dMgMCarlo2CardObj_cD1Ev);
    }
    return p;
}

// @symbol _ZN14dScMgMCarlo2_c13InitResourcesEv
/* Slot 0. Loads the card tiles and palette, sets up the shared table, and
 * deals the first board through the slot-18 reset. */
s32 dScMgMCarlo2_c::InitResources()
{
    char *raw = (char *)this;
    int lang;
    int tiles, palette;
    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(raw);
    lang = GetGameLanguage();
    tiles = LoadFile(data_ov006_0213d744[lang]);
    palette = LoadFile(0xbb);
    DecompressLZ16(tiles, 0x6400000);
    DecompressLZ16(tiles, 0x6600000);
    GX::LoadOBJPltt((void *)palette, 0, 0x100);
    GXS::LoadOBJPltt((void *)palette, 0, 0x100);
    Deallocate((void *)tiles);
    Deallocate((void *)palette);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(volatile unsigned short *)0x4000008 = (*(volatile unsigned short *)0x4000008 & ~3) | 1;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8((void *)(raw + 0x4660));
    if (func_ov006_020c1a88((void *)(raw + 0x4f38)) == 0) return 0;
    func_ov004_020b682c();
    OnYoshiTryEat(-1);
    this->unk_592a = 0;
    this->unk_0a8 = 0xa;
    this->unk_0ac = this->unk_0a8;
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c13OnYoshiTryEatEi
/* Resets the whole board: rebuild the 40 pieces, clear the match latch,
 * re-arm the shared table, then hand the score display a zero. */
void dScMgMCarlo2_c::OnYoshiTryEat(int /* arg */)
{
    char *raw = (char *)this;

    dScMgMCarlo2_c::SetupBoard(mArray);
    data_ov006_0213d6fc = 0;
    unk_592e = 0;
    mShared.unk_1e6 = 1;
    func_ov006_020c1604(raw + 0x4f38, 4, 4, raw + 0x592e);
    mShared.unk_01a = 1;
    unk_592a = 0;
    func_ov004_020adb1c(0);
    unk_5928 = 1;
}

// @symbol _ZN14dScMgMCarlo2_c13OnTurnIntoEggEi
int dScMgMCarlo2_c::OnTurnIntoEgg(int /* mode */)
{
    char *raw = (char *)this;

    short state = unk_5928;
    switch (state) {
    case 4:
        if (func_ov006_020c1718((int *)(raw + 0x4f38)) != 0) {
            short *p = (short *)(raw + 0x5928);
            *p += 1;
        }
        break;
    case 5: {
        unsigned char idx = data_020a0e40[0];
        /* Nested on purpose: `found = A && B` lets the compiler drop the
         * variable, and the ROM keeps it. */
        int found = 0;
        if (data_020a0de8[idx * 4] != 0) {
            found = data_020a0de9[idx * 4] != 0;
        }
        if (found != 0) {
            short *p;
            func_02012790(0x62);
            unk_592a = 0x1e;
            dScMgMCarlo2_c::FlipDealtCards();
            p = (short *)(raw + 0x5928);
            *p += 1;
        }
        break;
    }
    case 6:
    default:
        if (ApproachLinear2(unk_592a, 0, 1) != 0) {
            return 1;
        }
        break;
    }
    return 0;
}

// @symbol _ZN14dScMgMCarlo2_c8BehaviorEv
/* Slot 6. The state at unk_5928 is bumped through an int-cast pointer each
 * time: plain `unk_5928++` recomputes the address, and the ROM keeps it in a
 * register. data_ov006_0213d700 is the dealt-card count, shifted left 12. */
s32 dScMgMCarlo2_c::Behavior()
{
    unsigned char *raw = (unsigned char *)this;

    switch (unk_5928) {
    case 1:
        {
            s16 *p = (s16 *)((int)raw + 0x5928);
            (*p)++;
        }
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        /* fall through */
    case 2:
        data_ov006_0213d700 = unk_592e * 5 << 12;
        if (unk_592e == 4) {
            if (dScMgMCarlo2_c::BoardBusy() == 0) {
                unk_592e = 0;
                {
                    s16 *p = (s16 *)((int)raw + 0x5928);
                    (*p)++;
                }
            }
        }
        break;
    case 3:
        if (dScMgMCarlo2_c::BoardReady() == 0) {
            int dealt;
            if (data_ov006_0213d6fc == 1)
                data_ov006_0213d6fc = 0;
            dealt = data_ov006_0213d700 >> 12;
            if (data_ov006_0213d6f4 != 0 && dealt > 10 && dealt <= 18
                && func_ov006_020c1718(raw + 0x4f38) != 0) {
                unk_592e = 0;
                mShared.unk_1e6 = 0;
                func_ov006_020c1164(raw + 0x4f38, 2, &unk_592e);
                if (data_ov006_0213d6f4 == 2)
                    mShared.unk_01a = 0;
            } else {
                int lim = unk_592e + 0x12;
                if (dealt >= lim) {
                    int flag = (data_ov006_02142570 != 0 && data_ov006_02142574 != 0);
                    if (flag == 0)
                        data_ov006_0213d700 = lim << 12;
                }
            }
        } else {
            if (data_ov006_0213d6fc == 0) {
                if (dScMgMCarlo2_c::HasRemovablePair() != 0) {
                    data_ov006_0213d6fc = 1;
                } else {
                    if (data_ov006_0213d6f8 != 0) {
                        if (mShared.unk_01a == 1) {
                            mShared.unk_01a = 0;
                        } else if (func_ov006_020c16b4(raw + 0x4f38) != 0) {
                            func_ov006_020c0d68(raw + 0x4f38);
                            func_ov004_020b0a54(0x12);
                            mPromptEnabled = 0;
                            {
                                s16 *p = (s16 *)((int)raw + 0x5928);
                                (*p)++;
                            }
                        }
                    }
                    data_ov006_0213d6fc = 0;
                }
            }
        }
        break;
    }

    func_ov006_020c19d0(raw + 0x4f38);
    dScMgMCarlo2_c::UpdateBoard();
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c6RenderEv
/* Slot 9. Draws the score, then the cards still moving, then the settled
 * ones. Both passes walk mPrev from data_ov006_02142578. */
s32 dScMgMCarlo2_c::Render()
{
    char *raw = (char *)this;
    short score;
    dMgMCarlo2CardObj_c *lifted;
    int i;
    int j;
    dMgMCarlo2CardObj_c *flat;

    func_ov006_020c0aa8(raw + 0x4660);

    score = data_ov006_02142564;
    if (score > 0x270f)
        score = 0x270f;
    func_ov004_020b1ea4(0xe8, 0x28, score, 1, -1, 0, 0);

    RenderOamMainScreen(data_ov006_02133f18, 0xe8, 0x18, -1, -1);

    if (this->unk_5928 == 5)
        func_ov004_020b0d8c(raw, 0xe0, 0xa0);

    lifted = data_ov006_02142578;
    for (i = 0; i < 0x14; i++) {
        if (lifted == 0)
            break;
        if (lifted->mYStep > 0)
            lifted->Render();
        lifted = lifted->mPrev;
    }

    flat = data_ov006_02142578;
    for (j = 0; j < 0x14; j++) {
        if (flat == 0)
            break;
        if (flat->mYStep == 0)
            flat->Render();
        flat = flat->mPrev;
    }

    func_ov006_020c1804(raw + 0x4f38);
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c16CleanupResourcesEv
/* Slot 3. Sibling scenes call func_ov004_020ad90c with no argument; `this`
 * is already in r0 here, so either spelling compiles the same. */
s32 dScMgMCarlo2_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}

// @symbol _ZN19dMgMCarlo2CardObj_c4InitEi
void dMgMCarlo2CardObj_c::Init(int slot){
    mSlot = (short)slot;
    if (slot >= 0x14) {
        mDealDelay = 1;
    } else {
        mDealDelay = (short)(((slot % 5) << 1) + 1);
    }
    mLift = 0;
    if (slot == 0x13) data_ov006_02142578 = this;
    mVisible = 1;
    mXStep = 0;
    mYStep = 0;
    mState = 0;
    mFace = (unsigned char)dScMgMCarlo2_c::DrawCardValue();
    mNext = 0;
    mPrev = mNext;
}

// @symbol _ZN19dMgMCarlo2CardObj_c6DealInEi
void dMgMCarlo2CardObj_c::DealIn(int slot)
{
    int delta[3];

    if (slot >= 0x14)
        return;

    if (mSlot >= 0x14) {
        mX = (0x70 - ((data_ov006_0213d6f4 >> 2) << 1)) << 12;
        mY = -0x30000;
        data_ov006_02142578 = this;
        mState = 1;
    } else if (mState == 0) {
        mX = (0x70 - ((data_ov006_0213d6f4 >> 2) << 1)) << 12;
        mY = -0x30000;
        mState = 4;
        data_ov006_0214255c++;
    } else {
        mState = 4;
        data_ov006_0214255c++;
    }

    mTargetX = ((slot % 5) * 32 + 0x30) << 12;
    mTargetY = ((slot / 5) * 0x30) << 12;
    mSlot = (short)slot;

    Vec2_Sub(delta, &mTargetX, &mX);

    mXStep = delta[0];
    mYStep = delta[1];
    func_0203d630(&mXStep, 0x124);

    if (mXStep < 0)
        mXStep = -mXStep;
    if (mYStep < 0)
        mYStep = -mYStep;
}

// @symbol _ZN19dMgMCarlo2CardObj_c8FlipAwayEi
void dMgMCarlo2CardObj_c::FlipAway(int order) {
    mDealDelay = (short)((4 - order % 5) * 2);
    mState = 5;
}

// @symbol _ZN19dMgMCarlo2CardObj_c10IsPairWithEPS_
int dMgMCarlo2CardObj_c::IsPairWith(dMgMCarlo2CardObj_c *other)
{
    int mine, theirs, dx, dy;
    if (other->mFace != mFace)
        goto fail;
    theirs = other->mSlot;
    mine = mSlot;
    dx = mine % 5 - theirs % 5;
    dy = mine / 5 - theirs / 5;
    if (dx < 0)
        dx = -dx;
    if (dx >= 2)
        goto fail;
    if (dy < 0)
        dy = -dy;
    if (dy >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}

// @symbol _ZN19dMgMCarlo2CardObj_c7HitTestEv
int dMgMCarlo2CardObj_c::HitTest()
{
    u8 idx;
    int off;
    int touching;
    int dx, dy;

    if (data_ov006_0213d6fc == 0) return 0;
    if (dScMgMCarlo2_c::BoardBusy() != 0) goto fail;

    idx = data_020a0e40[0];
    off = idx * 4;
    touching = 0;
    if (data_020a0de8[off]) {
        if (data_020a0de9[off]) touching = 1;
    }
    if (touching == 0) goto fail;

    dx = data_020a0dea[idx * 4] - (mX >> 12);
    dy = data_020a0deb[idx * 4] - (mY >> 12);
    if (dx > 7 && dx < 0x28 && dy > 0 && dy < 0x31) return 1;
fail:
    return 0;
}

// @symbol _ZN19dMgMCarlo2CardObj_c6UpdateEi
void dMgMCarlo2CardObj_c::Update(int slot)
{
    switch (mState) {
    case 0:
        {
            short *p = &mDealDelay;
            short v = *(short *)p;
            *(short *)p = (short)(v - 1);
        }
        if (mDealDelay != 0)
            return;
        data_ov006_0213d6f4--;
        DealIn(slot);
        return;
    case 2:
        if (mSlot != slot) {
            DealIn(slot);
            return;
        }
        if (HitTest() == 0)
            return;
        if (data_ov006_02142570 == 0) {
            data_ov006_02142570 = this;
            mState = 3;
            Sound::PlayBank2_2D(0x153);
            return;
        }
        if (IsPairWith(data_ov006_02142570) != 0) {
            data_ov006_02142574 = this;
            data_ov006_02142558 = 0x20;
            mState = 3;
            Sound::PlayBank2_2D(0x154);
            return;
        }
        data_ov006_02142570->mState = 2;
        data_ov006_02142570 = 0;
        func_02012790(0xe);
        return;
    case 3:
        if (HitTest() == 0)
            return;
        mState = 2;
        if (data_ov006_02142570 != this)
            return;
        if (data_ov006_02142574 != 0)
            return;
        Sound::PlayBank2_2D(0x155);
        data_ov006_02142570 = 0;
        return;
    case 1:
    case 4:
        ApproachLinear(mX, mTargetX, mXStep);
        ApproachLinear(mY, mTargetY, mYStep);
        ApproachLinear(mLift, 0x4000, 0x300);
        if (func_0203d5dc(&mX, &mTargetX) != 0)
            return;
        if (mLift != 0x4000)
            return;
        mState = 2;
        ApproachLinear2(data_ov006_0214255c, 0, 1);
        mXStep = 0;
        mYStep = 0;
        return;
    case 5:
        if (ApproachLinear2(mDealDelay, 0, 1) == 0)
            return;
        ApproachLinear(mX, -0x30000, 0x10000);
    }
}

// @symbol _ZN19dMgMCarlo2CardObj_c6RenderEv
void dMgMCarlo2CardObj_c::Render()
{
    unsigned char state;
    if (mVisible == 0) return;
    state = mState;
    if (state == 0) return;
    if (state == 3) {
        if (((&data_020a0db0)[0] & 8) != 0) return;
    }
    {
        int idx = (mFace + 1) * 5 + (mLift >> 12);
        unsigned short sprite = data_ov006_0213d770[idx];
        Hud_RenderSprite(
            (void*)data_ov006_021425a8[sprite],
            (mX >> 12) + 0x18,
            (mY >> 12) + 0x18,
            -1,
            -1);
    }
}

// @symbol _ZN14dScMgMCarlo2_c10SetupBoardEP19dMgMCarlo2CardObj_c
void dScMgMCarlo2_c::SetupBoard(dMgMCarlo2CardObj_c* base)
{
    do {
        {
            s16 i = 0;
            do {
                data_ov006_02142580[i] = data_ov006_0212e954[i];
                i = i + 1;
            } while (i < 10);
        }
        {
            s16 j = 0;
            dMgMCarlo2CardObj_c* p = base;
            do {
                p->Init(j);
                p = p + 1;
                j = j + 1;
            } while (j < 0x28);
        }
        data_ov006_02142568 = 0;
        data_ov006_0214257c = base;
        data_ov006_0214256c = 0;
        data_ov006_02142570 = 0;
        data_ov006_02142574 = 0;
        data_ov006_02142558 = 0;
        data_ov006_0214255c = 0;
        data_ov006_0213d6f8 = 0x28;
        data_ov006_0213d6f4 = 0x28;
        data_ov006_02142560 = 0;
        data_ov006_02142564 = 0;
        data_ov006_0213d700 = 0;
        {
            s16 i = 0;
            dMgMCarlo2CardObj_c* h = base;
            do {
                dMgMCarlo2CardObj_c* t = &base[i + 1];
                dMgMCarlo2CardObj_c* save = h->mNext;
                h->mNext = t;
                t->mPrev = h;
                {
                    dMgMCarlo2CardObj_c* u = t->mNext;
                    if (u != 0) {
                        do { t = u; u = u->mNext; } while (u != 0);
                    }
                }
                t->mNext = save;
                if (t->mNext == 0) data_ov006_02142568 = t;
                h = h + 1;
                i++;
            } while (i < 0x27);
        }
    } while (HasRemovablePair() == 0);
}

// @symbol _ZN14dScMgMCarlo2_c16HasRemovablePairEv
int dScMgMCarlo2_c::HasRemovablePair()
{
    dMgMCarlo2CardObj_c *p, *q;
    p = data_ov006_0214257c;
    while (p != 0 && p->mSlot < 0x14) {
        q = p->mNext;
        while (q != 0 && q->mSlot < 0x14) {
            if (p->IsPairWith(q) != 0) return 1;
            q = q->mNext;
        }
        p = p->mNext;
    }
    return 0;
}

// @symbol _ZN14dScMgMCarlo2_c9BoardBusyEv
int dScMgMCarlo2_c::BoardBusy() {
    int ret = 1;
    int dealt = (data_ov006_0213d700 << 4) >> 0x10;
    int wanted = data_ov006_0213d6f8;
    if (wanted > 0x14) wanted = 0x14;
    if (dealt == wanted && data_ov006_0214255c == 0) {
        if (data_ov006_02142570 == 0 || data_ov006_02142574 == 0) {
            ret = 0;
        }
    }
    return ret;
}

// @symbol _ZN14dScMgMCarlo2_c10BoardReadyEv
/* One exit on purpose: early `return 0`s cost 8 bytes. */
int dScMgMCarlo2_c::BoardReady() {
    int dealt = (data_ov006_0213d700 << 4) >> 16;
    int ready = 0;
    int wanted = data_ov006_0213d6f8;

    if (wanted > 0x14) {
        wanted = 0x14;
    }
    if (dealt == wanted && data_ov006_0214255c == 0
        && (data_ov006_02142570 == 0 || data_ov006_02142574 == 0)) {
        ready = 1;
    }
    return ready;
}

// @symbol _ZN14dScMgMCarlo2_c13DrawCardValueEv
int dScMgMCarlo2_c::DrawCardValue(){
    unsigned char pick = 0;
    int total = 0;
    int i;
    int roll;
    for(i=0;i<0xa;i++) total += data_ov006_02142580[i];
    roll = (int)(((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * roll) >> 0xc;
    for(i=0;i<0xa;i++){
        total -= data_ov006_02142580[i];
        if(total < 0){
            pick = (unsigned char)i;
            data_ov006_02142580[i]--;
            break;
        }
    }
    return pick;
}

// @symbol _ZN14dScMgMCarlo2_c14FlipDealtCardsEv
void dScMgMCarlo2_c::FlipDealtCards(){
    dMgMCarlo2CardObj_c* node = data_ov006_0214257c;
    short i = 0;
    if((data_ov006_0213d700>>12) <= 0) return;
    do {
        if(node == 0) return;
        node->FlipAway(i);
        i = i+1;
        node = node->mNext;
    } while(i < (data_ov006_0213d700>>12));
}

// @symbol _ZN14dScMgMCarlo2_c11UpdateBoardEv
void dScMgMCarlo2_c::UpdateBoard()
{
    dMgMCarlo2CardObj_c* head = data_ov006_0214257c;

    if (data_ov006_02142570 != 0 && data_ov006_02142574 != 0) {
        data_ov006_02142558 -= 1;
        if (data_ov006_02142558 == 0) {
            dMgMCarlo2CardObj_c* first;
            dMgMCarlo2CardObj_c* second;
            dMgMCarlo2CardObj_c* tail;
            dMgMCarlo2CardObj_c* next;
            s16 secondSlot = data_ov006_02142574->mSlot;
            s16 firstSlot = data_ov006_02142570->mSlot;
            if (firstSlot > secondSlot)
                data_ov006_0213d700 = secondSlot << 12;
            else
                data_ov006_0213d700 = firstSlot << 12;

            first = data_ov006_02142570;
            if (data_ov006_0214257c == first) data_ov006_0214257c = first->mNext;
            if (data_ov006_0214256c == first) data_ov006_0214256c = first->mNext;
            if (data_ov006_02142578 == first) data_ov006_02142578 = first->mPrev;
            if (data_ov006_02142568 == first) data_ov006_02142568 = first->mPrev;
            if (first->mPrev != 0) first->mPrev->mNext = first->mNext;
            if (first->mNext != 0) first->mNext->mPrev = first->mPrev;
            first->mNext = 0;
            first->mPrev = first->mNext;

            second = data_ov006_02142574;
            if (data_ov006_0214257c == second) data_ov006_0214257c = second->mNext;
            if (data_ov006_0214256c == second) data_ov006_0214256c = second->mNext;
            if (data_ov006_02142578 == second) data_ov006_02142578 = second->mPrev;
            if (data_ov006_02142568 == second) data_ov006_02142568 = second->mPrev;
            if (second->mPrev != 0) second->mPrev->mNext = second->mNext;
            if (second->mNext != 0) second->mNext->mPrev = second->mPrev;
            second->mNext = 0;
            second->mPrev = second->mNext;

            first = data_ov006_02142570;
            tail = data_ov006_0214256c;
            if (tail == 0) {
                data_ov006_0214256c = first;
            } else {
                next = tail->mNext;
                if (next != 0) {
                    do { tail = next; next = next->mNext; } while (next != 0);
                }
                tail->mNext = first;
                first->mPrev = tail;
                first->mNext = 0;
            }

            second = data_ov006_02142574;
            tail = data_ov006_0214256c;
            if (tail == 0) {
                data_ov006_0214256c = second;
            } else {
                next = tail->mNext;
                if (next != 0) {
                    do { tail = next; next = next->mNext; } while (next != 0);
                }
                tail->mNext = second;
                second->mPrev = tail;
                second->mNext = 0;
            }

            data_ov006_02142574 = 0;
            data_ov006_02142570 = 0;
            ApproachLinear2(data_ov006_02142564, 0x270f, 2);
            ApproachLinear2(data_ov006_0213d6f8, 0, 2);
            func_ov004_020adb1c(data_ov006_02142564);

            if (data_ov006_0213d6f8 <= 0x14) {
                {
                    int i = 0;
                    s16 idx = *(volatile s16*)&data_ov006_02142560;
                    int* row = (int*)((char*)data_ov006_0212e97c + idx * 0x28);
                    do {
                        data_ov006_02142580[i] = data_ov006_02142580[i] + row[i];
                        i += 1;
                    } while (i < 10);
                }
                {
                    dMgMCarlo2CardObj_c* p = data_ov006_0214256c;
                    if (p != 0) {
                        do {
                            dMgMCarlo2CardObj_c* nxt = p->mNext;
                            if (data_ov006_0214257c == p) data_ov006_0214257c = nxt;
                            if (data_ov006_0214256c == p) data_ov006_0214256c = p->mNext;
                            if (data_ov006_02142578 == p) data_ov006_02142578 = p->mPrev;
                            if (data_ov006_02142568 == p) data_ov006_02142568 = p->mPrev;
                            if (p->mPrev != 0) p->mPrev->mNext = p->mNext;
                            if (p->mNext != 0) p->mNext->mPrev = p->mPrev;
                            p->mNext = 0;
                            p->mPrev = p->mNext;
                            p->Init((s16)(data_ov006_02142568->mSlot + 1));
                            {
                                dMgMCarlo2CardObj_c* h = data_ov006_02142568;
                                dMgMCarlo2CardObj_c* save = h->mNext;
                                h->mNext = p;
                                p->mPrev = h;
                                if (p->mNext != 0) {
                                    do { p = *(dMgMCarlo2CardObj_c* volatile*)&p->mNext; } while (p->mNext != 0);
                                }
                                p->mNext = save;
                                if (p->mNext == 0) data_ov006_02142568 = p;
                            }
                            p = nxt;
                        } while (p != 0);
                    }
                }
                ApproachLinear2(data_ov006_02142560, 9, 1);
                data_ov006_0213d6f4 += 0x14;
                data_ov006_0213d6f8 += 0x14;
            }
        }
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                if (head->mState == 1)
                    head->Update(i);
                i = i + 1;
                head = head->mNext;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    } else {
        int v = data_ov006_0213d6f8;
        if (v > 0x14) v = 0x14;
        ApproachLinear(data_ov006_0213d700, v << 12, 0x800);
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                head->Update(i);
                i = i + 1;
                head = head->mNext;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    }
}

// @symbol _ZN19dMgMCarlo2CardObj_cD1Ev
dMgMCarlo2CardObj_c::~dMgMCarlo2CardObj_c()
{
}

/* The scene's D1 and D0 are not written out: ~dScMgMCarlo2_c is inline in
 * the header, so InitResources is the key function, and the vtable emitted
 * here makes the compiler emit both out of line in cartridge order. */
