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
 * the two push and pop brackets bind. Each bracket was measured: without
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
// - Reveal_Start clears mReveal[0].unk_1b (byte 0x1b): nothing in
//   this TU reads it, so it has no name.
// - Boo_StateFadeAway and CountRubbedTiles flip a Boo's heading as
//   `mHeading = mHeading + 0x8000`; `+=` is a word longer in FadeAway
//   (measured 2026-11-07).
// - PaintSquare keeps opt_loop_invariants off and a hand-rotated loop; its
//   comment has the measurements.

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "OamAttr.h"
#include "dScMgTeresa_c.h"
#include "decl_common.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"
#include "PlayerInput.h"
#include "nitro/gx/gx_bgcnt.h"
#include "cstd.h"
#include "nitro/gx/gx_load.h"

/* Scene_StateResult's view of the ov004 score block at gMgScene
   (dScMgBase_c.h declares it void *): the current score at +0xb4 and the best
   at +0xb8. */
struct ScoreView { unsigned char pad[0xb4]; int b4; int b8; };

/* Everything this file calls or reads that decl_common.h does not declare. */
namespace s20afa20 {
void RenderOamMainScreen(OamAttr *, int, int, int, int);
}
using s20afa20::RenderOamMainScreen;
namespace s20b0aa0 {
void FreeGfxSlotsById(int);
}
using s20b0aa0::FreeGfxSlotsById;
namespace s20ad674 {
int GetGameLanguage(void);
}
using s20ad674::GetGameLanguage;
extern "C" {

extern void *LoadFile(int handle);

extern void DecompressLZ16(const void *src, void *dst);
}

struct Matrix2x2;
namespace s20afcf8 {
void DrawOamSprite(OamAttr *, int, int, Matrix2x2 *);
}
using s20afcf8::DrawOamSprite;
namespace s20af68c {
void Hud_RenderSprite(void *, int, int, int, int);
}
using s20af68c::Hud_RenderSprite;
namespace s20b2220 {
void Mg_DrawNumberRotated(int, int, int, int, int, int, unsigned short);
}
using s20b2220::Mg_DrawNumberRotated;
namespace s20b1e34 {
void Mg_DrawScoreTally(void *, int, int, int);
}
using s20b1e34::Mg_DrawScoreTally;
namespace s20adbe0 {
int Mg_IsNewRecord(void);
}
using s20adbe0::Mg_IsNewRecord;

extern int   func_020126e8(int a);

extern int   func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
#include "OAM.h"
#include "nitro/mi/memory.h"
extern "C" {
extern void OamRender10_d_s_mg_teresa(int sub, struct OamAttr *data, s32 x, s32 y, s32 palette,
                                      s32 priority, s32 scaleX, s32 scaleY, s32 rotation, s32 mode);
}

extern unsigned char gSubScreenLayers;

extern unsigned char gMainScreenLayers;

extern int sTeresaBigBooSprites[];

extern unsigned char sTeresaBigBooHoverPicks[];

extern unsigned short sTeresaBigBooFrameTimes[];

extern unsigned char sTeresaRevealSlideFrames[];

extern unsigned char sTeresaRevealAnimFrames[];

extern unsigned short sTeresaBg3FlashTimes[];

extern void *sTeresaExclaimSprites;

extern void *sTeresaVanishPuffSprites[];

extern int sTeresaBigBooHoverSprites[];

extern OamAttr *sMgDigitSprites[];

extern void *sTeresaMarkerSprites;

extern struct OamAttr *sTeresaRevealSprites[];

extern OamAttr **sTeresaCountdownSprites[];

extern void *sTeresaBooFrameSprites[];

extern int sTeresaBooTouchOffsets[];

extern int sTeresaSpawnCounts[];

extern int sTeresaSpawnKinds[];

extern int sTeresaSpawnBurst[];

extern int sTeresaPaintMask[];

extern int            sMgTopScreenYOffset;

/* The four pointer-to-member state tables. Pmf handlers take the Boo/slot
   index; the scene table in sTeresaSceneStateTable takes none. */
typedef void (dScMgTeresa_c::*Pmf)(int);

typedef void (dScMgTeresa_c::*Pmf0)();

