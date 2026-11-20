//cpp
/* unit020b4aa4 -- ov004 unit 0x020b4aa4..0x020b67e8: 43 functions, .text only.
 *
 * One translation unit of the shared minigame overlay, between unit020b2c84
 * and dMgState_c. No ROM RTTI or vtable falls in the range, so no class name
 * is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. sMgCoins is an array of twenty 0x24-byte
 * Sprite records. Each one runs its own member-pointer state every frame
 * (MgCoins_Update through Sprite::Update) and, while visible, is
 * drawn at its position by MgCoins_Draw. The Sprite states slide a
 * sprite from its position to a target by a fixed step, then hand it the
 * next state, named as a real &Sprite:: member pointer (mwcc emits each use
 * as its own 8-byte {fn, 0} .data constant, 0x020bc7d8..0x020bc828). Over the array
 * runs a second machine, one plain function pointer at sMgCoinsStep
 * that MgCoins_Update calls once a frame and that each step replaces
 * (or clears) when it is done. The external entry points that start a
 * sequence are MgCoins_Reset (reset), MgCoins_StartPayout,
 * MgCoins_StartLose, MgCoins_StartSpend, MgCoins_StartLoseOneMore,
 * MgCoins_StartRefund, MgCoins_RaiseBet and MgCoins_PlaceBet.
 * MgCoins_AddBetCoin is internal: its one caller is the touch check in
 * MgCoins_Update.
 *
 * Edges: 0x020b4aa4 is the first function after func_ov004_020b4aa0, the
 * last destructor helper __sinit_ov004_020b955c registers for the previous
 * unit. 0x020b67e8 is the end of Sprite's inline constructor, the last
 * function this unit's own static initializer (0x020b9ad0) references; the
 * array destructor and Sprite's destructor sit just before it. The next
 * function reads the .data block that starts at 0x020bc850, outside this
 * unit's .data window (0x020bc7d0..0x020bc83c).
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The MgCoins_ functions keep their ROM names (C++ linkage, as their symbols
 * show); the per-sprite functions are Sprite members.
 *
 * Retired one-function sources (ROM address order):
 *   0x020b4aa4  Coin_FlyToCounter (now Sprite::FlyToCounter)
 *   0x020b4b84  Coin_PlacePayout (now Sprite::PlacePayout)
 *   0x020b4c30  Coin_MoveThenRefund (now Sprite::MoveThenRefund)
 *   0x020b4cc4  Coin_Return (now Sprite::Return)
 *   0x020b4d50  Coin_RiseSpent (now Sprite::RiseSpent)
 *   0x020b4dfc  Coin_MoveThenRiseSpent (now Sprite::MoveThenRiseSpent)
 *   0x020b4e78  Coin_Spend (now Sprite::Spend)
 *   0x020b4f44  Coin_Rise (now Sprite::Rise)
 *   0x020b4ff0  Coin_MoveThenRise (now Sprite::MoveThenRise)
 *   0x020b506c  Coin_SendOff (now Sprite::SendOff)
 *   0x020b5108  Coin_Launch (now Sprite::Launch)
 *   0x020b51e4  Coin_Hide (now Sprite::Hide)
 *   0x020b51f0  Coin_MoveThenBank (now Sprite::MoveThenBank)
 *   0x020b5288  Coin_Move (now Sprite::Move)
 *   0x020b52b8  Coin_Draw (now Sprite::Draw)
 *   0x020b52fc  Coin_Update (now Sprite::Update)
 *   0x020b5334  Coin_Reset (now Sprite::Reset)
 *   0x020b5368  MgCoins_StepPayoutFinish
 *   0x020b53f0  MgCoins_StepPayoutNext
 *   0x020b556c  MgCoins_StepPayoutLayout
 *   0x020b56c8  MgCoins_StartPayout
 *   0x020b5768  MgCoins_StepRefund
 *   0x020b586c  MgCoins_StepSpendHold
 *   0x020b58c4  MgCoins_StepSpend
 *   0x020b5a54  MgCoins_StepHoldThenFinish
 *   0x020b5aac  MgCoins_StepLoseOneMore
 *   0x020b5c18  MgCoins_StepLose
 *   0x020b5d74  MgCoins_StartRefund
 *   0x020b5dd4  MgCoins_StartLoseOneMore
 *   0x020b5e40  MgCoins_StartSpend
 *   0x020b5ed0  MgCoins_StartLose
 *   0x020b5f6c  MgCoins_StepLaunchQuick
 *   0x020b612c  MgCoins_StepLaunch
 *   0x020b6234  MgCoins_AddBetCoin
 *   0x020b6324  MgCoins_RaiseBet
 *   0x020b63a0  MgCoins_PlaceBet
 *   0x020b6430  MgCoins_Draw
 *   0x020b653c  MgCoins_DrawDelta
 *   0x020b65e4  MgCoins_Update
 *   0x020b66d4  MgCoins_Reset
 *   0x020b67bc  MgCoins_Cleanup (now __arraydtor$192)
 *   0x020b67e0  Coin_Destroy (now Sprite::~Sprite)
 *   0x020b67e4  Coin_Construct (now Sprite::Sprite)
 */

