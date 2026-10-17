//cpp
/* unit020b4aa4 -- ov004 unit 0x020b4aa4..0x020b67e8: 43 functions, .text only.
 *
 * One translation unit of the shared minigame overlay, between unit020b2c84
 * and dMgState_c. No ROM RTTI or vtable falls in the range, so no class name
 * is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. sMgCoins is an array of twenty 0x24-byte
 * Sprite records. Each one runs its own member-pointer state every frame
 * (MgCoins_Update through Coin_Update) and, while visible, is
 * drawn at its position by MgCoins_Draw. The Sprite states slide a
 * sprite from its position to a target by a fixed step, then hand it the
 * next state; the records at pmf_Coin_MoveThenRise..0x020bc820 hold those
 * states, and the comment on each extern names its target. Over the array
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
 * unit. 0x020b67e8 is the end of Coin_Construct, the last helper
 * __sinit_ov004_020b9ad0 registers for this one (with MgCoins_Cleanup,
 * which runs __cxa_vec_cleanup over the array, and the empty per-element
 * destructor Coin_Destroy); the next function reads the .data block
 * that starts at 0x020bc850, outside this unit's .data window
 * (0x020bc7d0..0x020bc83c).
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 *
 * Retired one-function sources (ROM address order):
 *   0x020b4aa4  Coin_FlyToCounter
 *   0x020b4b84  Coin_PlacePayout
 *   0x020b4c30  Coin_MoveThenRefund
 *   0x020b4cc4  Coin_Return
 *   0x020b4d50  Coin_RiseSpent
 *   0x020b4dfc  Coin_MoveThenRiseSpent
 *   0x020b4e78  Coin_Spend
 *   0x020b4f44  Coin_Rise
 *   0x020b4ff0  Coin_MoveThenRise
 *   0x020b506c  Coin_SendOff
 *   0x020b5108  Coin_Launch
 *   0x020b51e4  Coin_Hide
 *   0x020b51f0  Coin_MoveThenBank
 *   0x020b5288  Coin_Move
 *   0x020b52b8  Coin_Draw
 *   0x020b52fc  Coin_Update
 *   0x020b5334  Coin_Reset
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
 *   0x020b67bc  MgCoins_Cleanup
 *   0x020b67e0  Coin_Destroy
 *   0x020b67e4  Coin_Construct
 */

#include "types.h"
#include "PlayerInput.h"
#include "dScMgBase_c.h"
#include "nitro/fx/fx.h"

struct Vec2i { int x, y; };

/* One of the twenty records at sMgCoins. Positions are Fix12
   screen pixels (Coin_Draw draws at mPos >> 12). */
struct Sprite;
typedef void (Sprite::*SpriteFn)();
struct Sprite {
    SpriteFn mState;   /* 0x00 run every frame by Coin_Update */
    Vec2i mPos;        /* 0x08 */
    Vec2i mTarget;     /* 0x10 */
    Vec2i mStep;       /* 0x18 per-frame step toward mTarget, both positive */
    s16 mDelay;        /* 0x20 frames to wait; also the rise counter */
    u8 mVisible;       /* 0x22 drawn while nonzero */
};

#ifndef SM64DS_PLATFORM_PC
typedef char Sprite_size_must_be_0x24[sizeof(Sprite) == 0x24 ? 1 : -1];
#endif

namespace Sound { void PlayBank2_2D(unsigned int); }
int ApproachLinear(int&, int, int);
int ApproachLinear2(short&, short, short);

extern "C" {
extern void Vec2_Sub(void* o, void* a, void* b);
extern void func_0203d680(void* out, void* in, int scale);
extern int __aeabi_idiv(int a, int b);
extern void __cxa_vec_cleanup(void *, unsigned int, unsigned int, void (*)(void *));
extern unsigned int func_02012790(unsigned int a);
extern int GetGameLanguage(void);
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);

extern void Mg_GainCredits(void* c);
extern void Mg_LoseCredits(void* x);
extern void Mg_DrawCreditsIcon(int a, int b, int c, int d);
extern int Mg_GetTopScreenYOffset(void);
extern int Mg_GetScoringMode(void);
extern void Mg_SubmitScore(int self);
extern void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *d);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);

