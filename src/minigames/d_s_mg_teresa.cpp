//cpp
/* Hide and Boo Seek. The player watches the Boo cross the room, the lights
 * go out, and they rub the Touch Screen to uncover every hiding Boo. The
 * first 15 rounds give three seconds; after that the clock is two. This
 * file is all 82 functions of dScMgTeresa_c's ov006 unit (.text
 * 0x0211cbd0..0x021207dc): the big Boo's flight, the countdown and the
 * reveal sprites, the 16 hiding Boos (spawn, flight, wall bounce, fade and
 * the touch test against the rubbed-out BG0 tiles), the scene states, and
 * the class's own overrides, and last the factory dScMgTeresa_c_classInit.
 * Below it is dScMgSound_c.
 *
 * _ZTS13dScMgTeresa_c 0x0213f9c0. _ZTI13dScMgTeresa_c 0x0213f9b4.
 * _ZTV13dScMgTeresa_c 0x0213fa0c.
 *
 * Nothing in the ROM names the helpers, so the members keep their
 * func_ov006_ names. InitResources, Behavior, Render and StartRound were
 * named from their vtable slots (see dScMgBase_c.h).
 *
 * Functions run in ROM order, lowest address first, because of
 * `#pragma defer_codegen off` below. Do not reorder, and do not drop that
 * pragma. It makes the out-of-line destructor come out D1, D0 as in the
 * ROM (the unused D2 trails it and is deadstripped), and it is what lets
 * the seven push and pop brackets bind. Each bracket was measured: without
 * it, its function stops matching. The destructor is out of line, so it is
 * the key function and this file emits the vtable and RTTI for the whole
 * base chain.
 *
 * The four PMF state tables dispatch on this class: the big Boo's flight
 * at sTeresaBigBooStateTable (9 states by BigBoo::state), the reveal pair at
 * sTeresaRevealStateTable (5 states by mReveal[0].state), the 16-Boo rows at
 * sTeresaBooStateTable (8 states), and the scene table sTeresaSceneStateTable
 * (5 no-argument states by mSceneState). __sinit_ov006_02132f68 fills them
 * from the .data descriptor records at data_ov006_0213f8xx.
 *
 * OAM::Render takes Fix12<int> by value, so that call stays mangled.
 */

// leftovers:
// - The 16 small Boos at 0x4660 are typed (dScMgTeresa_c::SmallBoo mBoos[16]).
//   Their handlers use mBoos[i] where that measured identical. StepBoo and
//   Boo_StateFadeAway spell most fields as `(char *)&mBoos[0].field + i * 0x24`
//   (field first, stride last, the cartridge's order); ResetRound,
//   CheckRevealDone, CheckRoundEnd and CountRubbedTiles walk a 0x24-stepped
//   pointer and read `((dScMgTeresa_c *)p)->mBoos[0].field`.
// - ResetRound clears 0x4bfc, 0x4c00 and 0x4c10 by offset: nothing in this TU
//   reads them, so they have no names. Reveal_Start clears mReveal's byte 0x1b
//   (unk_1b) by offset for the same reason.

#pragma defer_codegen off

#include "types.h"
#include "nitro/fx/fx.h"
#include "OamAttr.h"
#include "dScMgTeresa_c.h"
#include "decl_common.h"
#include "G2x.h"
#include "PlayerInput.h"
#include "G2.h"
#include "cstd.h"
#include "GX.h"

/* Scene_StateResult's view of the ov004 score block at gMgScene
   (dScMgBase_c.h declares it void *): the current score at +0xb4 and the best
   at +0xb8. */
struct ScoreView { unsigned char pad[0xb4]; int b4; int b8; };

/* Everything this file calls or reads that decl_common.h does not declare. */
extern "C" {

extern void  RenderOamMainScreen(void*, int, int, int, int);
extern void *LoadFile(int handle);
extern void  DecompressLZ16(const void *src, void *dst);
extern int   RandomIntInternal(int *seed);
extern void  FreeGfxSlotsById(int arg);
extern int   GetGameLanguage(void);
extern void  DrawOamSprite(int, int, int, int);
extern void  Hud_RenderSprite(void *fn, int a, int b, int c, int d);
extern void  Mg_DrawNumberRotated(int, int, int, int, int, int, int);
extern void  Mg_DrawScoreTally(void *, int, int, int);
extern int   Mg_IsNewRecord(void);
extern int   func_020126e8(int a);
extern int   func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void  MultiStore16(u16 val, void *dst, int nbytes);
struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};
extern void OamRender10_d_s_mg_teresa(int sub, struct OamAttr *data, s32 x, s32 y, s32 palette,
                                      s32 priority, s32 scaleX, s32 scaleY, s32 rotation, s32 mode);

extern unsigned char  data_0209d454;
extern unsigned char  data_0209d45c;
extern int sTeresaBigBooSprites[];
extern unsigned char sTeresaBigBooHoverPicks[];
extern unsigned short sTeresaBigBooFrameTimes[];
extern unsigned char sTeresaRevealSlideFrames[];
extern unsigned char sTeresaRevealAnimFrames[];
extern unsigned short sTeresaBg3FlashTimes[];
extern void *sTeresaExclaimSprites;
extern void *sTeresaVanishPuffSprites[];
extern int sTeresaBigBooHoverSprites[];
extern int sMgDigitSprites[];
extern void *sTeresaMarkerSprites;
extern struct OamAttr *sTeresaRevealSprites[];
extern int *sTeresaCountdownSprites[];
extern void *sTeresaBooFrameSprites[];
extern int sTeresaBooTouchOffsets[];
extern int sTeresaSpawnCounts[];
extern int sTeresaSpawnKinds[];
extern int sTeresaSpawnBurst[];
extern int sTeresaPaintMask[];
extern short          data_02082214[];
extern int            sMgTopScreenYOffset;

/* The four pointer-to-member state tables. Pmf handlers take the Boo/slot
   index; the scene table in sTeresaSceneStateTable takes none. */
typedef void (dScMgTeresa_c::*Pmf)(int);
typedef void (dScMgTeresa_c::*Pmf0)();
struct PmfEntry { Pmf0 pmf; };

extern Pmf sTeresaBigBooStateTable[];     /* big Boo flight states, by BigBoo::state */
extern Pmf sTeresaRevealStateTable[];     /* reveal states, by mReveal[0].state */
extern Pmf sTeresaBooStateTable[];        /* per-Boo states, one call per live row */
extern PmfEntry sTeresaSceneStateTable[]; /* scene states, by mSceneState */
/* The ROM PMF constants the four state tables copy from (unlicensed .data),
 * grouped by destination table in retail copy order. */
extern Pmf0 pmf_dScMgTeresa_c_Scene_StateInit;
extern Pmf0 pmf_dScMgTeresa_c_Scene_StateReveal;
extern Pmf0 pmf_dScMgTeresa_c_Scene_StatePlay;
extern Pmf0 pmf_dScMgTeresa_c_Scene_StateRoundEnd;
extern Pmf0 pmf_dScMgTeresa_c_Scene_StateResult;
extern Pmf pmf_dScMgTeresa_c_Boo_StateAppear;
extern Pmf pmf_dScMgTeresa_c_Boo_StateFly;
extern Pmf pmf_dScMgTeresa_c_Boo_StateFadeOut;
extern Pmf pmf_dScMgTeresa_c_Boo_StateHidden;
extern Pmf pmf_dScMgTeresa_c_Boo_StateHiddenTimed;
extern Pmf pmf_dScMgTeresa_c_Boo_StateFound;
extern Pmf pmf_dScMgTeresa_c_Boo_StateFadeIn;
extern Pmf pmf_dScMgTeresa_c_Boo_StateFadeAway;
extern Pmf pmf_dScMgTeresa_c_Reveal_StateStart;
extern Pmf pmf_dScMgTeresa_c_Reveal_StateSlideIn;
extern Pmf pmf_dScMgTeresa_c_Reveal_StateAnimate;
extern Pmf pmf_dScMgTeresa_c_Reveal_StateDipAndRise;
extern Pmf pmf_dScMgTeresa_c_Reveal_StateFinish;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateDropIn;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateHover;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateFlyLeft;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateFlyRight;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateOffscreen;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateExit;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateRemove;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateGone;
extern Pmf pmf_dScMgTeresa_c_BigBoo_StateResult;

}  /* extern "C" */

namespace Sound { void PlayBank2_2D(unsigned int id); }


// @symbol _ZN13dScMgTeresa_cD1Ev
// @symbol _ZN13dScMgTeresa_cD0Ev
/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. */
dScMgTeresa_c::~dScMgTeresa_c()
{
}


// @symbol _ZN13dScMgTeresa_c17BigBoo_ShowResultEv
void dScMgTeresa_c::BigBoo_ShowResult() {
    if (mRoundCleared) {
        mBigBoo[0].frame = 3;
    } else {
        mBigBoo[0].frame = 4;
    }
    mBigBoo[0].x = 0x80000;
    mBigBoo[0].y = 0x100000;
    mBigBoo[0].yVel = -0x1800;
    mBigBoo[0].frameTimer = 0;
    mBigBoo[0].moveRight = 0;
    mBigBoo[0].state = 8;
    mBigBoo[0].shown = 1;
    mCountdownShown = 0;
}