#include "types.h"
#include "PlayerInput.h"
#include "dScMgBase_c.h"
#include "OamAttr.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
#include "Vector2.h"
#include "Vector2Math.h"

/* One of the twenty records at sMgCoins. Positions are fx32
   screen pixels (Draw puts the sprite at mPos >> 12). */
struct Sprite;
typedef void (Sprite::*SpriteFn)();
struct Sprite {
    Sprite();
    ~Sprite();

    void Reset();
    void Update();
    void Draw();
    void Hide();
    void Launch(Vector2 *);
    void SendOff();
    void Spend();
    void Return();
    void PlacePayout(Vector2 *);

    /* The states Update runs through mState. */
    void Move();
    void MoveThenBank();
    void MoveThenRise();
    void Rise();
    void MoveThenRiseSpent();
    void RiseSpent();
    void MoveThenRefund();
    void FlyToCounter();

    SpriteFn mState;   /* 0x00 run every frame by Update */
    Vector2 mPos;      /* 0x08 */
    Vector2 mTarget;   /* 0x10 */
    Vector2 mStep;     /* 0x18 per-frame step toward mTarget, both positive */
    s16 mDelay;        /* 0x20 frames to wait; also the rise counter */
    u8 mVisible;       /* 0x22 drawn while nonzero */
};

#ifndef SM64DS_PLATFORM_PC
typedef char Sprite_size_must_be_0x24[sizeof(Sprite) == 0x24 ? 1 : -1];
#endif


extern "C" {
extern int GetGameLanguage(void);
extern void Hud_RenderSprite(OamAttr *a0, int a1, int a2, int a3, int a4);

extern void Mg_GainCredits(int c);
extern void Mg_LoseCredits(int x);
extern void Mg_DrawCreditsIcon(int a, int b, int c, int d);
extern int Mg_GetTopScreenYOffset(void);
extern int Mg_GetScoringMode(void);
extern void Mg_SubmitScore(int score);
extern void Mg_RenderSubSpriteMtx(OamAttr *a, int b, int c, struct Matrix2x2 *d);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);

/* const folds Coin_Rise and Coin_RiseSpent. This int has to stay a load, in .rodata. */
#pragma define_section coinframes ".rodata" ".rodata" ".rodata" far_data
#pragma section coinframes begin
int sMgCoinRiseFrames = 20;
#pragma section coinframes end
int sMgCoinsBetCap = 5;    /* cap MgCoins_AddBetCoin raises the count to */
int gMgCoinsShowDelta = 1; /* nonzero: sequences also show their count */
extern OamAttr *data_ov006_021338c4[];
extern OamAttr *data_ov006_021338ac[];
extern OamAttr *data_ov006_021338b8[];
extern OamAttr *data_ov006_021338a0[];
extern OamAttr *data_ov006_021338d0[];
OamAttr **sMgCoinsButtonSprites[] = {
    data_ov006_021338c4, data_ov006_021338ac, data_ov006_021338b8,
    data_ov006_021338a0, data_ov006_021338d0,
};
extern OamAttr *sMgTextSpritesEnglish[];
extern OamAttr *sMgTextSpritesSpanish[];
extern OamAttr *sMgTextSpritesFrench[];
extern OamAttr *sMgTextSpritesGerman[];
extern OamAttr *sMgTextSpritesItalian[];
OamAttr **sMgCoinsDeltaSprites[] = {
    sMgTextSpritesEnglish, sMgTextSpritesSpanish, sMgTextSpritesFrench,
    sMgTextSpritesGerman,  sMgTextSpritesItalian,
};
extern short gMgCoinsStatus; /* 1 when a sequence finishes; 2, 3, 4 while one runs */
extern int sMgCoinsDelta;   /* the count MgCoins_DrawDelta shows */
extern int sMgCoinsBetPlaced;   /* set once by MgCoins_PlaceBet */
extern int sMgCoinsSoundPending;   /* the sequence's sound is still to play */
extern int sMgCoinsLastLostAlt;   /* last sprite MgCoins_StepLoseOneMore stepped */
extern int sMgCoinsPayoutLeft;   /* count MgCoins_StartPayout lays out */
extern int sMgCoinsToStep;   /* sprites still to step */
extern int sMgCoinsIndex;   /* index of the sprite being stepped */
extern int sMgCoinsWait;   /* frames until the next step */
extern int sMgCoinsLastReturned;   /* last sprite MgCoins_StepRefund stepped */
extern int sMgCoinsLastSpent;   /* last sprite MgCoins_StepSpend stepped */
extern int sMgCoinsLaunchPending;   /* a MgCoins_StepLaunchQuick run is pending */
extern int sMgCoinsLastLost;   /* last sprite MgCoins_StepLose stepped */
extern int gMgCoinsInPlay;   /* sprites in play */
extern int sMgCoinsSlowLaunches;   /* MgCoins_StepLaunchQuick steps this many with a delay */
extern void (*sMgCoinsStep)(); /* the current step; 0 when idle */
extern int sMgCoinsDeltaShown;   /* nonzero: MgCoins_Draw shows the count */
extern int gGlobalTimer;
}
extern void MgCoins_StepPayoutFinish(void);
extern void MgCoins_StepPayoutNext(void);
extern void MgCoins_StepPayoutLayout(void);
extern void MgCoins_StepSpendHold(void);
extern void MgCoins_StepSpend(void);
extern void MgCoins_StepHoldThenFinish(void);
extern void MgCoins_StepLoseOneMore(void);
extern void MgCoins_StepLose(void);
extern void MgCoins_StepRefund(void);
extern void MgCoins_StepLaunchQuick(void);
extern void MgCoins_StepLaunch(void);
extern void MgCoins_AddBetCoin(void);
extern void MgCoins_DrawDelta(int arg);

