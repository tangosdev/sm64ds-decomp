//cpp
/* Two-player curling. mPiece falls from the top of the screen and wraps
 * once y passes 0xc8. mValue is the number SpawnValue drops between two
 * stones when they collide.
 *
 * Leftover: SpawnValue still reads the other stone as
 *   raw + 0x4000 + other * 0x30 + 0x660. mStone[other] folds that into one
 *   constant and loses the raw + 0x4000 base the ROM shares.
 * Leftover: DragUpdate and SeparateStones call
 *   _ZN4cstd5atan2E5Fix12IiES1_. cstd::atan2 takes Fix12 by value.
 * Leftover: StoneSpin, StoneSlide, NextStone, SeedStones, Play and
 *   ResetGame keep the ROM's raw `this + 0x46xx/0x55xx` addressing (and
 *   SeedStones its `struct E` + 0x4000 split base). Folding them through
 *   the named mStone/mMark/field members loses the base rematerialization,
 *   same as SpawnValue and the two mark functions.
 *
 * The whole class TU, .text 0x020e3854..0x020e6c28: 52 member functions
 * in ROM order from the destructor to InitResources, then the registry
 * factory dScMgCurling2_c_classInit. Behavior state machine: BeginRound
 * seeds a round into Play, Play ticks the drag table and every live stone
 * and hands off to NextThrow once all eleven rest, NextThrow either deals
 * the next stone or goes to EndRound, and Idle is the terminal state. The
 * stones themselves run a second machine -- StoneWait (armed, thrown when
 * the dragged stylus reaches them), StoneSlide, StoneRest and StoneSteer
 * -- dispatched through the same pointer-to-member table shape.
 */

// leftovers: SpawnValue still reads the other stone as
// raw + 0x4000 + other * 0x30 + 0x660 -- mStone[other] folds that into one
// constant and loses the raw + 0x4000 base the ROM shares with the stone
// operand (this TU's SpawnValue). DragUpdate and SeparateStones call
// _ZN4cstd5atan2E5Fix12IiES1_ -- cstd::atan2 takes Fix12 by value, and no
// header declares the scalar overload. The unk_55xx drag fields keep their
// names -- renaming them is header work on include/dScMgCurling2_c.h, which
// the upper-half TU also includes.

/* Required: ROM order, and the brackets on AgeMarks and DragUpdate. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgCurling2_c_AssetId {
    ASSET_D_2D_MG_BG_JOKYU_CURLING1_MATO_NSC_BIN = 0x0041,
    ASSET_D_2D_MG_BG_JOKYU_CURLING1_NSC_BIN = 0x0042,
    ASSET_D_2D_MG_BG_JOKYU_CURLING2_NSC_BIN = 0x0043,
    ASSET_D_2D_MG_BG_JOKYU_CURLING_NCL_BIN = 0x0045,
    ASSET_D_2D_MG_CURLING_NCG_BIN = 0x00c7,
    ASSET_D_2D_MG_CURLING_NCL_BIN = 0x00c8
};

#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling2_c.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"

/* The receiver for the pointer-to-member tables. It must stay incomplete:
 * mwccarm picks the pointer-to-member layout from whether the class is
 * complete. */
struct C;
typedef void (C::*PMF)(int);
typedef void (C::*PMF0)();
typedef void (C::*PMF1)(int);

/* Everything this file calls or reads that no included header declares.
 * Keep it above the first function. */
extern "C" {

extern int  Mg_GetSubmittedScore(void);
extern void Mg_SubmitScore(int self);
extern int  Mg_IsNewRecord(void);
extern int  Mg_GetTopRecord(void);
extern void *Ov004_LoadFileByPath(const char *p);
extern void Mg_RenderSubSpriteMtx(int a, int b, int c, int d);
extern void Mg_SetTopScreenYOffset(int a);
extern void Mg_SetSharedState(s32 state);
extern int  Mg_DrawTopLeftCounter(void *self);
extern void Mg_DrawTopRightCounter(int a, int b);
extern void Mg_DrawNumber(int a, int b, int c, int d, int e, int f, int g);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
extern int  Mg_RenderSpriteBoth(int a, int b, int c, int d, int e);
extern void func_02012718(int id, int v);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, s16 h);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern void Curling2_ResolveStoneCollision(dScMgCurling2_c *self, int idx);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern int  RandomIntInternal(int *seed);
extern int  GetGameLanguage(void);
extern int  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void SetBg2Offset(int a, int b);
extern void SetBg0Offset(int a, int b);
extern void DecompressLZ16(const void *src, void *dst);
extern int  LoadFile(int handle);
extern void Deallocate(void *p);
extern void Ov004_Deallocate(void *p);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void func_02056554(const void *src, int offset, int count);
extern void func_02056314(void *a, u32 b, u32 c);
extern void func_020563d4(const void *a, u32 b, u32 c);
extern void func_02056374(const void *a, u32 b, u32 c);
extern int  func_02054d88(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN2G212GetBG0ScrPtrEv(void);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u32 d);

extern int            data_0209d4b8;
extern u8             data_0209d45c;
extern u8             data_0209d454;
extern s16            data_02082214[];
extern unsigned char sCurling2StepXPicks[];
extern unsigned char sCurling2StepXFastPicks[];
extern int sCurling2StepModes[];
extern int            data_ov006_0212e504[];
extern int            data_ov006_0212e51c[];
extern int sCurling2ScrollX[];
extern int sCurling2ScrollY[];
extern int sCurlingBitSprites[];
extern int sCurling2StoneSprite;
extern int sCurling2TargetStoneSprite;
extern int sCurling2StonesSprites;
extern int *sCurling2CounterSprites[];
extern int sCurling2CursorSprites[];
extern char sCurling2BgJokyuNcgPath;

/* The seven pointer-to-member state tables this file's functions dispatch
 * through. __sinit_ov006_02130758 fills them from the 8-byte {code
 * pointer, zero adjustment} records in ov006 .data; their definitions are
 * the tail of this file. */
extern PMF0 sCurling2DragTable[];
extern PMF sCurling2StepModeTable[];
extern PMF sCurling2StepYTable[];
extern PMF sCurling2StepXFastTable[];
extern PMF1 sCurling2StoneTable[];
extern PMF sCurling2StepXTable[];
extern PMF0 sCurling2BehaviorTable[];
/* The ROM PMF constants all seven state tables copy from (unlicensed
 * .data), grouped by destination table in retail copy order. */
extern PMF0 pmf_dScMgCurling2_c_BeginRound;
extern PMF0 pmf_dScMgCurling2_c_Play;
extern PMF0 pmf_dScMgCurling2_c_NextThrow;
extern PMF0 pmf_dScMgCurling2_c_EndRound;
extern PMF0 pmf_dScMgCurling2_c_Idle;
extern PMF1 pmf_dScMgCurling2_c_StoneWait;
extern PMF1 pmf_dScMgCurling2_c_StoneSlide;
extern PMF1 pmf_dScMgCurling2_c_StoneRest;
extern PMF1 pmf_dScMgCurling2_c_StoneSteer;
extern PMF0 pmf_dScMgCurling2_c_DragBegin;
extern PMF0 pmf_dScMgCurling2_c_DragUpdate;
extern PMF pmf_dScMgCurling2_c_PickStepMode;
extern PMF pmf_dScMgCurling2_c_StepXOnly;
extern PMF pmf_dScMgCurling2_c_StepXAndY;
extern PMF pmf_dScMgCurling2_c_StepXPickFast;
extern PMF pmf_dScMgCurling2_c_StepXPushPosFast;
extern PMF pmf_dScMgCurling2_c_StepXPushNegFast;
extern PMF pmf_dScMgCurling2_c_StepXSettleFast;
extern PMF pmf_dScMgCurling2_c_StepXPick;
extern PMF pmf_dScMgCurling2_c_StepXPushPos;
extern PMF pmf_dScMgCurling2_c_StepXPushNeg;
extern PMF pmf_dScMgCurling2_c_StepXSettle;
extern PMF pmf_dScMgCurling2_c_StepYRestart;
extern PMF pmf_dScMgCurling2_c_StepYRampUp;
extern PMF pmf_dScMgCurling2_c_StepYRampDown;

}  /* extern "C" */

namespace cstd  { s32 sqrt(u64 value); }
namespace Sound { void PlayBank2_2D(unsigned int id); }


// @symbol _ZN15dScMgCurling2_cD1Ev
// @symbol _ZN15dScMgCurling2_cD0Ev
/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. The body is only the compiler-generated vtable store
 * and base destructor call. */
dScMgCurling2_c::~dScMgCurling2_c()
{
}


