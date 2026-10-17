//cpp
/* Pair-match minigame scene 2: 25 functions. The board is 40 card objects
 * on an intrusive doubly-linked list.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. There is no opt_propagation pragma: it applies to the whole
 * file, and turning it off breaks four other functions.
 *
 * leftovers:
 *   - the ov004 helpers and the data_ov006 globals are unnamed in
 *     symbols.txt.
 *   - the ROM has no dScMgMCarlo2_c constructor, so the factory builds the
 *     scene by hand.
 *   - the shared table at 0x4f38 is opaque in dScMgMCarlo2_c.h, so its
 *     helpers take its address.
 */


#include "dScMgMCarlo2_c.h"
#include "dScMgCard_c.h"
#include "types.h"
#include "decl_common.h"
#include "PlayerInput.h"

/* sMCarlo2Cards is the head of the live card list and _02142568 its
 * tail (SetupBoard builds it); _0214256c heads the list of cleared cards,
 * which UpdateBoard deals back onto the tail. _02142570 and _02142574 are the
 * first and second card picked, and _02142578 is the card in slot 19, the
 * last one on the board, where the draw passes start. */

/* Globals that decl_common.h does not already declare with these types. */
extern "C" {
extern dMgMCarlo2CardObj_c *sMCarlo2CardTail;
extern dMgMCarlo2CardObj_c *sMCarlo2CardHead;
extern dMgMCarlo2CardObj_c *sMCarlo2FirstPick;
extern dMgMCarlo2CardObj_c *sMCarlo2SecondPick;
extern dMgMCarlo2CardObj_c *sMCarlo2CardsTail;
extern dMgMCarlo2CardObj_c *sMCarlo2Cards;
extern s16 sMCarlo2BoardDelay;   /* frames left before the pair resolves  */
extern s16 sMCarlo2CardsMoving;  /* pieces still animating                */
extern s16 sMCarlo2BoardStep;    /* difficulty row into sMCarlo2BoardRows */
extern int sMCarlo2FaceCounts[]; /* per-face weights the picker draws from  */
/* Twenty-three sprite words. The static initializer copies them out of
 * sMgCardSprites in this order; an in-order copy does not. */
struct Carlo2SpriteWords {
    int a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22;
};
extern int sMgCardSprites[];
extern Carlo2SpriteWords sMCarlo2CardSprites;
extern unsigned short sMCarlo2CardFrameSprites[];
extern int sMCarlo2StartFaceCounts[]; /* the starting weights                     */
extern int sMCarlo2BoardRows[][10];   /* per-difficulty weight top-ups          */
extern int data_0209e650[];
extern int data_020a0db0;
extern unsigned char data_0209d454;
extern unsigned char data_0209d45c;

int  RandomIntInternal(void* seed);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
int  GetGameLanguage(void);
int  LoadFile(int handle);
void DecompressLZ16(int src, int dst);
void Vec2_Sub(int* out, int* a, int* b);
unsigned int func_02012790(unsigned int id);
int  func_0203d5dc(void* a, void* b);
void Mg_CommitRecords(void* t);
void Mg_SubmitScore(int score);
void LoadCard2SubBg3(void* c);
void Camera_InitDealerView(void* c);
void Dealer_StartShuffle(char *t, int unused, short a2, void *a3);
int  Dealer_IsIdle(void* t);
void Dealer_Draw(void* t);
void Dealer_Update(void* t);
int  Dealer_Load(void* t);

void *_ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
void __cxa_vec_ctor(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV14dScMgMCarlo2_c[];
void *_ZN19dMgMCarlo2CardObj_cC1Ev(void*);
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
        Dealer_Construct(p + 0x4f38);
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
    Mg_SetTopScreenYOffset(0x20);
    LoadCard2SubBg3(raw);
    lang = GetGameLanguage();
    tiles = LoadFile(sMCarlo2CardNcgByLanguage[lang]);
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
    Camera_InitDealerView((void *)(raw + 0x4660));
    if (Dealer_Load((void *)(raw + 0x4f38)) == 0) return 0;
    Mg_PlaceBannersOnTopScreen();
    StartRound(-1);
    this->mTimer = 0;
    this->mCredits = 0xa;
    this->mCreditsShown = this->mCredits;
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c10StartRoundEi
/* Resets the whole board: rebuild the 40 pieces, clear the match latch,
 * re-arm the shared table, then hand the score display a zero. */
void dScMgMCarlo2_c::StartRound(int /* arg */)
{
    char *raw = (char *)this;

    dScMgMCarlo2_c::SetupBoard(mArray);
    sMCarlo2PickAllowed = 0;
    mDealCount = 0;
    mShared.unk_1e6 = 1;
    Dealer_StartShuffle(raw + 0x4f38, 4, 4, raw + 0x592e);
    mShared.unk_01a = 1;
    mTimer = 0;
    Mg_SubmitScore(0);
    mRoundState = 1;
}

// @symbol _ZN14dScMgMCarlo2_c11CanEndStateEi
int dScMgMCarlo2_c::CanEndState(int /* mode */)
{
    char *raw = (char *)this;

    short state = mRoundState;
    switch (state) {
    case 4:
        if (Dealer_IsIdle((int *)(raw + 0x4f38)) != 0) {
            short *p = &mRoundState;
            *p += 1;
        }
        break;
    case 5: {
        unsigned char idx = gActivePlayerSlot;
        /* Nested on purpose: `found = A && B` lets the compiler drop the
         * variable, and the ROM keeps it. */
        int found = 0;
        if (gTouchHeld[idx * 4] != 0) {
            found = gTouchEdge[idx * 4] != 0;
        }
        if (found != 0) {
            short *p;
            func_02012790(0x62);
            mTimer = 0x1e;
            dScMgMCarlo2_c::FlipDealtCards();
            p = &mRoundState;
            *p += 1;
        }
        break;
    }
    case 6:
    default:
        if (ApproachLinear2(mTimer, 0, 1) != 0) {
            return 1;
        }
        break;
    }
    return 0;
}

// @symbol _ZN14dScMgMCarlo2_c8BehaviorEv
/* Slot 6. The state at mRoundState is bumped through an int-cast pointer each
 * time: plain `mRoundState++` recomputes the address, and the ROM keeps it in a
 * register. sMCarlo2DealProgress is the dealt-card count, shifted left 12. */
s32 dScMgMCarlo2_c::Behavior()
{
    unsigned char *raw = (unsigned char *)this;

    switch (mRoundState) {
    case 1:
        {
        s16 *p = &mRoundState;
        (*p)++;
        }
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        /* fall through */
    case 2:
        sMCarlo2DealProgress = mDealCount * 5 << 12;
        if (mDealCount == 4) {
            if (dScMgMCarlo2_c::BoardBusy() == 0) {
                mDealCount = 0;
                {
                    s16 *p = &mRoundState;
                    (*p)++;
                }
            }
        }
        break;
    case 3:
        if (dScMgMCarlo2_c::BoardReady() == 0) {
            int dealt;
            if (sMCarlo2PickAllowed == 1)
                sMCarlo2PickAllowed = 0;
            dealt = sMCarlo2DealProgress >> 12;
            if (sMCarlo2DeckSize != 0 && dealt > 10 && dealt <= 18 &&
                Dealer_IsIdle(raw + 0x4f38) != 0) {
                mDealCount = 0;
                mShared.unk_1e6 = 0;
                Dealer_StartReveal(raw + 0x4f38, 2, &mDealCount);
                if (sMCarlo2DeckSize == 2)
                    mShared.unk_01a = 0;
            } else {
                int lim = mDealCount + 0x12;
                if (dealt >= lim) {
                    int flag = (sMCarlo2FirstPick != 0 && sMCarlo2SecondPick != 0);
                    if (flag == 0)
                        sMCarlo2DealProgress = lim << 12;
                }
            }
        } else {
            if (sMCarlo2PickAllowed == 0) {
                if (dScMgMCarlo2_c::HasRemovablePair() != 0) {
                    sMCarlo2PickAllowed = 1;
                } else {
                    if (sMCarlo2CardsWanted != 0) {
                        if (mShared.unk_01a == 1) {
                            mShared.unk_01a = 0;
                        } else if (Dealer_IsIdleAtRest(raw + 0x4f38) != 0) {
                            Dealer_PlayLose(raw + 0x4f38);
                            Mg_SetSharedState(MG_STATE_GAME_OVER);
                            mPromptEnabled = 0;
                            {
                                s16 *p = &mRoundState;
                                (*p)++;
                            }
                        }
                    }
                    sMCarlo2PickAllowed = 0;
                }
            }
        }
        break;
    }

    Dealer_Update(raw + 0x4f38);
    dScMgMCarlo2_c::UpdateBoard();
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c6RenderEv
/* Slot 9. Draws the score, then the cards still moving, then the settled
 * ones. Both passes walk mPrev from sMCarlo2CardsTail. */
s32 dScMgMCarlo2_c::Render()
{
    char *raw = (char *)this;
    short score;
    dMgMCarlo2CardObj_c *lifted;
    int i;
    int j;
    dMgMCarlo2CardObj_c *flat;

    Camera_InitDealerView(raw + 0x4660);

    score = sMCarlo2Score;
    if (score > 0x270f)
        score = 0x270f;
    Mg_DrawNumber(0xe8, 0x28, score, 1, -1, 0, 0);

    RenderOamMainScreen(sMCarloHudSprite, 0xe8, 0x18, -1, -1);

    if (this->mRoundState == 5)
        Mg_DrawBlinkSprite(raw, 0xe0, 0xa0);

    lifted = sMCarlo2CardsTail;
    for (i = 0; i < 0x14; i++) {
        if (lifted == 0)
            break;
        if (lifted->mYStep > 0)
            lifted->Render();
        lifted = lifted->mPrev;
    }

    flat = sMCarlo2CardsTail;
    for (j = 0; j < 0x14; j++) {
        if (flat == 0)
            break;
        if (flat->mYStep == 0)
            flat->Render();
        flat = flat->mPrev;
    }

    Dealer_Draw(raw + 0x4f38);
    return 1;
}

// @symbol _ZN14dScMgMCarlo2_c16CleanupResourcesEv
/* Slot 3. Sibling scenes call Mg_CommitRecords with no argument; `this`
 * is already in r0 here, so either spelling compiles the same. */
s32 dScMgMCarlo2_c::CleanupResources()
{
    Mg_CommitRecords(this);
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
    if (slot == 0x13)
        sMCarlo2CardsTail = this;
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
        mX = (0x70 - ((sMCarlo2DeckSize >> 2) << 1)) << 12;
        mY = -0x30000;
        sMCarlo2CardsTail = this;
        mState = 1;
    } else if (mState == 0) {
        mX = (0x70 - ((sMCarlo2DeckSize >> 2) << 1)) << 12;
        mY = -0x30000;
        mState = 4;
        sMCarlo2CardsMoving++;
    } else {
        mState = 4;
        sMCarlo2CardsMoving++;
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

    if (sMCarlo2PickAllowed == 0)
        return 0;
    if (dScMgMCarlo2_c::BoardBusy() != 0) goto fail;

    idx = gActivePlayerSlot;
    off = idx * 4;
    touching = 0;
    if (gTouchHeld[off]) {
        if (gTouchEdge[off]) touching = 1;
    }
    if (touching == 0) goto fail;

    dx = gTouchX[idx * 4] - (mX >> 12);
    dy = gTouchY[idx * 4] - (mY >> 12);
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
        sMCarlo2DeckSize--;
        DealIn(slot);
        return;
    case 2:
        if (mSlot != slot) {
            DealIn(slot);
            return;
        }
        if (HitTest() == 0)
            return;
        if (sMCarlo2FirstPick == 0) {
            sMCarlo2FirstPick = this;
            mState = 3;
            Sound::PlayBank2_2D(0x153);
            return;
        }
        if (IsPairWith(sMCarlo2FirstPick) != 0) {
            sMCarlo2SecondPick = this;
            sMCarlo2BoardDelay = 0x20;
            mState = 3;
            Sound::PlayBank2_2D(0x154);
            return;
        }
        sMCarlo2FirstPick->mState = 2;
        sMCarlo2FirstPick = 0;
        func_02012790(0xe);
        return;
    case 3:
        if (HitTest() == 0)
            return;
        mState = 2;
        if (sMCarlo2FirstPick != this)
            return;
        if (sMCarlo2SecondPick != 0)
            return;
        Sound::PlayBank2_2D(0x155);
        sMCarlo2FirstPick = 0;
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
        ApproachLinear2(sMCarlo2CardsMoving, 0, 1);
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
        unsigned short sprite = sMCarlo2CardFrameSprites[idx];
        Hud_RenderSprite((void *)((int *)&sMCarlo2CardSprites)[sprite], (mX >> 12) + 0x18,
                         (mY >> 12) + 0x18, -1, -1);
    }
}

// @symbol _ZN14dScMgMCarlo2_c10SetupBoardEP19dMgMCarlo2CardObj_c
void dScMgMCarlo2_c::SetupBoard(dMgMCarlo2CardObj_c* base)
{
    do {
        {
            s16 i = 0;
            do {
                sMCarlo2FaceCounts[i] = sMCarlo2StartFaceCounts[i];
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
        sMCarlo2CardTail = 0;
        sMCarlo2Cards = base;
        sMCarlo2CardHead = 0;
        sMCarlo2FirstPick = 0;
        sMCarlo2SecondPick = 0;
        sMCarlo2BoardDelay = 0;
        sMCarlo2CardsMoving = 0;
        sMCarlo2CardsWanted = 0x28;
        sMCarlo2DeckSize = 0x28;
        sMCarlo2BoardStep = 0;
        sMCarlo2Score = 0;
        sMCarlo2DealProgress = 0;
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
                if (t->mNext == 0)
                    sMCarlo2CardTail = t;
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
    p = sMCarlo2Cards;
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
    int dealt = (sMCarlo2DealProgress << 4) >> 0x10;
    int wanted = sMCarlo2CardsWanted;
    if (wanted > 0x14) wanted = 0x14;
    if (dealt == wanted && sMCarlo2CardsMoving == 0) {
        if (sMCarlo2FirstPick == 0 || sMCarlo2SecondPick == 0) {
            ret = 0;
        }
    }
    return ret;
}

// @symbol _ZN14dScMgMCarlo2_c10BoardReadyEv
/* One exit on purpose: early `return 0`s cost 8 bytes. */
int dScMgMCarlo2_c::BoardReady() {
    int dealt = (sMCarlo2DealProgress << 4) >> 16;
    int ready = 0;
    int wanted = sMCarlo2CardsWanted;

    if (wanted > 0x14) {
        wanted = 0x14;
    }
    if (dealt == wanted && sMCarlo2CardsMoving == 0 &&
        (sMCarlo2FirstPick == 0 || sMCarlo2SecondPick == 0)) {
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
    for (i = 0; i < 0xa; i++)
        total += sMCarlo2FaceCounts[i];
    roll = (int)(((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * roll) >> 0xc;
    for(i=0;i<0xa;i++){
        total -= sMCarlo2FaceCounts[i];
        if(total < 0){
            pick = (unsigned char)i;
            sMCarlo2FaceCounts[i]--;
            break;
        }
    }
    return pick;
}

// @symbol _ZN14dScMgMCarlo2_c14FlipDealtCardsEv
void dScMgMCarlo2_c::FlipDealtCards(){
    dMgMCarlo2CardObj_c *node = sMCarlo2Cards;
    short i = 0;
    if ((sMCarlo2DealProgress >> 12) <= 0)
        return;
    do {
        if(node == 0) return;
        node->FlipAway(i);
        i = i+1;
        node = node->mNext;
    } while (i < (sMCarlo2DealProgress >> 12));
}

// @symbol _ZN14dScMgMCarlo2_c11UpdateBoardEv
void dScMgMCarlo2_c::UpdateBoard()
{
    dMgMCarlo2CardObj_c *head = sMCarlo2Cards;

    if (sMCarlo2FirstPick != 0 && sMCarlo2SecondPick != 0) {
        sMCarlo2BoardDelay -= 1;
        if (sMCarlo2BoardDelay == 0) {
            dMgMCarlo2CardObj_c* first;
            dMgMCarlo2CardObj_c* second;
            dMgMCarlo2CardObj_c* tail;
            dMgMCarlo2CardObj_c* next;
            s16 secondSlot = sMCarlo2SecondPick->mSlot;
            s16 firstSlot = sMCarlo2FirstPick->mSlot;
            if (firstSlot > secondSlot)
                sMCarlo2DealProgress = secondSlot << 12;
            else
                sMCarlo2DealProgress = firstSlot << 12;

            first = sMCarlo2FirstPick;
            if (sMCarlo2Cards == first)
                sMCarlo2Cards = first->mNext;
            if (sMCarlo2CardHead == first)
                sMCarlo2CardHead = first->mNext;
            if (sMCarlo2CardsTail == first)
                sMCarlo2CardsTail = first->mPrev;
            if (sMCarlo2CardTail == first)
                sMCarlo2CardTail = first->mPrev;
            if (first->mPrev != 0) first->mPrev->mNext = first->mNext;
            if (first->mNext != 0) first->mNext->mPrev = first->mPrev;
            first->mNext = 0;
            first->mPrev = first->mNext;

            second = sMCarlo2SecondPick;
            if (sMCarlo2Cards == second)
                sMCarlo2Cards = second->mNext;
            if (sMCarlo2CardHead == second)
                sMCarlo2CardHead = second->mNext;
            if (sMCarlo2CardsTail == second)
                sMCarlo2CardsTail = second->mPrev;
            if (sMCarlo2CardTail == second)
                sMCarlo2CardTail = second->mPrev;
            if (second->mPrev != 0) second->mPrev->mNext = second->mNext;
            if (second->mNext != 0) second->mNext->mPrev = second->mPrev;
            second->mNext = 0;
            second->mPrev = second->mNext;

            first = sMCarlo2FirstPick;
            tail = sMCarlo2CardHead;
            if (tail == 0) {
                sMCarlo2CardHead = first;
            } else {
                next = tail->mNext;
                if (next != 0) {
                    do { tail = next; next = next->mNext; } while (next != 0);
                }
                tail->mNext = first;
                first->mPrev = tail;
                first->mNext = 0;
            }

            second = sMCarlo2SecondPick;
            tail = sMCarlo2CardHead;
            if (tail == 0) {
                sMCarlo2CardHead = second;
            } else {
                next = tail->mNext;
                if (next != 0) {
                    do { tail = next; next = next->mNext; } while (next != 0);
                }
                tail->mNext = second;
                second->mPrev = tail;
                second->mNext = 0;
            }

            sMCarlo2SecondPick = 0;
            sMCarlo2FirstPick = 0;
            ApproachLinear2(sMCarlo2Score, 0x270f, 2);
            ApproachLinear2(sMCarlo2CardsWanted, 0, 2);
            Mg_SubmitScore(sMCarlo2Score);

            if (sMCarlo2CardsWanted <= 0x14) {
                {
                    int i = 0;
                    s16 idx = *(volatile s16 *)&sMCarlo2BoardStep;
                    int *row = (int *)((char *)sMCarlo2BoardRows + idx * 0x28);
                    do {
                        sMCarlo2FaceCounts[i] = sMCarlo2FaceCounts[i] + row[i];
                        i += 1;
                    } while (i < 10);
                }
                {
                    dMgMCarlo2CardObj_c *p = sMCarlo2CardHead;
                    if (p != 0) {
                        do {
                            dMgMCarlo2CardObj_c* nxt = p->mNext;
                            if (sMCarlo2Cards == p)
                                sMCarlo2Cards = nxt;
                            if (sMCarlo2CardHead == p)
                                sMCarlo2CardHead = p->mNext;
                            if (sMCarlo2CardsTail == p)
                                sMCarlo2CardsTail = p->mPrev;
                            if (sMCarlo2CardTail == p)
                                sMCarlo2CardTail = p->mPrev;
                            if (p->mPrev != 0) p->mPrev->mNext = p->mNext;
                            if (p->mNext != 0) p->mNext->mPrev = p->mPrev;
                            p->mNext = 0;
                            p->mPrev = p->mNext;
                            p->Init((s16)(sMCarlo2CardTail->mSlot + 1));
                            {
                                dMgMCarlo2CardObj_c *h = sMCarlo2CardTail;
                                dMgMCarlo2CardObj_c* save = h->mNext;
                                h->mNext = p;
                                p->mPrev = h;
                                if (p->mNext != 0) {
                                    do { p = *(dMgMCarlo2CardObj_c* volatile*)&p->mNext; } while (p->mNext != 0);
                                }
                                p->mNext = save;
                                if (p->mNext == 0)
                                    sMCarlo2CardTail = p;
                            }
                            p = nxt;
                        } while (p != 0);
                    }
                }
                ApproachLinear2(sMCarlo2BoardStep, 9, 1);
                sMCarlo2DeckSize += 0x14;
                sMCarlo2CardsWanted += 0x14;
            }
        }
        {
            s16 i = 0;
            if ((sMCarlo2DealProgress >> 12) <= 0)
                return;
            do {
                if (head == 0) return;
                if (head->mState == 1)
                    head->Update(i);
                i = i + 1;
                head = head->mNext;
            } while (i < (sMCarlo2DealProgress >> 12));
        }
    } else {
        int v = sMCarlo2CardsWanted;
        if (v > 0x14) v = 0x14;
        ApproachLinear(sMCarlo2DealProgress, v << 12, 0x800);
        {
            s16 i = 0;
            if ((sMCarlo2DealProgress >> 12) <= 0)
                return;
            do {
                if (head == 0) return;
                head->Update(i);
                i = i + 1;
                head = head->mNext;
            } while (i < (sMCarlo2DealProgress >> 12));
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

/* Retail copy order is not address order. mwcc emits __sinit_d_s_mg_m_carlo2.cpp. */
Carlo2SpriteWords sMCarlo2CardSprites = {
    sMgCardSprites[0],  sMgCardSprites[1],  sMgCardSprites[2],  sMgCardSprites[11],
    sMgCardSprites[12], sMgCardSprites[13], sMgCardSprites[14], sMgCardSprites[3],
    sMgCardSprites[4],  sMgCardSprites[5],  sMgCardSprites[6],  sMgCardSprites[9],
    sMgCardSprites[10], sMgCardSprites[7],  sMgCardSprites[8],  sMgCardSprites[15],
    sMgCardSprites[16], sMgCardSprites[17], sMgCardSprites[18], sMgCardSprites[19],
    sMgCardSprites[20], sMgCardSprites[21], sMgCardSprites[22],
};