/* The twenty sprites. mwcc builds them in this file's static initializer
   (retail 0x020b9ad0) with __cxa_vec_ctor and registers their cleanup
   thunk, __arraydtor, which it emits after the initializer. */
Sprite sMgCoins[0x14];

/* Reset every sprite and the sequence state. */
void MgCoins_Reset(void) {
    int i;
    for (i = 0; i < 20; i++)
        sMgCoins[i].Reset();
    sMgCoinsIndex = 0;
    gMgCoinsInPlay = 0;
    sMgCoinsBetCap = 5;
    sMgCoinsToStep = 0;
    sMgCoinsPayoutLeft = 0;
    sMgCoinsWait = 0;
    gMgCoinsStatus = 0;
    sMgCoinsBetPlaced = 0;
    sMgCoinsLaunchPending = 0;
    sMgCoinsSoundPending = 0;
    sMgCoinsDelta = 0;
    sMgCoinsDeltaShown = 0;
    gMgCoinsShowDelta = 0;
    sMgCoinsStep = 0;
}

/* One frame. A fresh touch inside the top-left 0x40 x 0x50 box adds a
   sprite (MgCoins_AddBetCoin) while that is allowed; then every sprite
   runs its state and the current step runs. */
void MgCoins_Update(void) {
    int i;
    Sprite *p;

    if (sMgCoinsBetPlaced == 0 && gMgCoinsInPlay != 0) {
        int idx = gActivePlayerSlot;
        int flag = gTouchInfo[idx].held != 0 && gTouchInfo[idx].edge != 0;
        if (flag) {
            int a = gTouchInfo[idx].x;
            int b = gTouchInfo[idx].y;
            if (a < 0x40 && b < 0x50) {
                MgCoins_AddBetCoin();
            }
        }
    }

    p = sMgCoins;
    for (i = 0; i < 20; i++) {
        p->Update();
        p++;
    }

    if (sMgCoinsStep != 0) {
        sMgCoinsStep();
    }
}

/* Draw the signed count on row 0x60: the sprite icon (Mg_DrawCreditsIcon)
   at x 0x6e, the per-language +0x34 graphic for arg >= 0 or +0x38 for
   arg < 0 at x 0x80, then |arg| at x 0x92. */
void MgCoins_DrawDelta(int arg) {
    int absv;
    int r4;
    OamAttr *gfx;

    if (arg < 0) absv = -arg; else absv = arg;

    if (arg >= 0) {
        int idx = GetGameLanguage();
        r4 = 0x14;
        gfx = sMgCoinsDeltaSprites[idx][13];
    } else {
        int idx = GetGameLanguage();
        r4 = 0x28;
        gfx = sMgCoinsDeltaSprites[idx][14];
    }

    Mg_DrawCreditsIcon(0x6e, 0x60, 0, -1);
    Mg_RenderSubSpriteMtx(gfx, 0x80, 0x60, 0);
    Mg_DrawBigNumber(0x92, 0x60, absv, 0, -1, 2, r4);
}

/* Draw: the touch button at (0x14, 0x30) while MgCoins_Update would
   take a touch, the count while sMgCoinsDeltaShown is set, then every
   sprite. */
void MgCoins_Draw(void) {
    int i;
    Sprite *p;
    if (sMgCoinsBetPlaced == 0 && gMgCoinsInPlay != 0) {
        if (gGlobalTimer & 0x10) {
            Hud_RenderSprite(sMgCoinsButtonSprites[GetGameLanguage()][0], 0x14, 0x30, -1, -1);
        } else {
            Hud_RenderSprite(sMgCoinsButtonSprites[GetGameLanguage()][1], 0x14, 0x30, -1, -1);
        }
        Hud_RenderSprite(sMgCoinsButtonSprites[GetGameLanguage()][2], 0x14, 0x30, -1, -1);
    }
    if (sMgCoinsDeltaShown != 0) {
        MgCoins_DrawDelta(sMgCoinsDelta);
    }
    for (p = sMgCoins, i = 0; i < 20; i++) {
        p->Draw();
        p++;
    }
}

