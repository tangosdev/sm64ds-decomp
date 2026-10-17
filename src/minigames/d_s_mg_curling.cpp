//cpp
/* One-player curling scene: the whole ov006 unit 0x020e0638..0x020e3854,
 * 47 functions in ROM order, from the destructor through InitResources to
 * the registry factory dScMgCurling_c_classInit. The destructor is out of
 * line and comes first, which makes this file the key function's home, so
 * the vtable and RTTI are emitted here too.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; do not
 * reorder. The helpers are all dScMgCurling_c members: the state tables
 * call them through pointers to member, the rest with the scene as r0.
 *
 * comment leftovers: a few spellings must stay raw because the typed form
 * changes codegen (each was measured -- the DIFF size is in parentheses):
 * - BitB_StateDriftLeft / BitB_StateDriftRight keep the `bits + 0x47NN +
 *   off` spelling; mBit[index] reschedules them (0ff0 also sits under
 *   inline_depth(0)).
 * - Popup_UpdateAll / 020e1680 / 020e3078 / 020e3388 sit under
 *   opt_strength_reduction off: 1608's volatile countdown launder, 1680's
 *   M() popup-Y launder and stone walk, 3078's stone walk and base-field
 *   words, and 3388's clear loops all DIFF when typed (20-999 words).
 * - Stone_Slide keeps its pang/p668/p660/p664 pointer temps;
 *   &mStone[idx] form moves the address math (45 words).
 * - Stone_Drag reads the dragged stone through DragView; the
 *   mStone[i] spelling costs one word.
 * - Bit_InitAll's second init walk keeps `char *bit`; the
 *   indexed form costs four words.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgCurling_c_AssetId {
    ASSET_D_2D_MG_BG_CURLING1_LEFT_NSC_BIN = 0x002d,
    ASSET_D_2D_MG_BG_CURLING1_NCL_BIN = 0x002f,
    ASSET_D_2D_MG_BG_CURLING1_NSC_BIN = 0x0030,
    ASSET_D_2D_MG_BG_CURLING1_RIGHT_NSC_BIN = 0x0031,
    ASSET_D_2D_MG_BG_CURLING2_NCL_BIN = 0x0033,
    ASSET_D_2D_MG_BG_CURLING2_NSC_BIN = 0x0034,
    ASSET_D_2D_MG_CURLING_NCG_BIN = 0x00c7,
    ASSET_D_2D_MG_CURLING_NCL_BIN = 0x00c8
};

#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling_c.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"

/* `C` is the receiver of the pointer-to-member state tables. It must stay
 * incomplete: mwccarm picks the pointer-to-member layout from whether the
 * class is complete, and completing it changes all four dispatchers. */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
typedef void (C::*PMF0)();

/* The dragged stone, as Stone_Drag reads it: the stone row plus
 * the aim point at 0x4eb0 past the five rows. */
struct DragStone {
    int x;
    int y;
    int pad[3];
    int grabX;
    int grabY;
    int pad2[3];
    unsigned char dragging;
    unsigned char pad3[3];
};

struct DragView {
    unsigned char pad[0x4660];
    DragStone stone[48];
    unsigned char pad2[0x10];
    int aimX;
    int aimY;
};

/* B4 is a 4-byte-record view of a touch lane, used by the stone-drag
 * reader; the other readers index the lanes flat. */
struct B4 { unsigned char v; unsigned char pad[3]; };

/* Launder: forces an address through an integer so it is not shared. */
#define M(p) ((int *)(int)(p))

/* Everything this file calls or reads. Class members cannot go inside an
 * `extern "C"` block, so the C-linkage names are collected here in one. */
extern "C" {

extern int  Mg_GetSubmittedScore(void);
extern void Mg_RenderSubSpriteMtx(int a, int b, int c, int d);
extern int  Mg_RenderSpriteBoth(int a, int b, int c, int d, int e);
extern void Mg_DrawNumberRotated(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_02012718(int a, int b);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern int  RandomIntInternal(int *seed);
extern int  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int  func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern int  Mg_IsNewRecord(void);
extern void Mg_SetSharedState(int state);
extern void Mg_SubmitScore(int score);
extern int  Mg_GetTopRecord(void);
extern int  Mg_DrawTopLeftCounter(int self);
extern char *Ov004_LoadFileByPath(void *p);
extern void Mg_SetTopScreenYOffset(int a);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void DecompressLZ16(const void *src, void *dst);
extern void *LoadFile(int handle);
extern void Deallocate(void *ptr);
extern void Ov004_Deallocate(void *p);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void func_02056374(const void *src, u32 offset, u32 count);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);

extern int  data_0209d4b8;
extern s16  data_02082214[];
extern unsigned char sCurlingBitAStates[];
extern unsigned char sCurlingBitBStates[];
extern u8 sCurlingScoreDelays[];
extern int sCurlingBitKinds[];
extern int sCurlingStonesLeftX[];
extern int sCurlingBitSprites[];
extern int sCurlingStoneLeftSprite;
extern int sCurlingStoneLeftShadowSprite;
extern int sCurlingAimMarkerSprites[];
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern char sCurlingMgDirPath;
extern char sCurlingBgCurling2NcgPath;
extern int sCurlingSlideZoneEdges[];
extern int sCurlingSlideShiftLeft[];
extern int sCurlingSlideShiftRight[];
extern int sCurlingSlideMinLeft[];
extern int sCurlingSlideMinRight[];
extern int sCurlingHouseSpots[];

/* The six state tables, filled at startup by the ov006 static init. */
extern PMF0 sCurlingAimTable[];
extern PMF sCurlingBitKindTable[];
extern PMF sCurlingBitStateTable[];
extern Entry sCurlingBitBStateTable[];
extern PMF sCurlingStoneTable[];
extern PMF sCurlingBitAStateTable[];
extern PMF0 sCurlingSceneTable[];
/* The ROM PMF constants the seven state tables copy from (unlicensed
 * .data), grouped by destination table in retail copy order. */
extern PMF0 pmf_dScMgCurling_c_Scene_StartRound;
extern PMF0 pmf_dScMgCurling_c_Scene_Play;
extern PMF0 pmf_dScMgCurling_c_Scene_Score;
extern PMF0 pmf_dScMgCurling_c_Scene_End;
extern PMF0 pmf_dScMgCurling_c_Scene_Idle;
extern PMF pmf_dScMgCurling_c_Stone_WaitRelease;
extern PMF pmf_dScMgCurling_c_Stone_Slide;
extern PMF pmf_dScMgCurling_c_Stone_Collide;
extern PMF pmf_dScMgCurling_c_Stone_Drag;
extern PMF0 pmf_dScMgCurling_c_Aim_WaitTouch;
extern PMF0 pmf_dScMgCurling_c_Aim_Swing;
extern PMF pmf_dScMgCurling_c_Bit_PickKind;
extern PMF pmf_dScMgCurling_c_Bit_RunKindB;
extern PMF pmf_dScMgCurling_c_Bit_RunKindA;
extern PMF pmf_dScMgCurling_c_BitB_StatePause;
extern PMF pmf_dScMgCurling_c_BitB_StateDriftRight;
extern PMF pmf_dScMgCurling_c_BitB_StateDriftLeft;
extern PMF pmf_dScMgCurling_c_BitB_StateDriftStop;
extern PMF pmf_dScMgCurling_c_BitA_StatePause;
extern PMF pmf_dScMgCurling_c_BitA_StateDriftRight;
extern PMF pmf_dScMgCurling_c_BitA_StateDriftLeft;
extern PMF pmf_dScMgCurling_c_BitA_StateDriftStop;
extern PMF pmf_dScMgCurling_c_Bit_StateFallStart;
extern PMF pmf_dScMgCurling_c_Bit_StateFallSpeedUp;
extern PMF pmf_dScMgCurling_c_Bit_StateFallSlowDown;

}  /* extern "C" */