// @symbol _ZN13dScMgTeresa_c14BigBoo_SetDoneEv
void dScMgTeresa_c::BigBoo_SetDone() {
    if (mBigBoo[0].active != 0) {
        mBigBoo[0].done = 1;
    }
}

// @symbol _ZN13dScMgTeresa_c11BigBoo_DrawEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::BigBoo_Draw() {
    int idx, v, p, q;
    if (mBigBoo[0].shown == 0)
        return;
    idx = mBigBoo[0].frame;
    p = mBigBoo[0].x >> 12;
    q = mBigBoo[0].y >> 12;
    if (mBigBoo[0].moveRight != 0)
        idx += 4;
    if (mBigBoo[0].state == 8)
        v = sTeresaBigBooHoverSprites[idx];
    else
        v = sTeresaBigBooSprites[idx];
    RenderOamMainScreen((void *)v, p, q, -1, -1);
}

// @symbol _ZN13dScMgTeresa_c18BigBoo_StateResultEi
void dScMgTeresa_c::BigBoo_StateResult(int idx) {

    if ((mBigBoo[idx].y >> 12) == 0xc0) {
        mBigBoo[idx].frameTimer++;
        if (mRoundCleared != 0) {
            if (mBigBoo[idx].frame >= 3)
                return;
            if (mBigBoo[idx].frameTimer < 4)
                return;
            mBigBoo[idx].frameTimer = 0;
            mBigBoo[idx].frame++;
        } else {
            if (mBigBoo[idx].frame >= 6)
                return;
            if (mBigBoo[idx].frameTimer < 8)
                return;
            mBigBoo[idx].frameTimer = 0;
            mBigBoo[idx].frame++;
        }
    } else {
        mBigBoo[idx].shown = 1;
        mBigBoo[idx].y += mBigBoo[idx].yVel;
        mBigBoo[idx].yVel -= 0x200;
        if ((mBigBoo[idx].y >> 12) > 0xc0)
            return;
        mBigBoo[idx].y = 0xc0000;
        mBigBoo[idx].yVel = 0;
        mBigBoo[idx].frameTimer = 0;
        if (mRoundCleared != 0)
            mBigBoo[idx].frame = 0;
    }
}

// @symbol _ZN13dScMgTeresa_c16BigBoo_StateGoneEv
void dScMgTeresa_c::BigBoo_StateGone() {}

// @symbol _ZN13dScMgTeresa_c18BigBoo_StateRemoveEi
void dScMgTeresa_c::BigBoo_StateRemove(int index) {
    void *data;
    mBigBoo[index].xVel = 0;
    mBigBoo[index].yVel = 0;
    mBigBoo[index].shown = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, (void *)0x6400000);
    mBigBoo[index].state = 7;
    Deallocate(data);
}

// @symbol _ZN13dScMgTeresa_c16BigBoo_StateExitEi
void dScMgTeresa_c::BigBoo_StateExit(int i) {
    unsigned short *p16 = &mBigBoo[i].frameTimer;
    int *sum;
    int *add;
    int v;
    *p16 = *p16 + 1;
    if (*p16 >= 8) {
        unsigned char *p8;
        int z = 0;
        *p16 = z;
        p8 = &mBigBoo[i].frame;
        *p8 = *p8 + 1;
        if (*p8 >= 4) *p8 = z;
    }
    add = &mBigBoo[i].xVel;
    sum = &mBigBoo[i].x;
    *sum = *sum + *add;
    v = *sum >> 12;
    if (mBigBoo[i].moveRight != 0) {
        if (v >= 0x120) {
            mBigBoo[i].state = 6;
            return;
        }
        if (*add <= 0x3000) *add = *add + 0x200;
    } else {
        if (v <= -0x20) {
            mBigBoo[i].state = 6;
            return;
        }
        if (*add >= -0x3000) *add = *add - 0x200;
    }
}

// @symbol _ZN13dScMgTeresa_c21BigBoo_StateOffscreenEi
void dScMgTeresa_c::BigBoo_StateOffscreen(int idx) {
    if (mBigBoo[idx].done != 0) {
        mBigBoo[idx].state = 6;
        return;
    }
    if (mBigBoo[idx].wait != 0) {
        mBigBoo[idx].wait--;
        return;
    }
    mBigBoo[idx].shown = 1;
    if ((mBigBoo[idx].x >> 0xc) < 0) {
        mBigBoo[idx].moveRight = 1;
        mBigBoo[idx].x = -0x80000;
        mBigBoo[idx].xVel = 0x4800;
        mBigBoo[idx].state = 3;
        return;
    }
    mBigBoo[idx].moveRight = 0;
    mBigBoo[idx].x = 0x180000;
    mBigBoo[idx].xVel = -0x4800;
    mBigBoo[idx].state = 2;
}

// @symbol _ZN13dScMgTeresa_c20BigBoo_StateFlyRightEi
void dScMgTeresa_c::BigBoo_StateFlyRight(int i) {
    int v;
    mBigBoo[i].frameTimer++;
    if (mBigBoo[i].frameTimer >= 4) {
        mBigBoo[i].frameTimer = 0;
        mBigBoo[i].frame++;
        if (mBigBoo[i].frame >= 4)
            mBigBoo[i].frame = 0;
    }
    mBigBoo[i].x += mBigBoo[i].xVel;
    v = mBigBoo[i].x >> 0xc;
    if (v >= 0xc0) {
        if (mBigBoo[i].xVel >= 0x800)
            mBigBoo[i].xVel -= 0x80;
    } else {
        if (mBigBoo[i].xVel <= 0x7000)
            mBigBoo[i].xVel += 0x400;
    }
    if (v >= 0x120) {
        mBigBoo[i].xVel = 0;
        mBigBoo[i].x = 0x120000;
        mBigBoo[i].state = 4;
        mBigBoo[i].wait = 8;
        mBigBoo[i].shown = 0;
        if (mBigBoo[i].done != 0)
            mBigBoo[i].state = 6;
    }
}

// @symbol _ZN13dScMgTeresa_c19BigBoo_StateFlyLeftEi
void dScMgTeresa_c::BigBoo_StateFlyLeft(int i) {
    int t;

    mBigBoo[i].frameTimer++;
    if (mBigBoo[i].frameTimer >= 4) {
        mBigBoo[i].frameTimer = 0;
        mBigBoo[i].frame++;
        if (mBigBoo[i].frame >= 4) {
            mBigBoo[i].frame = 0;
        }
    }

    mBigBoo[i].x = mBigBoo[i].x + mBigBoo[i].xVel;
    t = mBigBoo[i].x >> 0xc;
    if (t <= 0x40) {
        if (mBigBoo[i].xVel <= -0x800) {
            mBigBoo[i].xVel = mBigBoo[i].xVel + 0x80;
        }
    } else {
        if (mBigBoo[i].xVel >= -0x7000) {
            mBigBoo[i].xVel = mBigBoo[i].xVel - 0x400;
        }
    }
    if (t > -0x20) {
        return;
    }
    mBigBoo[i].x = -0x20000;
    mBigBoo[i].xVel = 0;
    mBigBoo[i].state = 4;
    mBigBoo[i].wait = 8;
    mBigBoo[i].shown = 0;
    if (mBigBoo[i].done != 0) {
        mBigBoo[i].state = 6;
    }
}

// @symbol _ZN13dScMgTeresa_c17BigBoo_StateHoverEi
void dScMgTeresa_c::BigBoo_StateHover(int i) {
    unsigned char state = mBigBoo[i].frame;
    if ((unsigned char)(state - 8) < 2)
    {
        mBigBoo[i].frameTimer++;
        if (mBigBoo[i].frameTimer >= sTeresaBigBooFrameTimes[(unsigned char)(state - 8)]) {
            mBigBoo[i].frameTimer = 0;
            mBigBoo[i].frame++;
        }
    }
    if (mBigBoo[i].wait != 0) {
        mBigBoo[i].wait--;
        return;
    }
    data_0209d45c &= ~4;
    SetBg3Offset(0, 0x100);
    {
    int rnd = RandomIntInternal(&data_0209d4b8);
    mBigBoo[i].state =
        sTeresaBigBooHoverPicks[((unsigned int)(((unsigned int)rnd >> 16) & 0x7fff) * 2) >> 15];
    }
    mBigBoo[i].frameTimer = 0;
    mBigBoo[i].frame = 0;
    if (mBigBoo[i].state == 2) {
        mBigBoo[i].moveRight = 0;
        mBigBoo[i].xVel = -0x4800;
        mBigBoo[i].x = 0x180000;
    } else {
        mBigBoo[i].moveRight = 1;
        mBigBoo[i].xVel = 0x4800;
        mBigBoo[i].x = -0x80000;
    }
    if (mBigBoo[i].done != 0) {
        mBigBoo[i].state = 5;
    }
}

// @symbol _ZN13dScMgTeresa_c18BigBoo_StateDropInEi
void dScMgTeresa_c::BigBoo_StateDropIn(int i) {
    mBigBoo[i].shown = 1;
    mBigBoo[i].y += mBigBoo[i].yVel;
    mBigBoo[i].yVel -= 0x100;
    if (mBigBoo[i].y >> 0xc > 0xc0)
        return;
    mBigBoo[i].y = 0xc0000;
    mBigBoo[i].yVel = 0;
    mBigBoo[i].state = 1;
    mBigBoo[i].wait = 0x30;
    Mg_ShowBanner(0xc, 0x80, 0x40, 1, -1, 0xd);
}