// @symbol _ZN15dScMgCurling2_c10DrawValuesEv
/* Draw every live mValue. mode 1 and mode 2 pick the last argument. */
void dScMgCurling2_c::DrawValues()
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        unsigned char mode = mValue[i].mode;
        if (mode != 0) {
            int x = mValue[i].x >> 0xc;
            int y = mValue[i].y >> 0xc;
            int value = mValue[i].value;
            if (mode == 1) {
                Mg_DrawNumber(x, y, value, -1, -1, 0, 0);
            } else {
                Mg_DrawNumber(x, y, value, -1, -1, 0, 0x32);
            }
        }
    }
}


// @symbol _ZN15dScMgCurling2_c9AgeValuesEv
/* Age mValue one frame: count lifetime down, apply yInc and decay it
 * by 0x40, and clear the entry when the lifetime expires. */
void dScMgCurling2_c::AgeValues()
{
    int i;
    for (i = 0; i < 0x3c; i++)
    {
        if (mValue[i].live != 0)
        {
            if (mValue[i].lifetime != 0)
            {
                mValue[i].lifetime -= 1;
                mValue[i].y += mValue[i].yInc;
                mValue[i].yInc -= 0x40;
            }
            else
            {
                mValue[i].live = 0;
                mValue[i].mode = 0;
            }
        }
    }
}


// @symbol _ZN15dScMgCurling2_c10SpawnValueEii
/* Take the first free mValue and place it midway between the two
 * stones. The number is (n+1)*10 when either stone carries the 0x468d
 * flag and (n+1)*100 when neither does, where n is the 0x55bf counter,
 * saturating at 0x17.
 *
 * `raw + 0x4000 + other * 0x30 + 0x660` is the same kind of record as
 * `raw + stone * 0x30 + 0x4660` on the other operand. Folding the split
 * base into one constant changes the code: the ROM shares raw + 0x4000
 * across the `other` reads. Do not tidy. */
void dScMgCurling2_c::SpawnValue(int stone, int other)
{
    char *raw = (char *)this;
    int i;
    int sx;
    int sy;
    unsigned char *count;

    for (i = 0; i < 0x3c; i++) {
        if (mValue[i].live != 0) continue;

        sx = mStone[stone].x + mStone[other].x;
        sy = mStone[stone].y + mStone[other].y;

        mValue[i].live = 1;
        mValue[i].mode = 1;
        mValue[i].x = sx >> 1;
        mValue[i].y = sy >> 1;
        mValue[i].lifetime = 0x40;
        mValue[i].xInc = 0;
        mValue[i].yInc = 0;

        /* The && and || arms really do compute the same value; collapsing them
           into one `||` changes the code, so the ROM branched twice too. */
        if (mStone[stone].target != 0 && mStone[other].target != 0) {
            mValue[i].value = (combo + 1) * 10;
        } else if (mStone[stone].target != 0 || mStone[other].target != 0) {
            mValue[i].value = (combo + 1) * 10;
        } else {
            mValue[i].value = (combo + 1) * 100;
            mValue[i].mode = 2;
        }

        count = (unsigned char *)(raw + 0x55bf);
        *count = *count + 1;
        if (combo >= 0x17) combo = 0x17;
        Mg_SubmitScore(mValue[i].value + Mg_GetSubmittedScore());
        return;
    }
}


// @symbol _ZN15dScMgCurling2_c11ClearValuesEv
/* Clear every collision value. */
void dScMgCurling2_c::ClearValues()
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        mValue[i].live = 0;
        mValue[i].mode = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c10DrawPiecesEv
/* Draw every piece whose drawEnable is set, as two stacked sprites
 * from sCurlingBitSprites. */
void dScMgCurling2_c::DrawPieces()
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mPiece[i].drawEnable) {
            int x = mPiece[i].x >> 0xc;
            int y = mPiece[i].y >> 0xc;
            Mg_RenderSubSpriteMtx(sCurlingBitSprites[mPiece[i].sprite0], x, y, 0);
            DrawOamSprite(sCurlingBitSprites[mPiece[i].sprite1], x, y, 0);
        }
    }
}


