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
 * comment leftovers: a few spellings keep `pointer + offset` arithmetic
 * because every member spelling changes codegen; their offsets are named
 * through CURL_OFF:
 * - Popup_UpdateAll / ScoreStones / Scene_Play / ClearStones sit under
 *   opt_strength_reduction off and walk the rows with a stepped `char *`.
 *   Popup_UpdateAll's volatile countdown launder and ScoreStones' M()
 *   popup-Y launder stay; ScoreStones' stone x/y reads and Scene_Play's
 *   timer pointer and state reads keep CURL_OFF.
 * - Stone_Slide keeps its angle/speed/position pointer temporaries; the
 *   angle and speed ones stay `&mStone[0].f + offset`, since
 *   &mStone[idx] moves their address math.
 * - Bit_InitAll's second init walk keeps `char *bit`, and its x, y, wait
 *   and texA accesses stay `bit + CURL_OFF(...)`: the member spelling grows
 *   the function by one to four words.
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
#include "G2x.h"

namespace G2 {
void *GetBG2CharPtr(void);
}

namespace G2S {
unsigned GetBG2CharPtr(void);
}

namespace GX {
void LoadBGPltt(const void *p, u32 a, u32 b);
void LoadOBJPltt(const void *p, u32 a, u32 b);
}

namespace GXS {
void LoadBGPltt(const void *p, u32 a, u32 b);
void LoadOBJPltt(const void *p, u32 a, u32 b);
}

/* `C` is the receiver of the pointer-to-member state tables. It must stay
 * incomplete: mwccarm picks the pointer-to-member layout from whether the
 * class is complete, and completing it changes all four dispatchers. */
struct C;
typedef void (C::*PMF)(int);
struct Entry {
    PMF pmf;
};
typedef void (C::*PMF0)();

/* B4 is a 4-byte-record view of a touch lane, used by the stone-drag
 * reader; the other readers index the lanes flat. */
struct B4 {
    unsigned char v;
    unsigned char pad[3];
};

/* Launder: forces an address through an integer so it is not shared. */
#define M(p) ((int *)(int)(p))

/* Byte offset of a dScMgCurling_c member, for the sites that must keep
   `pointer + constant` arithmetic because every member spelling moved bytes. */
#define CURL_OFF(m) ((int)&((dScMgCurling_c *)0)->m)

/* Everything this file calls or reads. Class members cannot go inside an
 * `extern "C"` block, so the C-linkage names are collected here in one. */
extern "C" {

extern int Mg_GetSubmittedScore(void);
extern void Mg_RenderSubSpriteMtx(int a, int b, int c, int d);
extern int Mg_RenderSpriteBoth(int a, int b, int c, int d, int e);
extern void Mg_DrawNumberRotated(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_02012718(int a, int b);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern int RandomIntInternal(int *seed);
extern int atan2_hold_d_s_mg_curling_1(int y, int x);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern int Mg_IsNewRecord(void);
extern void Mg_SetSharedState(int state);
extern void Mg_SubmitScore(int score);
extern int Mg_GetTopRecord(void);
extern int Mg_DrawTopLeftCounter(int self);
extern char *Ov004_LoadFileByPath(void *p);
extern void Mg_SetTopScreenYOffset(int a);

extern void DecompressLZ16(const void *src, void *dst);
extern void *LoadFile(int handle);
extern void Deallocate(void *ptr);
extern void Ov004_Deallocate(void *p);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void func_02056374(const void *src, u32 offset, u32 count);

extern int data_0209d4b8;
extern s16 data_02082214[];
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

} /* extern "C" */

namespace cstd {
s16 atan2(s32, s32);
int sqrt(u64 value);
}
namespace Sound {
u32 PlayBank2_2D(u32 id);
}

// @symbol _ZN14dScMgCurling_cD1Ev
// @symbol _ZN14dScMgCurling_cD0Ev
dScMgCurling_c::~dScMgCurling_c() {}
// @symbol _ZN14dScMgCurling_c11Bit_DrawAllEv
/* Draws the falling bits. The functions after this one are the bits'
 * states: each field is 0x24 * index past its base offset. */
void dScMgCurling_c::Bit_DrawAll() {
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
        if ((s16 &)mBit[index].wait2 < 0)
            (s16 &)mBit[index].wait2 = 0;
    } else {
        mBit[index].state2 = 2;
        (s16 &)mBit[index].wait2 =
            (short)(unsigned char)((((0x20 *
                                      (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                       0x7fff)) >>
                                     0xf)) +
                                   0x20);
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
        if ((s16 &)mBit[index].timer < 0)
            (s16 &)mBit[index].timer = 0;
        return;
    }
    if (mBit[index].vx > 0) {
        mBit[index].vx = mBit[index].vx - 8;
        if ((s16)mBit[index].vx < 0)
            mBit[index].vx = 0;
        return;
    }
    if (mBit[index].vx < 0) {
        mBit[index].vx = mBit[index].vx + 8;
        if (mBit[index].vx > 0)
            mBit[index].vx = 0;
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
        if ((s16 &)mBit[index].wait < 0)
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
        if ((s16 &)mBit[index].timer < 0)
            mBit[index].timer = 0;
        return;
    }

    mBit[index].state = 3;
    mBit[index].timer =
        (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >>
                         0xf) +
                        0x20);
}