// @symbol _ZN13dScMgTeresa_c13BigBoo_UpdateEv
void dScMgTeresa_c::BigBoo_Update()
{
    if (mBigBoo[0].active == 0)
        return;
    (this->*sTeresaBigBooStateTable[mBigBoo[0].state])(0);
}


// @symbol _ZN13dScMgTeresa_c12BigBoo_StartEv
void dScMgTeresa_c::BigBoo_Start()
{
    mBigBoo[0].active = 1;
    mBigBoo[0].x = 0x80000;
    mBigBoo[0].y = 0x100000;
    mBigBoo[0].xVel = 0;
    mBigBoo[0].yVel = -0x1800;
    mBigBoo[0].frameTimer = 0;
    mBigBoo[0].wait = 0;
    mBigBoo[0].state = 0;
    mBigBoo[0].frame = 8;
    mBigBoo[0].moveRight = 0;
    mBigBoo[0].done = 0;
    G2x::SetBlendAlpha((volatile unsigned short *)0x4000050, 0, 0xc, 0xc, 0x10);
}


// @symbol _ZN13dScMgTeresa_c11BigBoo_HideEv
void dScMgTeresa_c::BigBoo_Hide()
{
    mBigBoo[0].active = 0;
    mBigBoo[0].shown = 0;
}


// @symbol _ZN13dScMgTeresa_c13StepBg3ScrollEv
void dScMgTeresa_c::StepBg3Scroll() {
    if (mBg3FlashStep >= 8) {
        return;
    }
    mBg3FlashTick++;
    if (mBg3FlashTick >= sTeresaBg3FlashTimes[mBg3FlashStep]) {
        mBg3FlashStep++;
        mBg3FlashTick = 0;
        if (mBg3FlashStep & 1) {
            SetBg3Offset(0, 0);
        } else {
            SetBg3Offset(0, 0x100);
        }
    }
    if (mBg3FlashStep >= 8) {
        SetBg3Offset(0, 0);
    }
}

// @symbol _ZN13dScMgTeresa_c11Marker_DrawEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Marker_Draw()
{
  if(mMarker.visible==0) return;
  RenderOamMainScreen(sTeresaMarkerSprites, mMarker.x >> 0xc, mMarker.y >> 0xc, -1, -1);
}


// @symbol _ZN13dScMgTeresa_c13Marker_UpdateEv
/* Empty. Scene_StateReveal calls it with the object. */
void dScMgTeresa_c::Marker_Update()
{
}


// @symbol _ZN13dScMgTeresa_c11Marker_ShowEv
void dScMgTeresa_c::Marker_Show()
{
    mMarker.x = 0x50000;
    mMarker.y = 0x40000;
    mMarker.active = 1;
    mMarker.visible = 1;
}


// @symbol _ZN13dScMgTeresa_c11Marker_HideEv
void dScMgTeresa_c::Marker_Hide()
{
    mMarker.active = 0;
    mMarker.visible = 0;
}


// @symbol _ZN13dScMgTeresa_c11Reveal_DrawEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Reveal_Draw()
{
    if (mReveal[0].visible == 0) return;
    OAM::Render(0, sTeresaRevealSprites[mReveal[0].sprite], mReveal[0].x >> 12, mReveal[0].y >> 12,
                -1, 0, 0x1000, 0x1000, mReveal[0].angle, -1);
}

// @symbol _ZN13dScMgTeresa_c18Reveal_StateFinishEi
void dScMgTeresa_c::Reveal_StateFinish(int idx) {
    if (mReveal[idx].timer != 0) {
        mReveal[idx].timer -= 1;
        return;
    }
    mReveal[idx].visible = 0;
    mReveal[idx].active = 0;
    Marker_Hide();
    data_0209d45c |= 4;
    data_0209d454 |= 1;
    Sound::PlayBank2_2D(0x1f6);
    mSceneState = 2;
    StartBoosHiding();
    BigBoo_Start();
    Sound::PlayBank2_2D(0x1f5);
}

#pragma push
#pragma opt_common_subs off
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
// @symbol _ZN13dScMgTeresa_c22Reveal_StateDipAndRiseEi
void dScMgTeresa_c::Reveal_StateDipAndRise(int i) {
    u8 flag;

    flag = mReveal[i].phase;

    if (flag == 0) {
        mReveal[i].timer += 1;
        mReveal[i].y += mReveal[i].vy;
        mMarker.y += mReveal[i].vy;
        if (mReveal[i].vy >= 0x800)
            mReveal[i].vy -= 0x20;
        if (mReveal[i].timer >= 0x10) {
            mReveal[i].timer = 0;
            mReveal[i].phase += 1;
            mReveal[i].vy = 0;
        }
    } else if (flag == 1) {
        mReveal[i].timer += 1;
        if (mReveal[i].timer == 0x10)
            mReveal[i].vy = -0x1000;
        mReveal[i].y += mReveal[i].vy;
        mMarker.y += mReveal[i].vy;
        if (mReveal[i].vy <= -0x800)
            mReveal[i].vy += 0x20;
        if (mReveal[i].timer >= 0x20) {
            mReveal[i].timer = 0x20;
            mReveal[i].phase = 0;
            mReveal[i].state = 4;
        }
    }
}
#undef AT
#pragma pop

// @symbol _ZN13dScMgTeresa_c19Reveal_StateAnimateEi
void dScMgTeresa_c::Reveal_StateAnimate(int i) {
    mReveal[i].frameTimer++;
    if (mReveal[i].frameTimer < 8) return;
    mReveal[i].frameTimer = 0;
    mReveal[i].frame++;
    if (mReveal[i].frame >= 8) {
        mReveal[i].frame = 0;
        mReveal[i].sprite = 0;
        mReveal[i].state = 3;
        mReveal[i].timer = 0;
        mReveal[i].vy = 0x1000;
        Sound::PlayBank2_2D(0x1f9);
    } else {
        mReveal[i].sprite = sTeresaRevealAnimFrames[mReveal[i].frame];
    }
}

#pragma push
#pragma opt_common_subs off
// @symbol _ZN13dScMgTeresa_c19Reveal_StateSlideInEi
void dScMgTeresa_c::Reveal_StateSlideIn(int i) {
    mReveal[i].frameTimer += 1;
    if (mReveal[i].frameTimer >= 8) {
        mReveal[i].frameTimer = 0;
        mReveal[i].frame += 1;
        if (mReveal[i].frame >= 8)
            mReveal[i].frame = 0;
        mReveal[i].sprite = sTeresaRevealSlideFrames[mReveal[i].frame];
    }
    if (mReveal[i].x >> 12 <= 0x80) {
        mReveal[i].x = 0x80000;
    } else {
        mReveal[i].x += mReveal[i].vx;
        if (mReveal[i].vx <= -0x600)
            mReveal[i].vx += 0x20;
    }
    if (mReveal[i].timer != 0) {
        mReveal[i].timer -= 1;
        return;
    }
    Sound::PlayBank2_2D(0x1f3);
    mReveal[i].state = 2;
    mReveal[i].frame = 0;
    mReveal[i].sprite = 0;
    mReveal[i].frameTimer = 0;
    mReveal[i].vx = 0;
}
#pragma pop

// @symbol _ZN13dScMgTeresa_c17Reveal_StateStartEi
void dScMgTeresa_c::Reveal_StateStart(int i) {
    mReveal[i].state = 1;
    mReveal[i].timer = 0x40;
    mReveal[i].vx = -0xb00;
}

// @symbol _ZN13dScMgTeresa_c13Reveal_UpdateEv
void dScMgTeresa_c::Reveal_Update()
{
    if (mReveal[0].active == 0) return;
    (this->*sTeresaRevealStateTable[mReveal[0].state])(0);
}


// @symbol _ZN13dScMgTeresa_c12Reveal_StartEv
void dScMgTeresa_c::Reveal_Start()
{
    char *raw = (char *)this;
    mReveal[0].x = 0xa0000;
    mReveal[0].y = 0x60000;
    mReveal[0].active = 1;
    mReveal[0].visible = 1;
    mReveal[0].state = 0;
    mReveal[0].sprite = 0;
    mReveal[0].angle = 0;
    mReveal[0].frameTimer = 0;
    mReveal[0].timer = 0;
    /* mReveal[0].unk_1b: written only here, read nowhere in this TU */
    *(char *)(raw + 0x4bbb) = 0;
    mReveal[0].frame = 0;
    mReveal[0].phase = 0;
}


// @symbol _ZN13dScMgTeresa_c11Reveal_HideEv
void dScMgTeresa_c::Reveal_Hide()
{
    mReveal[0].visible = 0;
    mReveal[0].active = 0;
}

// @symbol _ZN13dScMgTeresa_c19FoundNumber_DrawAllEv
/* decl_common.h declares this one with a `void *` parameter, and
   Mg_RenderSubSpriteMtx as `(void *, int, int, void *)`. */