// @symbol _ZN15dScMgCurling2_c13StepYRampDownEi
/* countdown3, else ease yInc back toward 0x100, else clear yIndex. */
void dScMgCurling2_c::StepYRampDown(int entry)
{
    if (mPiece[entry].countdown3 != 0) {
        short *p = (short *)&mPiece[entry].countdown3;
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (mPiece[entry].yInc > 0x100) {
        int *q = &mPiece[entry].yInc;
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        mPiece[entry].yIndex = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c11StepYRampUpEi
/* Ramp yInc up toward yTarget, then run countdown3 down. When it
 * expires, set yIndex to 2 and reseed the countdown. */
void dScMgCurling2_c::StepYRampUp(int entry)
{
    if (mPiece[entry].yTarget > mPiece[entry].yInc) {
        mPiece[entry].yInc += 0x10;
        if (mPiece[entry].yTarget > mPiece[entry].yInc)
            mPiece[entry].yInc = mPiece[entry].yTarget;
    }
    if (mPiece[entry].countdown3 != 0) {
        mPiece[entry].countdown3 = mPiece[entry].countdown3 - 1;
        if (*(short *)&mPiece[entry].countdown3 < 0) *(short *)&mPiece[entry].countdown3 = 0;
    } else {
        mPiece[entry].yIndex = 2;
        *(short *)&mPiece[entry].countdown3 = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}


// @symbol _ZN15dScMgCurling2_c12StepYRestartEi
/* Zero yInc, pick a fresh yTarget and countdown3, set yIndex to 1. */
void dScMgCurling2_c::StepYRestart(int entry)
{
    unsigned int rnd;

    mPiece[entry].yInc = 0;
    rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mPiece[entry].yTarget = (((rnd << 4) >> 15) << 4) + 0x300;
    mPiece[entry].yIndex = 1;
    rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    rnd = ((rnd << 5) >> 15) + 0x20;
    *(short *)&mPiece[entry].countdown3 = (unsigned char)rnd;
}


// @symbol _ZN15dScMgCurling2_c11StepXSettleEi
/* Step the piece, hold while countdown is set (the hold decrements
 * countdown2, not countdown), then bleed xInc toward zero 8 a frame. */
void dScMgCurling2_c::StepXSettle(int entry)
{
    /* xInc, x, y: the ROM loads those bases in that order. */
    s32 *xInc = &mPiece[entry].xInc;
    s32 *x = &mPiece[entry].x;
    s32 *y = &mPiece[entry].y;
    *x = *x + *xInc;
    *y = *y + mPiece[entry].yInc;
    if (mPiece[entry].countdown != 0) {
        u16 *timer = &mPiece[entry].countdown2;
        *timer = *timer - 1;
        if (*(s16 *)timer < 0) *(s16 *)timer = 0;
        return;
    }
    if (*xInc > 0) {
        *xInc = *xInc - 8;
        if ((s16)*xInc < 0) *xInc = 0;
        return;
    }
    if (*xInc < 0) {
        *xInc = *xInc + 8;
        if (*xInc > 0) *xInc = 0;
        return;
    }
    mPiece[entry].xIndex = 0;
}


// @symbol _ZN15dScMgCurling2_c12StepXPushNegEi
/* Step the piece, hold for countdown, push xInc down to a -0x300
 * floor (the store at the floor is +0x300), hold for countdown2, then
 * hand xIndex to 3. */
void dScMgCurling2_c::StepXPushNeg(int entry)
{
    unsigned short count;

    mPiece[entry].x = mPiece[entry].x + mPiece[entry].xInc;
    mPiece[entry].y = mPiece[entry].y + mPiece[entry].yInc;

    if (mPiece[entry].countdown != 0) {
        mPiece[entry].countdown = mPiece[entry].countdown - 1;
        if (*(short *)&mPiece[entry].countdown < 0)
            mPiece[entry].countdown = 0;
        return;
    }

    if (mPiece[entry].xInc > -0x300) {
        mPiece[entry].xInc -= 8;
        if (mPiece[entry].xInc <= -0x300)
            mPiece[entry].xInc = 0x300;
    }

    count = mPiece[entry].countdown2;
    if (count != 0) {
        mPiece[entry].countdown2 = count - 1;
        if (*(short *)&mPiece[entry].countdown2 < 0)
            mPiece[entry].countdown2 = 0;
        return;
    }

    mPiece[entry].xIndex = 3;
    mPiece[entry].countdown2 = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}


// @symbol _ZN15dScMgCurling2_c12StepXPushPosEi
/* The mirror of StepXPushNeg: same shape, x increment pushed the
 * other way to a +0x300 ceiling. */
void dScMgCurling2_c::StepXPushPos(int entry)
{
    unsigned short count;

    mPiece[entry].x += mPiece[entry].xInc;
    mPiece[entry].y += mPiece[entry].yInc;

    count = mPiece[entry].countdown;
    if (count != 0) {
        *(short *)&mPiece[entry].countdown = count - 1;
        if (*(short *)&mPiece[entry].countdown < 0)
            *(short *)&mPiece[entry].countdown = 0;
        return;
    }

    if (mPiece[entry].xInc < 0x300) {
        mPiece[entry].xInc += 8;
        if (mPiece[entry].xInc >= 0x300)
            mPiece[entry].xInc = 0x300;
    }

    count = mPiece[entry].countdown2;
    if (count != 0) {
        *(short *)&mPiece[entry].countdown2 = count - 1;
        if (*(short *)&mPiece[entry].countdown2 < 0)
            *(short *)&mPiece[entry].countdown2 = 0;
        return;
    }

    *(char *)&mPiece[entry].xIndex = 3;
    *(short *)&mPiece[entry].countdown2 = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}


// @symbol _ZN15dScMgCurling2_c9StepXPickEi
/* Hold for countdown, then zero xInc, pick xIndex from
 * sCurling2StepXPicks and reseed both countdowns. */
void dScMgCurling2_c::StepXPick(int entry)
{
    if (mPiece[entry].countdown != 0) {
        mPiece[entry].countdown = mPiece[entry].countdown - 1;
        if (*(short *)&mPiece[entry].countdown < 0) *(short *)&mPiece[entry].countdown = 0;
        return;
    }
    mPiece[entry].xInc = 0;
    mPiece[entry].xIndex =
        sCurling2StepXPicks[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                                << 1 >>
                            15];
    mPiece[entry].countdown = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    mPiece[entry].countdown2 = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}


// @symbol _ZN15dScMgCurling2_c9StepXAndYEi
/* Run one piece's xIndex callback, then its yIndex callback.
 * yIndex is re-read through the pointer-to-member receiver. */
void dScMgCurling2_c::StepXAndY(int entry)
{
    C *self = (C *)this;
    unsigned char k0 = mPiece[entry].xIndex;
    (self->*sCurling2StepXTable[k0])(entry);
    unsigned char k1 = ((dScMgCurling2_c *)self)->mPiece[entry].yIndex;
    (self->*sCurling2StepYTable[k1])(entry);
}


// @symbol _ZN15dScMgCurling2_c15StepXSettleFastEi
/* Step the piece, then bleed xInc toward zero 0x20 a frame. */
void dScMgCurling2_c::StepXSettleFast(int entry)
{
    /* x, xInc, y -- the ROM's load order, not a tidy one. */
    int *x = &mPiece[entry].x;
    int *xInc = &mPiece[entry].xInc;
    int *y = &mPiece[entry].y;
    *x += *xInc;
    *y += mPiece[entry].yInc;
    if (*xInc > 0) {
        *xInc -= 0x20;
        if ((int)(short)*xInc < 0) *xInc = 0;
    } else if (*xInc < 0) {
        *xInc += 0x20;
        if (*xInc > 0) *xInc = 0;
    } else {
        mPiece[entry].xIndex = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c16StepXPushNegFastEi
/* As StepXPushNeg, 0x20 a frame and a -0x400 floor. The store at the
 * floor is +0x400, and xIndex goes to 3 with no new countdown. */
void dScMgCurling2_c::StepXPushNegFast(int entry)
{
    mPiece[entry].x = mPiece[entry].x + mPiece[entry].xInc;
    mPiece[entry].y = mPiece[entry].y + mPiece[entry].yInc;

    if (mPiece[entry].countdown != 0) {
        *(s16 *)&mPiece[entry].countdown =
            (s16)(mPiece[entry].countdown - 1);
        if (*(s16 *)&mPiece[entry].countdown < 0)
            *(s16 *)&mPiece[entry].countdown = 0;
        return;
    }

    if (mPiece[entry].xInc > -0x400) {
        mPiece[entry].xInc -= 0x20;
        if (mPiece[entry].xInc <= -0x400)
            mPiece[entry].xInc = 0x400;
    }

    if (mPiece[entry].countdown2 != 0) {
        *(s16 *)&mPiece[entry].countdown2 =
            (s16)(mPiece[entry].countdown2 - 1);
        if (*(s16 *)&mPiece[entry].countdown2 < 0)
            *(s16 *)&mPiece[entry].countdown2 = 0;
        return;
    }

    mPiece[entry].xIndex = 3;
}


// @symbol _ZN15dScMgCurling2_c16StepXPushPosFastEi
/* The mirror of StepXPushNegFast, x increment pushed to a +0x400
 * ceiling. */
void dScMgCurling2_c::StepXPushPosFast(int entry)
{
    mPiece[entry].x += mPiece[entry].xInc;
    mPiece[entry].y += mPiece[entry].yInc;

    {
        unsigned short count = mPiece[entry].countdown;
        if (count != 0) {
            *(short *)&mPiece[entry].countdown = (short)(count - 1);
            if (*(short *)&mPiece[entry].countdown < 0) {
                *(short *)&mPiece[entry].countdown = 0;
            }
            return;
        }
    }

    if (mPiece[entry].xInc < 0x400) {
        mPiece[entry].xInc += 0x20;
        if (mPiece[entry].xInc >= 0x400) {
            mPiece[entry].xInc = 0x400;
        }
    }

    {
        unsigned short count2 = mPiece[entry].countdown2;
        if (count2 != 0) {
            *(short *)&mPiece[entry].countdown2 = (short)(count2 - 1);
            if (*(short *)&mPiece[entry].countdown2 < 0) {
                *(short *)&mPiece[entry].countdown2 = 0;
            }
            return;
        }
    }

    mPiece[entry].xIndex = 3;
}


// @symbol _ZN15dScMgCurling2_c13StepXPickFastEi
/* Hold for countdown, then zero xInc, pick a fresh yInc and an xIndex
 * from sCurling2StepXFastPicks, and reseed both countdowns. */
void dScMgCurling2_c::StepXPickFast(int entry)
{
    if (mPiece[entry].countdown != 0) {
        mPiece[entry].countdown = mPiece[entry].countdown - 1;
        if (*(short *)&mPiece[entry].countdown < 0) *(short *)&mPiece[entry].countdown = 0;
        return;
    }
    mPiece[entry].xInc = 0;
    mPiece[entry].yInc = ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 4) + 0x600;
    mPiece[entry].xIndex =
        sCurling2StepXFastPicks[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                                    << 1 >>
                                15];
    mPiece[entry].countdown = (unsigned short)((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) & 0xff);
    mPiece[entry].countdown2 = (unsigned short)(((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x30) >> 0xf) + 0x30) & 0xff);
}


// @symbol _ZN15dScMgCurling2_c9StepXOnlyEi
/* Run one piece's sCurling2StepXFastTable callback, indexed by xIndex. */
void dScMgCurling2_c::StepXOnly(int entry)
{
    unsigned char idx = mPiece[entry].xIndex;
    (((C *)this)->*sCurling2StepXFastTable[idx])(entry);
}


// @symbol _ZN15dScMgCurling2_c12PickStepModeEi
/* Pick modeIndex from sCurling2StepModes (1 on one roll in eight)
 * and clear xIndex. */
void dScMgCurling2_c::PickStepMode(int entry)
{
    unsigned rnd = (unsigned)RandomIntInternal(&data_0209d4b8);
    int pick = 0;
    unsigned roll = ((rnd >> 16) & 0x7fff) << 3 >> 0xf;
    if (roll == 5) pick = 1;
    mPiece[entry].modeIndex = sCurling2StepModes[pick];
    mPiece[entry].xIndex = 0;
}


// @symbol _ZN15dScMgCurling2_c10StepPiecesEv
/* Step every piece whose updateEnable is set, and wrap the ones whose
 * y has passed 0xc8 back to a random x at y = -0x8000. */
void dScMgCurling2_c::StepPieces()
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mPiece[i].updateEnable != 0) {
            unsigned char idx = mPiece[i].modeIndex;
            (((C *)this)->*sCurling2StepModeTable[idx])(i);
            if ((mPiece[i].y >> 0xc) >= 0xc8) {
                mPiece[i].x = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                mPiece[i].y = -0x8000;
                mPiece[i].xIndex = 0;
                mPiece[i].modeIndex = 0;
                mPiece[i].yIndex = 0;
            }
        }
    }
}


// @symbol _ZN15dScMgCurling2_c10SeedPiecesEv
/* Clear every piece, then seed it: enabled, a random x, two sprite
 * indices (the second is the first plus one to four, modulo five) and a
 * y spread over the screen. The first x, the y at -0x8000 and the first
 * countdown store are overwritten a few lines later; those dead stores
 * are the ROM's. */
void dScMgCurling2_c::SeedPieces()
{
    int i;
    unsigned int rnd;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    for (; i < 0x32; i++)
    {
        mPiece[i].x = 0;
        mPiece[i].y = 0;
        mPiece[i].xInc = 0;
        mPiece[i].yInc = 0;
        mPiece[i].countdown = 0;
        mPiece[i].countdown2 = 0;
        mPiece[i].countdown3 = 0;
        mPiece[i].updateEnable = 0;
        mPiece[i].modeIndex = 0;
        mPiece[i].xIndex = 0;
        mPiece[i].yIndex = 0;
        mPiece[i].drawEnable = 0;
        mPiece[i].sprite0 = 0;
        mPiece[i].sprite1 = 1;
    }

    i = 0;
    for (; i < 0x32; i++)
    {
        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((rnd >> 16) & 0x7fff) << 5;
        mPiece[i].x = (int)((m >> 0xf)) << 0xf;
        mPiece[i].y = -0x8000;
        mPiece[i].updateEnable = 1;
        mPiece[i].drawEnable = 1;
        mPiece[i].modeIndex = 0;
        mPiece[i].xIndex = 0;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        mPiece[i].sprite0 = (char)(((rnd >> 16) & 0x7fff) * 5 >> 0xf);

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        /* Own local: `mPiece[i].sprite0 + shift` puts the byte in r0. */
        unsigned char spr = mPiece[i].sprite0;
        v = spr + ((((rnd >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        mPiece[i].sprite1 = (char)v;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        mPiece[i].countdown = (short)(((i & 7) << 6) + (((rnd >> 16) & 0x7fff) * 0x30 >> 0xf));

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((rnd >> 16) & 0x7fff) << 5;
        mPiece[i].x = (int)((m >> 0xf)) << 0xf;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((rnd >> 16) & 0x7fff) * 0x1a) >> 0xf;
        mPiece[i].y = (((q << 3) - 8)) << 0xc;
        mPiece[i].countdown = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c9DrawMarksEv
/* Draw the five score marks whose drawn flag is set, each with its
 * value as the caption.
 *
 * `(i & 0xFFFFFFFF)` stays: 0xFFFFFFFF is unsigned, so the mask makes this
 * subscript unsigned, and deleting it changes the code. The records stay
 * at raw offsets -- &mMark[i] folds the base differently. */
void dScMgCurling2_c::DrawMarks()
{
    int x;
    int i;
    unsigned char *raw = (unsigned char *)this;

    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
    for (i = 0; i < 5; i++) {
        unsigned char *rec = raw + (i & 0xFFFFFFFF) * 0x10;
        if (*(unsigned char *)(rec + 0x487d)) {
            x = *(int *)(rec + 0x4870) >> 12;
            Mg_DrawBigNumber(x, *(int *)(rec + 0x4874) >> 12,
                                *(unsigned short *)(rec + 0x4878), -1, -1, 0, 0);
        }
    }
}


/* Age the five score marks: run the countdown down
 * and, on the frame it reaches zero, set the flag that DrawMarks draws on and
 * play sound 0x1bc. The `opt_strength_reduction off` bracket is needed
 * (see the banner). The records stay at raw offsets -- &mMark[i] folds
 * the base differently. */
#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN15dScMgCurling2_c8AgeMarksEv
void dScMgCurling2_c::AgeMarks()
{
    char *raw = (char *)this;
    int i;

    for (i = 0; i < 5; i++) {
        char *rec = raw + (i << 4);
        unsigned short *timer;
        if (mMark[i].enable == 0)
            continue;
        if (mMark[i].countdown == 0)
            continue;
        timer = (unsigned short *)(rec + 0x487a);
        *timer = *timer - 1;
        if (*timer != 0) continue;
        mMark[i].drawn = 1;
        Sound::PlayBank2_2D(0x1bc);
    }
}
#pragma pop


// @symbol _ZN15dScMgCurling2_c10DrawCursorEv
/* Draw the drag cursor at the mDragX/mDragY drag point,
 * offset up and left, while the mShowCursor flag is set. */
void dScMgCurling2_c::DrawCursor()
{
    if (mShowCursor == 0)
        return;

    int x = mDragX;
    int y = mDragY;
    Mg_RenderSpriteBoth((int)sCurling2CursorSprites, (x >> 12) - 0x20, (y >> 12) - 8, -1, 0);
}

/* Stylus drag update. Runs while the player drags a stone before the throw. With no touch in
 * the current input record it just drops the drag flags. Otherwise it moves
 * the drag point to the stylus plus the grab offset, clamps it to the rink,
 * and bails out (restoring the old point) when the stylus moved a unit or
 * less. The vertical direction-change tracker (mDragPhase, mLastDragDy,
 * mTurnY) plays the scrape sound on the first move and flips phases on
 * reversals; the throw angle (mThrowAngle) is atan2 of the move with the
 * horizontal halved, snapped to straight up inside [0x4000,0x8000] and to
 * zero below, then averaged with the previous frame; the throw power
 * (mThrowPower) follows the move length times 9 with a 0xc000 cap, rising
 * instantly and decaying by half. Finally the grab offset is recomputed
 * from the new point and a fresh read of the input index.
 *
 * Both pragmas are load-bearing. The ROM shares nothing across an extended
 * basic block that 2004/b56 would share by default: dx >> 1 is recomputed
 * after the atan2 call and the input index is re-read at the end, so
 * opt_common_subs is off, and every value the ROM does share is a named
 * local here (i4, bx/by, dy2, j-free ax/ay). opt_propagation off keeps the
 * stylus byte loads above the first store (the ROM loads both before it)
 * and puts i4 in r4. Loading by before bx, and ay before ax, colours the
 * two scratch pairs the ROM's way; the twin Aim_Swing in
 * dScMgCurling_c reads the same. */
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
// @symbol _ZN15dScMgCurling2_c10DragUpdateEv
void dScMgCurling2_c::DragUpdate()
{
    dScMgCurling2_c *self = this;
    u8 idx;
    int dx;
    int oldx;
    int oldy;
    int diff;
    u16 oldAng;
    int mag;
    int i4;

    idx = gActivePlayerSlot;
    i4 = idx * 4;
    if (gTouchHeld[idx * 4] != 0) {
        int dy2;
        int dy;
        u8 bx;
        u8 by;

        by = gTouchY[i4];
        bx = gTouchX[i4];
        oldx = self->mDragX;
        oldy = self->mDragY;
        self->mDragX = self->mDragOffsetX + (bx << 12);
        self->mDragY = self->mDragOffsetY + (by << 12);
        if (self->mDragY <= 0x94000)
            self->mDragY = 0x94000;
        if (self->mDragX <= 0x20000)
            self->mDragX = 0x20000;
        if (self->mDragX >= 0xe0000)
            self->mDragX = 0xe0000;
        if (self->mDragY >= 0xb8000)
            self->mDragY = 0xb8000;
        dx = (self->mDragX - oldx) >> 12;
        dy = (self->mDragY - oldy) >> 12;
        dy2 = dy * dy;
        if (cstd::sqrt((u64)(dx * dx + dy2)) <= 1) {
            self->mDragX = oldx;
            self->mDragY = oldy;
            return;
        }
        diff = (self->mDragY - self->mTurnY) >> 12;
        if (self->mDragPhase == 0) {
            func_02012718(0x1d6, self->mDragX);
            self->mDragPhase = 2;
            self->mLastDragDy = (self->mDragY - self->mTurnY) >> 12;
            self->mTurnY = self->mDragY;
        } else if (self->mDragPhase == 1) {
            if (self->mLastDragDy * diff > 0) {
                if (diff < 0) diff = -diff;
                if (diff >= 0xa)
                    self->mDragPhase = 0;
            } else {
                self->mLastDragDy = diff;
                self->mTurnY = self->mDragY;
            }
        } else {
            if (self->mLastDragDy * diff < 0)
                self->mDragPhase = 1;
            self->mLastDragDy = (self->mDragY - self->mTurnY) >> 12;
            self->mTurnY = self->mDragY;
        }
        oldAng = self->mThrowAngle;
        self->mThrowAngle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = self->mThrowAngle;

            if (a <= 0x8000 && a >= 0x4000) {
                self->mThrowAngle = 0x8000;
            } else if (a <= 0x4000) {
                self->mThrowAngle = 0;
            }
        }
        self->mThrowAngle = (self->mThrowAngle + oldAng) >> 1;
        mag = cstd::sqrt((u64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
        mag = (mag << 12) >> 4;
        if (mag >= 0xc000) mag = 0xc000;
        if (mag > self->mThrowPower)
            self->mThrowPower = mag;
        if (self->mThrowPower > mag)
            self->mThrowPower -= (self->mThrowPower - mag) >> 1;
        {
            int j = gActivePlayerSlot;
            int ay = (self->mDragY >> 12) - gTouchY[j * 4];
            int ax = (self->mDragX >> 12) - gTouchX[j * 4];

            self->mDragOffsetX = ax << 12;
            self->mDragOffsetY = ay << 12;
        }
        return;
    }
    self->mDragState = 0;
    self->mShowCursor = 1;
}
#pragma pop


// @symbol _ZN15dScMgCurling2_c9DragBeginEv
/* Begin a drag: latch the grab offset and the first angle. Entry 0 of
 * sCurling2DragTable; DragUpdate is entry 1. */
void dScMgCurling2_c::DragBegin()
{
    int idx;
    int flag = 0;
    int x;
    int y;

    idx = gActivePlayerSlot;
    /* `4 & 0xFFFFFFFF` is not decoration: 0xFFFFFFFF is unsigned, so the mask
       makes this one subscript unsigned where the next is signed, and that is
       what the ROM's index arithmetic does.  Deleting it costs 20 words. */
    if (gTouchHeld[idx * (4 & 0xFFFFFFFF)] != 0)
    {
        if (gTouchEdge[idx * 4] != 0)
            flag = 1;
    }
    if (flag == 0)
        return;

    x = (mDragX >> 0xc) - gTouchX[idx * 4];
    y = (mDragY >> 0xc) - gTouchY[idx * 4];

    mDragOffsetX = x << 0xc;
    mDragOffsetY = y << 0xc;
    mDragState = 1;
    mThrowAngle = 0xc000;

    if (sndCooldown == 0)
    {
        func_02012718(0x1d2, mDragX);
        sndCooldown = 6;
    }

    unk_55a0 = 0;
    unk_55a4 = 0;
    mGrabX = mDragX + mDragOffsetX;
    mTurnY = mDragY + mDragOffsetY;
    mLastDragDy = 0xff;
    mDragPhase = 0;
}


// @symbol _ZN15dScMgCurling2_c11DrawCounterEv
/* Draw the counter at the bottom of the screen: a fixed
 * sprite, a language-dependent label out of sCurling2CounterSprites, and 5 minus
 * the thrown count, floored at zero. */
void dScMgCurling2_c::DrawCounter()
{
    int left;
    if (mShowCounter == 0)
        return;
    left = 5 - thrown;
    if (left < 0)
        left = 0;
    Mg_RenderSubSpriteMtx((int)&sCurling2StoneSprite, 0xd0, 0xb4, 0);
    Mg_RenderSubSpriteMtx(sCurling2CounterSprites[GetGameLanguage()][1], 0xe8, 0xb4, 0);
    Mg_DrawBigNumber(0xf8, 0xb4, left, 0, 0, 0, 0);
}


// @symbol _ZN15dScMgCurling2_c10DrawStonesEv
/* Draw each live, visible stone on both screens: the body
 * sprite, picked by the stone's target flag, and a shadow eight pixels down. */
void dScMgCurling2_c::DrawStones()
{
    int x, y;
    int i;

    for (i = 0; i < 0xb; i++) {
        void *tex;
        if (mStone[i].active == 0) continue;
        if (mStone[i].visible == 0) continue;
        x = mStone[i].x >> 12;
        y = mStone[i].y >> 12;
        if (mStone[i].target != 0)
            tex = &sCurling2TargetStoneSprite;
        else
            tex = &sCurling2StoneSprite;
        RenderOamBothScreens(tex, x, y, -1, 1, 0);
        RenderOamBothScreens(&sCurling2StonesSprites, x, y + 8, -1, 2, 0);
    }
}


// @symbol _ZN15dScMgCurling2_c14SeparateStonesEi
/* Stone separation. The two-player rink's twin of SeparateStones in
 * dScMgCurling_c: eleven stones at a 0x30 stride instead of five at 0x2c,
 * and after the bump sound the pushed pair gets a SpawnValue. Keep vx and
 * vy named and the inner scan's locals declared in the loop body; both
 * shapes are needed, as in the Curling version. */
void dScMgCurling2_c::SeparateStones(int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 11; i++) {
        if (mStone[i].active == 0) continue;
        if (idx == i) continue;
        dx = (mStone[i].x - mStone[idx].x) >> 12;
        dy = (mStone[i].y - mStone[idx].y) >> 12;
        dist = cstd::sqrt((u64)(dx * dx + dy * dy));
        ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        if (dist > 0x18) continue;
        {
            int cs;
            int sn;
            int vx;
            int vy;

            k = (ang >> 4) * 2;
            cs = data_02082214[k + 1];
            vx = FX_MUL(cs, 0x1a);
            mStone[i].x = mStone[idx].x + (vx << 12);
            sn = data_02082214[k];
            vy = FX_MUL(sn, 0x1a);
            mStone[i].y = mStone[idx].y + (vy << 12);
            for (j = 0; j < 11; j++) {
                int dx2;
                int dy2;
                int dist2;
                u16 ang2;

                if (mStone[j].active == 0) continue;
                if (i == j) continue;
                dx2 = (mStone[j].x - mStone[i].x) >> 12;
                dy2 = (mStone[j].y - mStone[i].y) >> 12;
                dist2 = cstd::sqrt((u64)(dx2 * dx2 + dy2 * dy2));
                ang2 = _ZN4cstd5atan2E5Fix12IiES1_(dy2, dx2);
                if (dist2 > 0x18) continue;
                {
                    int cs2;
                    int sn2;
                    int vx2;
                    int vy2;

                    k = (ang2 >> 4) * 2;
                    cs2 = data_02082214[k + 1];
                    vx2 = FX_MUL(cs2, 0x1a);
                    mStone[j].x = mStone[i].x + (vx2 << 12);
                    sn2 = data_02082214[k];
                    vy2 = FX_MUL(sn2, 0x1a);
                    mStone[j].y = mStone[i].y + (vy2 << 12);
                    func_02012718(0xe8, mStone[idx].x);
                    SpawnValue(idx, i);
                    return;
                }
            }
            return;
        }
    }
}


// @symbol Curling2_ResolveStoneCollision
/* Stone collision: the first live, moving stone within 0x18 of stone idx
 * takes the hit. Both velocities are rotated into the contact frame, the
 * along-contact parts swapped, and the results become each stone's new
 * angle and speed; the moving stone is set back 0x1b000 along the contact
 * line, the hit one is clamped to the rink. Then the bump sound and a
 * SpawnValue for the pair. StoneSlide calls it directly and StoneRest
 * through a veneer; it keeps C linkage under its recovered name.
 *
 * Two spellings are codegen levers, not style: the contact angle's cosine
 * and sine get explicit long long copies (cw, sw), cast where they are
 * made -- without the cast the sine's sign word is hoisted above the hit
 * stone's table reads -- and the hit stone's vex is computed before the
 * contact table words are read, which keeps both loads below the hit
 * stone's chain. The rest (one rel/k/c/s set for both stones, pointer
 * locals for the fields written after a call, the hitAngle reference) are
 * described in notes/mwccarm-codegen.md 6cz. */
extern "C" void Curling2_ResolveStoneCollision(dScMgCurling2_c *self, int idx)
{
    long long sw;
    long long cw;
    int i;
    int dx;
    int dy;

    for (i = 0; i < 11; i++) {
        if (self->mStone[i].active == 0) continue;
        if (idx == i) continue;
        if (self->mStone[i].state == 0) continue;
        if (self->mStone[i].state == 3) continue;
        dy = self->mStone[i].y;
        dx = self->mStone[i].x - self->mStone[idx].x;
        dy -= self->mStone[idx].y;
        if ((cstd::sqrt((u64)((long long)dx * dx + (long long)dy * dy)) >> 12) >= 0x18) continue;
        {
            int nex;
            int ney;
            u16 ang;
            u16 rel;
            int k;
            int E;
            int c;
            int s;
            int sP;
            int cP;
            int vmx;
            int vmy;
            int vex;
            int vey;
            int yi;
            int xi;
            int *pHitX;
            int *pX;
            int *pHitY;

            dx = self->mStone[i].x - self->mStone[idx].x;
            dy = self->mStone[i].y - self->mStone[idx].y;
            pHitX = &self->mStone[i].x;
            pX = &self->mStone[idx].x;
            pHitY = &self->mStone[i].y;
            ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            u16 &hitAngle = self->mStone[i].angle;
            E = (ang >> 4) * 2;
            rel = self->mStone[idx].angle - ang;
            k = (rel >> 4) * 2;
            c = data_02082214[k + 1];
            s = data_02082214[k];
            vmx = FX_MUL(c, self->mStone[idx].speed);
            vmy = FX_MUL(s, self->mStone[idx].speed);
            rel = self->mStone[i].angle - ang;
            k = (rel >> 4) * 2;
            c = data_02082214[k + 1];
            s = data_02082214[k];
            vex = FX_MUL(c, self->mStone[i].speed);
            sP = data_02082214[E];
            cP = data_02082214[E + 1];
            vey = FX_MUL(s, self->mStone[i].speed);
            cw = (long long)cP;
            dy = (int)((cw * vex + 0x800) >> 12) - FX_MUL(sP, vmy);
            sw = (long long)sP;
            dx = (int)((sw * vex + 0x800) >> 12) + FX_MUL(cP, vmy);
            nex = FX_MUL(cP, vmx) - FX_MUL(sP, vey);
            ney = (int)((sw * vmx + 0x800) >> 12) + FX_MUL(cP, vey);
            self->mStone[idx].angle = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
            self->mStone[idx].speed = cstd::sqrt((u64)((long long)dy * dy + (long long)dx * dx));
            self->mStone[idx].x = self->mStone[i].x - FX_MUL(cP, 0x1b000);
            self->mStone[idx].y = self->mStone[i].y - (int)((sw * 0x1b000 + 0x800) >> 12);
            xi = self->mStone[idx].x >> 12;
            yi = self->mStone[idx].y >> 12;
            if (xi - 0xc < 0) {
                xi = self->mStone[idx].x - 0xc000;
                *pHitX += xi;
                self->mStone[idx].x = 0xc000;
            }
            if (xi + 0xc > 0x100) {
                *pHitX += self->mStone[idx].x - 0xf4000;
                self->mStone[idx].x = 0xf4000;
            }
            if (yi - 0xc < -0xe0) {
                self->mStone[idx].y = -0xd4000;
                *pHitY = self->mStone[idx].y + 0x18000;
            }
            hitAngle = _ZN4cstd5atan2E5Fix12IiES1_(ney, nex);
            self->mStone[i].speed = cstd::sqrt((u64)((long long)nex * nex + (long long)ney * ney));
            self->mStone[idx].state = 1;
            self->mStone[i].state = 1;
            if (self->mStone[i].speed >= 0x3800) {
                self->mStone[i].fast = 1;
            } else {
                self->mStone[i].fast = 0;
            }
            func_02012718(0xe8, *pX);
            self->SpawnValue(idx, i);
            return;
        }
    }
}


// @symbol _ZN15dScMgCurling2_c9StoneSpinEi
/* Spin-rate update for the stone sprite: -sin(heading) * speed, scaled.
 * The offsets stay raw this + i * 0x30 + 0x46xx, like SpawnValue's
 * 0x4000 base in the lower TU: folding them through &mStone[i] changes
 * the base rematerialization. */
void dScMgCurling2_c::StoneSpin(int i)
{
    char *o = (char *)this + i * 0x30;
    int h = *(unsigned short *)(o + 0x4686);
    int idx = (((h >> 4) << 1) + 1) << 1;
    int s = *(short *)((char *)data_02082214 + idx);
    int v = *(int *)(o + 0x4668);
    long long m = (long long)s * v;
    int hi = (int)(((unsigned long long)(m + 0x800)) >> 12);
    *(short *)(o + 0x4682) = (short)((-hi) >> 2);
}


// @symbol _ZN15dScMgCurling2_c10StoneSteerEi
/* Stone state 3: while the touch is down, the stylus feeds velX/velY out
 * of the input-record deltas (x clamped to the rink); once it lifts, the
 * stone is armed again and, when the drag point is close, snaps above it. */
void dScMgCurling2_c::StoneSteer(int idx)
{
    u8 i = gActivePlayerSlot;
    int off = i * 4;
    if (gTouchHeld[i * 4] != 0) {
        int bp = mStone[idx].velX;
        mStone[idx].x = bp + (gTouchX[off] << 12);
        int t = mStone[idx].x >> 12;
        if (t < 0xe) mStone[idx].x = 0xe000;
        if (t > 0xf2) mStone[idx].x = 0xf2000;
        int av = mStone[idx].x >> 12;
        int bv = av - gTouchX[i * 4];
        int cv = mStone[idx].y >> 12;
        int dv = cv - gTouchY[i * 4];
        mStone[idx].velX = bv << 12;
        mStone[idx].velY = dv << 12;
    } else {
        mStone[idx].state = 0;
        int dx = (mDragX - mStone[idx].x) >> 12;
        int cy = mStone[idx].y;
        int dy = (mDragY - cy) >> 12;
        if (dx < -0x2e) return;
        if (dx > 0x2e) return;
        if (dy < -0x14) return;
        if (dy > 0x14) return;
        mDragY = cy + 0x15000;
    }
}


// @symbol _ZN15dScMgCurling2_c9StoneRestEi
/* Stone state 2: the resting handler veneers to the sourceless collision
 * step. See the file-top leftover for Curling2_ResolveStoneCollision. */
void dScMgCurling2_c::StoneRest(int i)
{
    Curling2_ResolveStoneCollision(this, i);
}


// @symbol _ZN15dScMgCurling2_c10StoneSlideEi
/* Stone state 1: integrate by speed at the heading, bounce off the rink
 * walls (the bounce flips the heading and plays 0x1d4), decay speed to a
 * stop, run the collision step and the spin update, then feed the rolling
 * sound from speed and position. */
void dScMgCurling2_c::StoneSlide(int idx)
{
    char *c = (char *)this;
    int m;
    int zi;
    u16 *pang;
    int *p668;
    int *p660;
    int *p664;
    int x;
    int z;
    int xi;
    int v;
    int p;
    int w;
    int *pd;
    int sn;
    int cs;

    m = idx * 0x30;

    pang = (u16 *)(c + 0x4686 + m);
    p668 = (int *)(c + 0x4668 + m);
    p660 = (int *)(c + 0x4660 + m);
    p664 = (int *)(c + 0x4664 + m);

    sn = data_02082214[((*pang) >> 4) * 2 + 1];
    *p660 += FX_MUL(sn, *p668);
    cs = data_02082214[((*pang) >> 4) * 2];
    *p664 += FX_MUL(cs, *p668);
    *(u16 *)(c + 0x4684 + m) += *(u16 *)(c + m + 0x4682);

    x = *p660;
    z = *p664;
    xi = x >> 12;
    zi = z >> 12;

    if (xi + 0xc >= 0x100) {
        *pang = 0x8000 - *pang;
        *p660 = 0xf4000;
        func_02012718(0x1d4, *p660);
    } else if (xi - 0xc < 0) {
        *pang = 0x8000 - *pang;
        *p660 = 0xc000;
        func_02012718(0x1d4, *p660);
    }

    if (zi + 0xc > 0xc0) {
        *(u16 *)(c + 0x4686 + m) = -*(u16 *)(c + 0x4686 + m);
        *p664 = 0xb4000;
        func_02012718(0x1d4, *p660);
    } else if (zi - 0xc < -0xe0) {
        *(u16 *)(c + 0x4686 + m) = -*(u16 *)(c + 0x4686 + m);
        *p664 = -0xd4000;
        func_02012718(0x1d4, *p660);
    }

    zi = *p668;
    v = zi >> 9;
    if (v <= 0x1c)
        v = 0x1c;

    *(int *)(c + 0x4668 + m) -= v;
    pd = (int *)(c + 0x4668 + m);
    if (*(int *)(c + 0x4668 + m) <= 0) {
        *p668 = 0;
        *(u8 *)(c + idx * 0x30 + 0x4688) = 2;
    }

    Curling2_ResolveStoneCollision((dScMgCurling2_c *)c, idx);
    StoneSpin(idx);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    *(int *)(c + 0x467c + m) = func_02012468(*(int *)(c + 0x467c + m), 2, 0xe7, 7, p, w, func_020126e8(*p660), 0);
}


// @symbol _ZN15dScMgCurling2_c9StoneWaitEi
/* Stone state 0: armed, waiting. While a drag is on (mDragState == 1) and
 * the drag point overlaps the stone, throw it at the latched angle and
 * power (clamped to the legal cone), mark it fast when the power is high,
 * then re-arm the spawn slot and separate it from any stone it landed on. */
void dScMgCurling2_c::StoneWait(int idx)
{
    if (mDragState != 1)
        return;

    int v = (mDragX - mStone[idx].x) >> 12;
    int w = (mDragY - mStone[idx].y) >> 12;
    if (v < -0x2e) return;
    if (v > 0x2e) return;
    if (w < -0x14) return;
    if (w > 0x14) return;

    mStone[idx].state = 1;
    mStone[idx].angle = mThrowAngle;
    mStone[idx].speed = mThrowPower;

    if (mThrowAngle < 0x9800u || mThrowAngle > 0xe800u) {
        if (mThrowAngle >= 0x4000u && mThrowAngle <= 0x9800u) {
            mThrowAngle = 0x9800;
        } else {
            mThrowAngle = 0xe800;
        }
        mStone[idx].angle = mThrowAngle;
    }

    if (mThrowPower >= 0x3800) {
        mStone[idx].fast = 1;
    } else {
        mStone[idx].fast = 0;
    }

    spawning = 1;
    mSpawnTimer = 0;
    combo = 0;
    unk_55c0 = 0;
    unk_55c1 = 0;
    SeparateStones(idx);

    int vol = 0x7f;
    if (mStone[idx].fast == 0) vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0,
                  func_020126e8(mStone[idx].x));
}


// @symbol _ZN15dScMgCurling2_c9NextStoneEv
/* Queue and deal the next stone: while spawning is set and the timer has
 * run out, activate stone[thrown] at the top of the rink, seed the house
 * stones on the first deal (the later deals play 0x1d7), then re-arm the
 * drag point and the draw flags. */
void dScMgCurling2_c::NextStone()
{
    char *self = (char *)this;
    if ((spawning) == 0) {
        return;
    }
    if ((*((unsigned short *) ((self + 0x5500) + 0xb4))) != 0)
    {
        mSpawnTimer -= 1;
        if (*((short *) ((self + 0x5500) + 0xb4)) <= 0)
        {
            *((unsigned short *) ((self + 0x5500) + 0xb4)) = 0;
        }
        return;
    }
    spawning = 0;
    {
        int n = thrown;
        char *ip;
        char *p;
        char *q;
        if (n >= 5)
        {
            return;
        }
        ip = self + (n * 0x30);
        p = ip + 0x4000;
        *((unsigned char *) ((ip + 0x4000) + 0x689)) = 1;
        *((unsigned char *) ((ip + 0x4000) + 0x68a)) = 1;
        *((int *) (p + 0x660)) = 0x80000;
        *((int *) ((ip + 0x4000) + 0x664)) = 0x80000;
        q = ip + 0x4600;
        *((unsigned short *) (q + 0x80)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x68b)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x688)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x68d)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x68c)) = 0;
        if ((thrown) == 0) {
            SeedStones();
        } else {
            Sound::PlayBank2_2D(0x1d7);
        }
        *((unsigned char *) ((int)self + 0x55ba)) += 1;
        mDragX = 0x80000;
        mDragY = 0xb0000;
        mDragState = 0;
        mShowCursor = 1;
        ClearStoneFlags();
    }
}


// @symbol _ZN15dScMgCurling2_c10SeedStonesEv
/* Place the six house stones (mStone[5]..mStone[10]) at their fixed
 * positions, already at rest. */
void dScMgCurling2_c::SeedStones()
{
    struct E
    {
        char p[0x30];
    };
    int i;
    for (i = 0; i < 6; i++)
    {
        char *e = ((char *) (&((struct E *) this)[i + 5])) + 0x4000;
        *((unsigned char *) (e + 0x689)) = 1;
        *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x68a)) = 1;
        *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x660)) = data_ov006_0212e504[i] << 0xc;
        *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x664)) = (-(data_ov006_0212e51c[i] + 0x20)) << 0xc;
        *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x688)) = 2;
        *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x668)) = 0;
        *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x68d)) = 1;
    }
}