// @symbol _ZN14dScMgCurling_c20BitA_StateDriftRightEi
void dScMgCurling_c::BitA_StateDriftRight(int index) {
    unsigned short timer;

    mBit[index].x += mBit[index].vx;
    mBit[index].y += mBit[index].vy;

    timer = mBit[index].wait;
    if (timer != 0) {
        (s16 &)mBit[index].wait = timer - 1;
        if ((s16 &)mBit[index].wait < 0)
            (s16 &)mBit[index].wait = 0;
        return;
    }

    if (mBit[index].vx < 0x300) {
        mBit[index].vx += 8;
        if (mBit[index].vx >= 0x300)
            mBit[index].vx = 0x300;
    }

    timer = mBit[index].timer;
    if (timer != 0) {
        (s16 &)mBit[index].timer = timer - 1;
        if ((s16 &)mBit[index].timer < 0)
            (s16 &)mBit[index].timer = 0;
        return;
    }

    mBit[index].state = 3;
    (s16 &)mBit[index].timer =
        (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 &
        0xff;
}

// @symbol _ZN14dScMgCurling_c15BitA_StatePauseEi
void dScMgCurling_c::BitA_StatePause(int index) {
    if (mBit[index].wait != 0) {
        mBit[index].wait = mBit[index].wait - 1;
        if ((s16 &)mBit[index].wait < 0)
            (s16 &)mBit[index].wait = 0;
        return;
    }
    mBit[index].vx = 0;
    mBit[index].state =
        sCurlingBitAStates[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                               << 1 >>
                           15];
    mBit[index].wait =
        (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                        0x7fff)) >>
                               0xf);
    mBit[index].timer =
        (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                         0x7fff)) >>
                                0xf) +
                               0x60);
}

// @symbol _ZN14dScMgCurling_c12Bit_RunKindAEi
void dScMgCurling_c::Bit_RunKindA(int index) {
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
        if ((int)(short)*vx < 0)
            *vx = 0;
    } else if (*vx < 0) {
        *vx += 0x20;
        if (*vx > 0)
            *vx = 0;
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

    unsigned short *wait = &mBit[index].wait;
    if (*wait != 0) {
        *wait = *wait - 1;
        short left = *(short *)wait;
        if (left < 0) {
            *wait = 0;
        }
        return;
    }

    int *vx = &mBit[index].vx;
    if (*vx < 0x400) {
        *vx = *vx + 0x20;
        if (*vx >= 0x400) {
            *vx = 0x400;
        }
    }

    unsigned short *timer = &mBit[index].timer;
    if (*timer != 0) {
        *timer = *timer - 1;
        short left = *(short *)timer;
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
        (s16 &)mBit[idx].wait = left;
        if ((s16 &)mBit[idx].wait < 0)
            (s16 &)mBit[idx].wait = 0;
        return;
    }
    mBit[idx].vx = 0;
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mBit[idx].vy = (int)(((roll << 5) >> 15) << 4) + 0x600;
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mBit[idx].state = sCurlingBitBStates[(roll << 1) >> 15];
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    (s16 &)mBit[idx].wait = (unsigned char)((roll << 4) >> 15);
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    (s16 &)mBit[idx].timer = (unsigned char)(((roll * 0x30) >> 15) + 0x30);
}

// @symbol _ZN14dScMgCurling_c12Bit_RunKindBEi
void dScMgCurling_c::Bit_RunKindB(int idx) {
    unsigned char state = mBit[idx].state;
    (((C *)this)->*sCurlingBitBStateTable[state].pmf)(idx);
}

#pragma push
#pragma opt_propagation off
// @symbol _ZN14dScMgCurling_c12Bit_PickKindEi
void dScMgCurling_c::Bit_PickKind(int idx) {
    unsigned roll = (unsigned)RandomIntInternal(&data_0209d4b8);
    int k = 0;
    unsigned pick = ((roll >> 16) & 0x7fff) << 3 >> 0xf;
    if (pick == 5)
        k = 1;
    mBit[idx].kind = sCurlingBitKinds[k];
    mBit[idx].state = 0;
}
#pragma pop

// @symbol _ZN14dScMgCurling_c13Bit_UpdateAllEv
void dScMgCurling_c::Bit_UpdateAll() {
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mBit[i].on != 0) {
            unsigned char kind = mBit[i].kind;
            (((C *)this)->*sCurlingBitKindTable[kind])(i);
            if ((mBit[i].y >> 0xc) >= 0xc8) {
                mBit[i].x = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                                << 5 >>
                            0xf << 0xf;
                mBit[i].y = -0x8000;
                mBit[i].state = 0;
                mBit[i].kind = 0;
                mBit[i].state2 = 0;
            }
        }
    }
}