extern void Coin_PlacePayout(Sprite* s, int* in);
extern void Coin_Return(Sprite* s);
extern void Coin_Spend(Sprite* s);
extern void Coin_SendOff(Sprite* s);
extern void Coin_Launch(Sprite* s, int* in);
extern void Coin_Hide(Sprite* s);
extern void Coin_Draw(Sprite* s);
extern void Coin_Update(Sprite* s);
extern void Coin_Reset(Sprite* s);
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
extern void Coin_Destroy(void);

extern int sMgCoinRiseFrames[];
extern int sMgCoinsBetCap;   /* cap MgCoins_AddBetCoin raises the count to */
extern int gMgCoinsShowDelta;   /* nonzero: sequences also show their count */
/* Sprite states: member-pointer records (ov004 relocs.txt names each
   target). */
extern SpriteFn pmf_Coin_MoveThenRise; /* Coin_MoveThenRise */
extern SpriteFn pmf_Coin_RiseSpent; /* Coin_RiseSpent */
extern SpriteFn pmf_Coin_Move; /* Coin_Move */
extern SpriteFn pmf_Coin_Move_2; /* Coin_Move */
extern SpriteFn pmf_Coin_Rise; /* Coin_Rise */
extern SpriteFn pmf_Coin_MoveThenBank; /* Coin_MoveThenBank */
extern SpriteFn pmf_Coin_FlyToCounter; /* Coin_FlyToCounter */
extern SpriteFn pmf_Coin_MoveThenRiseSpent; /* Coin_MoveThenRiseSpent */
extern SpriteFn pmf_Coin_MoveThenRefund; /* Coin_MoveThenRefund */
extern SpriteFn pmf_Coin_Move_3; /* Coin_Move */
extern int *sMgCoinsButtonSprites[];
extern unsigned char *sMgCoinsDeltaSprites[];
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
extern struct Sprite sMgCoins[];
/* The address of sMgCoins[0].mVisible; the ROM indexes the
   array's visible bytes from it. */
extern unsigned char sMgCoinsVisible[];
extern int data_020a0db0;
}

// @symbol Coin_Construct
extern "C" void Coin_Construct(void) {}

// @symbol Coin_Destroy
extern "C" void Coin_Destroy(void)
{
}

// @symbol MgCoins_Cleanup
extern "C" void MgCoins_Cleanup(void)
{
    __cxa_vec_cleanup(sMgCoins, 0x14, 0x24, (void (*)(void *))Coin_Destroy);
}