struct PmfEntry { Pmf0 pmf; };

extern Pmf sTeresaBigBooStateTable[];
/* big Boo flight states, by BigBoo::state */
extern Pmf sTeresaRevealStateTable[];
/* reveal states, by mReveal[0].state */
extern Pmf sTeresaBooStateTable[];
/* per-Boo states, one call per live row */
extern PmfEntry sTeresaSceneStateTable[];
/* scene states, by mSceneState */
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
/* extern "C" */

namespace Sound { void PlayBank2_2D(unsigned int id); }

/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. */
dScMgTeresa_c::~dScMgTeresa_c()
{
}

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

void dScMgTeresa_c::BigBoo_SetDone() {
    if (mBigBoo[0].active != 0) {
        mBigBoo[0].done = 1;
    }
}

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
    RenderOamMainScreen((OamAttr *)v, p, q, -1, -1);
}

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

void dScMgTeresa_c::BigBoo_StateGone() {}

void dScMgTeresa_c::BigBoo_StateRemove(int index) {
    void *data;
    mBigBoo[index].xVel = 0;
    mBigBoo[index].yVel = 0;
    mBigBoo[index].shown = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, G2_GetOBJCharPtr());
    mBigBoo[index].state = 7;
    Deallocate(data);
}

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
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    SetBg3Offset(0, 0x100);
    {
        /* The draw lands in its own int: through MG_RAND the pick colours
           differently (5 words). */
        int rnd = RandomIntInternal(&data_0209d4b8.value);
        mBigBoo[i].state = sTeresaBigBooHoverPicks[((((u32)rnd >> 16) & 0x7fff) * 2) >> 15];
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

void dScMgTeresa_c::BigBoo_Update()
{
    if (mBigBoo[0].active == 0)
        return;
    (this->*sTeresaBigBooStateTable[mBigBoo[0].state])(0);
}

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
    G2_SetBlendAlpha(0, 0xc, 0xc, 0x10);
}

void dScMgTeresa_c::BigBoo_Hide()
{
    mBigBoo[0].active = 0;
    mBigBoo[0].shown = 0;
}

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

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Marker_Draw()
{
  if(mMarker.visible==0) return;
  RenderOamMainScreen((OamAttr *)sTeresaMarkerSprites, mMarker.x >> 0xc, mMarker.y >> 0xc, -1, -1);
}

/* Empty. Scene_StateReveal calls it with the object. */
void dScMgTeresa_c::Marker_Update()
{
}

void dScMgTeresa_c::Marker_Show()
{
    mMarker.x = 0x50000;
    mMarker.y = 0x40000;
    mMarker.active = 1;
    mMarker.visible = 1;
}

void dScMgTeresa_c::Marker_Hide()
{
    mMarker.active = 0;
    mMarker.visible = 0;
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Reveal_Draw()
{
    if (mReveal[0].visible == 0) return;
    OAM::Render(0, sTeresaRevealSprites[mReveal[0].sprite], mReveal[0].x >> 12, mReveal[0].y >> 12,
                -1, 0, 0x1000, 0x1000, mReveal[0].angle, -1);
}

void dScMgTeresa_c::Reveal_StateFinish(int idx) {
    if (mReveal[idx].timer != 0) {
        mReveal[idx].timer -= 1;
        return;
    }
    mReveal[idx].visible = 0;
    mReveal[idx].active = 0;
    Marker_Hide();
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gSubScreenLayers |= 1;
    Sound::PlayBank2_2D(0x1f6);
    mSceneState = 2;
    StartBoosHiding();
    BigBoo_Start();
    Sound::PlayBank2_2D(0x1f5);
}

#define AT(p, off) (((p)+(off)))
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

void dScMgTeresa_c::Reveal_StateStart(int i) {
    mReveal[i].state = 1;
    mReveal[i].timer = 0x40;
    mReveal[i].vx = -0xb00;
}

void dScMgTeresa_c::Reveal_Update()
{
    if (mReveal[0].active == 0) return;
    (this->*sTeresaRevealStateTable[mReveal[0].state])(0);
}

void dScMgTeresa_c::Reveal_Start()
{
    mReveal[0].x = 0xa0000;
    mReveal[0].y = 0x60000;
    mReveal[0].active = 1;
    mReveal[0].visible = 1;
    mReveal[0].state = 0;
    mReveal[0].sprite = 0;
    mReveal[0].angle = 0;
    mReveal[0].frameTimer = 0;
    mReveal[0].timer = 0;
    /* written only here, read nowhere in this TU */
    mReveal[0].unk_1b = 0;
    mReveal[0].frame = 0;
    mReveal[0].phase = 0;
}

void dScMgTeresa_c::Reveal_Hide()
{
    mReveal[0].visible = 0;
    mReveal[0].active = 0;
}

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
            Mg_RenderSubSpriteMtx(sMgDigitSprites[mFoundNumbers[i].sprite + 1], a, b, 0);
        }
    }
}