void dScMgTeresa_c::FoundNumber_DrawAll() {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mFoundNumbers[i].visible) {
            int b = mFoundNumbers[i].y >> 12;
            int a = mFoundNumbers[i].x >> 12;
            if (b <= 8) b = 8;
            if (b >= 0xb8) b = 0xb8;
            Mg_RenderSubSpriteMtx((void *)sMgDigitSprites[mFoundNumbers[i].sprite + 1], a, b,
                                  (void *)0);
        }
    }
}

// @symbol _ZN13dScMgTeresa_c20FoundNumber_ClearAllEv
void dScMgTeresa_c::FoundNumber_ClearAll() {
    int i;
    for (i = 0; i < 0x10; i++) {
        mFoundNumbers[i].active = 0;
        mFoundNumbers[i].visible = 0;
    }
}

// @symbol _ZN13dScMgTeresa_c21FoundNumber_CountBusyEv
int dScMgTeresa_c::FoundNumber_CountBusy() {
    int cnt = 0;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mFoundNumbers[i].active != 0 && mFoundNumbers[i].state != 2) {
            cnt++;
        }
    }
    return cnt != 0;
}

// @symbol _ZN13dScMgTeresa_c21FoundNumber_UpdateAllEv
void dScMgTeresa_c::FoundNumber_UpdateAll() {
    int i;
    for (i = 0; i < 16; i++) {
        unsigned char v;
        if (mFoundNumbers[i].active == 0) {
            continue;
        }
        v = mFoundNumbers[i].state;
        if (v == 0) {
            if (mFoundNumbers[i].timer != 0) {
                mFoundNumbers[i].timer -= 1;
                if ((short)mFoundNumbers[i].timer < 0) {
                    mFoundNumbers[i].timer = 0;
                }
            } else {
                Sound::PlayBank2_2D(0x1bc);
                mFoundNumbers[i].vy = -0x400;
                mFoundNumbers[i].timer = 0x10;
                mFoundNumbers[i].state += 1;
                mFoundNumbers[i].visible = 1;
            }
        } else if (v == 1) {
            mFoundNumbers[i].y += mFoundNumbers[i].vy;
            mFoundNumbers[i].vy -= 0x80;
            if (mFoundNumbers[i].timer != 0) {
                mFoundNumbers[i].timer -= 1;
                if ((short)mFoundNumbers[i].timer < 0) {
                    mFoundNumbers[i].timer = 0;
                }
            } else {
                mFoundNumbers[i].timer = 0x18;
                mFoundNumbers[i].state += 1;
            }
        }
    }
}

// @symbol _ZN13dScMgTeresa_c17FoundNumber_SpawnEi
void dScMgTeresa_c::FoundNumber_Spawn(int i) {
    int n;
    for (n = 0; n < 0x10; n++) {
        if (mFoundNumbers[n].active == 0) {
            mFoundNumbers[n].active = 1;
            mFoundNumbers[n].visible = 0;
            mFoundNumbers[n].x = mBoos[i].mX;
            mFoundNumbers[n].y = mBoos[i].mY - 0x10000;
            mFoundNumbers[n].timer = (mBoos[i].mFoundOrder * 0x3c) + 0x10;
            mFoundNumbers[n].state = 0;
            mFoundNumbers[n].sprite = mBoos[i].mFoundOrder;
            return;
        }
    }
}

// @symbol _ZN13dScMgTeresa_c25FoundNumber_SpawnForFoundEv
void dScMgTeresa_c::FoundNumber_SpawnForFound() {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState == 5)
            FoundNumber_Spawn(i);
    }
}

// @symbol _ZN13dScMgTeresa_c18VanishPuff_DrawAllEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::VanishPuff_DrawAll() {
    int i;
    for (i = 0; i < 16; i++) {
        if (mVanishPuffs[i].visible) {
            Hud_RenderSprite(sTeresaVanishPuffSprites[mVanishPuffs[i].frame],
                             mVanishPuffs[i].x >> 12, mVanishPuffs[i].y >> 12, -1, -1);
        }
    }
}

// @symbol _ZN13dScMgTeresa_c20VanishPuff_UpdateAllEv
void dScMgTeresa_c::VanishPuff_UpdateAll() {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mVanishPuffs[i].active == 0)
            continue;
        mVanishPuffs[i].timer += 1;
        if (mVanishPuffs[i].timer < 4)
            continue;
        mVanishPuffs[i].timer = 0;
        mVanishPuffs[i].frame += 1;
        if (mVanishPuffs[i].frame >= 5) {
            mVanishPuffs[i].frame = 0;
            mVanishPuffs[i].visible = 0;
            mVanishPuffs[i].active = 0;
        }
    }
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN13dScMgTeresa_c16VanishPuff_SpawnEi
void dScMgTeresa_c::VanishPuff_Spawn(int param) {
    int i;
    for (i = 0; i < 16; i++) {
        if (mVanishPuffs[i].active == 0) {
            mVanishPuffs[i].x = mBoos[param].mX;
            mVanishPuffs[i].y = mBoos[param].mY;
            mVanishPuffs[i].timer = 0;
            mVanishPuffs[i].frame = 0;
            mVanishPuffs[i].active = 1;
            mVanishPuffs[i].visible = 1;
            return;
        }
    }
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c13DrawCountdownEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::DrawCountdown()
{
    if (mCountdownShown == 0)
        return;
    Mg_DrawNumberRotated(0x80, 0x60, mTimeLeft, 1, 0, 0x800, 0);
    DrawOamSprite(sTeresaCountdownSprites[GetGameLanguage()][3], 0x80, 0x48, 0);
}


// @symbol _ZN13dScMgTeresa_c15UpdateCountdownEv
void dScMgTeresa_c::UpdateCountdown()
{
    if (mCountdownRunning == 0)
        return;
    if (mTimeLeft == 0)
        return;
    mCountdownFrames += 1;
    if (mCountdownFrames >= 0x3c) {
        mCountdownFrames = 0;
        mTimeLeft -= 1;
        if ((s16)mTimeLeft < 0)
            mTimeLeft = 0;
        if (mTimeLeft != 0)
            Sound::PlayBank2_2D(0xa7);
        else
            Sound::PlayBank2_2D(0xa6);
    }
    if (mTimeLeft == 0) {
        mTimeLeft = 0;
        mCountdownFrames = 0;
    }
}


// @symbol _ZN13dScMgTeresa_c14StartCountdownEv
void dScMgTeresa_c::StartCountdown() {
    if (mRevealDone == 0)
        return;
    if (mCountdownRunning != 0)
        return;
    if (mLevel >= 0xf)
        mTimeLeft = 2;
    else
        mTimeLeft = 3;
    mCountdownFrames = 0;
    mCountdownShown = 1;
    mCountdownRunning = 1;
    Sound::PlayBank2_2D(0xa7);
}

// @symbol _ZN13dScMgTeresa_c15Exclaim_DrawAllEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Exclaim_DrawAll()
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mExclaims[i].visible != 0) {
            int v0 = mExclaims[i].x >> 12;
            int v4 = mExclaims[i].y >> 12;
            if (v4 <= 8) v4 = 8;
            if (v4 >= 0xb8) v4 = 0xb8;
            Hud_RenderSprite(sTeresaExclaimSprites, v0, v4, -1, -1);
        }
    }
}


// @symbol _ZN13dScMgTeresa_c17Exclaim_UpdateAllEv
void dScMgTeresa_c::Exclaim_UpdateAll()
{
    int i;

    for (i = 0; i < 0x10; i++) {
        if (mExclaims[i].active != 0) {
            if (mExclaims[i].timer != 0) {
                u8 *p = &mExclaims[i].timer;
                int v;
                *p -= 1;
                v = mExclaims[i].timer;
                if (v < 0)
                    mExclaims[i].timer = 0;
            } else {
                mExclaims[i].active = 0;
                mExclaims[i].visible = 0;
            }
        }
    }
}


// @symbol _ZN13dScMgTeresa_c13Exclaim_SpawnEi
void dScMgTeresa_c::Exclaim_Spawn(int idx)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mExclaims[i].active == 0) {
            mExclaims[i].active = 1;
            mExclaims[i].visible = 1;
            mExclaims[i].x = mBoos[idx].mX;
            mExclaims[i].y = mBoos[idx].mY - 0x18000;
            mExclaims[i].timer = 0x60;
            return;
        }
    }
}


// @symbol _ZN13dScMgTeresa_c15CheckRevealDoneEv
void dScMgTeresa_c::CheckRevealDone()
{
    char *raw = (char *)this;
    int found;
    int i;
    char* p;
    if (mRevealDone != 0)
        return;
    found = 0;
    for (i = 0, p = raw; i < 0x10; i++, p += 0x24) {
        if (((dScMgTeresa_c *)p)->mBoos[0].mInPlay != 0) {
            if (((dScMgTeresa_c *)p)->mBoos[0].mState <= 2) {
                found++;
                break;
            }
        }
    }
    if (found != 0)
        return;
    FreeGfxSlotsById(0xc);
    mRevealDone++;
}