namespace cstd { int sqrt(u64 value); }
namespace Sound { u32 PlayBank2_2D(u32 id); }

// @symbol _ZN14dScMgCurling_cD1Ev
// @symbol _ZN14dScMgCurling_cD0Ev
dScMgCurling_c::~dScMgCurling_c()
{
}
// @symbol _ZN14dScMgCurling_c11Bit_DrawAllEv
/* Draws the falling bits. The functions after this one are the bits'
 * states: each field is 0x24 * index past its base offset. */
void dScMgCurling_c::Bit_DrawAll()
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mBit[i].shown) {
            int x = mBit[i].x >> 0xc;
            int y = mBit[i].y >> 0xc;
            Mg_RenderSubSpriteMtx(sCurlingBitSprites[mBit[i].texA], x, y, 0);
            DrawOamSprite(sCurlingBitSprites[mBit[i].texB], x, y, 0);
        }
    }
}

// @symbol _ZN14dScMgCurling_c21Bit_StateFallSlowDownEi
void dScMgCurling_c::Bit_StateFallSlowDown(int index) {
    if (mBit[index].wait2 != 0) {
        short *p = (short *)&mBit[index].wait2;
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (mBit[index].vy > 0x100) {
        int *q = &mBit[index].vy;
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        mBit[index].state2 = 0;
    }
}

// @symbol _ZN14dScMgCurling_c20Bit_StateFallSpeedUpEi
void dScMgCurling_c::Bit_StateFallSpeedUp(int index) {
    if (mBit[index].vyCap > mBit[index].vy) {
        mBit[index].vy += 0x10;
        if (mBit[index].vyCap > mBit[index].vy)
            mBit[index].vy = mBit[index].vyCap;
    }
    if (mBit[index].wait2 != 0) {
        mBit[index].wait2 = mBit[index].wait2 - 1;
        if ((s16&)mBit[index].wait2 < 0) (s16&)mBit[index].wait2 = 0;
    } else {
        mBit[index].state2 = 2;
        (s16&)mBit[index].wait2 = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}

// @symbol _ZN14dScMgCurling_c18Bit_StateFallStartEi
void dScMgCurling_c::Bit_StateFallStart(int index) {
    unsigned int roll;
    mBit[index].vy = 0;
    roll = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mBit[index].vyCap = (((roll << 4) >> 15) << 4) + 0x300;
    mBit[index].state2 = 1;
    roll = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    roll = ((roll << 5) >> 15) + 0x20;
    (s16 &)mBit[index].wait2 = (unsigned char)roll;
}

// @symbol _ZN14dScMgCurling_c19BitA_StateDriftStopEi
void dScMgCurling_c::BitA_StateDriftStop(int index) {
    mBit[index].x = mBit[index].x + mBit[index].vx;
    mBit[index].y = mBit[index].y + mBit[index].vy;
    if (mBit[index].wait != 0) {
        mBit[index].timer = mBit[index].timer - 1;
        if ((s16&)mBit[index].timer < 0) (s16&)mBit[index].timer = 0;
        return;
    }
    if (mBit[index].vx > 0) {
        mBit[index].vx = mBit[index].vx - 8;
        if ((s16)mBit[index].vx < 0) mBit[index].vx = 0;
        return;
    }
    if (mBit[index].vx < 0) {
        mBit[index].vx = mBit[index].vx + 8;
        if (mBit[index].vx > 0) mBit[index].vx = 0;
        return;
    }
    mBit[index].state = 0;
}

// @symbol _ZN14dScMgCurling_c19BitA_StateDriftLeftEi
void dScMgCurling_c::BitA_StateDriftLeft(int index) {
    unsigned short timer;

    mBit[index].x = mBit[index].x + mBit[index].vx;
    mBit[index].y = mBit[index].y + mBit[index].vy;

    if (mBit[index].wait != 0) {
        mBit[index].wait = mBit[index].wait - 1;
        if ((s16&)mBit[index].wait < 0)
            mBit[index].wait = 0;
        return;
    }

    if (mBit[index].vx > -0x300) {
        mBit[index].vx -= 8;
        if (mBit[index].vx <= -0x300)
            mBit[index].vx = 0x300;
    }

    timer = mBit[index].timer;
    if (timer != 0) {
        mBit[index].timer = timer - 1;
        if ((s16&)mBit[index].timer < 0)
            mBit[index].timer = 0;
        return;
    }

    mBit[index].state = 3;
    mBit[index].timer = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}

// @symbol _ZN14dScMgCurling_c20BitA_StateDriftRightEi
void dScMgCurling_c::BitA_StateDriftRight(int index) {
    unsigned short timer;

    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;

    timer = mBit[index].wait;
    if (timer != 0) {
        (s16&)mBit[index].wait = timer - 1;
        if ((s16&)mBit[index].wait < 0)
            (s16&)mBit[index].wait = 0;
        return;
    }

    if (mBit[index].vx < 0x300) {
        mBit[index].vx += 8;
        if (mBit[index].vx >= 0x300)
            mBit[index].vx = 0x300;
    }

    timer = mBit[index].timer;
    if (timer != 0) {
        (s16&)mBit[index].timer = timer - 1;
        if ((s16&)mBit[index].timer < 0)
            (s16&)mBit[index].timer = 0;
        return;
    }

    mBit[index].state = 3;
    (s16&)mBit[index].timer = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}

// @symbol _ZN14dScMgCurling_c15BitA_StatePauseEi
void dScMgCurling_c::BitA_StatePause(int index) {
    if (mBit[index].wait != 0) {
        mBit[index].wait = mBit[index].wait - 1;
        if ((s16&)mBit[index].wait < 0) (s16&)mBit[index].wait = 0;
        return;
    }
    mBit[index].vx = 0;
    mBit[index].state =
        sCurlingBitAStates[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                               << 1 >>
                           15];
    mBit[index].wait = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    mBit[index].timer = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}

// @symbol _ZN14dScMgCurling_c12Bit_RunKindAEi
void dScMgCurling_c::Bit_RunKindA(int index)
{
    C *self = (C *)this;
    unsigned char state = mBit[index].state;
    (self->*sCurlingBitAStateTable[state])(index);
    unsigned char state2 = mBit[index].state2;
    (self->*sCurlingBitStateTable[state2])(index);
}

// @symbol _ZN14dScMgCurling_c19BitB_StateDriftStopEi
void dScMgCurling_c::BitB_StateDriftStop(int index) {
    int *x = &mBit[index].x;
    int *vx = &mBit[index].vx;
    int *y = &mBit[index].y;
    *x += *vx;
    *y += mBit[index].vy;
    if (*vx > 0) {
        *vx -= 0x20;
        if ((int)(short)*vx < 0) *vx = 0;
    } else if (*vx < 0) {
        *vx += 0x20;
        if (*vx > 0) *vx = 0;
    } else {
        mBit[index].state = 0;
    }
}

// @symbol _ZN14dScMgCurling_c19BitB_StateDriftLeftEi
void dScMgCurling_c::BitB_StateDriftLeft(int index) {
    int off = index * 0x24;

    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;

    u16 *wait = &mBit[index].wait;
    if (*wait != 0) {
        *wait = *wait - 1;
        s16 left = *(s16 *)wait;
        if (left < 0) {
            *wait = 0;
        }
        return;
    }

    int *vx = &mBit[index].vx;
    if (*vx > -0x400) {
        *vx -= 0x20;
        if (*vx <= -0x400) {
            *vx = 0x400;
        }
    }

    u16 *timer = &mBit[index].timer;
    if (*timer != 0) {
        *timer = *timer - 1;
        s16 left = *(s16 *)timer;
        if (left < 0) {
            *timer = 0;
        }
        return;
    }

    mBit[index].state = 3;
}

#pragma push
#pragma inline_depth(0)
// @symbol _ZN14dScMgCurling_c20BitB_StateDriftRightEi
void dScMgCurling_c::BitB_StateDriftRight(int index) {
    int off = index * 0x24;

    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;

    unsigned short* wait = &mBit[index].wait;
    if (*wait != 0) {
        *wait = *wait - 1;
        short left = *(short*)wait;
        if (left < 0) {
            *wait = 0;
        }
        return;
    }

    int* vx = &mBit[index].vx;
    if (*vx < 0x400) {
        *vx = *vx + 0x20;
        if (*vx >= 0x400) {
            *vx = 0x400;
        }
    }

    unsigned short* timer = &mBit[index].timer;
    if (*timer != 0) {
        *timer = *timer - 1;
        short left = *(short*)timer;
        if (left < 0) {
            *timer = 0;
        }
        return;
    }

    mBit[index].state = 3;
#pragma pop
}

// @symbol _ZN14dScMgCurling_c15BitB_StatePauseEi
void dScMgCurling_c::BitB_StatePause(int idx) {
    unsigned short wait;
    short left;
    unsigned int roll;
    wait = mBit[idx].wait;
    if (wait != 0) {
        left = (short)(wait - 1);
        (s16&)mBit[idx].wait = left;
        if ((s16&)mBit[idx].wait < 0) (s16&)mBit[idx].wait = 0;
        return;
    }
    mBit[idx].vx = 0;
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mBit[idx].vy = (int)(((roll << 5) >> 15) << 4) + 0x600;
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mBit[idx].state = sCurlingBitBStates[(roll << 1) >> 15];
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    (s16&)mBit[idx].wait = (unsigned char)((roll << 4) >> 15);
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    (s16&)mBit[idx].timer = (unsigned char)(((roll * 0x30) >> 15) + 0x30);
}

// @symbol _ZN14dScMgCurling_c12Bit_RunKindBEi
void dScMgCurling_c::Bit_RunKindB(int idx)
{
    unsigned char state = mBit[idx].state;
    (((C *)this)->*sCurlingBitBStateTable[state].pmf)(idx);
}

#pragma push
#pragma opt_propagation off
// @symbol _ZN14dScMgCurling_c12Bit_PickKindEi
void dScMgCurling_c::Bit_PickKind(int idx)
{
    unsigned roll = (unsigned)RandomIntInternal(&data_0209d4b8);
    int k = 0;
    unsigned pick = ((roll >> 16) & 0x7fff) << 3 >> 0xf;
    if (pick == 5) k = 1;
    mBit[idx].kind = sCurlingBitKinds[k];
    mBit[idx].state = 0;
}
#pragma pop

// @symbol _ZN14dScMgCurling_c13Bit_UpdateAllEv
void dScMgCurling_c::Bit_UpdateAll()
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mBit[i].on != 0) {
            unsigned char kind = mBit[i].kind;
            (((C *)this)->*sCurlingBitKindTable[kind])(i);
            if ((mBit[i].y >> 0xc) >= 0xc8) {
                mBit[i].x = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                mBit[i].y = -0x8000;
                mBit[i].state = 0;
                mBit[i].kind = 0;
                mBit[i].state2 = 0;
            }
        }
    }
}