void dScMgTeresa_c::FoundNumber_ClearAll() {
    int i;
    for (i = 0; i < 0x10; i++) {
        mFoundNumbers[i].active = 0;
        mFoundNumbers[i].visible = 0;
    }
}

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

void dScMgTeresa_c::FoundNumber_SpawnForFound() {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState == 5)
            FoundNumber_Spawn(i);
    }
}

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

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::DrawCountdown()
{
    if (mCountdownShown == 0)
        return;
    Mg_DrawNumberRotated(0x80, 0x60, mTimeLeft, 1, 0, 0x800, 0);
    DrawOamSprite(sTeresaCountdownSprites[GetGameLanguage()][3], 0x80, 0x48, 0);
}

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

void dScMgTeresa_c::CheckRevealDone()
{
    int found;
    int i;
    if (mRevealDone != 0)
        return;
    found = 0;
    for (i = 0; i < 0x10; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState <= 2) {
            found++;
            break;
        }
    }
    if (found != 0)
        return;
    FreeGfxSlotsById(0xc);
    mRevealDone++;
}

void dScMgTeresa_c::CheckRoundEnd()
{
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
        int i;
        for (i = 0; i < 0x10; i++) {
            if (mBoos[i].mInPlay != 0 && mBoos[i].mFound == 0) {
                count++;
                break;
            }
        }
        if (count != 0)
            return;
        mSceneState = 3;
        mSceneTimer = 0x60;
        BigBoo_SetDone();
        mRoundCleared = 1;
    }
}

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
                mLastSettledBoo = i;
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

#define A(a) (*(a))
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
        r = ((int(*)[8])G2S_GetBG0CharPtr())[(x >> 3) + ((y >> 3) << 5)];
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
        mFirstFoundBoo = idx;
}
#undef A

void dScMgTeresa_c::CountRubbedTiles(int idx)
{
    int gx, gy;
    int cnt;
    int y;
    int x;
    int *row;

    cnt = 0;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            gx = x + ((mBoos[idx].mX >> 12) - 4);
            gy = y + ((mBoos[idx].mY >> 12) - 4);
            row = (int *)((char *)G2S_GetBG0CharPtr() + ((gx >> 3) + ((gy >> 3) << 5)) * 32);
            if (((row[gy & 7] >> ((gx & 7) << 2)) & 0xf) == 0)
                cnt++;
        }
    }
    if (cnt == 0)
        return;
    if (mBoos[idx].mKind == 0)
        return;
    mBoos[idx].mHeading = mBoos[idx].mHeading + 0x8000;
}

void dScMgTeresa_c::MoveBoo(int i)
{
    if (mBoos[i].mKind == 0) {
        return;
    }
    StepBoo(i);
    BounceBooOffWalls(i);
}