// @symbol _ZN13dScMgTeresa_c13CheckRoundEndEv
void dScMgTeresa_c::CheckRoundEnd()
{
    unsigned char *raw = (unsigned char *)this;
    if (mTimeLeft == 0 && mRevealDone != 0) {
        mRoundCleared = 0;
        mSceneState = 3;
        mSceneTimer = 0x60;
        BigBoo_SetDone();
        mRubLocked = 1;
        return;
    }
    {
        int count = 0;
        int i = 0;
        unsigned char* p = raw;
        do {
            if (((dScMgTeresa_c *)p)->mBoos[0].mInPlay != 0) {
                if (((dScMgTeresa_c *)p)->mBoos[0].mFound == 0) {
                    count++;
                    break;
                }
            }
            i++;
            p += 0x24;
        } while (i < 0x10);
        if (count != 0)
            return;
        mSceneState = 3;
        mSceneTimer = 0x60;
        BigBoo_SetDone();
        mRoundCleared = 1;
    }
}


// @symbol _ZN13dScMgTeresa_c8DrawBoosEv
/* Draws the 16 Boo sprites. Each 0x24-byte slot at +0x4660 holds a position
 * (x, y in 20.12), a translucency flag, a visible flag, an OAM priority and an
 * animation/frame pair that picks the sprite's attribute block from a table of
 * seven frames per animation. Visible slots go through OAM::Render at unit
 * scale, forced semi-transparent (mode 1) when the flag is set and in the
 * attribute's own mode (-1) otherwise. The mode is declared between the frame
 * index and the y read on purpose: that is what puts the -1 where the ROM
 * keeps it. decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::DrawBoos()
{
    int i;

    for (i = 0; i < 16; i++) {
        if (mBoos[i].mVisible != 0) {
            void **frames = sTeresaBooFrameSprites;
            int translucent = mBoos[i].mFade;
            int x = mBoos[i].mX >> 12;
            int frame = mBoos[i].mFrame;
            int anim = mBoos[i].mAnim;
            int idx = anim * 7 + frame;
            int mode = -1;
            int y = mBoos[i].mY >> 12;
            int priority = mBoos[i].mPriority;

            if (translucent != 0)
                mode = 1;
            OAM::Render(1, (struct OamAttr *)frames[idx], x, y, -1, priority, 0x1000, 0x1000, 0,
                        mode);
        }
    }
}


// @symbol _ZN13dScMgTeresa_c14AreBoosSettledEv
void dScMgTeresa_c::AreBoosSettled()
{
    int found = 0;
    int i = 0;
    for (; i < 0x10; i++) {
        if (mBoos[i].mInPlay != 0) {
            unsigned int t = (mBoos[i].mState + 0xfd) & 0xff;
            if (t <= 1) {
                mBoos[i].mState = 6;
                mBoos[i].mTimer = 0;
                mBoos[i].mFade = 0;
                mBoos[i].mSettled = 1;
                mBoos[i].mFrame = 3;
                mBoos[i].mPlaySettleSound = 0;
                mLastSettledBoo = (unsigned char)i;
                mBoos[i].mDelay = 1;
                found++;
            }
        }
    }
    if (found != 0) {
        unsigned char idx = mLastSettledBoo;
        mBoos[idx].mPlaySettleSound = 1;
    } else {
        unsigned char idx = mLastSettledBoo;
        mBoos[idx].mPlaySettleSound = 0;
    }
}


#define A(a) (*(u8*)(a))
// @symbol _ZN13dScMgTeresa_c12TouchTestBooEi
void dScMgTeresa_c::TouchTestBoo(int idx)
{
    int x;
    int y;
    int count;
    int firstOk;
    int i;
    int* r;
    int word;

    if (mBoos[idx].mFound != 0)
        return;

    count = 0;
    firstOk = 0;
    for (i = 0; i < 5; i++) {
        x = (mBoos[idx].mX >> 12) - sTeresaBooTouchOffsets[i * 2];
        y = (mBoos[idx].mY >> 12) - sTeresaBooTouchOffsets[i * 2 + 1];
        r = ((int(*)[8])(char *)G2S::GetBG0CharPtr())[(x >> 3) + ((y >> 3) << 5)];
        word = r[y & 7];
        if (((word >> ((x & 7) << 2)) & 0xf) == 0) {
            count++;
            if (i == 0)
                firstOk++;
        }
    }

    if (count < 2)
        return;
    if (firstOk == 0)
        return;

    mBoos[idx].mFound = 1;
    mBoos[idx].mPriority = 0;
    mBoos[idx].mState = 5;
    mBoos[idx].mFoundOrder = mFoundCount;
    mBoos[idx].mTimer = 0x40;
    mFoundCount++;
    Exclaim_Spawn(idx);
    func_02012718(0x1f0, mBoos[idx].mX);
    if (mFirstFoundBoo == 0xff)
        mFirstFoundBoo = (u8)idx;
}
#undef A


#pragma push
#pragma opt_common_subs off
// @symbol _ZN13dScMgTeresa_c16CountRubbedTilesEi
void dScMgTeresa_c::CountRubbedTiles(int idx)
{
    char *self = (char *)this;
    int gx, gy;
    int cnt;
    int y;
    int x;
    int *pA;
    int *pB;
    char *tile;
    int tidx;
    int *row;
    int word;
    int off;

    off = idx * 0x24;
    cnt = 0;
    pA = &((dScMgTeresa_c *)(self + off))->mBoos[0].mX;
    pB = &((dScMgTeresa_c *)(self + off))->mBoos[0].mY;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            gx = x + ((*pA >> 12) - 4);
            gy = y + ((*pB >> 12) - 4);
            tile = (char *)G2S::GetBG0CharPtr();
            tidx = (gx >> 3) + ((gy >> 3) << 5);
            row = (int *)(tile + (tidx << 5));
            /* pin row complete before nibble extract */
            row = (int *)((char *)row + (gy - gy));
            word = row[gy & 7];
            if (((word >> ((gx & 7) << 2)) & 0xf) == 0)
                cnt++;
        }
    }
    if (cnt == 0)
        return;
    if (((dScMgTeresa_c *)(self + off))->mBoos[0].mKind == 0)
        return;
    mBoos[idx].mHeading += 0x8000;
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c7MoveBooEi
void dScMgTeresa_c::MoveBoo(int i)
{
    if (mBoos[i].mKind == 0) {
        return;
    }
    StepBoo(i);
    BounceBooOffWalls(i);
}

// @symbol _ZN13dScMgTeresa_c17Boo_StateFadeAwayEi
void dScMgTeresa_c::Boo_StateFadeAway(int i) {
    char *c = (char *)this;
    u8 *s;
    int o = i * 0x24;
    u8 *fade;

    /* Most mBoos[i] fields are spelled (char *)&mBoos[0].field + o: the
       cartridge adds the field offset to `this` first and the stride last,
       and mBoos[i].field (stride first) moves bytes. */
    if (*(u16 *)((char *)&mBoos[0].mDelay + o) != 0) {
        (*(u16 *)((char *)&mBoos[0].mDelay + o))--;
        if (*(u16 *)((char *)&mBoos[0].mDelay + o) == 0)
            Sound::PlayBank2_2D(0x1f1);
    }

    s = (u8 *)((char *)&mBoos[0].mFrame + o);
    if (*s != 6) {
        u16 *p70 = (u16 *)((char *)&mBoos[0].mAnimTimer + o);
        *p70 = *p70 + 1;
        if (*p70 >= 0xf) {
            *p70 = 0;
            (*s)++;
            if (*s >= 6)
                *s = 3;
        }
    }

    StepBoo(i);
    BounceBooOffWalls(i);

    {
        char *row = c + o;
        if (data_02082214[((((dScMgTeresa_c *)row)->mBoos[0].mHeading >> 4) << 1) + 1] >= 0)
            ((dScMgTeresa_c *)row)->mBoos[0].mAnim = 1;
        else
            ((dScMgTeresa_c *)row)->mBoos[0].mAnim = 0;
    }

    if (((dScMgTeresa_c *)((char *)c + o))->mBoos[0].mTurnTimer != 0) {
        u16 *p72 = (u16 *)((char *)&mBoos[0].mTurnTimer + o);
        *p72 = *p72 - 1;
        if (*p72 != 0)
            return;
        *(u16 *)((char *)&mBoos[0].mHeading + o) += 0x8000;
        *s = 6;
        if (((dScMgTeresa_c *)((char *)c + o))->mBoos[0].mPlaySettleSound != 0)
            Sound::PlayBank2_2D(0x1f4);
        return;
    }

    {
        u16 *p6e = (u16 *)((char *)&mBoos[0].mTimer + o);
        *p6e = *p6e + 1;
        if (*p6e < 4)
            return;
        fade = (u8 *)((char *)&mBoos[0].mFade + o);
        *fade = *fade + 1;
        *p6e = 0;
        if (i == mLastSettledBoo) {
            G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 4, 0x10 - *fade, 0x10);
        }
        if (*fade >= 0x10) {
            *(u8 *)((char *)&mBoos[0].mInPlay + o) = 0;
            *(u8 *)((char *)&mBoos[0].mVisible + o) = 0;
        }
    }
}