/* Once: add up to n sprites, capped by the scene's mCredits, and start
   launching them (MgCoins_StepLaunch). */
void MgCoins_PlaceBet(int n) {
    dScMgBase_c *p;
    int v;
    if (sMgCoinsBetPlaced != 0)
        return;
    p = gMgScene;
    v = p ? p->mCredits : 0;
    if (n <= v)
        v = n;
    gMgCoinsInPlay += v;
    sMgCoinsStep = MgCoins_StepLaunch;
    Sound::PlayBank2_2D(0x14d);
    sMgCoinsBetPlaced = 1;
}

/* Raise the count toward 5 by c and launch, the first c with a delay. */
void MgCoins_RaiseBet(int c) {
    sMgCoinsBetCap = 5;
    ApproachLinear(gMgCoinsInPlay, 5, c);
    sMgCoinsIndex = 0;
    sMgCoinsSlowLaunches = c;
    sMgCoinsLaunchPending = 1;
    sMgCoinsWait = 0;
    sMgCoinsStep = MgCoins_StepLaunchQuick;
}

/* Add one sprite and launch it, or play sound 0xe when the scene's mCredits
   is 0, the count is at its cap, or MgCoins_PlaceBet has run. Does
   nothing while a launch is pending (sMgCoinsLaunchPending). */
void MgCoins_AddBetCoin(void) {
    int x;
    if (gMgScene != 0) {
        x = gMgScene->mCredits;
    } else {
        x = 0;
    }
    if (x == 0) {
        func_02012790(0xe);
        return;
    }
    if (sMgCoinsBetPlaced != 0 || gMgCoinsInPlay >= sMgCoinsBetCap) {
        func_02012790(0xe);
        return;
    }
    if (sMgCoinsLaunchPending != 0) {
        return;
    }
    sMgCoinsLaunchPending = 1;
    ApproachLinear(gMgCoinsInPlay, sMgCoinsBetCap, 1);
    sMgCoinsIndex = gMgCoinsInPlay - 1;
    Sound::PlayBank2_2D(0x14e);
    sMgCoinsStep = MgCoins_StepLaunchQuick;
}

/* Step: every 0x18 frames send the next sprite to
   (index * 16 + 8, -Mg_GetTopScreenYOffset() - 16) until all sprites in play
   are out. */
void MgCoins_StepLaunch(void) {
    Vector2 in;
    int n;
    int idx;
    if (gMgCoinsInPlay <= sMgCoinsIndex) return;
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0) return;
    n = -Mg_GetTopScreenYOffset();
    idx = sMgCoinsIndex;
    n = (n - 0x10) << 12;
    in.x = (idx * 16 + 8) << 12;
    in.y = n;
    sMgCoins[idx].Launch(&in);
    Sound::PlayBank2_2D(0x14d);
    {
        int b = sMgCoinsIndex;
        if (b == gMgCoinsInPlay - 1) {
            gMgCoinsStatus = 1;
            sMgCoinsStep = 0;
            return;
        }
        sMgCoinsIndex = b + 1;
        sMgCoinsWait = 0x18;
    }
}

/* Step: as MgCoins_StepLaunch, but only the first sMgCoinsSlowLaunches
   sprites wait 0x18 frames and play the sound; the rest go one per frame,
   with no wait or sound. */
void MgCoins_StepLaunchQuick(void) {
    Vector2 inA;
    Vector2 inB;
    if (gMgCoinsInPlay <= sMgCoinsIndex)
        return;
    if (sMgCoinsIndex < sMgCoinsSlowLaunches) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        {
            int v1 = (-Mg_GetTopScreenYOffset() - 0x10) << 12;
            int v0 = (sMgCoinsIndex * 16 + 8) << 12;
            inA.x = v0;
            inA.y = v1;
        }
        sMgCoins[sMgCoinsIndex].Launch(&inA);
        Sound::PlayBank2_2D(0x14d);
        sMgCoinsWait = 0x18;
        if (sMgCoinsIndex == gMgCoinsInPlay - 1) {
            sMgCoinsLaunchPending = 0;
            gMgCoinsStatus = 1;
            sMgCoinsStep = 0;
            return;
        }
        sMgCoinsIndex = sMgCoinsIndex + 1;
        return;
    }
    {
        int v1 = (-Mg_GetTopScreenYOffset() - 0x10) << 12;
        int v0 = (sMgCoinsIndex * 16 + 8) << 12;
        inB.x = v0;
        inB.y = v1;
        sMgCoins[sMgCoinsIndex].Launch(&inB);
    }
    sMgCoinsLaunchPending = 0;
    if (sMgCoinsIndex == gMgCoinsInPlay - 1) {
        gMgCoinsStatus = 1;
        sMgCoinsStep = 0;
        return;
    }
    sMgCoinsIndex = sMgCoinsIndex + 1;
}