// @symbol _ZN14dScMgCurling_c11Bit_InitAllEv
void dScMgCurling_c::Bit_InitAll()
{
    int i;
    char *bit;
    unsigned int roll;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    for (; i < 0x32; i++)
    {
        mBit[i].x = 0;
        mBit[i].y = 0;
        mBit[i].vx = 0;
        mBit[i].vy = 0;
        mBit[i].wait = 0;
        mBit[i].timer = 0;
        mBit[i].wait2 = 0;
        mBit[i].on = 0;
        mBit[i].kind = 0;
        mBit[i].state = 0;
        mBit[i].state2 = 0;
        mBit[i].shown = 0;
        mBit[i].texA = 0;
        mBit[i].texB = 1;
    }

    i = 0;
    bit = (char *)this;
    for (; i < 0x32; i++)
    {
        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((roll >> 16) & 0x7fff) << 5;
        *(int *)(bit + 0x478c) = (int)((m >> 0xf)) << 0xf;
        *(int *)(bit + 0x4790) = -0x8000;
        *(char *)(bit + 0x47a8) = 1;
        *(char *)(bit + 0x47ac) = 1;
        *(char *)(bit + 0x47a9) = 0;
        *(char *)(bit + 0x47aa) = 0;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(bit + 0x47ad) = (char)(((roll >> 16) & 0x7fff) * 5 >> 0xf);

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(bit + 0x47ad) + ((((roll >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(bit + 0x47ae) = (char)v;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(bit + 0x47a0) = (short)(((i & 7) << 6) + (((roll >> 16) & 0x7fff) * 0x30 >> 0xf));

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((roll >> 16) & 0x7fff) << 5;
        *(int *)(bit + 0x478c) = (int)((m >> 0xf)) << 0xf;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((roll >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(bit + 0x4790) = (((q << 3) - 8)) << 0xc;
        *(short *)(bit + 0x47a0) = 0;
        bit += 0x24;
    }
}

// @symbol _ZN14dScMgCurling_c13Popup_DrawAllEv
void dScMgCurling_c::Popup_DrawAll()
{
    int i;
    for (i = 0; i < 5; i++) {
        if (mPopup[i].shown != 0) {
            Mg_DrawBigNumber(mPopup[i].x >> 12, mPopup[i].y >> 12, mPopup[i].points, -1, -1, 0, 0);
        }
    }
    if (mRoundOver != 0) {
        int r = Mg_GetSubmittedScore();
        Mg_DrawNumberRotated(0x80, 0x60, r, 1, 0, 0x800, 0);
    }
}

#pragma push
// @symbol _ZN14dScMgCurling_c15Popup_UpdateAllEv
#pragma opt_strength_reduction off
void dScMgCurling_c::Popup_UpdateAll()
{
    char *raw = (char *)this;
    int i;
    for (i = 0; i < 5; i++) {
        char *popup = raw + (i << 4);
        if (mPopup[i].live == 0)
            continue;
        if (mPopup[i].countdown == 0)
            continue;
        {
            volatile unsigned short *timer = (volatile unsigned short*)((unsigned int)popup + 0x4746);
            *timer = *timer - 1;
            if (*timer != 0) continue;
        }
        mPopup[i].shown = 1;
        Sound::PlayBank2_2D(0x1bc);
    }
}
#pragma pop

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN14dScMgCurling_c11ScoreStonesEv
void dScMgCurling_c::ScoreStones()
{
    char *raw = (char *)this;
    int slot = 0;
    int i, j;
    char *stone;
    u8 *delay;

    for (j = 0; j < 5; j++) {
        if (mPopup[j].live == 0) {
            slot = j;
            break;
        }
    }

    delay = &sCurlingScoreDelays[slot];
    stone = raw;
    for (i = 0; i < 5; i++, stone += 0x2c) {
        int dx, dz, dist, top;

        if (*(u8 *)(stone + 0x4689) == 0)
            continue;
        dx = *(int *)(stone + 0x4660) - mHouseX;
        dz = *(int *)(stone + 0x4664) - mHouseY;
        {
            int ax = dx >> 12;
            int az = dz >> 12;
            dist = cstd::sqrt((u64)(s64)(ax * ax + az * az));
        }
        mPopup[slot].live = 1;
        mPopup[slot].x = *(int *)(stone + 0x4660);
        {
        int *popupY = M(raw + slot * 16 + 0x4740);
        *popupY = *(int *)(stone + 0x4664) + 0x1000;
        mPopup[slot].countdown = *delay;
        if (dist <= 8)
            mPopup[slot].points = 1000;
        else if (dist <= 0x18)
            mPopup[slot].points = 500;
        else if (dist <= 0x28)
            mPopup[slot].points = 300;
        else if (dist <= 0x38)
            mPopup[slot].points = 100;
        else
            mPopup[slot].points = 0;
        top = *popupY >> 12;
        if (top >= -32 && top <= 8)
            *popupY = -0x28000;
        }
        delay++;
        slot++;
    }
}
#pragma pop

// @symbol _ZN14dScMgCurling_c13DrawAimMarkerEv
/* Draws a sprite at the aimed stone's position (0x4eb0) once the stylus
 * handler has set 0x4ee5. */
void dScMgCurling_c::DrawAimMarker()
{
  if(mAimShown==0) return;
  int x=mAimX;
  int y=mAimY;
  Mg_RenderSpriteBoth((int)sCurlingAimMarkerSprites, (x >> 12) - 0x20, (y >> 12) - 8, -1, 0);
}

#pragma push
#pragma opt_common_subs off
// @symbol _ZN14dScMgCurling_c9Aim_SwingEv
/* Stylus handler for the stone being aimed. While the stylus is down, the
 * touch point plus the grab offset becomes the new position, clamped to the
 * 0x20000..0xe0000 by 0x94000..0xb8000 box; a move under two units is undone.
 * Otherwise it updates the swing tracking (0x4eea), turns the move into an
 * angle at 0x4ede clamped to the lower half turn and averaged with the last
 * one, and folds the move length into the speed at 0x4ec8. With the stylus
 * up, two flags are reset.
 *
 * Shapes that must stay: the touch records are read through a `u8 *` cast;
 * dx is computed before dy with no temporaries; the tail's index `j` is
 * wider than a byte; and the pragma keeps the 0x4eb0 and 0x4eb4 re-reads
 * after the clamps. */
void dScMgCurling_c::Aim_Swing()
{
    u8 idx;
    int dx, oldx, oldy;
    int diff;
    u16 ang;

    idx = gActivePlayerSlot;
    if (gTouchHeld[idx * 4] != 0) {
        int dy2, dy;

        int i4 = idx * 4;
        u8 *pa = (u8 *)gTouchX;
        u8 *pb = (u8 *)gTouchY;
        u8 bx = pa[i4];
        u8 by = pb[i4];

        oldx = mAimX;
        oldy = mAimY;
        mAimX = (bx << 12) + mGrabX;
        mAimY = mGrabY + (by << 12);

        if (mAimY <= 0x94000)
            mAimY = 0x94000;
        if (mAimX <= 0x20000)
            mAimX = 0x20000;
        if (mAimX >= 0xe0000)
            mAimX = 0xe0000;
        if (mAimY >= 0xb8000)
            mAimY = 0xb8000;


        dx = (mAimX - oldx) >> 12;
        dy = (mAimY - oldy) >> 12;
        dy2 = dy * dy;

        if (cstd::sqrt((s64)(dx * dx + dy2)) <= 1) {
            mAimX = oldx;
            mAimY = oldy;
            return;
        }

        diff = (mAimY - mAimHomeY) >> 12;
        if (mSwingState == 0) {
            func_02012718(0x1d6, mAimX);
            mSwingState = 2;
            mSwingDelta = (mAimY - mAimHomeY) >> 12;
            mAimHomeY = mAimY;
        } else if (mSwingState == 1) {
            if (mSwingDelta * diff > 0) {
                if (diff < 0)
                    diff = -diff;
                if (diff >= 0xa)
                    mSwingState = 0;
            } else {
                mSwingDelta = diff;
                mAimHomeY = mAimY;
            }
        } else {
            if (mSwingDelta * diff < 0)
                mSwingState = 1;
            mSwingDelta = (mAimY - mAimHomeY) >> 12;
            mAimHomeY = mAimY;
        }

        ang = mSwingAng;
        mSwingAng = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = mSwingAng;
            if (a <= 0x8000) {
                if (a >= 0x4000) {
                    mSwingAng = 0x8000;
                    goto ang_done;
                }
            }
            if (a <= 0x4000)
                mSwingAng = 0;
        }
    ang_done:;

        {
            int mag;
            mSwingAng = (u16)((mSwingAng + ang) >> 1);
            mag = cstd::sqrt((s64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
            mag = (mag << 12) >> 4;
            if (mag >= 0xc000)
                mag = 0xc000;
            if (mag > mSwingSpeed)
                mSwingSpeed = mag;
            {
                int cur = mSwingSpeed;
                if (cur > mag) {
                    mSwingSpeed = mSwingSpeed - ((cur - mag) >> 1);
                }
            }
        }


        {
            int px = mAimX;
            int py = mAimY;
            int j = gActivePlayerSlot;
            u8 jx = ((u8 *)gTouchX)[j * 4];
            int ax = (px >> 12) - jx;
            u8 jy = ((u8 *)gTouchY)[j * 4];
            int ay = (py >> 12) - jy;
            mGrabX = ax << 12;
            mGrabY = ay << 12;
        }

        return;
    }

    mStylusHeld = 0;
    mAimShown = 1;
}
#pragma pop

// @symbol _ZN14dScMgCurling_c13Aim_WaitTouchEv
void dScMgCurling_c::Aim_WaitTouch()
{
  int idx;
  int touching = 0;
  int x;
  int y;
  idx = gActivePlayerSlot;
  if (gTouchHeld[idx * (4 & 0xFFFFFFFF)] != 0)
  {
    if (gTouchEdge[idx * 4] != 0)
    {
      touching = 1;
    }
  }
  if (touching == 0)
  {
    return;
  }
  x = (mAimX >> 0xc) - ((u8 *)gTouchX)[idx * 4];
  y = (mAimY >> 0xc) - ((u8 *)gTouchY)[idx * 4];
  mGrabX = x << 0xc;
  mGrabY = y << 0xc;
  mStylusHeld = 1;
  mSwingAng = 0xc000;
  if (mSwingTimer == 0)
  {
    func_02012718(0x1d2, mAimX);
    mSwingTimer = 6;
  }
  unk_4ecc = 0;
  unk_4ed0 = 0;
  mAimHomeX = mAimX + mGrabX;
  mAimHomeY = mAimY + mGrabY;
  mSwingDelta = 0xff;
  mSwingState = 0;
}

// @symbol _ZN14dScMgCurling_c14DrawStonesLeftEv
void dScMgCurling_c::DrawStonesLeft()
{
    int x, y;
    int i;
    int remaining;
    int j;
    for (i = 0; i < 5; i++) {
        if (mStone[i].active == 0) continue;
        if (mStone[i].dealt == 0) continue;
        x = mStone[i].x >> 12;
        y = mStone[i].y >> 12;
        RenderOamBothScreens(&sCurlingStoneLeftSprite, x, y, -1, 1, 0);
        RenderOamBothScreens(&sCurlingStoneLeftShadowSprite, x, y + 8, -1, 2, 0);
    }
    remaining = 5 - mThrown;
    if (remaining < 0) remaining = 0;
    j = 0;
    if (remaining > 0) {
        for (; j < remaining; j++) {
            int slotX = sCurlingStonesLeftX[j];
            RenderOamBothScreens(&sCurlingStoneLeftSprite, slotX, 0xb0, -1, 1, 0);
            RenderOamBothScreens(&sCurlingStoneLeftShadowSprite, slotX, 0xb8, -1, 2, 0);
        }
    }
}

// @symbol _ZN14dScMgCurling_c14SeparateStonesEi
/* Stone separation. Stone idx has just moved: the first other active stone
 * within 24 units is pushed out to 26 units along the line between them,
 * then that stone gets the same check once against the rest (with a bump
 * sound). Only one push each way per call.
 *
 * The rotated offsets must be named (vx, vy), and the inner scan needs its
 * own dx, dy, dist and ang; sharing the outer ones changes the registers. */
void dScMgCurling_c::SeparateStones(int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 5; i++) {
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
            for (j = 0; j < 5; j++) {
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
                    return;
                }
            }
            return;
        }
    }
}

// @symbol _ZN14dScMgCurling_c21ResolveStoneCollisionEi
/* Stone idx has just moved, so find the first other stone it overlaps and
 * resolve the collision. The two exchange their velocity components along
 * the line between their centres and keep the components across it; the
 * moving stone is then pushed back to one stone width (0x1b000) along that
 * line and clamped to the board, with any overshoot passed on to the stone
 * it hit. Both stones end up moving, the hit stone is flagged fast above
 * 0x3800, and the knock sound plays panned to the x.
 *
 * Shapes that must stay: the contact angle is negated right after atan2,
 * before the velocities are read; the three fields written after a call
 * are reached through pointers taken beside each stone's reads, which puts
 * their addresses in the frame ahead of the temporaries; and the moving
 * stone's new x velocity reuses the outer dx. */
void dScMgCurling_c::ResolveStoneCollision(int idx)
{
    int i;
    int dx;
    int dy;

    for (i = 0; i < 5; i++) {
        if (mStone[i].active == 0) continue;
        if (idx == i) continue;
        if (mStone[i].state == 0) continue;
        if (mStone[i].state == 3) continue;
        dx = (mStone[i].x - mStone[idx].x) >> 12;
        dy = (mStone[i].y - mStone[idx].y) >> 12;
        if (cstd::sqrt((long long)(dx * dx + dy * dy)) > 0x18) continue;
        {
            u16 *pAngle;
            s32 *pSpeed;
            u16 *pHitAngle;
            int mTangent;
            int k;
            int cosA;
            int vmy;
            int sinA;
            u16 contact;
            int hTangent;
            int cosN;
            int hvx;
            u16 rel;
            int xi;
            int sinN;
            int mNormal;
            int yi;
            int vmx;
            int hNormal;
            int vhx;
            int mvy;
            int hvy;
            int vhy;

            /* Contact line, from the hit stone to the moving one. */
            dx = mStone[idx].x - mStone[i].x;
            dy = mStone[idx].y - mStone[i].y;
            contact = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            rel = -contact;

            /* Both velocities as x/y. A hit stone at rest takes half the
             * moving stone's y so the exchange below cannot stall. */
            pAngle = &mStone[idx].angle;
            pSpeed = &mStone[idx].speed;
            k = (mStone[idx].angle >> 4) * 2;
            vmx = FX_MUL(data_02082214[k + 1], mStone[idx].speed);
            vmy = FX_MUL(data_02082214[k], mStone[idx].speed);
            pHitAngle = &mStone[i].angle;
            k = (mStone[i].angle >> 4) * 2;
            vhx = FX_MUL(data_02082214[k + 1], mStone[i].speed);
            vhy = FX_MUL(data_02082214[k], mStone[i].speed);
            if (vhy == 0) vhy = vmy >> 1;

            /* sin/cos of -contact rotate into the contact frame; sin/cos of
             * +contact rotate back out. */
            k = (rel >> 4) * 2;
            sinN = data_02082214[k];
            cosN = data_02082214[k + 1];
            rel = -rel;
            k = (rel >> 4) * 2;
            sinA = data_02082214[k];
            cosA = data_02082214[k + 1];

            /* Normal and tangential components of each velocity. */
            mNormal = FX_MUL(cosN, vmx);
            mNormal -= FX_MUL(sinN, vmy);
            mTangent = FX_MUL(sinN, vmx) + FX_MUL(cosN, vmy);
            hNormal = FX_MUL(cosN, vhx) - FX_MUL(sinN, vhy);
            hTangent = FX_MUL(sinN, vhx) + FX_MUL(cosN, vhy);

            /* Swap the normal components and rotate back: dx/mvy for the
             * moving stone, hvx/hvy for the one it hit. */
            dx = FX_MUL(cosA, hNormal) - FX_MUL(sinA, mTangent);
            mvy = FX_MUL(sinA, hNormal) + FX_MUL(cosA, mTangent);
            hvx = FX_MUL(cosA, mNormal) - FX_MUL(sinA, hTangent);
            hvy = FX_MUL(sinA, mNormal) + FX_MUL(cosA, hTangent);

            *pAngle = _ZN4cstd5atan2E5Fix12IiES1_(mvy, dx);
            *pSpeed = cstd::sqrt((u64)((long long)dx * dx + (long long)mvy * mvy));

            /* Separate the stones along the contact line, then keep the
             * moving one on the board and hand any overshoot to the other. */
            mStone[idx].x = mStone[i].x + FX_MUL(cosA, 0x1b000);
            mStone[idx].y = mStone[i].y + FX_MUL(sinA, 0x1b000);
            xi = mStone[idx].x >> 12;
            yi = mStone[idx].y >> 12;
            if (xi - 0xc < 0) {
                xi = mStone[idx].x - 0xc000;
                mStone[i].x += xi;
                mStone[idx].x = 0xc000;
            }
            if (xi + 0xc > 0x100) {
                mStone[i].x += mStone[idx].x - 0xf4000;
                mStone[idx].x = 0xf4000;
            }
            if (yi - 0xc < -0xe0) {
                mStone[i].y += mStone[idx].y + 0xd4000;
                mStone[idx].y = -0xd4000;
            }

            *pHitAngle = _ZN4cstd5atan2E5Fix12IiES1_(hvy, hvx);
            mStone[i].speed = cstd::sqrt((u64)((long long)hvx * hvx + (long long)hvy * hvy));
            mStone[idx].state = 1;
            mStone[i].state = 1;
            if (mStone[i].speed >= 0x3800) {
                mStone[i].fast = 1;
            } else {
                mStone[i].fast = 0;
            }
            func_02012718(0xe8, mStone[idx].x);
            return;
        }
    }
}

// @symbol _ZN14dScMgCurling_c12SetStoneSpinEi
/* Sets stone i's spin (0x4682) from the x component of its velocity:
 * -(cos(angle) * speed) / 4. */
void dScMgCurling_c::SetStoneSpin(int i)
{
    int h = mStone[i].angle;
    int idx = (((h >> 4) << 1) + 1) << 1;
    int s = *(short *)((char *)data_02082214 + idx);
    int v = mStone[i].speed;
    long long m = (long long)s * v;
    int hi = (int)(((unsigned long long)(m + 0x800)) >> 12);
    mStone[i].spin = (short)((-hi) >> 2);
}

// @symbol _ZN14dScMgCurling_c10Stone_DragEi
/* Stone i while it is being dragged. With the stylus down the stone
 * follows the touch point plus the grab offset (0x4674/0x4678), its x kept
 * between 0xe and 0xf2 units, and the offset is recomputed; with the stylus
 * up the drag flag clears and, if the aimed stone (0x4eb0) overlaps it, the
 * aimed stone is moved just below it. */
void dScMgCurling_c::Stone_Drag(int i)
{
    DragView *w = (DragView *)this;
    unsigned char idx = gActivePlayerSlot;
    if (((struct B4 *)gTouchHeld)[idx].v) {
        int t, mm, nn;
        w->stone[i].x = w->stone[i].grabX + (((struct B4 *)gTouchX)[idx].v << 12);
        t = w->stone[i].x >> 12;
        if (t < 0xe) w->stone[i].x = 0xe000;
        if (t > 0xf2) w->stone[i].x = 0xf2000;
        mm = (w->stone[i].x >> 12) - ((struct B4 *)gTouchX)[gActivePlayerSlot].v;
        nn = (w->stone[i].y >> 12) - ((struct B4 *)gTouchY)[idx].v;
        w->stone[i].grabX = mm << 12;
        w->stone[i].grabY = nn << 12;
    } else {
        int dx, dy;
        w->stone[i].dragging = 0;
        dx = (w->aimX - w->stone[i].x) >> 12;
        dy = (w->aimY - w->stone[i].y) >> 12;
        if (dx < -0x2e) return;
        if (dx > 0x2e) return;
        if (dy < -0x14) return;
        if (dy <= 0x14) w->aimY = w->stone[i].y + 0x15000;
    }
}

// @symbol _ZN14dScMgCurling_c13Stone_CollideEi
/* A tail-call veneer to ResolveStoneCollision. */
void dScMgCurling_c::Stone_Collide(int idx)
{
    ResolveStoneCollision(idx);
}

// @symbol _ZN14dScMgCurling_c11Stone_SlideEi
/* Stone idx while it slides. Moves it by its velocity, adds the spin to
 * the angle, bounces it off the four walls (with a sound), then takes
 * friction off the speed: a base of speed/512 (at least 0x1c), more in the
 * five bands below the house from the sCurlingSlideZoneEdges tables, and a
 * steeper rate while the stone still travels upward above -0x20. At zero
 * speed the stone stops (state 2). The collision and spin helpers run, and
 * the slide sound at 0x467c is retuned to the speed. */
void dScMgCurling_c::Stone_Slide(int idx)
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
    int sbv;
    int i2;
    int s;
    int p;
    int w;
    int *pd;
    int sn;
    int cs;

    m = idx * 0x2c;

    pang = (u16 *)(c + 0x4686 + m);
    p668 = (int *)(c + 0x4668 + m);
    p660 = (int *)(c + 0x4660 + m);
    p664 = (int *)(c + 0x4664 + m);

    sn = data_02082214[((*pang) >> 4) * 2 + 1];
    *p660 += FX_MUL(sn, *p668);
    cs = data_02082214[((*pang) >> 4) * 2];
    *p664 += FX_MUL(cs, *p668);
    mStone[idx].heading += (u16)mStone[idx].spin;

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
        mStone[idx].angle = -mStone[idx].angle;
        *p664 = 0xb4000;
        func_02012718(0x1d4, *p660);
    } else if (zi - 0xc < -0xe0) {
        mStone[idx].angle = -mStone[idx].angle;
        *p664 = -0xd4000;
        func_02012718(0x1d4, *p660);
    }

    zi = *p668;
    v = zi >> 9;
    if (v <= 0x1c)
        v = 0x1c;
    for (i2 = 0, sbv = *p664 >> 12; i2 < 5; i2++) {
        if (sbv <= -(sCurlingSlideZoneEdges[i2] + 0x20)) {
            s = data_02082214[(*pang >> 4) * 2];
            if (s < 0) {
                v = zi >> sCurlingSlideShiftLeft[i2];
                if (v < sCurlingSlideMinLeft[i2])
                    v = sCurlingSlideMinLeft[i2];
            } else if (s > 0) {
                v = zi >> sCurlingSlideShiftRight[i2];
                if (v < sCurlingSlideMinRight[i2])
                    v = sCurlingSlideMinRight[i2];
            }
            break;
        }
    }

    if (sbv > -0x20) {
        s = data_02082214[(*pang >> 4) * 2];
        if (s > 0) {
            v = zi >> 3;
            if (v <= 0x180)
                v = 0x180;
        }
    }

    mStone[idx].speed -= v;
    pd = &mStone[idx].speed;
    if (mStone[idx].speed <= 0) {
        *p668 = 0;
        mStone[idx].state = 2;
    }

    ResolveStoneCollision(idx);
    SetStoneSpin(idx);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    mStone[idx].slideSnd = func_02012468(mStone[idx].slideSnd, 2, 0xe7, 7, p, w, func_020126e8(*p660), 0);
}

// @symbol _ZN14dScMgCurling_c17Stone_WaitReleaseEi
/* Releases stone idx if the aimed stone (0x4eb0) overlaps it: it takes the
 * aim angle (0x4ede, clamped to the lower half turn) and the swing speed
 * (0x4ec8), is flagged fast above 0x3800, and starts sliding. The aim is
 * then put away, the stones are separated, and the throw sound plays at a
 * volume that depends on the fast flag. */
void dScMgCurling_c::Stone_WaitRelease(int idx)
{
    int v, w, vol;

    if (mStylusHeld != 1) return;

    v = (mAimX - mStone[idx].x) >> 12;
    w = (mAimY - mStone[idx].y) >> 12;
    if (v < -0x2e) return;
    if (v > 0x2e) return;
    if (w < -0x14) return;
    if (w > 0x14) return;

    mStone[idx].state = 1;
    mStone[idx].angle = mSwingAng;
    mStone[idx].speed = mSwingSpeed;

    if (mSwingAng < 0x9800u || mSwingAng > 0xe800u) {
        if (mSwingAng >= 0x4000u && mSwingAng <= 0x9800u) {
            mSwingAng = 0x9800;
        } else {
            mSwingAng = 0xe800;
        }
        mStone[idx].angle = mSwingAng;
    }

    if (mSwingSpeed >= 0x3800) {
        mStone[idx].fast = 1;
    } else {
        mStone[idx].fast = 0;
    }

    mNextStone = 1;
    mStoneDelay = 0;
    SeparateStones(idx);

    vol = 0x7f;
    if (mStone[idx].fast == 0) vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0, func_020126e8(mStone[idx].x));
}

// @symbol _ZN14dScMgCurling_c17BringOutNextStoneEv
/* Brings out the next stone once the 0x4ee0 delay has run down: it starts
 * active at (0x80000, 0x80000), the stone count at 0x4ee6 goes up (with a
 * sound from the second stone on), and the aim point resets. */
void dScMgCurling_c::BringOutNextStone()
{
    if (mNextStone == 0) return;
    if (mStoneDelay != 0)
    {
        (u16&)mStoneDelay -= 1;
        if ((short)mStoneDelay <= 0)
            mStoneDelay = 0;
        return;
    }
    mNextStone = 0;
    {
        int idx = mThrown;
        if (idx >= 5)
            return;
        mStone[idx].active = 1;
        mStone[idx].dealt = 1;
        mStone[idx].x = 0x80000;
        mStone[idx].y = 0x80000;
        mStone[idx].timer = 0;
        mStone[idx].fast = 0;
    }
    if (mThrown != 0)
        Sound::PlayBank2_2D(0x1d7);
    mThrown++;
    mAimX = 0x80000;
    mAimY = 0xb0000;
    mStylusHeld = 0;
    mAimShown = 1;
}

// @symbol _ZN14dScMgCurling_c10Scene_IdleEv
/* The scene's idle state. */
void dScMgCurling_c::Scene_Idle()
{
}

// @symbol _ZN14dScMgCurling_c9Scene_EndEv
/* The scene's end state. After the 0x4ee2 delay it asks the base whether
 * the game is over, moves to state 4 with the base's own end state, and
 * clears the stones' visible flags. */
void dScMgCurling_c::Scene_End()
{
    int i;
    if (mStateDelay != 0) {
        (u16&)mStateDelay -= 1;
        if ((s16&)mStateDelay <= 0)
            mStateDelay = 0;
        return;
    }
    if (Mg_IsNewRecord() != 0) {
        mRoundOver = 0;
        mState = 4;
        Mg_SetSharedState(MG_STATE_RESULTS);
    } else {
        mState = 4;
        Mg_SetSharedState(MG_STATE_RESULTS);
    }
    mPromptEnabled = 0;
    mAimShown = 0;
    for (i = 0; i < 5; i++) {
        mStone[i].dealt = 0;
    }
}

// @symbol _ZN14dScMgCurling_c11Scene_ScoreEv
/* The scoring state. Runs the popup countdowns, then after the 0x4ee2
 * delay either starts the next stone (state 1) or, once all five have been
 * thrown, ends the round (state 3) with the popups' points summed into the
 * score. The popups are cleared either way. */
void dScMgCurling_c::Scene_Score()
{
    int i;
    int sum;

    Popup_UpdateAll();

    if (mStateDelay != 0)
    {
        (u16&)mStateDelay -= 1;
        if ((s16&)mStateDelay <= 0)
            mStateDelay = 0;
        return;
    }

    mState = 1;
    if (mThrown >= 5)
    {
        mState = 3;
        mStateDelay = 0x80;
        mRoundOver = 1;
        sum = 0;
        Sound::PlayBank2_2D(0x1bc);
        for (i = 0; i < 5; i++)
            sum += mPopup[i].points;
        Mg_SubmitScore(sum);
    }

    for (i = 0; i < 5; i++)
    {
        mPopup[i].x = 0;
        mPopup[i].y = 0;
        mPopup[i].points = 0;
        mPopup[i].countdown = 0;
        mPopup[i].shown = 0;
        mPopup[i].live = 0;
    }

    BringOutNextStone();
}

// @symbol _ZN14dScMgCurling_c10Scene_PlayEv
/* The playing state. After the 0x4ee2 delay it shows the HUD once, runs
 * the aim state (0x4ee4) through its table, then every active stone's own
 * state through the other table, remembering the last position of each.
 * When no stone is still moving, the popups are scored and the scene moves
 * to state 2 with a 0xc0 delay. */
void dScMgCurling_c::Scene_Play()
{
    char *c = (char *)this;
    if (mStateDelay != 0) {
        u16 *q = &mStateDelay;
        *q = *q - 1;
        return;
    }
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    if (mSwingTimer != 0) {
        u8 *q = (u8 *)(c + 0x4ee9);
        *q = *q - 1;
    }
    (((C *)c)->*sCurlingAimTable[mStylusHeld])();

    {
        int count = 0;
        int i = 0;
        char *p = c;
        for (; i < 5; i++, p += 0x2c) {
            if (*(u8 *)(p + 0x4689) != 0) {
                *(int *)(p + 0x466c) = *(int *)(p + 0x4660);
                *(int *)(p + 0x4670) = *(int *)(p + 0x4664);
                if (*(u16 *)(p + 0x4680) != 0) {
                    u16 *q = (u16 *)(p + 0x4680);
                    *q = *q - 1;
                }
                (((C *)c)->*sCurlingStoneTable[*(u8 *)(p + 0x4688)])(i);
                if (*(u8 *)(p + 0x4688) != 2) count++;
            }
        }
        if (count != 0) return;
    }
    mState = 2;
    ScoreStones();
    mStateDelay = 0xc0;
}

// @symbol _ZN14dScMgCurling_c16Scene_StartRoundEv
/* Starts a round: clears the stones, brings out the first one and enters
 * the aiming state. */
void dScMgCurling_c::Scene_StartRound()
{
    ClearStones();
    mNextStone = 1;
    (s16&)mStoneDelay = 0;
    BringOutNextStone();
    mState = 1;
}

// @symbol _ZN14dScMgCurling_c9PickHouseEv
/* Picks the house (0x4e9c): a fresh random one of three that differs from
 * the last, or the first one when the slot still holds its 0xff reset.
 * The house centre comes from sCurlingHouseSpots and its background from
 * one of three files. */
void dScMgCurling_c::PickHouse()
{
    int m;
    void *file;
    if (mHouse == 0xff) {
        mHouse = 0;
    } else {
        m = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;
        if (mHouse == m) {
            m += ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) + 1;
            if (m >= 3) m -= 3;
        }
        mHouse = m;
    }
    mHouseX = sCurlingHouseSpots[mHouse * 2] << 12;
    mHouseY = (sCurlingHouseSpots[mHouse * 2 + 1] - 0xe0) << 12;
    if (mHouse == 0) {
        file = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NSC_BIN);
    } else if (mHouse == 1) {
        file = LoadFile(ASSET_D_2D_MG_BG_CURLING1_LEFT_NSC_BIN);
    } else if (mHouse == 2) {
        file = LoadFile(ASSET_D_2D_MG_BG_CURLING1_RIGHT_NSC_BIN);
    }
    func_020563d4(file, 0, 0x800);
    Deallocate(file);
}