void dScMgTeresa_c::Boo_StateFadeAway(int i) {
    if (mBoos[i].mDelay != 0) {
        mBoos[i].mDelay--;
        if (mBoos[i].mDelay == 0)
            Sound::PlayBank2_2D(0x1f1);
    }

    if (mBoos[i].mFrame != 6) {
        mBoos[i].mAnimTimer++;
        if (mBoos[i].mAnimTimer >= 0xf) {
            mBoos[i].mAnimTimer = 0;
            mBoos[i].mFrame++;
            if (mBoos[i].mFrame >= 6)
                mBoos[i].mFrame = 3;
        }
    }

    StepBoo(i);
    BounceBooOffWalls(i);

    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;

    if (mBoos[i].mTurnTimer != 0) {
        mBoos[i].mTurnTimer--;
        if (mBoos[i].mTurnTimer != 0)
            return;
        mBoos[i].mHeading = mBoos[i].mHeading + 0x8000;
        mBoos[i].mFrame = 6;
        if (mBoos[i].mPlaySettleSound != 0)
            Sound::PlayBank2_2D(0x1f4);
        return;
    }

    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;
    mBoos[i].mFade++;
    mBoos[i].mTimer = 0;
    if (i == mLastSettledBoo) {
        G2S_SetBlendAlpha(0, 4, 0x10 - mBoos[i].mFade, 0x10);
    }
    if (mBoos[i].mFade >= 0x10) {
        mBoos[i].mInPlay = 0;
        mBoos[i].mVisible = 0;
    }
}

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

    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
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
        G2S_SetBlendAlpha(0, 1, mBoos[i].mFade, 0x10);
    }

    if (mBoos[i].mFade < 0x10)
        return;

    mBoos[i].mTurnTimer = 0x80;
    mBoos[i].mTimer = 0;
    mBoos[i].mFade = 0;
    mBoos[i].mState = 7;
}

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
    if (FX_CosIdx(mBoos[idx].mHeading >> 4) >= 0)
        mBoos[idx].mAnim = 1;
    else
        mBoos[idx].mAnim = 0;
    TouchTestBoo(idx);
}

void dScMgTeresa_c::Boo_StateHidden(int i) {
    MoveBoo(i);
    CountRubbedTiles(i);
    TouchTestBoo(i);
    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
}

void dScMgTeresa_c::Boo_StateFadeOut(int i) {
    StepBoo(i);
    BounceBooOffWalls(i);
    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;
    mBoos[i].mFade++;
    mBoos[i].mTimer = 0;
    if (i == 0)
        G2S_SetBlendAlpha(0, 1, 0x10 - mBoos[i].mFade, 0x10);
    if (mBoos[i].mFade < 0x10)
        return;
    mBoos[i].mFade = 0;
    mBoos[i].mState = 3;
    mBoos[i].mPriority = 1;
}

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

void dScMgTeresa_c::StepBoo(int i) {
    int c, s;
    c = FX_CosIdx(mBoos[i].mHeading >> 4);
    mBoos[i].mX += FX_MUL(c, mBoos[i].mSpeed);
    s = FX_SinIdx(mBoos[i].mHeading >> 4);
    mBoos[i].mY += FX_MUL(s, mBoos[i].mSpeed);
}

void dScMgTeresa_c::StartBoosHiding()
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState == 1)
            mBoos[i].mState = 2;
    }
}

void dScMgTeresa_c::Boo_StateFly(int i) {
    mBoos[i].mTimer = 0;
    StepBoo(i);
    BounceBooOffWalls(i);
    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
}

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

    sel = mLevel;
    if (sel >= 0xf) {
        int k;
        k = MG_RAND(4);
        if (k + 0xa == mBooPattern) {
            k += MG_RAND(3) + 1;
            if (k >= 4) k -= 4;
        }
        sel = k + 0xa;
    }
    mBooPattern = sel;
    count = sTeresaSpawnCounts[sel];
    mBooCount = count;

    phase = MG_RAND(16) << 12;
    valA = ((MG_RAND(8) << 3) + 0x60) << 12;
    valB = ((MG_RAND(8) << 3) + 0x60) << 12;

    i = 0;
    if (count <= 0) {
        return;
    }
    flagA = sTeresaSpawnBurst[sel];
    cval = sTeresaSpawnKinds[sel];
    full = 0x10000;
    do {
        if (flagA == 0) {
            mBoos[i].mX = ((MG_RAND(0xf) << 4) + 0x10) << 12;
            mBoos[i].mY = ((MG_RAND(0xb) << 4) + 0x10) << 12;
            mBoos[i].mHeading = MG_RAND(32) << 11;
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
            if (FX_CosIdx(ang >> 4) >= 0) {
                mBoos[i].mAnim = 1;
            } else {
                mBoos[i].mAnim = 0;
            }
        }
        i++;
    } while (i < count);
}

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
        VecFx32 d;
        int dist;
        d.x = mRubX - mRubPrevX;
        d.y = mRubY - mRubPrevY;
        dist = cstd::sqrt(((d.x) * (d.x) + (d.y) * (d.y)));
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