/* Start MgCoins_StepLose over every sprite in play. */
void MgCoins_StartLose(void) {
    gMgCoinsStatus = 2;
    sMgCoinsWait = 0;
    sMgCoinsToStep = gMgCoinsInPlay;
    if (gMgCoinsShowDelta != 0) {
        sMgCoinsDelta = -gMgCoinsInPlay;
        sMgCoinsDeltaShown = 1;
    }
    sMgCoinsSoundPending = 1;
    sMgCoinsStep = MgCoins_StepLose;
}

/* Start MgCoins_StepSpend over x sprites. */
void MgCoins_StartSpend(int x) {
    int cond = gMgCoinsShowDelta;
    gMgCoinsStatus = 2;
    sMgCoinsWait = 0;
    sMgCoinsToStep = x;
    if (cond != 0) {
        sMgCoinsDelta = -x;
        sMgCoinsDeltaShown = 1;
    }
    sMgCoinsSoundPending = 1;
    sMgCoinsStep = MgCoins_StepSpend;
}

/* Start MgCoins_StepLoseOneMore with one more sprite still to step
   (sMgCoinsToStep + 1; 1 after a finished run). */
void MgCoins_StartLoseOneMore(void) {
    gMgCoinsStatus = 2;
    sMgCoinsWait = 0;
    sMgCoinsToStep = sMgCoinsToStep + 1;
    sMgCoinsSoundPending = 1;
    sMgCoinsStep = MgCoins_StepLoseOneMore;
}

/* Start MgCoins_StepRefund over every sprite in play, after 0x1e frames. */
void MgCoins_StartRefund(void) {
    gMgCoinsStatus = 4;
    sMgCoinsWait = 0x1e;
    sMgCoinsToStep = gMgCoinsInPlay;
    sMgCoinsStep = MgCoins_StepRefund;
}

/* Step: every 0x10 frames send the current sprite off with
   Sprite::SendOff, walking the index down; then wait until the last
   one is hidden (mVisible 0) and hold 0x3c frames (MgCoins_StepHoldThenFinish). */
void MgCoins_StepLose(void) {
    if (sMgCoinsToStep > 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        sMgCoins[sMgCoinsIndex].SendOff();
        if (sMgCoinsSoundPending != 0) {
            int t = gMgScene ? gMgScene->mCredits : 0;
            if (t != 0)
                Sound::PlayBank2_2D(0x14b);
            else
                Sound::PlayBank2_2D(0x14c);
            sMgCoinsSoundPending = 0;
        }
        sMgCoinsLastLost = sMgCoinsIndex;
        ApproachLinear(sMgCoinsIndex, 0, 1);
        if (ApproachLinear(sMgCoinsToStep, 0, 1) == 0)
            sMgCoinsWait = 0x10;
        return;
    }
    if (sMgCoins[sMgCoinsLastLost].mVisible != 0)
        return;
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepHoldThenFinish;
}

/* Step: MgCoins_StepLose with a different sound rule. */
void MgCoins_StepLoseOneMore(void) {
    if (sMgCoinsToStep > 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        sMgCoins[sMgCoinsIndex].SendOff();
        if (sMgCoinsSoundPending != 0) {
            int t = gMgScene ? gMgScene->mCredits : 0;
            if (t == 0 && sMgCoinsIndex == 0)
                Sound::PlayBank2_2D(0x14c);
            else
                Sound::PlayBank2_2D(0x14b);
            sMgCoinsSoundPending = 0;
        }
        sMgCoinsLastLostAlt = sMgCoinsIndex;
        ApproachLinear(sMgCoinsIndex, 0, 1);
        if (ApproachLinear(sMgCoinsToStep, 0, 1) == 0)
            sMgCoinsWait = 0x10;
        return;
    }
    if (sMgCoins[sMgCoinsLastLostAlt].mVisible != 0)
        return;
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepHoldThenFinish;
}