// @symbol MgCoins_Reset
/* Reset every sprite and the sequence state. */
extern "C" void MgCoins_Reset(void) {
    int i;
    for (i = 0; i < 0x14; i++)
        Coin_Reset(&sMgCoins[i]);
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

// @symbol MgCoins_Update
/* One frame. A fresh touch inside the top-left 0x40 x 0x50 box adds a
   sprite (MgCoins_AddBetCoin) while that is allowed; then every sprite
   runs its state and the current step runs. */
extern "C" void MgCoins_Update(void) {
    int i;
    Sprite *p;

    if (sMgCoinsBetPlaced == 0 && gMgCoinsInPlay != 0) {
        int idx = gActivePlayerSlot;
        int flag = ((unsigned char (*)[4])gTouchHeld)[idx][0] != 0 && ((unsigned char (*)[4])gTouchEdge)[idx][0] != 0;
        if (flag) {
            int a = ((unsigned char (*)[4])gTouchX)[idx][0];
            int b = ((unsigned char (*)[4])gTouchY)[idx][0];
            if (a < 0x40 && b < 0x50) {
                MgCoins_AddBetCoin();
            }
        }
    }

    p = sMgCoins;
    for (i = 0; i < 0x14; i++) {
        Coin_Update(p);
        p++;
    }

    if (sMgCoinsStep != 0) {
        sMgCoinsStep();
    }
}

// @symbol MgCoins_DrawDelta
/* Draw the signed count on row 0x60: the sprite icon (Mg_DrawCreditsIcon)
   at x 0x6e, the per-language +0x34 graphic for arg >= 0 or +0x38 for
   arg < 0 at x 0x80, then |arg| at x 0x92. */
extern "C" void MgCoins_DrawDelta(int arg)
{
    int absv;
    int r4;
    int gfx;

    if (arg < 0) absv = -arg; else absv = arg;

    if (arg >= 0) {
        int idx = GetGameLanguage();
        r4 = 0x14;
        gfx = *(int *)(sMgCoinsDeltaSprites[idx] + 0x34);
    } else {
        int idx = GetGameLanguage();
        r4 = 0x28;
        gfx = *(int *)(sMgCoinsDeltaSprites[idx] + 0x38);
    }

    Mg_DrawCreditsIcon(0x6e, 0x60, 0, -1);
    Mg_RenderSubSpriteMtx((void *)gfx, 0x80, 0x60, 0);
    Mg_DrawBigNumber(0x92, 0x60, absv, 0, -1, 2, r4);
}

// @symbol MgCoins_Draw
/* Draw: the touch button at (0x14, 0x30) while MgCoins_Update would
   take a touch, the count while sMgCoinsDeltaShown is set, then every
   sprite. */
extern "C" void MgCoins_Draw(void) {
    int i;
    Sprite *p;
    if (sMgCoinsBetPlaced == 0 && gMgCoinsInPlay != 0) {
        if (data_020a0db0 & 0x10) {
            Hud_RenderSprite((void*)sMgCoinsButtonSprites[GetGameLanguage()][0], 0x14, 0x30, -1, -1);
        } else {
            Hud_RenderSprite((void*)sMgCoinsButtonSprites[GetGameLanguage()][1], 0x14, 0x30, -1, -1);
        }
        Hud_RenderSprite((void*)sMgCoinsButtonSprites[GetGameLanguage()][2], 0x14, 0x30, -1, -1);
    }
    if (sMgCoinsDeltaShown != 0) {
        MgCoins_DrawDelta(sMgCoinsDelta);
    }
    for (p = sMgCoins, i = 0; i < 0x14; i++) {
        Coin_Draw(p);
        p++;
    }
}

// @symbol MgCoins_PlaceBet
/* Once: add up to n sprites, capped by the scene's mCredits, and start
   launching them (MgCoins_StepLaunch). */
extern "C" void MgCoins_PlaceBet(int n){
  dScMgBase_c *p;
  int v;
  if (sMgCoinsBetPlaced != 0) return;
  p = gMgScene;
  v = p ? p->mCredits : 0;
  if (n <= v) v = n;
  gMgCoinsInPlay += v;
  sMgCoinsStep = MgCoins_StepLaunch;
  Sound::PlayBank2_2D(0x14d);
  sMgCoinsBetPlaced = 1;
}

// @symbol MgCoins_RaiseBet
/* Raise the count toward 5 by c and launch, the first c with a delay. */
extern "C" void MgCoins_RaiseBet(int c) {
    sMgCoinsBetCap = 5;
    ApproachLinear(gMgCoinsInPlay, 5, c);
    sMgCoinsIndex = 0;
    sMgCoinsSlowLaunches = c;
    sMgCoinsLaunchPending = 1;
    sMgCoinsWait = 0;
    sMgCoinsStep = MgCoins_StepLaunchQuick;
}

// @symbol MgCoins_AddBetCoin
/* Add one sprite and launch it, or play sound 0xe when the scene's mCredits
   is 0, the count is at its cap, or MgCoins_PlaceBet has run. Does
   nothing while a launch is pending (sMgCoinsLaunchPending). */
extern "C" void MgCoins_AddBetCoin(void) {
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

// @symbol MgCoins_StepLaunch
/* Step: every 0x18 frames send the next sprite to
   (index * 16 + 8, -Mg_GetTopScreenYOffset() - 16) until all sprites in play
   are out. */
extern "C" void MgCoins_StepLaunch(void)
{
    int in[2];
    int n;
    int idx;
    if (gMgCoinsInPlay <= sMgCoinsIndex) return;
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0) return;
    n = -Mg_GetTopScreenYOffset();
    idx = sMgCoinsIndex;
    n = (n - 0x10) << 12;
    in[0] = (idx * 16 + 8) << 12;
    in[1] = n;
    Coin_Launch(&sMgCoins[idx], in);
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

// @symbol MgCoins_StepLaunchQuick
/* Step: as MgCoins_StepLaunch, but only the first sMgCoinsSlowLaunches
   sprites wait 0x18 frames and play the sound; the rest go one per frame,
   with no wait or sound. */
extern "C" void MgCoins_StepLaunchQuick(void)
{
    int inA[2];
    int inB[2];
    if (gMgCoinsInPlay <= sMgCoinsIndex)
        return;
    if (sMgCoinsIndex < sMgCoinsSlowLaunches) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        {
            int v1 = (-Mg_GetTopScreenYOffset() - 0x10) << 12;
            int v0 = (sMgCoinsIndex * 16 + 8) << 12;
            inA[0] = v0;
            inA[1] = v1;
        }
        Coin_Launch(&sMgCoins[sMgCoinsIndex], inA);
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
        inB[0] = v0;
        inB[1] = v1;
        Coin_Launch(&sMgCoins[sMgCoinsIndex], inB);
    }
    sMgCoinsLaunchPending = 0;
    if (sMgCoinsIndex == gMgCoinsInPlay - 1) {
        gMgCoinsStatus = 1;
        sMgCoinsStep = 0;
        return;
    }
    sMgCoinsIndex = sMgCoinsIndex + 1;
}

// @symbol MgCoins_StartLose
/* Start MgCoins_StepLose over every sprite in play. */
extern "C" void MgCoins_StartLose(void) {
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

// @symbol MgCoins_StartSpend
/* Start MgCoins_StepSpend over x sprites. */
extern "C" void MgCoins_StartSpend(int x){
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

// @symbol MgCoins_StartLoseOneMore
/* Start MgCoins_StepLoseOneMore with one more sprite still to step
   (sMgCoinsToStep + 1; 1 after a finished run). */
extern "C" void MgCoins_StartLoseOneMore(void){
  gMgCoinsStatus = 2;
  sMgCoinsWait = 0;
  sMgCoinsToStep = sMgCoinsToStep + 1;
  sMgCoinsSoundPending = 1;
  sMgCoinsStep = MgCoins_StepLoseOneMore;
}

// @symbol MgCoins_StartRefund
/* Start MgCoins_StepRefund over every sprite in play, after 0x1e frames. */
extern "C" void MgCoins_StartRefund(void) {
    gMgCoinsStatus = 4;
    sMgCoinsWait = 0x1e;
    sMgCoinsToStep = gMgCoinsInPlay;
    sMgCoinsStep = MgCoins_StepRefund;
}

// @symbol MgCoins_StepLose
/* Step: every 0x10 frames send the current sprite off with
   Coin_SendOff, walking the index down; then wait until the last
   one is hidden (mVisible 0) and hold 0x3c frames (MgCoins_StepHoldThenFinish). */
extern "C" void MgCoins_StepLose(void)
{
    if (sMgCoinsToStep > 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        Coin_SendOff(&sMgCoins[sMgCoinsIndex]);
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
    if (sMgCoinsVisible[sMgCoinsLastLost * 0x24] != 0)
        return;
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepHoldThenFinish;
}

// @symbol MgCoins_StepLoseOneMore
/* Step: MgCoins_StepLose with a different sound rule. */
extern "C" void MgCoins_StepLoseOneMore(void)
{
    if (sMgCoinsToStep > 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        Coin_SendOff(&sMgCoins[sMgCoinsIndex]);
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
    if (sMgCoinsVisible[sMgCoinsLastLostAlt * 0x24] != 0)
        return;
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepHoldThenFinish;
}

// @symbol MgCoins_StepHoldThenFinish
/* Step: wait out sMgCoinsWait, then finish. */
extern "C" void MgCoins_StepHoldThenFinish(void) {
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
        return;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

// @symbol MgCoins_StepSpend
/* Step: every 0x10 frames bring a sprite in with Coin_Spend,
   walking the index up and wrapping at 0x14, while the scene's mCredits is
   nonzero; then wait until the last one is hidden (mVisible 0) and hold
   0x3c frames (MgCoins_StepSpendHold). */
extern "C" void MgCoins_StepSpend(void)
{
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
        Coin_Spend(&sMgCoins[sMgCoinsIndex]);
        sMgCoinsLastSpent = sMgCoinsIndex;
        if (ApproachLinear(sMgCoinsIndex, 0x14, 1) != 0)
            sMgCoinsIndex = 0;
        if (ApproachLinear(sMgCoinsToStep, 0, 1) == 0)
            sMgCoinsWait = 0x10;
        return;
    }
    if (sMgCoinsVisible[sMgCoinsLastSpent * 0x24] != 0)
        return;
    sMgCoinsWait = 0x3c;
    sMgCoinsStep = MgCoins_StepSpendHold;
}

// @symbol MgCoins_StepSpendHold
/* Step: wait out sMgCoinsWait, then finish. */
extern "C" void MgCoins_StepSpendHold(void) {
    if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
        return;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

// @symbol MgCoins_StepRefund
/* Step: every 0xc frames send the current sprite back to the corner with
   Coin_Return, walking the index down; finish when the last one
   reaches the corner. */
extern "C" void MgCoins_StepRefund() {
    if (sMgCoinsToStep > 0) {
        if (ApproachLinear(sMgCoinsWait, 0, 1) == 0)
            return;
        Coin_Return(&sMgCoins[sMgCoinsIndex]);
        sMgCoinsLastReturned = sMgCoinsIndex;
        ApproachLinear(sMgCoinsIndex, 0, 1);
        if (ApproachLinear(sMgCoinsToStep, 0, 1) == 0) {
            sMgCoinsWait = 0xc;
        }
        return;
    }
    if (sMgCoinsVisible[sMgCoinsLastReturned * 0x24] != 0)
        return;
    gMgCoinsStatus = 1;
    sMgCoinsStep = 0;
}

// @symbol MgCoins_StartPayout
/* Hide every sprite and lay out p of them (MgCoins_StepPayoutLayout). p is a
   count; the callers in d_s_mg_bsc.cpp and d_s_mg_card.cpp pass an int. */
extern "C" void MgCoins_StartPayout(char* p)
{
    int i;
    int cond;
    Sprite* it = sMgCoins;
    for (i = 0; i < 0x14; i++) {
        Coin_Hide(it);
        it++;
    }
    cond = gMgCoinsShowDelta;
    sMgCoinsPayoutLeft = (int)p;
    sMgCoinsWait = 0;
    gMgCoinsStatus = 3;
    if (cond != 0) {
        sMgCoinsDelta = (int)p;
        sMgCoinsDeltaShown = 1;
    }
    sMgCoinsStep = MgCoins_StepPayoutLayout;
}


// @symbol MgCoins_StepPayoutLayout
/* Step: place up to 20 sprites on a grid five wide (rows 16 pixels apart,
   centred when fewer than five), each with a growing start delay, then
   hand over to MgCoins_StepPayoutNext. */
extern "C" void MgCoins_StepPayoutLayout(void)
{
    int step, idx, n, limit;
    int cx, rx;
    int sp[2];
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
                sp[1] = (ret - 0x10) << 0xc;
            }
            sp[0] = 0x60000;
            {
                int c2 = sMgCoinsPayoutLeft;
                if (c2 < 5)
                    sp[0] = (0x80 - ((c2 - 1) << 3)) << 0xc;
            }
            sp[0] = sp[0] + cx;
            sp[1] = sp[1] + rx;
            Coin_PlacePayout(e, sp);
            e->mDelay += delay;
            idx++;
            e++;
            delay += step;
        } while (idx < limit);
    }
    sMgCoinsWait = 0;
    sMgCoinsStep = MgCoins_StepPayoutNext;
}

// @symbol MgCoins_StepPayoutNext
/* Step: once no sprite is visible, lay out the next batch the same way
   (the count is decremented as sprites reach the corner); when none are
   left, hold 0x3c frames and go to MgCoins_StepPayoutFinish. */
extern "C" void MgCoins_StepPayoutNext(void)
{
    int step, i, idx, n, limit;
    Sprite* e;
    int sp[2];

    e = sMgCoins;
    for (i = 0; i < 0x14; i++) {
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
                sp[1] = (ret - 0x10) << 0xc;
                sp[0] = 0x60000;
                {
                    int c2 = sMgCoinsPayoutLeft;
                    if (c2 < 5) {
                        sp[0] = (0x80 - ((c2 - 1) << 3)) << 0xc;
                    }
                }
                sp[0] = sp[0] + cx;
                sp[1] = sp[1] + rx;
                Coin_PlacePayout(e, sp);
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

// @symbol MgCoins_StepPayoutFinish
/* Step: after sMgCoinsWait frames, report the scene's mCredits
   through Mg_SubmitScore when Mg_GetScoringMode returns 2, then
   finish. */
extern "C" void MgCoins_StepPayoutFinish(void){
  if(ApproachLinear(sMgCoinsWait, 0, 1)==0) return;
  if(Mg_GetScoringMode()==2){
    dScMgBase_c *p = gMgScene;
    Mg_SubmitScore(p ? p->mCredits : 0);
  }
  sMgCoinsWait = 0;
  gMgCoinsStatus = 1;
  sMgCoinsStep = 0;
}

// @symbol Coin_Reset
/* Reset one sprite: hidden, no delay, plain movement state. */
extern "C" void Coin_Reset(Sprite *s) {
    Coin_Hide(s);
    s->mDelay = 0;
    s->mState = pmf_Coin_Move;
}

// @symbol Coin_Update
extern "C" void Coin_Update(Sprite *s) {
  (s->*(s->mState))();
}

// @symbol Coin_Draw
extern "C" void Coin_Draw(Sprite *s)
{
    if (s->mVisible == 0)
        return;
    Mg_DrawCreditsIcon(s->mPos.x >> 12, s->mPos.y >> 12, -1, -1);
}

// @symbol Coin_Move
/* State: move toward the target and stay. */
extern "C" void Coin_Move(Sprite *s)
{
    ApproachLinear(s->mPos.x, s->mTarget.x, s->mStep.x);
    ApproachLinear(s->mPos.y, s->mTarget.y, s->mStep.y);
}

// @symbol Coin_MoveThenBank
/* State: move; on arrival call Mg_GainCredits(1), hide, and stop. */
extern "C" void Coin_MoveThenBank(Sprite* s) {
  int out[2];
  ApproachLinear(s->mPos.x, s->mTarget.x, s->mStep.x);
  ApproachLinear(s->mPos.y, s->mTarget.y, s->mStep.y);
  Vec2_Sub(out, &s->mTarget, &s->mPos);
  if (out[0] != 0) return;
  if (out[1] != 0) return;
  Mg_GainCredits((void*)1);
  Coin_Hide(s);
  s->mState = pmf_Coin_Move_2;
}

// @symbol Coin_Hide
extern "C" void Coin_Hide(Sprite *s)
{
    s->mVisible = 0;
}

// @symbol Coin_Launch
/* Launch from the corner (12, 12) to `in`, at 1/16 of the distance per
   frame. Unless the sprite is already in Coin_MoveThenBank, call
   Mg_LoseCredits(1). */
extern "C" void Coin_Launch(Sprite* s, int* in)
{
    int loc0[2];
    int loc2[2];
    s->mVisible = 1;
    s->mPos.x = 0xc000;
    s->mPos.y = 0xc000;
    s->mTarget.x = in[0];
    s->mTarget.y = in[1];
    Vec2_Sub(loc0, &s->mTarget, &s->mPos);
    func_0203d680(loc2, loc0, 0x100);
    s->mStep.x = loc2[0];
    s->mStep.y = loc2[1];
    {
        int v = s->mStep.x;
        if (v < 0) v = -v;
        s->mStep.x = v;
    }
    {
        int v = s->mStep.y;
        if (v < 0) v = -v;
        s->mStep.y = v;
    }
    if (s->mState != pmf_Coin_MoveThenBank)
        Mg_LoseCredits((void*)1);
    s->mState = pmf_Coin_Move_3;
}

// @symbol Coin_SendOff
/* Send the sprite to x = 128 at 0xc0/0x1000 of the distance per frame
   (move state Coin_MoveThenRise, then the rise in Coin_Rise). */
extern "C" void Coin_SendOff(Sprite *s)
{
    Vec2i a;
    Vec2i b;
    s->mTarget.x = 0x80000;
    Vec2_Sub(&a, &s->mTarget, &s->mPos);
    func_0203d680(&b, &a, 0xc0);
    s->mStep.x = b.x;
    s->mStep.y = b.y;
    {
        int x = s->mStep.x;
        if (x < 0) x = -x;
        s->mStep.x = x;
    }
    {
        int y = s->mStep.y;
        if (y < 0) y = -y;
        s->mStep.y = y;
    }
    s->mDelay = 0x14;
    Sound::PlayBank2_2D(0x14a);
    s->mState = pmf_Coin_MoveThenRise;
}

// @symbol Coin_MoveThenRise
/* State: move; on arrival rise (Coin_Rise). */
extern "C" void Coin_MoveThenRise(Sprite *s) {
    int v[2];
    ApproachLinear(s->mPos.x, s->mTarget.x, s->mStep.x);
    ApproachLinear(s->mPos.y, s->mTarget.y, s->mStep.y);
    Vec2_Sub(v, &s->mTarget, &s->mPos);
    if (v[0] != 0) return;
    if (v[1] == 0) s->mState = pmf_Coin_Rise;
}

// @symbol Coin_Rise
/* State: rise. g = sMgCoinRiseFrames[0] is 0x14 in the ROM; for mDelay
   0x14 down to 1, y changes by -3.5 px a frame, 0.2 px less each frame,
   ending near +0.3 px (about 32 px up in all). When mDelay reaches 0 the
   sprite leaves play and resets. */
extern "C" void Coin_Rise(Sprite* s){
  int* p = &s->mPos.y;
  int g = sMgCoinRiseFrames[0];
  int n = -(((short)s->mDelay - (g >> 1)) << 12);
  int q;
  int v;
  n = p ? n : n;  /* code shape: dropping it makes the function 12 bytes shorter */
  q = __aeabi_idiv(n, g);  /* `n / g` here compiles differently (measured) */
  v = FX_MUL((q - 0x600), 0x4000);
  *p += v;
  if (ApproachLinear2(s->mDelay, 0, 1) == 0)
    return;
  ApproachLinear(gMgCoinsInPlay, 0, 1);
  Coin_Reset(s);
}

// @symbol Coin_Spend
/* Bring a sprite from the corner (12, 12) to
   (128, -Mg_GetTopScreenYOffset() - 16) (move state Coin_MoveThenRiseSpent,
   then the rise in Coin_RiseSpent). */
extern "C" void Coin_Spend(Sprite *s)
{
    Vec2i a;
    Vec2i b;
    Mg_LoseCredits((void*)1);
    s->mVisible = 1;
    s->mPos.x = 0xc000;
    s->mPos.y = 0xc000;
    s->mTarget.x = 0x80000;
    s->mTarget.y = ((-Mg_GetTopScreenYOffset()) - 0x10) << 12;
    Vec2_Sub(&a, &s->mTarget, &s->mPos);
    func_0203d680(&b, &a, 0xc0);
    s->mStep.x = b.x;
    s->mStep.y = b.y;
    {
        int x = s->mStep.x;
        if (x < 0) x = -x;
        s->mStep.x = x;
    }
    {
        int y = s->mStep.y;
        if (y < 0) y = -y;
        s->mStep.y = y;
    }
    s->mDelay = 0x14;
    Sound::PlayBank2_2D(0x14a);
    s->mState = pmf_Coin_MoveThenRiseSpent;
}

// @symbol Coin_MoveThenRiseSpent
/* State: move; on arrival rise (Coin_RiseSpent). */
extern "C" void Coin_MoveThenRiseSpent(Sprite *s) {
    int v[2];
    ApproachLinear(s->mPos.x, s->mTarget.x, s->mStep.x);
    ApproachLinear(s->mPos.y, s->mTarget.y, s->mStep.y);
    Vec2_Sub(v, &s->mTarget, &s->mPos);
    if (v[0] != 0) return;
    if (v[1] == 0) s->mState = pmf_Coin_RiseSpent;
}

// @symbol Coin_RiseSpent
/* State: Coin_Rise's rise, dividing with `/`. */
extern "C" void Coin_RiseSpent(Sprite* s){
  int* p = &s->mPos.y;
  int g = sMgCoinRiseFrames[0];
  int n = -(((short)s->mDelay - (g >> 1)) << 12);
  int q = n / g;
  int v = FX_MUL((q - 0x600), 0x4000);
  *p = *p + v;
  if (ApproachLinear2(s->mDelay, 0, 1) == 0)
    return;
  ApproachLinear(gMgCoinsInPlay, 0, 1);
  Coin_Reset(s);
}

// @symbol Coin_Return
/* Send the sprite back to the corner (12, 12) at 0xc0/0x1000 of the
   distance per frame (Coin_MoveThenRefund). */
extern "C" void Coin_Return(Sprite* s){
  Vec2i v1;
  Vec2i v2;
  int a, b;
  s->mTarget.x = 0xc000;
  s->mTarget.y = 0xc000;
  Vec2_Sub(&v1, &s->mTarget, &s->mPos);
  func_0203d680(&v2, &v1, 0xc0);
  s->mStep.x = v2.x;
  s->mStep.y = v2.y;
  a = s->mStep.x;
  if (a < 0) a = -a;
  s->mStep.x = a;
  b = s->mStep.y;
  if (b < 0) b = -b;
  s->mStep.y = b;
  s->mState = pmf_Coin_MoveThenRefund;
}

// @symbol Coin_MoveThenRefund
/* State: move; on arrival leave play, call Mg_GainCredits(1), reset. */
extern "C" void Coin_MoveThenRefund(Sprite* s){
  int v[2];
  ApproachLinear(s->mPos.x, s->mTarget.x, s->mStep.x);
  ApproachLinear(s->mPos.y, s->mTarget.y, s->mStep.y);
  Vec2_Sub(v, &s->mTarget, &s->mPos);
  if (v[0] != 0) return;
  if (v[1] != 0) return;
  ApproachLinear(gMgCoinsInPlay, 0, 1);
  Mg_GainCredits((void*)1);
  Coin_Reset(s);
}

// @symbol Coin_PlacePayout
/* Place a sprite at `in`, visible, aimed at the corner (12, 12) at 1/16 of
   the distance per frame (Coin_FlyToCounter). */
extern "C" void Coin_PlacePayout(Sprite* s, int* in){
  Vec2i a;
  Vec2i b;
  s->mPos.x = in[0];
  s->mPos.y = in[1];
  s->mDelay = 0;
  s->mVisible = 1;
  s->mTarget.x = 0xc000;
  s->mTarget.y = 0xc000;
  Vec2_Sub(&a, &s->mTarget, &s->mPos);
  func_0203d680(&b, &a, 0x100);
  s->mStep.x = b.x;
  s->mStep.y = b.y;
  {
    int x = s->mStep.x;
    if (x < 0) x = -x;
    s->mStep.x = x;
  }
  {
    int y = s->mStep.y;
    if (y < 0) y = -y;
    s->mStep.y = y;
  }
  s->mState = pmf_Coin_FlyToCounter;
}

// @symbol Coin_FlyToCounter
/* State: wait out mDelay, then move; on arrival play sound 0x149, call
   Mg_GainCredits(1), take one from the layout count, and reset. */
extern "C" void Coin_FlyToCounter(Sprite* s) {
    Vec2i t;
    if (ApproachLinear2(s->mDelay, 0, 1) == 0) return;
    ApproachLinear(s->mPos.x, s->mTarget.x, s->mStep.x);
    ApproachLinear(s->mPos.y, s->mTarget.y, s->mStep.y);
    Vec2_Sub(&t, &s->mTarget, &s->mPos);
    if (t.x != 0) return;
    if (t.y != 0) return;
    if (ApproachLinear2(s->mDelay, 0, 1) == 0) return;
    Sound::PlayBank2_2D(0x149);
    Mg_GainCredits((void*)1);
    ApproachLinear(sMgCoinsPayoutLeft, 0, 1);
    Coin_Reset(s);
}