void dScMgTeresa_c::ReadRubTouch() {
    int i = gActivePlayerSlot;
    if (gTouchHeld[i * 4] != 0 && mRubTouching == 0) {
        mRubX = gTouchX[i * 4];
        mRubY = gTouchY[i * 4];
        mRubPrevX = mRubX;
        mRubPrevY = mRubY;
        mRubTouching += 1;
    }
    if (gTouchHeld[gActivePlayerSlot * 4] == 0) {
        mRubTouching = 0;
    }
    if (mRubTouching == 0)
        return;
    mRubX = gTouchX[gActivePlayerSlot * 4];
    mRubY = gTouchY[gActivePlayerSlot * 4];
}

void dScMgTeresa_c::ResetRound() {

    for (int i = 0; i < 0x10; i++) {
        mBoos[i].mX = 0;
        mBoos[i].mY = 0;
        mBoos[i].mTimer = 0;
        mBoos[i].mAnimTimer = 0;
        mBoos[i].mTurnTimer = 0;
        mBoos[i].mInPlay = 0;
        mBoos[i].mState = 0;
        mBoos[i].mSettled = 0;
        mBoos[i].mVisible = 0;
        mBoos[i].mPriority = 0;
        mBoos[i].mFound = 0;
        mBoos[i].mFrame = 0;
        mBoos[i].mAnim = 0;
        mBoos[i].mFade = 0;
    }
    for (int i = 0; i < 0x10; i++) {
        mExclaims[i].x = 0;
        mExclaims[i].y = 0;
        mExclaims[i].timer = 0;
        mExclaims[i].active = 0;
        mExclaims[i].visible = 0;
    }
    for (int i = 0; i < 0x10; i++) {
        mVanishPuffs[i].active = 0;
        mVanishPuffs[i].visible = 0;
    }
    mRubX = 0;
    mRubY = 0;
    mRubPrevX = 0;
    mRubPrevY = 0;
    pad_4bfc = 0;
    pad_4c00 = 0;
    pad_4c10 = 0;
    mSceneTimer = 0;
    mBg3FlashTimer = 0;
    mRubTouching = 0;
    mRoundCleared = 0;
    mRevealDone = 0;
    mTimeLeft = 0;
    mCountdownFrames = 0;
    mCountdownShown = 0;
    mCountdownRunning = 0;
    mFoundCount = 0;
    Reveal_Hide();
    Marker_Hide();
    mBg3FlashTick = 0;
    mBg3FlashStep = 0;
    BigBoo_Hide();
    mRubSound = 0;
    mFirstFoundBoo = 0xff;
    mRubLocked = 0;
}

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
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    gSubScreenLayers = gSubScreenLayers & ~1;
    Sound::PlayBank2_2D(0x1f7);
    BigBoo_ShowResult();
    if (mRoundCleared == 0)
        return;
    Sound::PlayBank2_2D(0x1f2);
}

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

void dScMgTeresa_c::Scene_StateReveal() {
    Reveal_Update();
    Marker_Update();
    UpdateBoos();
}

void dScMgTeresa_c::Scene_StateInit() {
    mSceneState = 1;
}

#pragma push
/* Kept: with loop invariants hoisted, the constants spill, the stack frame
 * grows and buf moves to another slot. The hand-rotated do/while is kept
 * too: `for (j = 0; j < n; j++)` with y = cy - half + j is the right size
 * but schedules cx - half and cy - half ahead of the n <= 0 exit (12 words
 * off; corner in the for initialiser, a word short; measured 2026-11-07). */
#pragma opt_loop_invariants off
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
                            addr = (int *)((char *)G2S_GetBG0CharPtr() +
                                           ((x / 8) + (y / 8) * 32) * 32 + (y & 7) * 4);
                            MI_CpuCopy32(addr, &buf, 4);
                            buf = (buf & (-1 ^ (0xf << ((x & 7) * 4)))) | (val << ((x & 7) * 4));
                            MI_CpuCopy32(&buf, addr, 4);
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