// @symbol _ZN14dScMgCurling_c10ResetHouseEv
/* Marks the house as not chosen yet. */
void dScMgCurling_c::ResetHouse()
{
    mHouse = 255;
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN14dScMgCurling_c11ClearStonesEv
/* Zeroes the five stones, the five score popups and the aim fields, and
 * resets the score. */
void dScMgCurling_c::ClearStones()
{
    int i;
    char *c = (char *)this;
    char *r = c;
    for (i = 0; i < 5; i++) {
        *(int *)(r + 0x4660) = 0;
        *(int *)(r + 0x4664) = 0;
        *(int *)(r + 0x4668) = 0;
        *(int *)(r + 0x466c) = 0;
        *(int *)(r + 0x4670) = 0;
        *(int *)(r + 0x467c) = 0;
        *(short *)(r + 0x4682) = 0;
        *(short *)(r + 0x4684) = 0;
        *(short *)(r + 0x4686) = 0;
        *(unsigned char *)(r + 0x4688) = 0;
        *(unsigned char *)(r + 0x4689) = 0;
        *(unsigned char *)(r + 0x468a) = 0;
        r += 0x2c;
    }
    for (i = 0; i < 5; i++) {
        mPopup[i].x = 0;
        mPopup[i].y = 0;
        mPopup[i].points = 0;
        mPopup[i].countdown = 0;
        mPopup[i].live = 0;
        mPopup[i].shown = 0;
    }
    (s16&)mStoneDelay = 0;
    mThrown = 0;
    mAimX = 0;
    mAimY = 0;
    mGrabX = 0;
    mGrabY = 0;
    mSwingSpeed = 0;
    (s16&)mSwingAng = 0;
    mStylusHeld = 0;
    mAimShown = 0;
    mNextStone = 0;
    mRoundOver = 0;
    (s16&)mStateDelay = 0;
    (s16&)unk_4edc = 0;
    mSwingTimer = 0;
    Mg_SubmitScore(0);
}
#pragma pop

// @symbol _ZN14dScMgCurling_c10StartRoundEi
/* Slot 18, one of dScMgBase_c's own undeclared slots; the inherited label
 * is the tree-wide mislabel and this is not a destructor. Restarts the
 * scene: state 0, the stones cleared, a house picked, the blend set up
 * again and the base's handle refreshed. */
void dScMgCurling_c::StartRound(int /* arg */)
{
    mState = 0;
    ClearStones();
    PickHouse();
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    unk_4ed8 = Mg_GetTopRecord();
}

// @symbol _ZN14dScMgCurling_c6RenderEv
/* Slot 9. The base's frame, then the popups, the stones, the aim sprite
 * and the falling bits. */
s32 dScMgCurling_c::Render()
{
    Mg_DrawTopLeftCounter(Mg_GetTopRecord());
    Popup_DrawAll();
    DrawStonesLeft();
    DrawAimMarker();
    Bit_DrawAll();
    return 1;
}

// @symbol _ZN14dScMgCurling_c8BehaviorEv
/* Slot 6. The scene state through its table, then the falling bits. The
 * table's receiver C stays incomplete. */
s32 dScMgCurling_c::Behavior()
{
    C *c = (C *)this;
    int j = mState;
    (c->*sCurlingSceneTable[j])();
    Bit_UpdateAll();
    return 1;
}

// @symbol _ZN14dScMgCurling_c13InitResourcesEv
/* Slot 0. Loads the board and the stone graphics for both screens, the
 * palettes and the house, then starts the first round with a 0x40 delay. */
s32 dScMgCurling_c::InitResources()
{
    char *a = Ov004_LoadFileByPath(&sCurlingMgDirPath);
    char *b = Ov004_LoadFileByPath(&sCurlingBgCurling2NcgPath);
    void *f;

    if (a == 0 || b == 0) return 0;

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1220;
    DecompressLZ16(a, _ZN2G213GetBG2CharPtrEv());

    f = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NCL_BIN);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NSC_BIN);
    func_020563d4(f, 0, 0x800);
    Deallocate(f);

    {
        void *c7 = LoadFile(ASSET_D_2D_MG_CURLING_NCG_BIN);
        void *c8 = LoadFile(ASSET_D_2D_MG_CURLING_NCL_BIN);
        DecompressLZ16(c7, (void *)0x6400000);
        _ZN2GX11LoadOBJPlttEPKvjj(c8, 0, 0x100);

        data_0209d454 |= 4;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x814;
        DecompressLZ16(b, (void *)_ZN3G2S13GetBG2CharPtrEv());

        f = LoadFile(ASSET_D_2D_MG_BG_CURLING2_NCL_BIN);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);

        f = LoadFile(ASSET_D_2D_MG_BG_CURLING2_NSC_BIN);
        func_02056374(f, 0, 0x800);
        Deallocate(f);

        Ov004_Deallocate(a);
        Ov004_Deallocate(b);

        DecompressLZ16(c7, (void *)0x6600000);
        _ZN3GXS11LoadOBJPlttEPKvjj(c8, 0, 0x100);
        Deallocate(c7);
        Deallocate(c8);
    }

    ClearStones();
    ResetHouse();
    PickHouse();
    mNextStone = 1;
    mStoneDelay = 0;
    BringOutNextStone();
    Bit_InitAll();
    mState = 1;
    Mg_SetTopScreenYOffset(0x20);
    mStateDelay = 0x40;
    mRunDuringFade = 1;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    unk_4ed8 = Mg_GetTopRecord();
    return 1;
}