// @symbol _ZN13dScMgTeresa_c15Boo_StateFadeInEi
void dScMgTeresa_c::Boo_StateFadeIn(int i) {

    if (mBoos[i].mDelay != 0) {
        mBoos[i].mDelay--;
        if (mBoos[i].mDelay == 0) {
            if (mBoos[i].mPlaySettleSound != 0)
                Sound::PlayBank2_2D(0x1f1);
        }
    }

    mBoos[i].mAnimTimer++;
    if (mBoos[i].mAnimTimer >= 0xf) {
        mBoos[i].mAnimTimer = 0;
        mBoos[i].mFrame++;
        if (mBoos[i].mFrame >= 6)
            mBoos[i].mFrame = 3;
    }

    StepBoo(i);
    BounceBooOffWalls(i);

    if (data_02082214[2 * (mBoos[i].mHeading >> 4) + 1] >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;

    if (mBoos[i].mFade >= 0x10)
        return;

    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;

    mBoos[i].mFade++;
    mBoos[i].mPriority = 0;
    mBoos[i].mTimer = 0;

    if (i == mLastSettledBoo) {
        G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 1,
            mBoos[i].mFade, 0x10);
    }

    if (mBoos[i].mFade < 0x10)
        return;

    mBoos[i].mTurnTimer = 0x80;
    mBoos[i].mTimer = 0;
    mBoos[i].mFade = 0;
    mBoos[i].mState = 7;
}

// @symbol _ZN13dScMgTeresa_c14Boo_StateFoundEi
void dScMgTeresa_c::Boo_StateFound(int idx) {
    if (mSceneState != 4)
        return;
    if (mBoos[idx].mTimer != 0) {
        mBoos[idx].mTimer--;
        return;
    }
    VanishPuff_Spawn(idx);
    mBoos[idx].mVisible = 0;
    mBoos[idx].mInPlay = 0;
    if (mFirstFoundBoo != idx)
        return;
    Sound::PlayBank2_2D(0x1c7);
}

// @symbol _ZN13dScMgTeresa_c20Boo_StateHiddenTimedEi
void dScMgTeresa_c::Boo_StateHiddenTimed(int idx) {
    if (mBoos[idx].mTimer != 0) {
        mBoos[idx].mTimer--;
        if ((s16)mBoos[idx].mTimer <= 0) {
            mBoos[idx].mTimer = 0;
            mBoos[idx].mState = 3;
            return;
        }
    }
    StepBoo(idx);
    BounceBooOffWalls(idx);
    if (data_02082214[(mBoos[idx].mHeading >> 4) * 2 + 1] >= 0)
        mBoos[idx].mAnim = 1;
    else
        mBoos[idx].mAnim = 0;
    TouchTestBoo(idx);
}

// @symbol _ZN13dScMgTeresa_c15Boo_StateHiddenEi
void dScMgTeresa_c::Boo_StateHidden(int i) {
    MoveBoo(i);
    CountRubbedTiles(i);
    TouchTestBoo(i);
    if (data_02082214[(mBoos[i].mHeading >> 4) * 2 + 1] >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
}

// @symbol _ZN13dScMgTeresa_c16Boo_StateFadeOutEi
void dScMgTeresa_c::Boo_StateFadeOut(int i) {
    StepBoo(i);
    BounceBooOffWalls(i);
    if (data_02082214[2 * (mBoos[i].mHeading >> 4) + 1] >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;
    mBoos[i].mFade++;
    mBoos[i].mTimer = 0;
    if (i == 0)
        G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 1, 0x10 - mBoos[i].mFade, 0x10);
    if (mBoos[i].mFade < 0x10)
        return;
    mBoos[i].mFade = 0;
    mBoos[i].mState = 3;
    mBoos[i].mPriority = 1;
}

// @symbol _ZN13dScMgTeresa_c17BounceBooOffWallsEi
void dScMgTeresa_c::BounceBooOffWalls(int i)
{
    int xt = mBoos[i].mX >> 12;
    int yt = mBoos[i].mY >> 12;
    if (xt + 8 > 0xfe) {
        mBoos[i].mHeading = 0x8000 - mBoos[i].mHeading;
        mBoos[i].mX = 0xf6000;
    } else if (xt - 8 < 2) {
        mBoos[i].mHeading = 0x8000 - mBoos[i].mHeading;
        mBoos[i].mX = 0xa000;
    }
    if (yt + 8 > 0xbe) {
        mBoos[i].mHeading = -mBoos[i].mHeading;
        mBoos[i].mY = 0xb6000;
    } else if (yt - 8 < 2) {
        mBoos[i].mHeading = -mBoos[i].mHeading;
        mBoos[i].mY = 0xa000;
    }
}


// @symbol _ZN13dScMgTeresa_c7StepBooEi
void dScMgTeresa_c::StepBoo(int i) {
    int n = i * 0x24;
    char *pm = (char *)&mBoos[0].mSpeed;
    char *pa = (char *)&mBoos[0].mHeading;
    int *new_var;
    short *tbl = data_02082214;
    int angle = *((unsigned short *)(pa + n));
    int s = tbl[((angle >> 4) * 2) + 1];
    int mult = *((int *)(pm + n));
    char *p0 = (char *)&mBoos[0].mX;
    *((int *)(p0 + n)) = (*((int *)(p0 + n))) + FX_MUL(*(new_var = &s), mult);
    {
        int angle2 = *((unsigned short *)(pa + n));
        int mult2 = *((int *)(pm + n));
        int s2 = tbl[(angle2 >> 4) * 2];
        char *p1 = (char *)&mBoos[0].mY;
        *((int *)(p1 + n)) = (*((int *)(p1 + n))) + FX_MUL(s2, mult2);
    }
}

// @symbol _ZN13dScMgTeresa_c15StartBoosHidingEv
void dScMgTeresa_c::StartBoosHiding()
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState == 1)
            mBoos[i].mState = 2;
    }
}

// @symbol _ZN13dScMgTeresa_c12Boo_StateFlyEi
void dScMgTeresa_c::Boo_StateFly(int i) {
    mBoos[i].mTimer = 0;
    StepBoo(i);
    BounceBooOffWalls(i);
    if (data_02082214[(mBoos[i].mHeading >> 4) * 2 + 1] >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
}

// @symbol _ZN13dScMgTeresa_c15Boo_StateAppearEi
void dScMgTeresa_c::Boo_StateAppear(int idx) {
    unsigned char t;
    mBoos[idx].mVisible = 1;
    mBoos[idx].mTimer = 0x60;
    mBoos[idx].mSettled = 0;
    mBoos[idx].mState = 1;
    mBoos[idx].mSpeed = 0xc00;
    t = mBoos[idx].mKind;
    if (t == 0)
        return;
    if (t == 8)
        mBoos[idx].mSpeed -= 0x200;
    else
        mBoos[idx].mSpeed += (t - 1) << 9;
}

// @symbol _ZN13dScMgTeresa_c10AnimateBooEi
void dScMgTeresa_c::AnimateBoo(int i)
{
    if (mBoos[i].mState >= 6) return;
    mBoos[i].mAnimTimer++;
    if (mBoos[i].mAnimTimer < 0xf) return;
    mBoos[i].mAnimTimer = 0;
    mBoos[i].mFrame++;
    if (mBoos[i].mFrame >= 3)
        mBoos[i].mFrame = 0;
}


// @symbol _ZN13dScMgTeresa_c10UpdateBoosEv
void dScMgTeresa_c::UpdateBoos()
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mBoos[i].mInPlay) {
            (this->*sTeresaBooStateTable[mBoos[i].mState])(i);
            AnimateBoo(i);
        }
    }
}