// @symbol _ZN14dScMgCurling_c11Bit_InitAllEv
void dScMgCurling_c::Bit_InitAll() {
    int i;
    char *bit;
    unsigned int roll;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    for (; i < 0x32; i++) {
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

    /* bit walks `this` one dScMgCurling_bit per pass, so its mBit[0] is
       mBit[i]. The texA read and the second x/y/wait stores stay raw: spelled
       as members they change the generated code. */
    i = 0;
    bit = (char *)this;
    for (; i < 0x32; i++) {
        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((roll >> 16) & 0x7fff) << 5;
        ((dScMgCurling_c *)bit)->mBit[0].x = (int)((m >> 0xf)) << 0xf;
        ((dScMgCurling_c *)bit)->mBit[0].y = -0x8000;
        ((dScMgCurling_c *)bit)->mBit[0].on = 1;
        ((dScMgCurling_c *)bit)->mBit[0].shown = 1;
        ((dScMgCurling_c *)bit)->mBit[0].kind = 0;
        ((dScMgCurling_c *)bit)->mBit[0].state = 0;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        ((dScMgCurling_c *)bit)->mBit[0].texA = (char)(((roll >> 16) & 0x7fff) * 5 >> 0xf);

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(bit + CURL_OFF(mBit[0].texA)) +
            ((((roll >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        ((dScMgCurling_c *)bit)->mBit[0].texB = (char)v;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        ((dScMgCurling_c *)bit)->mBit[0].wait =
            (short)(((i & 7) << 6) + (((roll >> 16) & 0x7fff) * 0x30 >> 0xf));

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((roll >> 16) & 0x7fff) << 5;
        *(int *)(bit + CURL_OFF(mBit[0].x)) = (int)((m >> 0xf)) << 0xf;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((roll >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(bit + CURL_OFF(mBit[0].y)) = (((q << 3) - 8)) << 0xc;
        *(short *)(bit + CURL_OFF(mBit[0].wait)) = 0;
        bit += 0x24;
    }
}

// @symbol _ZN14dScMgCurling_c13Popup_DrawAllEv
void dScMgCurling_c::Popup_DrawAll() {
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
void dScMgCurling_c::Popup_UpdateAll() {
    char *raw = (char *)this;
    int i;
    for (i = 0; i < 5; i++) {
        char *popup = raw + (i << 4);
        if (mPopup[i].live == 0)
            continue;
        if (mPopup[i].countdown == 0)
            continue;
        {
            volatile unsigned short *timer =
                (volatile u16 *)((unsigned int)&((dScMgCurling_c *)popup)->mPopup[0].countdown);
            *timer = *timer - 1;
            if (*timer != 0)
                continue;
        }
        mPopup[i].shown = 1;
        Sound::PlayBank2_2D(0x1bc);
    }
}
#pragma pop

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN14dScMgCurling_c11ScoreStonesEv
void dScMgCurling_c::ScoreStones() {
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

        if (((dScMgCurling_c *)stone)->mStone[0].active == 0)
            continue;
        dx = *(int *)(stone + CURL_OFF(mStone[0].x)) - mHouseX;
        dz = *(int *)(stone + CURL_OFF(mStone[0].y)) - mHouseY;
        {
            int ax = dx >> 12;
            int az = dz >> 12;
            dist = cstd::sqrt((u64)(s64)(ax * ax + az * az));
        }
        mPopup[slot].live = 1;
        mPopup[slot].x = *(int *)(stone + CURL_OFF(mStone[0].x));
        {
            int *popupY = M(&((dScMgCurling_c *)(raw + slot * 16))->mPopup[0].y);
            *popupY = *(int *)(stone + CURL_OFF(mStone[0].y)) + 0x1000;
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
/* Draws a sprite at the aimed stone's position (mAimX/mAimY) once the
 * stylus handler has set mAimShown. */
void dScMgCurling_c::DrawAimMarker() {
    if (mAimShown == 0)
        return;
    int x = mAimX;
    int y = mAimY;
    Mg_RenderSpriteBoth((int)sCurlingAimMarkerSprites, (x >> 12) - 0x20, (y >> 12) - 8, -1, 0);
}

#pragma push
#pragma opt_common_subs off
// @symbol _ZN14dScMgCurling_c9Aim_SwingEv
/* Stylus handler for the stone being aimed. While the stylus is down, the
 * touch point plus the grab offset becomes the new position, clamped to the
 * 0x20000..0xe0000 by 0x94000..0xb8000 box; a move under two units is undone.
 * Otherwise it updates the swing tracking (mSwingState), turns the move into
 * an angle at mSwingAng clamped to the lower half turn and averaged with the last
 * one, and folds the move length into mSwingSpeed. With the stylus
 * up, two flags are reset.
 *
 * Shapes that must stay: the touch records are read through a `u8 *` cast;
 * dx is computed before dy with no temporaries; the tail's index `j` is
 * wider than a byte; and the pragma keeps the mAimX and mAimY re-reads
 * after the clamps. */
void dScMgCurling_c::Aim_Swing() {
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
            mLastSwingDeltaY = (mAimY - mAimHomeY) >> 12;
            mAimHomeY = mAimY;
        } else if (mSwingState == 1) {
            if (mLastSwingDeltaY * diff > 0) {
                if (diff < 0)
                    diff = -diff;
                if (diff >= 0xa)
                    mSwingState = 0;
            } else {
                mLastSwingDeltaY = diff;
                mAimHomeY = mAimY;
            }
        } else {
            if (mLastSwingDeltaY * diff < 0)
                mSwingState = 1;
            mLastSwingDeltaY = (mAimY - mAimHomeY) >> 12;
            mAimHomeY = mAimY;
        }

        ang = mSwingAng;
        mSwingAng = cstd::atan2(dy, dx >> 1);
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
void dScMgCurling_c::Aim_WaitTouch() {
    int idx;
    int touching = 0;
    int x;
    int y;
    idx = gActivePlayerSlot;
    if (gTouchHeld[idx * (4 & 0xFFFFFFFF)] != 0) {
        if (gTouchEdge[idx * 4] != 0) {
            touching = 1;
        }
    }
    if (touching == 0) {
        return;
    }
    x = (mAimX >> 0xc) - ((u8 *)gTouchX)[idx * 4];
    y = (mAimY >> 0xc) - ((u8 *)gTouchY)[idx * 4];
    mGrabX = x << 0xc;
    mGrabY = y << 0xc;
    mStylusHeld = 1;
    mSwingAng = 0xc000;
    if (mSwingTimer == 0) {
        func_02012718(0x1d2, mAimX);
        mSwingTimer = 6;
    }
    unk_4ecc = 0;
    unk_4ed0 = 0;
    mAimHomeX = mAimX + mGrabX;
    mAimHomeY = mAimY + mGrabY;
    mLastSwingDeltaY = 0xff;
    mSwingState = 0;
}

// @symbol _ZN14dScMgCurling_c14DrawStonesLeftEv
void dScMgCurling_c::DrawStonesLeft() {
    int x, y;
    int i;
    int remaining;
    int j;
    for (i = 0; i < 5; i++) {
        if (mStone[i].active == 0)
            continue;
        if (mStone[i].dealt == 0)
            continue;
        x = mStone[i].x >> 12;
        y = mStone[i].y >> 12;
        RenderOamBothScreens(&sCurlingStoneLeftSprite, x, y, -1, 1, 0);
        RenderOamBothScreens(&sCurlingStoneLeftShadowSprite, x, y + 8, -1, 2, 0);
    }
    remaining = 5 - mThrown;
    if (remaining < 0)
        remaining = 0;
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
 * The rotated offsets need separate temporaries, and the inner scan needs
 * its own displacement, distance and angle; sharing the outer ones changes the registers. */
void dScMgCurling_c::SeparateStones(int stoneIndex) {
    int hitIndex;
    int nextIndex;
    int dx;
    int dy;
    int distance;
    u16 contactAngle;
    int angleIndex;

    for (hitIndex = 0; hitIndex < 5; hitIndex++) {
        if (mStone[hitIndex].active == 0)
            continue;
        if (stoneIndex == hitIndex)
            continue;
        dx = (mStone[hitIndex].x - mStone[stoneIndex].x) >> 12;
        dy = (mStone[hitIndex].y - mStone[stoneIndex].y) >> 12;
        distance = cstd::sqrt((u64)(dx * dx + dy * dy));
        contactAngle = cstd::atan2(dy, dx);
        if (distance > 0x18)
            continue;
        {
            int contactCos;
            int contactSin;
            int separationX;
            int separationY;

            angleIndex = (contactAngle >> 4) * 2;
            contactCos = data_02082214[angleIndex + 1];
            separationX = FX_MUL(contactCos, 0x1a);
            mStone[hitIndex].x = mStone[stoneIndex].x + (separationX << 12);
            contactSin = data_02082214[angleIndex];
            separationY = FX_MUL(contactSin, 0x1a);
            mStone[hitIndex].y = mStone[stoneIndex].y + (separationY << 12);
            for (nextIndex = 0; nextIndex < 5; nextIndex++) {
                int dx2;
                int dy2;
                int nextDistance;
                u16 nextContactAngle;

                if (mStone[nextIndex].active == 0)
                    continue;
                if (hitIndex == nextIndex)
                    continue;
                dx2 = (mStone[nextIndex].x - mStone[hitIndex].x) >> 12;
                dy2 = (mStone[nextIndex].y - mStone[hitIndex].y) >> 12;
                nextDistance = cstd::sqrt((u64)(dx2 * dx2 + dy2 * dy2));
                nextContactAngle = cstd::atan2(dy2, dx2);
                if (nextDistance > 0x18)
                    continue;
                {
                    int nextContactCos;
                    int nextContactSin;
                    int nextSeparationX;
                    int nextSeparationY;

                    angleIndex = (nextContactAngle >> 4) * 2;
                    nextContactCos = data_02082214[angleIndex + 1];
                    nextSeparationX = FX_MUL(nextContactCos, 0x1a);
                    mStone[nextIndex].x = mStone[hitIndex].x + (nextSeparationX << 12);
                    nextContactSin = data_02082214[angleIndex];
                    nextSeparationY = FX_MUL(nextContactSin, 0x1a);
                    mStone[nextIndex].y = mStone[hitIndex].y + (nextSeparationY << 12);
                    func_02012718(0xe8, mStone[stoneIndex].x);
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
void dScMgCurling_c::ResolveStoneCollision(int stoneIndex) {
    int hitIndex;
    int dx;
    int dy;

    for (hitIndex = 0; hitIndex < 5; hitIndex++) {
        if (mStone[hitIndex].active == 0)
            continue;
        if (stoneIndex == hitIndex)
            continue;
        if (mStone[hitIndex].state == 0)
            continue;
        if (mStone[hitIndex].state == 3)
            continue;
        dx = (mStone[hitIndex].x - mStone[stoneIndex].x) >> 12;
        dy = (mStone[hitIndex].y - mStone[stoneIndex].y) >> 12;
        if (cstd::sqrt((long long)(dx * dx + dy * dy)) > 0x18)
            continue;
        {
            u16 *pAngle;
            s32 *pSpeed;
            u16 *pHitAngle;
            int movingTangent;
            int angleIndex;
            int contactCos;
            int movingVelocityY;
            int contactSin;
            u16 contactAngle;
            int hitTangent;
            int inverseContactCos;
            int hitResultX;
            u16 relativeAngle;
            int xi;
            int inverseContactSin;
            int movingNormal;
            int pixelY;
            int movingVelocityX;
            int hitNormal;
            int hitVelocityX;
            int movingResultY;
            int hitResultY;
            int hitVelocityY;

            /* Contact line, from the hit stone to the moving one. */
            dx = mStone[stoneIndex].x - mStone[hitIndex].x;
            dy = mStone[stoneIndex].y - mStone[hitIndex].y;
            contactAngle = cstd::atan2(dy, dx);
            relativeAngle = -contactAngle;
            /* Both velocities as x/y. If the hit stone has zero y velocity,
             * it takes half the moving stone's y velocity before the exchange. */
            pAngle = &mStone[stoneIndex].angle;
            pSpeed = &mStone[stoneIndex].speed;
            angleIndex = (mStone[stoneIndex].angle >> 4) * 2;
            movingVelocityX = FX_MUL(data_02082214[angleIndex + 1], mStone[stoneIndex].speed);
            movingVelocityY = FX_MUL(data_02082214[angleIndex], mStone[stoneIndex].speed);
            pHitAngle = &mStone[hitIndex].angle;
            angleIndex = (mStone[hitIndex].angle >> 4) * 2;
            hitVelocityX = FX_MUL(data_02082214[angleIndex + 1], mStone[hitIndex].speed);
            hitVelocityY = FX_MUL(data_02082214[angleIndex], mStone[hitIndex].speed);
            if (hitVelocityY == 0)
                hitVelocityY = movingVelocityY >> 1;
            /* sin/cos of -contactAngle rotate into the contact frame; sin/cos of
             * +contactAngle rotate back out. */
            angleIndex = (relativeAngle >> 4) * 2;
            inverseContactSin = data_02082214[angleIndex];
            inverseContactCos = data_02082214[angleIndex + 1];
            relativeAngle = -relativeAngle;
            angleIndex = (relativeAngle >> 4) * 2;
            contactSin = data_02082214[angleIndex];
            contactCos = data_02082214[angleIndex + 1];

            /* Normal and tangential components of each velocity. */
            movingNormal = FX_MUL(inverseContactCos, movingVelocityX);
            movingNormal -= FX_MUL(inverseContactSin, movingVelocityY);
            movingTangent = FX_MUL(inverseContactSin, movingVelocityX) +
                            FX_MUL(inverseContactCos, movingVelocityY);
            hitNormal =
                FX_MUL(inverseContactCos, hitVelocityX) - FX_MUL(inverseContactSin, hitVelocityY);
            hitTangent =
                FX_MUL(inverseContactSin, hitVelocityX) + FX_MUL(inverseContactCos, hitVelocityY);
            /* Swap the normal components and rotate back: dx/movingResultY for the
             * moving stone, hitResultX/hitResultY for the one it hit. */
            dx = FX_MUL(contactCos, hitNormal) - FX_MUL(contactSin, movingTangent);
            movingResultY = FX_MUL(contactSin, hitNormal) + FX_MUL(contactCos, movingTangent);
            hitResultX = FX_MUL(contactCos, movingNormal) - FX_MUL(contactSin, hitTangent);
            hitResultY = FX_MUL(contactSin, movingNormal) + FX_MUL(contactCos, hitTangent);
            *pAngle = cstd::atan2(movingResultY, dx);
            *pSpeed =
                cstd ::sqrt((u64)((long long)dx * dx + (long long)movingResultY * movingResultY));

            /* Separate the stones along the contact line, then keep the
             * moving one on the board and hand any overshoot to the other. */
            mStone[stoneIndex].x = mStone[hitIndex].x + FX_MUL(contactCos, 0x1b000);
            mStone[stoneIndex].y = mStone[hitIndex].y + FX_MUL(contactSin, 0x1b000);
            xi = mStone[stoneIndex].x >> 12;
            pixelY = mStone[stoneIndex].y >> 12;
            if (xi - 0xc < 0) {
                xi = mStone[stoneIndex].x - 0xc000;
                mStone[hitIndex].x += xi;
                mStone[stoneIndex].x = 0xc000;
            }
            if (xi + 0xc > 0x100) {
                mStone[hitIndex].x += mStone[stoneIndex].x - 0xf4000;
                mStone[stoneIndex].x = 0xf4000;
            }
            if (pixelY - 0xc < -0xe0) {
                mStone[hitIndex].y += mStone[stoneIndex].y + 0xd4000;
                mStone[stoneIndex].y = -0xd4000;
            }

            *pHitAngle = cstd::atan2(hitResultY, hitResultX);
            mStone[hitIndex].speed = cstd::sqrt(
                (u64)((long long)hitResultX * hitResultX + (long long)hitResultY * hitResultY));
            mStone[stoneIndex].state = 1;
            mStone[hitIndex].state = 1;
            if (mStone[hitIndex].speed >= 0x3800) {
                mStone[hitIndex].fast = 1;
            } else {
                mStone[hitIndex].fast = 0;
            }
            func_02012718(0xe8, mStone[stoneIndex].x);
            return;
        }
    }
}

// @symbol _ZN14dScMgCurling_c12SetStoneSpinEi
/* Sets stone i's spin from the x component of its velocity:
 * -(cos(angle) * speed) / 4. */
void dScMgCurling_c::SetStoneSpin(int stoneIndex) {
    int heading = mStone[stoneIndex].angle;
    int tableOffset = (((heading >> 4) << 1) + 1) << 1;
    int cosHeading = *(short *)((char *)data_02082214 + tableOffset);
    int speed = mStone[stoneIndex].speed;
    long long product = (long long)cosHeading * speed;
    int velocityX = (int)(((unsigned long long)(product + 0x800)) >> 12);
    mStone[stoneIndex].spin = (short)((-velocityX) >> 2);
}

// @symbol _ZN14dScMgCurling_c10Stone_DragEi
/* Stone i while it is being dragged. With the stylus down the stone
 * follows the touch point plus the grab offset (grabX/grabY), its x kept
 * between 0xe and 0xf2 units, and the offset is recomputed; with the stylus
 * up the drag state clears and, if the aimed stone (mAimX/mAimY) overlaps it, the
 * aimed stone is moved just below it. */
void dScMgCurling_c::Stone_Drag(int stoneIndex) {
    unsigned char playerSlot = gActivePlayerSlot;
    if (((struct B4 *)gTouchHeld)[playerSlot].v) {
        int stoneXPixels, grabXPixels, grabYPixels;
        mStone[stoneIndex].x =
            mStone[stoneIndex].grabX + (((struct B4 *)gTouchX)[playerSlot].v << 12);
        stoneXPixels = mStone[stoneIndex].x >> 12;
        if (stoneXPixels < 0xe)
            mStone[stoneIndex].x = 0xe000;
        if (stoneXPixels > 0xf2)
            mStone[stoneIndex].x = 0xf2000;
        grabXPixels = (mStone[stoneIndex].x >> 12) - ((struct B4 *)gTouchX)[gActivePlayerSlot].v;
        grabYPixels = (mStone[stoneIndex].y >> 12) - ((struct B4 *)gTouchY)[playerSlot].v;
        mStone[stoneIndex].grabX = grabXPixels << 12;
        mStone[stoneIndex].grabY = grabYPixels << 12;
    } else {
        int dx, dy;
        mStone[stoneIndex].state = 0;
        dx = (mAimX - mStone[stoneIndex].x) >> 12;
        dy = (mAimY - mStone[stoneIndex].y) >> 12;
        if (dx < -0x2e)
            return;
        if (dx > 0x2e)
            return;
        if (dy < -0x14)
            return;
        if (dy <= 0x14)
            mAimY = mStone[stoneIndex].y + 0x15000;
    }
}

// @symbol _ZN14dScMgCurling_c13Stone_CollideEi
/* A tail-call veneer to ResolveStoneCollision. */
void dScMgCurling_c::Stone_Collide(int idx) {
    ResolveStoneCollision(idx);
}

// @symbol _ZN14dScMgCurling_c11Stone_SlideEi
/* Stone idx while it slides. Moves it by its velocity, adds the spin to
 * the angle, bounces it off the four walls (with a sound), then takes
 * friction off the speed: a base of speed/512 (at least 0x1c), more in the
 * five bands below the house from the sCurlingSlideZoneEdges tables, and a
 * steeper rate while the stone still travels upward above -0x20. At zero
 * speed the stone stops (state 2). The collision and spin helpers run, and
 * the slide sound (slideSnd) is retuned to the speed. */
void dScMgCurling_c::Stone_Slide(int stoneIndex) {
    char *c = (char *)this;
    int stoneOffset;
    int zi;
    u16 *pAngle;
    int *pSpeed;
    int *pX;
    int *pY;
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
    int cosAngle;
    int sinAngle;

    stoneOffset = stoneIndex * 0x2c;

    pAngle = (u16 *)((char *)&mStone[0].angle + stoneOffset);
    pSpeed = (int *)((char *)&mStone[0].speed + stoneOffset);
    pX = &mStone[stoneIndex].x;
    pY = &mStone[stoneIndex].y;

    cosAngle = data_02082214[((*pAngle) >> 4) * 2 + 1];
    *pX += FX_MUL(cosAngle, *pSpeed);
    sinAngle = data_02082214[((*pAngle) >> 4) * 2];
    *pY += FX_MUL(sinAngle, *pSpeed);
    mStone[stoneIndex].heading += (u16)mStone[stoneIndex].spin;

    x = *pX;
    z = *pY;
    xi = x >> 12;
    zi = z >> 12;

    if (xi + 0xc >= 0x100) {
        *pAngle = 0x8000 - *pAngle;
        *pX = 0xf4000;
        func_02012718(0x1d4, *pX);
    } else if (xi - 0xc < 0) {
        *pAngle = 0x8000 - *pAngle;
        *pX = 0xc000;
        func_02012718(0x1d4, *pX);
    }

    if (zi + 0xc > 0xc0) {
        mStone[stoneIndex].angle = -mStone[stoneIndex].angle;
        *pY = 0xb4000;
        func_02012718(0x1d4, *pX);
    } else if (zi - 0xc < -0xe0) {
        mStone[stoneIndex].angle = -mStone[stoneIndex].angle;
        *pY = -0xd4000;
        func_02012718(0x1d4, *pX);
    }

    zi = *pSpeed;
    v = zi >> 9;
    if (v <= 0x1c)
        v = 0x1c;
    for (i2 = 0, sbv = *pY >> 12; i2 < 5; i2++) {
        if (sbv <= -(sCurlingSlideZoneEdges[i2] + 0x20)) {
            s = data_02082214[(*pAngle >> 4) * 2];
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
        s = data_02082214[(*pAngle >> 4) * 2];
        if (s > 0) {
            v = zi >> 3;
            if (v <= 0x180)
                v = 0x180;
        }
    }

    mStone[stoneIndex].speed -= v;
    pd = &mStone[stoneIndex].speed;
    if (mStone[stoneIndex].speed <= 0) {
        *pSpeed = 0;
        mStone[stoneIndex].state = 2;
    }

    ResolveStoneCollision(stoneIndex);
    SetStoneSpin(stoneIndex);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    mStone[stoneIndex].slideSnd =
        func_02012468(mStone[stoneIndex].slideSnd, 2, 0xe7, 7, p, w, func_020126e8(*pX), 0);
}

// @symbol _ZN14dScMgCurling_c17Stone_WaitReleaseEi
/* Releases stone idx if the aimed stone (mAimX/mAimY) overlaps it: it takes
 * the aim angle (mSwingAng, clamped to the lower half turn) and the swing
 * speed (mSwingSpeed), is flagged fast above 0x3800, and starts sliding. The aim is
 * then put away, the stones are separated, and the throw sound plays at a
 * volume that depends on the fast flag. */
void dScMgCurling_c::Stone_WaitRelease(int idx) {
    int v, w, vol;

    if (mStylusHeld != 1)
        return;

    v = (mAimX - mStone[idx].x) >> 12;
    w = (mAimY - mStone[idx].y) >> 12;
    if (v < -0x2e)
        return;
    if (v > 0x2e)
        return;
    if (w < -0x14)
        return;
    if (w > 0x14)
        return;

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
    if (mStone[idx].fast == 0)
        vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0, func_020126e8(mStone[idx].x));
}

// @symbol _ZN14dScMgCurling_c17BringOutNextStoneEv
/* Brings out the next stone once the mStoneDelay delay has run down: it starts
 * active at (0x80000, 0x80000), the stone count mThrown goes up (with a
 * sound from the second stone on), and the aim point resets. */
void dScMgCurling_c::BringOutNextStone() {
    if (mNextStone == 0)
        return;
    if (mStoneDelay != 0) {
        (u16 &)mStoneDelay -= 1;
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
void dScMgCurling_c::Scene_Idle() {}

// @symbol _ZN14dScMgCurling_c9Scene_EndEv
/* The scene's end state. After the mStateDelay delay it asks the base whether
 * the game is over, moves to state 4 with the base's own end state, and
 * clears the stones' visible flags. */
void dScMgCurling_c::Scene_End() {
    int i;
    if (mStateDelay != 0) {
        (u16 &)mStateDelay -= 1;
        if ((s16 &)mStateDelay <= 0)
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
/* The scoring state. Runs the popup countdowns, then after the mStateDelay
 * delay either starts the next stone (state 1) or, once all five have been
 * thrown, ends the round (state 3) with the popups' points summed into the
 * score. The popups are cleared either way. */
void dScMgCurling_c::Scene_Score() {
    int i;
    int sum;

    Popup_UpdateAll();

    if (mStateDelay != 0) {
        (u16 &)mStateDelay -= 1;
        if ((s16 &)mStateDelay <= 0)
            mStateDelay = 0;
        return;
    }

    mState = 1;
    if (mThrown >= 5) {
        mState = 3;
        mStateDelay = 0x80;
        mRoundOver = 1;
        sum = 0;
        Sound::PlayBank2_2D(0x1bc);
        for (i = 0; i < 5; i++)
            sum += mPopup[i].points;
        Mg_SubmitScore(sum);
    }

    for (i = 0; i < 5; i++) {
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
/* The playing state. After the mStateDelay delay it shows the HUD once, runs
 * the aim state (mStylusHeld) through its table, then every active stone's own
 * state through the other table, remembering the last position of each.
 * When no stone is still moving, the popups are scored and the scene moves
 * to state 2 with a 0xc0 delay. */
void dScMgCurling_c::Scene_Play() {
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
        u8 *q = &mSwingTimer;
        *q = *q - 1;
    }
    (((C *)c)->*sCurlingAimTable[mStylusHeld])();

    {
        int count = 0;
        int i = 0;
        char *p = c;
        for (; i < 5; i++, p += 0x2c) {
            if (((dScMgCurling_c *)p)->mStone[0].active != 0) {
                ((dScMgCurling_c *)p)->mStone[0].lastX = ((dScMgCurling_c *)p)->mStone[0].x;
                ((dScMgCurling_c *)p)->mStone[0].lastY = ((dScMgCurling_c *)p)->mStone[0].y;
                if (((dScMgCurling_c *)p)->mStone[0].timer != 0) {
                    u16 *q = (u16 *)(p + CURL_OFF(mStone[0].timer));
                    *q = *q - 1;
                }
                (((C *)c)->*sCurlingStoneTable[*(u8 *)(p + CURL_OFF(mStone[0].state))])(i);
                if (*(u8 *)(p + CURL_OFF(mStone[0].state)) != 2)
                    count++;
            }
        }
        if (count != 0)
            return;
    }
    mState = 2;
    ScoreStones();
    mStateDelay = 0xc0;
}

// @symbol _ZN14dScMgCurling_c16Scene_StartRoundEv
/* Starts a round: clears the stones, brings out the first one and enters
 * the aiming state. */
void dScMgCurling_c::Scene_StartRound() {
    ClearStones();
    mNextStone = 1;
    (s16 &)mStoneDelay = 0;
    BringOutNextStone();
    mState = 1;
}

// @symbol _ZN14dScMgCurling_c9PickHouseEv
/* Picks the house (mHouse): a fresh random one of three that differs from
 * the last, or the first one when the slot still holds its 0xff reset.
 * The house centre comes from sCurlingHouseSpots and its background from
 * one of three files. */
void dScMgCurling_c::PickHouse() {
    int m;
    void *file;
    if (mHouse == 0xff) {
        mHouse = 0;
    } else {
        m = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;
        if (mHouse == m) {
            m += ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) + 1;
            if (m >= 3)
                m -= 3;
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
void dScMgCurling_c::ResetHouse() {
    mHouse = 255;
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN14dScMgCurling_c11ClearStonesEv
/* Zeroes the five stones, the five score popups and the aim fields, and
 * resets the score. */
void dScMgCurling_c::ClearStones() {
    int i;
    char *c = (char *)this;
    char *r = c;
    for (i = 0; i < 5; i++) {
        ((dScMgCurling_c *)r)->mStone[0].x = 0;
        ((dScMgCurling_c *)r)->mStone[0].y = 0;
        ((dScMgCurling_c *)r)->mStone[0].speed = 0;
        ((dScMgCurling_c *)r)->mStone[0].lastX = 0;
        ((dScMgCurling_c *)r)->mStone[0].lastY = 0;
        ((dScMgCurling_c *)r)->mStone[0].slideSnd = 0;
        ((dScMgCurling_c *)r)->mStone[0].spin = 0;
        ((dScMgCurling_c *)r)->mStone[0].heading = 0;
        ((dScMgCurling_c *)r)->mStone[0].angle = 0;
        ((dScMgCurling_c *)r)->mStone[0].state = 0;
        ((dScMgCurling_c *)r)->mStone[0].active = 0;
        ((dScMgCurling_c *)r)->mStone[0].dealt = 0;
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
    (s16 &)mStoneDelay = 0;
    mThrown = 0;
    mAimX = 0;
    mAimY = 0;
    mGrabX = 0;
    mGrabY = 0;
    mSwingSpeed = 0;
    (s16 &)mSwingAng = 0;
    mStylusHeld = 0;
    mAimShown = 0;
    mNextStone = 0;
    mRoundOver = 0;
    (s16 &)mStateDelay = 0;
    (s16 &)unk_4edc = 0;
    mSwingTimer = 0;
    Mg_SubmitScore(0);
}
#pragma pop

// @symbol _ZN14dScMgCurling_c10StartRoundEi
/* Slot 18, one of dScMgBase_c's own undeclared slots; the inherited label
 * is the tree-wide mislabel and this is not a destructor. Restarts the
 * scene: state 0, the stones cleared, a house picked, the blend set up
 * again and the base's handle refreshed. */
void dScMgCurling_c::StartRound(int /* arg */) {
    mState = 0;
    ClearStones();
    PickHouse();
    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    mTopRecord = Mg_GetTopRecord();
}

// @symbol _ZN14dScMgCurling_c6RenderEv
/* Slot 9. The base's frame, then the popups, the stones, the aim sprite
 * and the falling bits. */
s32 dScMgCurling_c::Render() {
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
s32 dScMgCurling_c::Behavior() {
    C *c = (C *)this;
    int j = mState;
    (c->*sCurlingSceneTable[j])();
    Bit_UpdateAll();
    return 1;
}

// @symbol _ZN14dScMgCurling_c13InitResourcesEv
/* Slot 0. Loads the board and the stone graphics for both screens, the
 * palettes and the house, then starts the first round with a 0x40 delay. */
s32 dScMgCurling_c::InitResources() {
    char *a = Ov004_LoadFileByPath(&sCurlingMgDirPath);
    char *b = Ov004_LoadFileByPath(&sCurlingBgCurling2NcgPath);
    void *f;

    if (a == 0 || b == 0)
        return 0;

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1220;
    DecompressLZ16(a, G2::GetBG2CharPtr());

    f = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NCL_BIN);
    GX::LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_MG_BG_CURLING1_NSC_BIN);
    func_020563d4(f, 0, 0x800);
    Deallocate(f);

    {
        void *c7 = LoadFile(ASSET_D_2D_MG_CURLING_NCG_BIN);
        void *c8 = LoadFile(ASSET_D_2D_MG_CURLING_NCL_BIN);
        DecompressLZ16(c7, (void *)0x6400000);
        GX::LoadOBJPltt(c8, 0, 0x100);

        data_0209d454 |= 4;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x814;
        DecompressLZ16(b, (void *)G2S::GetBG2CharPtr());

        f = LoadFile(ASSET_D_2D_MG_BG_CURLING2_NCL_BIN);
        GXS::LoadBGPltt(f, 0x60, 0x1a0);
        Deallocate(f);

        f = LoadFile(ASSET_D_2D_MG_BG_CURLING2_NSC_BIN);
        func_02056374(f, 0, 0x800);
        Deallocate(f);

        Ov004_Deallocate(a);
        Ov004_Deallocate(b);

        DecompressLZ16(c7, (void *)0x6600000);
        GXS::LoadOBJPltt(c8, 0, 0x100);
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
    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    mTopRecord = Mg_GetTopRecord();
    return 1;
}

/* Reconstructed source-style name: SM64DS proves the dScMgCurling_c RTTI
 * identity, MG_CURLING registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: MgShuffleShell_Spawn. */
// @symbol dScMgCurling_c_classInit
extern "C" void *dScMgCurling_c_classInit() {
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