/* Slot 20, OnNewRecord (see dScMgBase_c.h). A thunk:
   FreeGfxSlotsById(8). */
void dScMgTeresa_c::OnNewRecord()
{
    FreeGfxSlotsById(8);
}

/* Vtable slot 18, an override of dScMgBase_c::StartRound(int). The
   signature must repeat the base declaration exactly, or mwcc appends a slot
   instead of overriding. */
void dScMgTeresa_c::StartRound(int reset) {
    /* volatile: the cartridge stores the fill value to the stack and reloads
       it (strh/ldrh [sp]) before MIi_CpuClear16. Plain locals, by-value and
       by-reference wrappers all fold the constant; no other spelling found. */
    volatile unsigned short val;
    if (reset == 0) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc
           field address into its own register (add r2,r4,#0xbc / ldr [r2] /
           str [r2]) and the function grows a word; the cartridge re-issues
           ldr r1,[r4,#0xbc]. Same lever as dScMgHanachan_c::StartRound. */
        const unsigned int level = mLevel;
        mLevel = level + 1;
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
    char *dst = (char *)G2S_GetBG0CharPtr();
    val = 0x1111;
    MI_CpuFill16(dst, val, 0x6000);
    mSceneState = 0;
    FreeGfxSlotsById(0x1d);
    mBannerTimer = 0x20;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    void *h = LoadFile(0x101);
    DecompressLZ16(h, G2_GetOBJCharPtr());
    Deallocate(h);
}

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

/* Vtable slot 6: runs the current state's handler from the pointer-to-member
   table sTeresaSceneStateTable, indexed by mSceneState. */
s32 dScMgTeresa_c::Behavior()
{
    (this->*sTeresaSceneStateTable[mSceneState].pmf)();
    Exclaim_UpdateAll();
    return 1;
}

/* Vtable slot 0. */
s32 dScMgTeresa_c::InitResources()
{
    void *f1;
    void *f2;
    void *t;
    /* volatile: each fill value round-trips through the stack before
       MIi_CpuClear16, as in StartRound. */
    volatile u16 fa;
    volatile u16 fb;
    volatile u16 fc;
    G2_SetBG2Priority(0);
    G2_BG2Mosaic(0);
    G2_SetBG2Offset(0, 0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);
    {
        void *p = G2_GetBG2ScrPtr();
        fa = 0x5300;
        MI_CpuFill16(p, fa, 0x800);
    }
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    G2_SetBG3Priority(0);
    G2_BG3Mosaic(0);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9800,
                         GX_BG_CHARBASE_0x10000);
    t = LoadFile(0x9b);
    DecompressLZ16(t, G2_GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0x9c);
    GX_LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0x9e);
    GX_LoadBG3Scr(t, 0, 0x800);
    Deallocate(t);
    t = LoadFile(0x9d);
    GX_LoadBG3Scr(t, 0x800, 0x800);
    Deallocate(t);
    f1 = LoadFile(0x101);
    f2 = LoadFile(0x102);
    DecompressLZ16(f1, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(f2, 0, 0x100);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                      GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(0);
    G2S_SetBG0Offset(0, 0);
    G2S_SetBG0Priority(0);
    {
        void *p = G2S_GetBG0ScrPtr();
        fb = 0;
        MI_CpuFill16(p, fb, 0x800);
    }
    Mg_InitBrush((char *)this, 0, 0, 0);
    {
        void *p = G2S_GetBG0CharPtr();
        fc = 0x1111;
        MI_CpuFill16(p, fc, 0x6000);
    }
    gSubScreenLayers |= 4;
    G2S_SetBG2Priority(1);
    G2S_BG2Mosaic(0);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x08000);
    t = LoadFile(0x9f);
    DecompressLZ16(t, G2S_GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0xa0);
    GXS_LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0xa1);
    GXS_LoadBG2Scr(t, 0, 0x800);
    Deallocate(t);
    DecompressLZ16(f1, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(f2, 0, 0x100);
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
void *dScMgTeresa_c_classInit() {
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