// @symbol _ZN13dScMgTeresa_c9SpawnBoosEv
void dScMgTeresa_c::SpawnBoos() {
    int sel;
    int count;
    unsigned short phase;
    int i;
    int valA;
    int flagA;
    int valB;
    int cval;
    int full;
    unsigned int r;

    sel = mLevel;
    if (sel >= 0xf) {
        int k;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        k = (r << 2) >> 15;
        if (k + 0xa == mBooPattern) {
            k += (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            if (k >= 4) k -= 4;
        }
        sel = k + 0xa;
    }
    mBooPattern = sel;
    count = sTeresaSpawnCounts[sel];
    mBooCount = count;

    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    phase = ((r << 4) >> 15) << 12;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    valA = ((((r << 3) >> 15) << 3) + 0x60) << 12;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    valB = ((((r << 3) >> 15) << 3) + 0x60) << 12;

    i = 0;
    if (count <= 0) {
        return;
    }
    flagA = sTeresaSpawnBurst[sel];
    cval = sTeresaSpawnKinds[sel];
    full = 0x10000;
    do {
        if (flagA == 0) {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            mBoos[i].mX = ((((r * 0xf) >> 15) << 4) + 0x10) << 12;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            mBoos[i].mY = ((((r * 0xb) >> 15) << 4) + 0x10) << 12;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            mBoos[i].mHeading = ((r << 5) >> 15) << 11;
        } else {
            mBoos[i].mX = valA;
            mBoos[i].mY = valB;
            int q = full / count;
            mBoos[i].mHeading = q * i + phase;
        }
        mBoos[i].mKind = cval;
        mBoos[i].mTimer = 0;
        mBoos[i].mAnimTimer = 0;
        mBoos[i].mTurnTimer = 0;
        mBoos[i].mDelay = 0;
        mBoos[i].mInPlay = 1;
        mBoos[i].mState = 0;
        mBoos[i].mSettled = 0;
        mBoos[i].mPriority = 0;
        mBoos[i].mFound = 0;
        mBoos[i].mFrame = 0;
        {
            int ang = mBoos[i].mHeading;
            if (data_02082214[(ang >> 4) * 2 + 1] >= 0) {
                mBoos[i].mAnim = 1;
            } else {
                mBoos[i].mAnim = 0;
            }
        }
        i++;
    } while (i < count);
}

#pragma push
#pragma opt_common_subs off
// @symbol _ZN13dScMgTeresa_c13UpdateRubbingEv
void dScMgTeresa_c::UpdateRubbing() {
    int close;

    if (mRevealDone == 0)
        return;
    if (mRubLocked != 0)
        return;

    close = 0;
    ReadRubTouch();

    if (mRubTouching != 0) {
        /* Not Vector3: its C++ destructor changes this frame. */
        struct { Fix12i x, y, z; } d;
        int dist;
        d.x = mRubX - mRubPrevX;
        d.y = mRubY - mRubPrevY;
        dist = cstd::sqrt((u64)((d.x) * (d.x) + (d.y) * (d.y)));
        if (dist <= 3)
            close = 1;
        if (close == 0) {
            int snd;
            int cur;
            Mg_DrawLine(mRubPrevX, mRubPrevY, mRubX, mRubY, 0, 8);
            snd = func_020126e8(mRubX << 12);
            cur = mRubSound;
            mRubSound = func_02012468(cur, 2, 0x1F8, 4, 0, 0, snd, 0);
        }
    }
    if (close == 0) {
        mRubPrevX = mRubX;
        mRubPrevY = mRubY;
    }
}
#pragma pop

// @symbol _ZN13dScMgTeresa_c12ReadRubTouchEv
void dScMgTeresa_c::ReadRubTouch() {
    int i = gActivePlayerSlot;
    if (gTouchHeld[(unsigned int)i * 4] != 0 && mRubTouching == 0) {
        mRubX = gTouchX[i * 4];
        mRubY = gTouchY[i * 4];
        mRubPrevX = mRubX;
        mRubPrevY = mRubY;
        mRubTouching += 1;
    }
    if (gTouchHeld[(unsigned int)gActivePlayerSlot * 4] == 0) {
        mRubTouching = 0;
    }
    if (mRubTouching == 0)
        return;
    mRubX = gTouchX[(unsigned int)gActivePlayerSlot * 4];
    mRubY = gTouchY[(unsigned int)gActivePlayerSlot * 4];
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN13dScMgTeresa_c10ResetRoundEv
void dScMgTeresa_c::ResetRound()
{
    char *p = (char *)this;
  int v;
  int i;
  char *q;
  v = 0;
  q = p;
  i = 0;
  do
  {
      ((dScMgTeresa_c *)q)->mBoos[0].mX = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mY = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mTimer = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mAnimTimer = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mTurnTimer = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mInPlay = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mState = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mSettled = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mVisible = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mPriority = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mFound = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mFrame = v;
      ((dScMgTeresa_c *)q)->mBoos[0].mAnim = v;
      i++;
      ((dScMgTeresa_c *)q)->mBoos[0].mFade = v;
      q += 0x24;
  }
  while (i < 0x10);
  q = p;
  v = 0;
  i = 0;
  do
  {
      ((dScMgTeresa_c *)q)->mExclaims[0].x = i;
      ((dScMgTeresa_c *)q)->mExclaims[0].y = i;
      ((dScMgTeresa_c *)q)->mExclaims[0].timer = i;
      ((dScMgTeresa_c *)q)->mExclaims[0].active = i;
      v++;
      ((dScMgTeresa_c *)q)->mExclaims[0].visible = i;
      q += 0xc;
  }
  while (v < 0x10);
  v = 0;
  do
  {
      ((dScMgTeresa_c *)(p + (v << 4)))->mVanishPuffs[0].active = i;
      ((dScMgTeresa_c *)(p + (v << 4)))->mVanishPuffs[0].visible = i;
      v++;
  }
  while (v < 0x10);
  mRubX = i;
  mRubY = i;
  mRubPrevX = i;
  mRubPrevY = i;
  /* 0x4bfc, 0x4c00 and 0x4c10 are cleared here and read nowhere in this TU. */
  *((int *) (p + 0x4bfc)) = i;
  *((int *) (p + 0x4c00)) = i;
  *((short *) (p + 0x4c10)) = i;
  mSceneTimer = i;
  mBg3FlashTimer = i;
  mRubTouching = i;
  mRoundCleared = i;
  mRevealDone = i;
  mTimeLeft = i;
  mCountdownFrames = i;
  mCountdownShown = i;
  mCountdownRunning = i;
  mFoundCount = i;
  Reveal_Hide();
  Marker_Hide();
  mBg3FlashTick = i;
  mBg3FlashStep = i;
  BigBoo_Hide();
  mRubSound = i;
  mFirstFoundBoo = 0xff;
  mRubLocked = i;
}
#pragma pop

// @symbol _ZN13dScMgTeresa_c17Scene_StateResultEv
void dScMgTeresa_c::Scene_StateResult() {
    BigBoo_Update();
    UpdateBoos();
    VanishPuff_UpdateAll();
    if (mSceneTimer == 0)
        return;
    mSceneTimer -= 1;
    if ((s16)mSceneTimer > 0)
        return;
    mSceneTimer = 0;
    if (mRoundCleared != 0) {
        struct ScoreView *p;
        Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
        p = (struct ScoreView *)gMgScene;
        if (p != 0) {
            if (p->b4 < 0x270f) {
                p->b4 += 1;
            }
            if (p->b4 > p->b8) {
                p->b8 = p->b4;
            }
        }
        Mg_SubmitScore(gMgScene ? ((struct ScoreView *)gMgScene)->b4 : 0);
    } else {
        if (Mg_IsNewRecord() != 0) {
            mCountdownShown = 0;
        }
        Mg_ShowBanner(8, 0x80, 0x60, 1, -1, 0xd);
        Mg_SetSharedState(MG_STATE_GAME_OVER);
    }
    mPromptEnabled = 0;
}

// @symbol _ZN13dScMgTeresa_c19Scene_StateRoundEndEv
void dScMgTeresa_c::Scene_StateRoundEnd() {
    BigBoo_Update();
    UpdateRubbing();
    if (mSceneTimer != 0) {
        mSceneTimer -= 1;
        if ((s16)mSceneTimer > 0)
            return;
        if (mRoundCleared != 0) {
            FoundNumber_SpawnForFound();
            mBg3FlashTimer = 0x60;
            return;
        }
        FoundNumber_SpawnForFound();
        AreBoosSettled();
        mBg3FlashTimer = 0x60;
        return;
    }
    FoundNumber_UpdateAll();
    UpdateBoos();
    if (FoundNumber_CountBusy() != 0)
        return;
    if (mBg3FlashTimer != 0) {
        StepBg3Scroll();
        mBg3FlashTimer -= 1;
        if ((s16)mBg3FlashTimer < 0)
            mBg3FlashTimer = 0;
        return;
    }
    FoundNumber_ClearAll();
    mSceneState = 4;
    mSceneTimer = 0x60;
    *(int *)0x4001000 = *(int *)0x4001000 & ~0xe000;
    data_0209d454 = data_0209d454 & ~1;
    Sound::PlayBank2_2D(0x1f7);
    BigBoo_ShowResult();
    if (mRoundCleared == 0)
        return;
    Sound::PlayBank2_2D(0x1f2);
}

// @symbol _ZN13dScMgTeresa_c15Scene_StatePlayEv
void dScMgTeresa_c::Scene_StatePlay() {
    if (mBannerTimer != 0) {
        mBannerTimer -= 1;
        if (mBannerTimer == 0) {
            FreeGfxSlotsById(0xd);
            if (mPromptBlinkCount == 0) {
                mPromptEnabled = 1;
                mPromptBlinkCount = 1;
                mPromptBlinkTimer = 0;
            }
        }
    }
  UpdateCountdown();
  UpdateRubbing();
  UpdateBoos();
  CheckRevealDone();
  StartCountdown();
  BigBoo_Update();
  CheckRoundEnd();
}

// @symbol _ZN13dScMgTeresa_c17Scene_StateRevealEv
void dScMgTeresa_c::Scene_StateReveal() {
    Reveal_Update();
    Marker_Update();
    UpdateBoos();
}

// @symbol _ZN13dScMgTeresa_c15Scene_StateInitEv
void dScMgTeresa_c::Scene_StateInit() {
    mSceneState = 1;
}

#pragma push
#pragma opt_loop_invariants off
// @symbol _ZN13dScMgTeresa_c11PaintSquareEiiii
/* dScMgTeresa_c::PaintSquare - slot 34, a STIPPLED brush.

   Same square walk and the same 4bpp nibble arithmetic as dScMgBase_c's, with
   one difference that changes what it is for: each cell is painted only if
   `sTeresaPaintMask[row] & (1 << (col*4))` is set, so the table is a
   bitmapped shape and this stamps that shape rather than a solid block.

   It also never consults the layer index at +0x6c -- it always draws into sub
   BG0 -- which is why `this` arrives and is never read.  The comment in
   include/dScMgTeresa_c.h predicted both of these before the slot was
   declared. */
void dScMgTeresa_c::PaintSquare(int cx, int cy, int val, int n)
{
    int half;
    int x0;
    int j;
    int x;
    int buf;
    int *addr;
    int i;
    int y;

    half = n / 2;
    j = 0;
    if (j < n) {
        y = cy - half;
        x0 = cx - half;
        do {
            for (i = 0; i < n; i++) {
                if (sTeresaPaintMask[j] & (1 << (i * 4))) {
                    x = i + x0;
                    if (x >= 0 && x < 0x100) {
                        if (y >= -0xc0 - sMgTopScreenYOffset && y >= 0 && y < 0xc0) {
                            addr = (int *)((char *)G2S::GetBG0CharPtr() + ((x / 8) + (y / 8) * 32) * 32 + (y & 7) * 4);
                            MultiCopy_Int(addr, &buf, 4);
                            buf = (buf & (-1 ^ (0xf << ((x & 7) * 4)))) | (val << ((x & 7) * 4));
                            MultiCopy_Int(&buf, addr, 4);
                        }
                    }
                }
            }
            y++;
            j++;
        } while (j < n);
    }
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c11OnNewRecordEv
/* Slot 20, OnNewRecord (see dScMgBase_c.h). A thunk:
   FreeGfxSlotsById(8). */
void dScMgTeresa_c::OnNewRecord()
{
    FreeGfxSlotsById(8);
}


// @symbol _ZN13dScMgTeresa_c10StartRoundEi
/* Vtable slot 18, an override of dScMgBase_c::StartRound(int). The
   signature must repeat the base declaration exactly, or mwcc appends a slot
   instead of overriding. */
void dScMgTeresa_c::StartRound(int reset) {
    volatile unsigned short val;
    if (reset == 0) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc
           field address into its own register (add r2,r4,#0xbc / ldr [r2] /
           str [r2]) and the function grows a word; the cartridge re-issues
           ldr r1,[r4,#0xbc]. Same lever as dScMgHanachan_c::StartRound. */
        mLevel = *(const unsigned int *)&mLevel + 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        mHudScore = 0;
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    }
    ResetRound();
    Reveal_Start();
    Marker_Show();
    SpawnBoos();
    char* dst = (char *)G2S::GetBG0CharPtr();
    val = 0x1111;
    MultiStore16(val, dst, 0x6000);
    mSceneState = 0;
    FreeGfxSlotsById(0x1d);
    mBannerTimer = 0x20;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    void *h = LoadFile(0x101);
    DecompressLZ16(h, (void *)0x6400000);
    Deallocate(h);
}

// @symbol _ZN13dScMgTeresa_c6RenderEv
/* Vtable slot 9. */
s32 dScMgTeresa_c::Render()
{
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
    FoundNumber_DrawAll();
    DrawCountdown();
    Exclaim_DrawAll();
    VanishPuff_DrawAll();
    DrawBoos();
    Reveal_Draw();
    Marker_Draw();
    BigBoo_Draw();
    return 1;
}


// @symbol _ZN13dScMgTeresa_c8BehaviorEv
/* Vtable slot 6: runs the current state's handler from the pointer-to-member
   table sTeresaSceneStateTable, indexed by mSceneState. */
s32 dScMgTeresa_c::Behavior()
{
    (this->*sTeresaSceneStateTable[mSceneState].pmf)();
    Exclaim_UpdateAll();
    return 1;
}


// @symbol _ZN13dScMgTeresa_c13InitResourcesEv
/* Vtable slot 0. */
s32 dScMgTeresa_c::InitResources()
{
    char *self = (char *)this;
    void *f1;
    void *f2;
    void *t;
    volatile u16 fa;
    volatile u16 fb;
    volatile u16 fc;
    *((volatile u16 *) 0x400000c) &= ~3;
    *((volatile u16 *) 0x400000c) &= ~0x40;
    *((volatile u32 *) 0x4000018) = 0;
    *((volatile u16 *) 0x400000c) = ((*((volatile u16 *) 0x400000c)) & 0x43) | 0x1210;
    {
        void *p = G2::GetBG2ScrPtr();
        fa = 0x5300;
        MultiStore16(fa, p, 0x800);
    }
    data_0209d45c |= 8;
    *((volatile u16 *) 0x400000e) = (*((volatile u16 *) 0x400000e)) & (~3);
    *((volatile u16 *) 0x400000e) &= ~0x40;
    *((volatile u32 *) 0x400001c) = 0;
    *((volatile u16 *) 0x400000e) = ((*((volatile u16 *) 0x400000e)) & 0x43) | 0x9310;
    t = LoadFile(0x9b);
    DecompressLZ16(t, G2::GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0x9c);
    GX::LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0x9e);
    func_02056314(t, 0, 0x800);
    Deallocate(t);
    t = LoadFile(0x9d);
    func_02056314(t, 0x800, 0x800);
    Deallocate(t);
    f1 = LoadFile(0x101);
    f2 = LoadFile(0x102);
    DecompressLZ16(f1, (void *) 0x6400000);
    GX::LoadOBJPltt(f2, 0, 0x100);
    *((volatile u16 *) 0x4001008) = ((*((volatile u16 *) 0x4001008)) & 0x43) | 0x218;
    *((volatile u16 *) 0x4001008) &= ~0x40;
    *((volatile u32 *) 0x4001010) = 0;
    *((volatile u16 *) 0x4001008) &= ~3;
    {
        void *p = G2S::GetBG0ScrPtr();
        fb = 0;
        MultiStore16(fb, p, 0x800);
    }
    Mg_InitBrush(self, 0, 0, 0);
    {
        void *p = (char *)G2S::GetBG0CharPtr();
        fc = 0x1111;
        MultiStore16(fc, p, 0x6000);
    }
    data_0209d454 |= 4;
    *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & (~3)) | 1;
    *((volatile u16 *) 0x400100c) &= ~0x40;
    *((volatile u32 *) 0x4001018) = 0;
    *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & 0x43) | 0x408;
    t = LoadFile(0x9f);
    DecompressLZ16(t, G2S::GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0xa0);
    GXS::LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0xa1);
    func_02056374(t, 0, 0x800);
    Deallocate(t);
    DecompressLZ16(f1, (void *) 0x6600000);
    GXS::LoadOBJPltt(f2, 0, 0x100);
    Deallocate(f1);
    Deallocate(f2);
    Mg_SetTopScreenYOffset(0x20);
    ResetRound();
    mBooPattern = 0xff;
    Marker_Show();
    Reveal_Start();
    SpawnBoos();
    mSceneState = 1;
    mBannerTimer = 0x20;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    sMgClearBannerX = 0x80;
    sMgClearBannerY = -128;
    mHudScore = 0;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgTeresa_c through RTTI,
 * allocation size, vtable identity, and the MG_TERESA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgHideAndBooSeek_Spawn. */