// @symbol _ZN15dScMgCurling2_c15ClearStoneFlagsEv
/* Clear the +0x2c flag on every stone. */
void dScMgCurling2_c::ClearStoneFlags()
{
    int i;
    for (i = 0; i < 0xb; i++) {
        mStone[i].unk2c = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c4IdleEv
/* Behavior state 4: terminal. */
void dScMgCurling2_c::Idle()
{
}


// @symbol _ZN15dScMgCurling2_c8EndRoundEv
/* Behavior state 3: the last stone has settled. Wait out the state timer,
 * clear the round flags, hide the stones, drop the prompt, hand the scene
 * to Mg_SetSharedState and park in state 4. */
void dScMgCurling2_c::EndRound()
{
    int i;
    if (mStateTimer != 0) {
        mStateTimer -= 1;
        if (*(s16 *)&mStateTimer <= 0)
            mStateTimer = 0;
        return;
    }

    if (Mg_IsNewRecord() != 0) {
        gameOver = 0;
        mState = 4;
        Mg_SetSharedState(MG_STATE_RESULTS);
    } else {
        mState = 4;
        Mg_SetSharedState(MG_STATE_RESULTS);
    }
    mPromptEnabled = 0;
    mShowCursor = 0;
    for (i = 0; i < 0xb; i++) {
        mStone[i].visible = 0;
    }
    mShowCounter = 0;
}


// @symbol _ZN15dScMgCurling2_c9NextThrowEv
/* Behavior state 2, reached once every stone rests. Age the score marks,
 * then either go back to Play after dealing the next stone or -- with all
 * five thrown -- set gameOver, sound the horn and advance to EndRound. */
#pragma push
#pragma opt_strength_reduction off
void dScMgCurling2_c::NextThrow()
{
    int i;
    AgeMarks();
    if (mStateTimer != 0) {
        mStateTimer -= 1;
        if (*(s16 *)&mStateTimer <= 0) mStateTimer = 0;
        return;
    }
    mState = 1;
    if (thrown >= 5) {
        mState = 3;
        mStateTimer = 0x40;
        gameOver = 1;
        Sound::PlayBank2_2D(0x1d8);
    }
    for (i = 0; i < 5; i++) {
        mMark[i].x = 0;
        mMark[i].y = 0;
        mMark[i].value = 0;
        mMark[i].countdown = 0;
        mMark[i].drawn = 0;
        mMark[i].enable = 0;
    }
    NextStone();
}
#pragma pop


// @symbol _ZN15dScMgCurling2_c4PlayEv
/* Behavior state 1: one frame of play. Enable the prompt on the first
 * pass, tick the sound throttle, run the drag-state handler, then step
 * every live stone through its own state handler (prev position latched
 * first, hit timer ticked). When all eleven are at rest, hand off to
 * NextThrow with the usual 0x40 delay. */
void dScMgCurling2_c::Play()
{
    char *c = (char *)this;
    if (mStateTimer != 0) {
        u16* q = &mStateTimer;
        *q = *q - 1;
        return;
    }
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    if (sndCooldown != 0) {
        u8* q = (u8*)(c + 0x55bd);
        *q = *q - 1;
    }
    (((C *)c)->*sCurling2DragTable[mDragState])();

    {
        int count = 0;
        int i = 0;
        char* p = c;
        for (; i < 0xb; i++, p += 0x30) {
            if (*(u8*)(p + 0x4689) != 0) {
                *(int*)(p + 0x466c) = *(int*)(p + 0x4660);
                *(int*)(p + 0x4670) = *(int*)(p + 0x4664);
                if (*(u16*)(p + 0x4680) != 0) {
                    u16* q = (u16*)(p + 0x4680);
                    *q = *q - 1;
                }
                (((C *)c)->*sCurling2StoneTable[*(u8 *)(p + 0x4688)])(i);
                if (*(u8*)(p + 0x4688) != 2) count++;
            }
        }
        if (count != 0) return;
    }
    mState = 2;
    mStateTimer = 0x40;
}


// @symbol _ZN15dScMgCurling2_c10BeginRoundEv
/* Behavior state 0: (re)start a round -- full game reset, arm the spawn
 * slot, deal the first stone, then play. */
void dScMgCurling2_c::BeginRound()
{
    ResetGame();
    spawning = 1;
    mSpawnTimer = 0;
    mShowCounter = 1;
    NextStone();
    mState = 1;
}


// @symbol _ZN15dScMgCurling2_c6ScrollEv
/* Pick the rink's scroll mode: 0xff in scrollIdx forces mode 0, otherwise
 * a random mode different from the current one. Apply the mode's BG2/BG0
 * offsets and set the BG0 flag in data_0209d45c unless the mode is 0. */
void dScMgCurling2_c::Scroll()
{
    int idx;

    if (scrollIdx == 0xff) {
        scrollIdx = 0;
    } else {
        idx = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 4 >> 15;
        if (scrollIdx == idx) {
            int t = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 3 >> 15;
            t += 1;
            idx += t;
            if (idx >= 4) idx -= 4;
        }
        scrollIdx = idx;
    }

    bg2x = sCurling2ScrollX[scrollIdx * 2];
    bg2y = sCurling2ScrollY[scrollIdx * 2];
    bg0x = sCurling2ScrollX[scrollIdx * 2 + 1];
    bg0y = sCurling2ScrollY[scrollIdx * 2 + 1];

    SetBg2Offset(0x80 - bg2x, 0x80 - bg2y);

    if (scrollIdx == 0) {
        data_0209d45c &= ~1;
        return;
    }
    SetBg0Offset(0x80 - bg0x, 0x80 - bg0y);
    data_0209d45c |= 1;
}


// @symbol _ZN15dScMgCurling2_c11ResetScrollEv
/* Mark the scroll mode unset so Scroll repicks it. */
void dScMgCurling2_c::ResetScroll()
{
    scrollIdx = 0xff;
}


// @symbol _ZN15dScMgCurling2_c9ResetGameEv
/* Clear the stones, the score marks and every game variable back to a
 * fresh round, then clear the collision values. */
void dScMgCurling2_c::ResetGame()
{
    char *c = (char *)this;
    int i;
    char *r = c;
    for (i = 0; i < 0xb; i++)
    {
        *((int *) (r + 0x4660)) = 0;
        *((int *) (r + 0x4664)) = 0;
        *((int *) (r + 0x4668)) = 0;
        *((int *) (r + 0x466c)) = 0;
        *((int *) (r + 0x4670)) = 0;
        *((int *) (r + 0x467c)) = 0;
        *((short *) (r + 0x4682)) = 0;
        *((short *) (r + 0x4684)) = 0;
        *((short *) (r + 0x4686)) = 0;
        *((unsigned char *) (r + 0x4688)) = 0;
        *((unsigned char *) (r + 0x4689)) = 0;
        *((unsigned char *) (r + 0x468a)) = 0;
        r += 0x30;
    }

    for (i = 0; i < 5; i++)
    {
        char *q = c + (i * ((unsigned long) 0x10));
        *((int *) (q + 0x4870)) = 0;
        *((int *) (q + 0x4874)) = 0;
        *((short *) (q + 0x4878)) = 0;
        *((short *) (q + 0x487a)) = 0;
        *((unsigned char *) (q + 0x487c)) = 0;
        *((unsigned char *) (q + 0x487d)) = 0;
    }

    *((short *) (c + 0x55b4)) = 0;
    thrown = 0;
    mDragX = 0;
    mDragY = 0;
    mDragOffsetX = 0;
    mDragOffsetY = 0;
    mThrowPower = 0;
    *((short *) (c + 0x55b2)) = 0;
    mDragState = 0;
    mShowCursor = 0;
    spawning = 0;
    gameOver = 0;
    *((short *) (c + 0x55b6)) = 0;
    *((short *) (c + 0x55b0)) = 0;
    sndCooldown = 0;
    Mg_SubmitScore(0);
    ClearValues();
}


// @symbol _ZN15dScMgCurling2_c10StartRoundEi
/* Slot 18. Yoshi ate the puck: restart the round and re-arm the screen
 * blend and the frame counter. */
void dScMgCurling2_c::StartRound(int /* arg */)
{
    mState = 0;
    ResetGame();
    Scroll();
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    mTopRecord = Mg_GetTopRecord();
}


// @symbol _ZN15dScMgCurling2_c6RenderEv
/* Slot 9. Per-frame draw: score marks, the throws counter, stones,
 * collision values, pieces and the drag cursor. */
s32 dScMgCurling2_c::Render()
{
    Mg_DrawTopLeftCounter((void *)Mg_GetTopRecord());
    DrawMarks();
    DrawCounter();
    DrawStones();
    DrawValues();
    DrawPieces();
    DrawCursor();
    return 1;
}


// @symbol _ZN15dScMgCurling2_c8BehaviorEv
/* Slot 6. One state-handler call through sCurling2BehaviorTable, then the
 * falling pieces and the collision values. */
s32 dScMgCurling2_c::Behavior()
{
    C *c = (C *)this;
    (c->*sCurling2BehaviorTable[mState])();
    StepPieces();
    AgeValues();
    return 1;
}


// @symbol _ZN15dScMgCurling2_c13InitResourcesEv
/* Slot 0. Load the rink and stone graphics for both engines, decompress
 * the board layouts, set up the blends, then reset into the first round
 * with a 0x40 state delay. */
s32 dScMgCurling2_c::InitResources()
{
    char *r6 = (char *)Ov004_LoadFileByPath(&sCurling2BgJokyuNcgPath);
    void *f;
    void *c7;
    void *c8;
    volatile u16 sp4;
    volatile u16 sp6;

    if (r6 == 0) return 0;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 2;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1220;
    DecompressLZ16(r6, (void *)func_02054d88());

    f = (void *)LoadFile(ASSET_D_2D_MG_BG_JOKYU_CURLING_NCL_BIN);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = (void *)LoadFile(ASSET_D_2D_MG_BG_JOKYU_CURLING1_NSC_BIN);
    func_02056314(f, 0, 0x800);
    Deallocate(f);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x5420;

    f = (void *)LoadFile(ASSET_D_2D_MG_BG_JOKYU_CURLING1_MATO_NSC_BIN);
    {
        char *b = (char *)_ZN2G212GetBG2ScrPtrEv();
        sp4 = 0x4300;
        MultiStore16(sp4, b, 0x1000);
    }
    func_020563d4(f, 0, 0x800);

    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 2;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x5620;
    {
        char *b = (char *)_ZN2G212GetBG0ScrPtrEv();
        sp6 = 0x4300;
        MultiStore16(sp6, b, 0x1000);
    }
    func_02056554(f, 0, 0x800);
    Deallocate(f);

    c7 = (void *)LoadFile(ASSET_D_2D_MG_CURLING_NCG_BIN);
    c8 = (void *)LoadFile(ASSET_D_2D_MG_CURLING_NCL_BIN);
    DecompressLZ16(c7, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(c8, 0, 0x100);

    data_0209d454 |= 4;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x814;
    DecompressLZ16(r6, (void *)_ZN3G2S13GetBG2CharPtrEv());

    f = (void *)LoadFile(ASSET_D_2D_MG_BG_JOKYU_CURLING_NCL_BIN);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = (void *)LoadFile(ASSET_D_2D_MG_BG_JOKYU_CURLING2_NSC_BIN);
    func_02056374(f, 0, 0x800);
    Deallocate(f);

    Ov004_Deallocate(r6);

    DecompressLZ16(c7, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(c8, 0, 0x100);
    Deallocate(c7);
    Deallocate(c8);

    ResetGame();
    ResetScroll();
    Scroll();
    spawning = 1;
    mSpawnTimer = 0;
    mShowCounter = 1;
    NextStone();
    SeedPieces();
    mState = 1;
    Mg_SetTopScreenYOffset(0x20);
    mStateTimer = 0x40;
    mRunDuringFade = 1;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    mTopRecord = Mg_GetTopRecord();
    return 1;
}


/* Reconstructed source-style name: SM64DS proves the dScMgCurling2_c RTTI
 * identity, MG_CURLING_J registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: func_ov006_020e6bf4. */
extern "C" void *dScMgCurling2_c_classInit()
{
    return new dScMgCurling2_c;
}


/* Static-init globals (was the handwritten __sinit_ov006_02130758 shard).
 * All seven PMF state tables plain-copied from the ROM PMF constants, in
 * retail copy order. */
PMF0 sCurling2BehaviorTable[5] = {
    pmf_dScMgCurling2_c_BeginRound, pmf_dScMgCurling2_c_Play, pmf_dScMgCurling2_c_NextThrow,
    pmf_dScMgCurling2_c_EndRound,   pmf_dScMgCurling2_c_Idle,
};
PMF1 sCurling2StoneTable[4] = {
    pmf_dScMgCurling2_c_StoneWait,
    pmf_dScMgCurling2_c_StoneSlide,
    pmf_dScMgCurling2_c_StoneRest,
    pmf_dScMgCurling2_c_StoneSteer,
};
PMF0 sCurling2DragTable[2] = {
    pmf_dScMgCurling2_c_DragBegin,
    pmf_dScMgCurling2_c_DragUpdate,
};
PMF sCurling2StepModeTable[3] = {
    pmf_dScMgCurling2_c_PickStepMode,
    pmf_dScMgCurling2_c_StepXOnly,
    pmf_dScMgCurling2_c_StepXAndY,
};
PMF sCurling2StepXFastTable[4] = {
    pmf_dScMgCurling2_c_StepXPickFast,
    pmf_dScMgCurling2_c_StepXPushPosFast,
    pmf_dScMgCurling2_c_StepXPushNegFast,
    pmf_dScMgCurling2_c_StepXSettleFast,
};
PMF sCurling2StepXTable[4] = {
    pmf_dScMgCurling2_c_StepXPick,
    pmf_dScMgCurling2_c_StepXPushPos,
    pmf_dScMgCurling2_c_StepXPushNeg,
    pmf_dScMgCurling2_c_StepXSettle,
};
PMF sCurling2StepYTable[3] = {
    pmf_dScMgCurling2_c_StepYRestart,
    pmf_dScMgCurling2_c_StepYRampUp,
    pmf_dScMgCurling2_c_StepYRampDown,
};