/* Reconstructed source-style name: SM64DS proves the dScMgCurling_c RTTI
 * identity, MG_CURLING registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: MgShuffleShell_Spawn. */
// @symbol dScMgCurling_c_classInit
extern "C" void *dScMgCurling_c_classInit()
{
    return new dScMgCurling_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_021304ac shard).
 * The seven PMF state tables plain-copied from the ROM PMF constants, in
 * retail copy order: the five no-arg scene states, the four per-state
 * tables, the two stylus states, the three kind states, the four Entry
 * states, the four reveal states, then the three state2 states. */
PMF0 sCurlingSceneTable[5] = {
    pmf_dScMgCurling_c_Scene_StartRound, pmf_dScMgCurling_c_Scene_Play,
    pmf_dScMgCurling_c_Scene_Score,      pmf_dScMgCurling_c_Scene_End,
    pmf_dScMgCurling_c_Scene_Idle,
};
PMF sCurlingStoneTable[4] = {
    pmf_dScMgCurling_c_Stone_WaitRelease,
    pmf_dScMgCurling_c_Stone_Slide,
    pmf_dScMgCurling_c_Stone_Collide,
    pmf_dScMgCurling_c_Stone_Drag,
};
PMF0 sCurlingAimTable[2] = {
    pmf_dScMgCurling_c_Aim_WaitTouch,
    pmf_dScMgCurling_c_Aim_Swing,
};
PMF sCurlingBitKindTable[3] = {
    pmf_dScMgCurling_c_Bit_PickKind,
    pmf_dScMgCurling_c_Bit_RunKindB,
    pmf_dScMgCurling_c_Bit_RunKindA,
};
Entry sCurlingBitBStateTable[4] = {
    {pmf_dScMgCurling_c_BitB_StatePause},
    {pmf_dScMgCurling_c_BitB_StateDriftRight},
    {pmf_dScMgCurling_c_BitB_StateDriftLeft},
    {pmf_dScMgCurling_c_BitB_StateDriftStop},
};
PMF sCurlingBitAStateTable[4] = {
    pmf_dScMgCurling_c_BitA_StatePause,
    pmf_dScMgCurling_c_BitA_StateDriftRight,
    pmf_dScMgCurling_c_BitA_StateDriftLeft,
    pmf_dScMgCurling_c_BitA_StateDriftStop,
};
PMF sCurlingBitStateTable[3] = {
    pmf_dScMgCurling_c_Bit_StateFallStart,
    pmf_dScMgCurling_c_Bit_StateFallSpeedUp,
    pmf_dScMgCurling_c_Bit_StateFallSlowDown,
};