// @symbol dScMgTeresa_c_classInit
extern "C" void *dScMgTeresa_c_classInit()
{
    return new dScMgTeresa_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_02132f68 shard).
 * The four PMF state tables plain-copied from the ROM PMF constants, in
 * retail copy order: the scene table, the per-Boo table, the reveal-pair
 * table, then the big-Boo flight table. */
PmfEntry sTeresaSceneStateTable[5] = {
    {pmf_dScMgTeresa_c_Scene_StateInit},   {pmf_dScMgTeresa_c_Scene_StateReveal},
    {pmf_dScMgTeresa_c_Scene_StatePlay},   {pmf_dScMgTeresa_c_Scene_StateRoundEnd},
    {pmf_dScMgTeresa_c_Scene_StateResult},
};
Pmf sTeresaBooStateTable[8] = {
    pmf_dScMgTeresa_c_Boo_StateAppear,      pmf_dScMgTeresa_c_Boo_StateFly,
    pmf_dScMgTeresa_c_Boo_StateFadeOut,     pmf_dScMgTeresa_c_Boo_StateHidden,
    pmf_dScMgTeresa_c_Boo_StateHiddenTimed, pmf_dScMgTeresa_c_Boo_StateFound,
    pmf_dScMgTeresa_c_Boo_StateFadeIn,      pmf_dScMgTeresa_c_Boo_StateFadeAway,
};
Pmf sTeresaRevealStateTable[5] = {
    pmf_dScMgTeresa_c_Reveal_StateStart,   pmf_dScMgTeresa_c_Reveal_StateSlideIn,
    pmf_dScMgTeresa_c_Reveal_StateAnimate, pmf_dScMgTeresa_c_Reveal_StateDipAndRise,
    pmf_dScMgTeresa_c_Reveal_StateFinish,
};
Pmf sTeresaBigBooStateTable[9] = {
    pmf_dScMgTeresa_c_BigBoo_StateDropIn,    pmf_dScMgTeresa_c_BigBoo_StateHover,
    pmf_dScMgTeresa_c_BigBoo_StateFlyLeft,   pmf_dScMgTeresa_c_BigBoo_StateFlyRight,
    pmf_dScMgTeresa_c_BigBoo_StateOffscreen, pmf_dScMgTeresa_c_BigBoo_StateExit,
    pmf_dScMgTeresa_c_BigBoo_StateRemove,    pmf_dScMgTeresa_c_BigBoo_StateGone,
    pmf_dScMgTeresa_c_BigBoo_StateResult,
};