/* Step: wait out sMgCoinsWait, then finish. */
void MgCoins_StepHoldThenFinish(void) {
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
        return;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

/* Step: every 0x10 frames bring a sprite in with Sprite::Spend,
   walking the index up and wrapping at 0x14, while the scene's mCredits is
   nonzero; then wait until the last one is hidden (mVisible 0) and hold
   0x3c frames (MgCoins_StepSpendHold). */
void MgCoins_StepSpend(void) {
    if (sMgCoinsToStep > 0 && (gMgScene ? gMgScene->mCredits : 0) != 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        if (sMgCoinsSoundPending != 0) {
            int t = gMgScene ? gMgScene->mCredits : 0;
            if (t <= sMgCoinsToStep)
                Sound::PlayBank2_2D(0x14c);
            else
                Sound::PlayBank2_2D(0x14b);
            sMgCoinsSoundPending = 0;
        }
        sMgCoins[sMgCoinsIndex].Spend();
        sMgCoinsLastSpent = sMgCoinsIndex;
        if (ApproachLinear(sMgCoinsIndex, 0x14, 1) != 0)
            sMgCoinsIndex = 0;
        if (ApproachLinear(sMgCoinsToStep, 0, 1) == 0)
            sMgCoinsWait = 0x10;
        return;
    }
    if (sMgCoins[sMgCoinsLastSpent].mVisible != 0)
        return;
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepSpendHold;
}

/* Step: wait out sMgCoinsWait, then finish. */
void MgCoins_StepSpendHold(void) {
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
        return;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

/* Step: every 0xc frames send the current sprite back to the corner with
   Sprite::Return, walking the index down; finish when the last one
   reaches the corner. */
void MgCoins_StepRefund() {
    if (sMgCoinsToStep > 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        sMgCoins[sMgCoinsIndex].Return();
        sMgCoinsLastReturned = sMgCoinsIndex;
        ApproachLinear(sMgCoinsIndex, 0, 1);
        if (ApproachLinear(sMgCoinsToStep, 0, 1) == 0) {
            sMgCoinsWait = 0xc;
        }
        return;
    }
    if (sMgCoins[sMgCoinsLastReturned].mVisible != 0)
        return;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

/* Hide every sprite and lay out count of them (MgCoins_StepPayoutLayout). */
void MgCoins_StartPayout(int count) {
    int i;
    int cond;
    Sprite* it = sMgCoins;
    for (i = 0; i < 20; i++) {
        it->Hide();
        it++;
    }
    cond = gMgCoinsShowDelta;
    sMgCoinsPayoutLeft = count;
    sMgCoinsWait = 0;
    gMgCoinsStatus = 3;
    if (cond != 0) {
        sMgCoinsDelta = count;
        sMgCoinsDeltaShown = 1;
    }
    sMgCoinsStep = MgCoins_StepPayoutLayout;
}

/* Step: place up to 20 sprites on a grid five wide (rows 16 pixels apart,
   centred when fewer than five), each with a growing start delay, then
   hand over to MgCoins_StepPayoutNext. */
void MgCoins_StepPayoutLayout(void) {
    int step, idx, n, limit;
    int cx, rx;
    Vector2 sp;
    Sprite* e;
    int delay;
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
        return;
    step = 8;
    n = sMgCoinsPayoutLeft;
    if (n > 0x14) limit = 0x14; else limit = n;
    if (n > 0x14) step >>= 1;
    idx = 0;
    if (limit > 0) {
        e = sMgCoins;
        delay = idx;
        do {
            int row = idx / 5;
            int col = idx % 5;
            cx = col << 0x10;
            rx = -(row << 0x10);
            {
                int ret = -Mg_GetTopScreenYOffset();
                sp.y = (ret - 0x10) << 0xc;
            }
            sp.x = 0x60000;
            {
                int c2 = sMgCoinsPayoutLeft;
                if (c2 < 5)
                    sp.x = (0x80 - ((c2 - 1) << 3)) << 0xc;
            }
            sp.x = sp.x + cx;
            sp.y = sp.y + rx;
            e->PlacePayout(&sp);
            e->mDelay += delay;
            idx++;
            e++;
            delay += step;
        } while (idx < limit);
    }
    sMgCoinsWait = 0;
    sMgCoinsStep = MgCoins_StepPayoutNext;
}

/* Step: once no sprite is visible, lay out the next batch the same way
   (the count is decremented as sprites reach the corner); when none are
   left, hold 0x3c frames and go to MgCoins_StepPayoutFinish. */
void MgCoins_StepPayoutNext(void) {
    int step, i, idx, n, limit;
    Sprite* e;
    Vector2 sp;

    e = sMgCoins;
    for (i = 0; i < 20; i++) {
        if (e->mVisible != 0)
            return;
        e++;
    }

    n = sMgCoinsPayoutLeft;
    if (n > 0) {
        if (n > 0x14)
            limit = 0x14;
        else
            limit = n;
        step = 8;
        if (n > 0x14)
            step >>= 1;
        /* idx counts the sprites placed; i is the next start delay */
        idx = 0;
        if (limit > 0) {
            e = sMgCoins;
            i = idx;
            do {
                int row = idx / 5;
                int col = idx % 5;
                int cx = col << 0x10;
                int rx = -(row << 0x10);
                int ret = -Mg_GetTopScreenYOffset();
                sp.y = (ret - 0x10) << 0xc;
                sp.x = 0x60000;
                {
                    int c2 = sMgCoinsPayoutLeft;
                    if (c2 < 5) {
                        sp.x = (0x80 - ((c2 - 1) << 3)) << 0xc;
                    }
                }
                sp.x = sp.x + cx;
                sp.y = sp.y + rx;
                e->PlacePayout(&sp);
                e->mDelay = i;
                idx++;
                e++;
                i += step;
            } while (idx < limit);
        }
        sMgCoinsWait = 0;
        return;
    }
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepPayoutFinish;
}

/* Step: after sMgCoinsWait frames, report the scene's mCredits
   through Mg_SubmitScore when Mg_GetScoringMode returns 2, then
   finish. */
void MgCoins_StepPayoutFinish(void) {
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
        return;
    if (Mg_GetScoringMode() == 2) {
        dScMgBase_c *p = gMgScene;
        Mg_SubmitScore(p ? p->mCredits : 0);
    }
    sMgCoinsWait = 0;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

/* Reset one sprite: hidden, no delay, plain movement state. */
void Sprite::Reset() {
    Hide();
    mDelay = 0;
    mState = &Sprite::Move;
}

void Sprite::Update() {
    (this->*mState)();
}

void Sprite::Draw() {
    if (mVisible == 0)
        return;
    Mg_DrawCreditsIcon(mPos.x >> 12, mPos.y >> 12, -1, -1);
}

/* State: move toward the target and stay. */
void Sprite::Move() {
    ApproachLinear(mPos.x, mTarget.x, mStep.x);
    ApproachLinear(mPos.y, mTarget.y, mStep.y);
}

/* State: move; on arrival call Mg_GainCredits(1), hide, and stop. */
void Sprite::MoveThenBank() {
    Vector2 out;
    ApproachLinear(mPos.x, mTarget.x, mStep.x);
    ApproachLinear(mPos.y, mTarget.y, mStep.y);
    Vec2_Sub(&out, &mTarget, &mPos);
    if (out.x != 0)
        return;
    if (out.y != 0)
        return;
    Mg_GainCredits(1);
    Hide();
    mState = &Sprite::Move;
}

void Sprite::Hide() {
    mVisible = 0;
}

/* Launch from the corner (12, 12) to `in`, at 1/16 of the distance per
   frame. Unless the sprite is already in Sprite::MoveThenBank, call
   Mg_LoseCredits(1). */
void Sprite::Launch(Vector2 *in) {
    Vector2 loc0;
    Vector2 loc2;
    mVisible = 1;
    mPos.x = 0xc000;
    mPos.y = 0xc000;
    mTarget.x = in->x;
    mTarget.y = in->y;
    Vec2_Sub(&loc0, &mTarget, &mPos);
    func_0203d680(&loc2, &loc0, 0x100);
    mStep.x = loc2.x;
    mStep.y = loc2.y;
    {
        int v = mStep.x;
        if (v < 0) v = -v;
        mStep.x = v;
    }
    {
        int v = mStep.y;
        if (v < 0) v = -v;
        mStep.y = v;
    }
    if (mState != &Sprite::MoveThenBank)
        Mg_LoseCredits(1);
    mState = &Sprite::Move;
}

/* Send the sprite to x = 128 at 0xc0/0x1000 of the distance per frame
   (move state Sprite::MoveThenRise, then the rise in Sprite::Rise). */
void Sprite::SendOff() {
    Vector2 a;
    Vector2 b;
    mTarget.x = 0x80000;
    Vec2_Sub(&a, &mTarget, &mPos);
    func_0203d680(&b, &a, 0xc0);
    mStep.x = b.x;
    mStep.y = b.y;
    {
        int x = mStep.x;
        if (x < 0) x = -x;
        mStep.x = x;
    }
    {
        int y = mStep.y;
        if (y < 0) y = -y;
        mStep.y = y;
    }
    mDelay = 20;
    Sound::PlayBank2_2D(0x14a);
    mState = &Sprite::MoveThenRise;
}

/* State: move; on arrival rise (Sprite::Rise). */
void Sprite::MoveThenRise() {
    Vector2 v;
    ApproachLinear(mPos.x, mTarget.x, mStep.x);
    ApproachLinear(mPos.y, mTarget.y, mStep.y);
    Vec2_Sub(&v, &mTarget, &mPos);
    if (v.x != 0)
        return;
    if (v.y == 0)
        mState = &Sprite::Rise;
}

/* State: rise. g = sMgCoinRiseFrames is 0x14 in the ROM; for mDelay
   0x14 down to 1, y changes by -3.5 px a frame, 0.2 px less each frame,
   ending near +0.3 px (about 32 px up in all). When mDelay reaches 0 the
   sprite leaves play and resets. */
void Sprite::Rise() {
    int g = sMgCoinRiseFrames;
    int n = -((mDelay - (g >> 1)) << 12);
    mPos.y += FX_MUL(n / g - 0x600, 0x4000);
    if (ApproachLinear2(mDelay, 0, 1) == 0)
        return;
    ApproachLinear(gMgCoinsInPlay, 0, 1);
    Reset();
}

/* Bring a sprite from the corner (12, 12) to
   (128, -Mg_GetTopScreenYOffset() - 16) (move state Sprite::MoveThenRiseSpent,
   then the rise in Sprite::RiseSpent). */
void Sprite::Spend() {
    Vector2 a;
    Vector2 b;
    Mg_LoseCredits(1);
    mVisible = 1;
    mPos.x = 0xc000;
    mPos.y = 0xc000;
    mTarget.x = 0x80000;
    mTarget.y = ((-Mg_GetTopScreenYOffset()) - 0x10) << 12;
    Vec2_Sub(&a, &mTarget, &mPos);
    func_0203d680(&b, &a, 0xc0);
    mStep.x = b.x;
    mStep.y = b.y;
    {
        int x = mStep.x;
        if (x < 0) x = -x;
        mStep.x = x;
    }
    {
        int y = mStep.y;
        if (y < 0) y = -y;
        mStep.y = y;
    }
    mDelay = 20;
    Sound::PlayBank2_2D(0x14a);
    mState = &Sprite::MoveThenRiseSpent;
}

/* State: move; on arrival rise (Sprite::RiseSpent). */
void Sprite::MoveThenRiseSpent() {
    Vector2 v;
    ApproachLinear(mPos.x, mTarget.x, mStep.x);
    ApproachLinear(mPos.y, mTarget.y, mStep.y);
    Vec2_Sub(&v, &mTarget, &mPos);
    if (v.x != 0)
        return;
    if (v.y == 0)
        mState = &Sprite::RiseSpent;
}

/* State: the same rise as Sprite::Rise. */
void Sprite::RiseSpent() {
    int g = sMgCoinRiseFrames;
    int n = -((mDelay - (g >> 1)) << 12);
    mPos.y += FX_MUL(n / g - 0x600, 0x4000);
    if (ApproachLinear2(mDelay, 0, 1) == 0)
        return;
    ApproachLinear(gMgCoinsInPlay, 0, 1);
    Reset();
}

/* Send the sprite back to the corner (12, 12) at 0xc0/0x1000 of the
   distance per frame (Sprite::MoveThenRefund). */
void Sprite::Return() {
    Vector2 v1;
    Vector2 v2;
    int a, b;
    mTarget.x = 0xc000;
    mTarget.y = 0xc000;
    Vec2_Sub(&v1, &mTarget, &mPos);
    func_0203d680(&v2, &v1, 0xc0);
    mStep.x = v2.x;
    mStep.y = v2.y;
    a = mStep.x;
    if (a < 0)
        a = -a;
    mStep.x = a;
    b = mStep.y;
    if (b < 0)
        b = -b;
    mStep.y = b;
    mState = &Sprite::MoveThenRefund;
}

/* State: move; on arrival leave play, call Mg_GainCredits(1), reset. */
void Sprite::MoveThenRefund() {
    Vector2 v;
    ApproachLinear(mPos.x, mTarget.x, mStep.x);
    ApproachLinear(mPos.y, mTarget.y, mStep.y);
    Vec2_Sub(&v, &mTarget, &mPos);
    if (v.x != 0)
        return;
    if (v.y != 0)
        return;
    ApproachLinear(gMgCoinsInPlay, 0, 1);
    Mg_GainCredits(1);
    Reset();
}

/* Place a sprite at `in`, visible, aimed at the corner (12, 12) at 1/16 of
   the distance per frame (Sprite::FlyToCounter). */
void Sprite::PlacePayout(Vector2 *in) {
    Vector2 a;
    Vector2 b;
    mPos.x = in->x;
    mPos.y = in->y;
    mDelay = 0;
    mVisible = 1;
    mTarget.x = 0xc000;
    mTarget.y = 0xc000;
    Vec2_Sub(&a, &mTarget, &mPos);
    func_0203d680(&b, &a, 0x100);
    mStep.x = b.x;
    mStep.y = b.y;
    {
        int x = mStep.x;
        if (x < 0)
            x = -x;
        mStep.x = x;
    }
    {
        int y = mStep.y;
        if (y < 0)
            y = -y;
        mStep.y = y;
    }
    mState = &Sprite::FlyToCounter;
}

/* State: wait out mDelay, then move; on arrival play sound 0x149, call
   Mg_GainCredits(1), take one from the layout count, and reset. */
void Sprite::FlyToCounter() {
    Vector2 t;
    if (ApproachLinear2(mDelay, 0, 1) == 0)
        return;
    ApproachLinear(mPos.x, mTarget.x, mStep.x);
    ApproachLinear(mPos.y, mTarget.y, mStep.y);
    Vec2_Sub(&t, &mTarget, &mPos);
    if (t.x != 0)
        return;
    if (t.y != 0)
        return;
    if (ApproachLinear2(mDelay, 0, 1) == 0)
        return;
    Sound::PlayBank2_2D(0x149);
    Mg_GainCredits(1);
    ApproachLinear(sMgCoinsPayoutLeft, 0, 1);
    Reset();
}

/* The constructor and destructor are empty and inline: mwcc emits their used
   bodies at the end of the object, after __arraydtor, which is where the
   cartridge has D1 (0x020b67e0) and then C1 (0x020b67e4). */
inline Sprite::~Sprite() {}

inline Sprite::Sprite() {}
