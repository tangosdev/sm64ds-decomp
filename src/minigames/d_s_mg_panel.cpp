//cpp
/* Puzzle Panic, the whole run 0x0210428c..0x0210788c, 72 functions, closing
 * with the factory dScMgPanel_c_classInit, then the state tables and the
 * registry profile, from which mwcc emits __sinit_d_s_mg_panel.cpp. Touch a
 * panel to flip it and its neighbours until every mFace matches mGoal.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; do not
 * reorder or drop the pragma. It also makes the bracketed optimizer pragmas
 * bind to their own functions.
 *
 * The destructor is out of line and written first, so this file owns the
 * key function and emits the vtable and typeinfo; the compiler emits D1 and
 * D0 from the one body in cartridge order.
 *
 * The 62 helpers that take the scene as their first argument are members
 * (the address is the method name; no ROM spelling survives), declared in
 * dScMgPanel_c.h. Each body opens by re-viewing `this` under the type it used
 * to take, so the local struct views and raw-offset accesses are unchanged.
 * The three empty handlers Shake_StateIdle, Banner_StateIdle and
 * IntroCard_StateDone are members too: the state tables point at them. The
 * `_ac4`-style suffixes on the local struct tags only keep each helper's
 * view apart.
 *
 * Leftover, each measured against the matching build:
 * - Particle_DrawAll, Particle_UpdateAll, Particle_Spawn,
 *   Particle_ClearAll: `PanelPart *p = (PanelPart *)(scene + 0x46a8)`
 *   with p->x / p->show / p++ DIFFs. The 0x18 byte step stays.
 * - Scroll_Update, Scroll_Start: `PanelScroll *` at 0x4694
 *   DIFFs. Scroll_Stop's one `on` store matches.
 * - Answer_StateFlipSolution, UpdateMoves: mWidth and mDelay stay raw;
 *   `(u16 *)(int)&mDelay` then an s16 compare DIFFs by 25 words.
 * - Answer_StateFindSolution, Answer_StateResetBoard, Scene_StateGameOver,
 *   Scene_StateShowAnswer, Scene_StateRewind, Scene_StatePlay: mKind
 *   and mCount (and mFace, mDelay where used) DIFF as members.
 * - CheckSolved: the mFace / mGoal compare DIFFs as members.
 * - UpdateTouch, FlipAt, Panel_StartAll: mWidth
 *   DIFFs as a member; so does mKind in Panel_StartAll.
 * - DrawPanels, UpdateTouch keep their own local struct
 *   views; Scene_StateResult and FlipAt's mFace flip go
 *   through a computed byte pointer. Not tried.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgPanel_c_AssetId {
    ASSET_D_2D_MG_BG_PAZZLE_PANEL_DOWN_NSC_BIN = 0x0069,
    ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCG_BIN = 0x006a,
    ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCL_BIN = 0x006b,
    ASSET_D_2D_MG_BG_PAZZLE_PANEL_UP_NSC_BIN = 0x006c,
    ASSET_D_2D_MG_PAZZLE_PANEL_F_NCG_BIN = 0x00e2,
    ASSET_D_2D_MG_PAZZLE_PANEL_G_NCG_BIN = 0x00e3,
    ASSET_D_2D_MG_PAZZLE_PANEL_I_NCG_BIN = 0x00e4,
    ASSET_D_2D_MG_PAZZLE_PANEL_NCG_BIN = 0x00e5,
    ASSET_D_2D_MG_PAZZLE_PANEL_NCL_BIN = 0x00e6,
    ASSET_D_2D_MG_PAZZLE_PANEL_S_NCG_BIN = 0x00e7,
    ASSET_D_2D_YOSIFINX_BIKKURI_NSC_BIN = 0x01e5,
    ASSET_D_2D_YOSIFINX_NORMAL_NSC_BIN = 0x01e6
};

#pragma defer_codegen off /* per-function opt pragmas, and .text in source order */

#include "types.h"
#include "dScMgPanel_c.h"
#include "SpawnInfo.h"
#include "private/ov006_m8c.h"

#include "Sound.h"

/* BG2 offset on both screens. Scroll_Update runs it: phase 0 shakes
   x by +-2.0 for 0x3c frames and drops particles (Particle_Spawn) at
   both side edges, then y accelerates up to 512.0. Scroll_Start arms
   it; Scroll_Stop clears `on` and the offsets. */
struct PanelScroll {
    int x;       /* 0x4694 */
    int y;       /* 0x4698 */
    int z;       /* 0x469c */
    int vel;     /* 0x46a0 */
    u8 on;       /* 0x46a4 */
    u8 phase;    /* 0x46a5 */
    u8 timer;    /* 0x46a6 */
    u8 gap;      /* 0x46a7 */
};
typedef char PanelScroll_size_must_be_0x14[sizeof(PanelScroll) == 0x14 ? 1 : -1];

namespace G2   { char *GetBG2ScrPtr(); }
namespace G2S  { char *GetBG2ScrPtr(); char *GetBG3CharPtr(); }
namespace GX   { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace GXS  { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }

/* Callees of the member helpers. Declared here, not in the bodies: a
   declaration inside a member body gets C++ linkage and names a symbol the ROM
   doesn't have. */
extern "C" {
int  RandomIntInternal(int *seed);
void SetBg2Offset(int x, int y);
void SetSubBg2Offset(int x, int y);
void SetSubBg0Offset(int a, int b);
unsigned int func_02012790(unsigned int a);
void FreeGfxSlotsById(int arg);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void Hud_RenderSprite(void *a, int b, int c, int d, int e);
void Mg_SubmitScore(int a);
void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *d);
void Mg_RenderSpriteBoth(void *a, int b, int c, int d, int e);
void Mg_SetSharedState(int a);
void Mg_RenderSubSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5);
int  __aeabi_idiv(int a, int b);
void Mg_DrawBigNumber(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
}

/* The state tables are arrays of member-function pointers, defined at the end
   of this file. Each dispatcher calls through one of them. */
typedef void (dScMgPanel_c::*PanelStateFn)();
typedef void (dScMgPanel_c::*PanelStateFnI)(int);

// @symbol _ZN12dScMgPanel_cD1Ev
/* The complete-object destructor (D1).  Nothing this class owns needs
   destroying, so the whole 0x24 bytes are the compiler's own-vtable write and
   the tail call into ~dScMgBase_c -- the same empty body dScMgCurling_c has. */
dScMgPanel_c::~dScMgPanel_c()
{
}

// @symbol _ZN12dScMgPanel_cD0Ev
/* dScMgPanel_c::~dScMgPanel_c (D0, deleting destructor).  There is no second
   definition to write: mwcc emits D0 from the ONE definition above, and
   dScMgBase_c's own operator delete covers the deallocation.  Under
   `#pragma defer_codegen off` the variants come out D1, D0, D2, which is the
   cartridge order; the trailing D2 is homeless and is deadstripped. */

// @symbol _ZN12dScMgPanel_c16Particle_DrawAllEv
/* Draw the live particles: 64 slots of 0x18 bytes, sprite chosen by the
   slot's kind byte and positioned from its Fix12 x/y. */
void dScMgPanel_c::Particle_DrawAll()
{
    char *scene = (char *)this;
    extern int sPanelParticleSprites[];
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (self->mParticles[i].visible != 0) {
            Mg_RenderSpriteBoth((void *)sPanelParticleSprites[self->mParticles[i].kind],
                                self->mParticles[i].x >> 12, self->mParticles[i].y >> 12, -1, -1);
        }
    }
}

// @symbol _ZN12dScMgPanel_c18Particle_UpdateAllEv
/* Advance the 64 particles one frame: gravity on the free ones, a spin/fade
   counter on all of them, and retire a slot once its counter runs out. */
#pragma push
#pragma opt_common_subs off
void dScMgPanel_c::Particle_UpdateAll()
{
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (self->mParticles[i].active != 0) {
            if (self->mParticles[i].stuck == 0) {
                self->mParticles[i].x += self->mParticles[i].vx;
                self->mParticles[i].y += self->mParticles[i].vy;
                self->mParticles[i].vy -= 0x300;
                if (self->mParticles[i].vx > 0)
                    self->mParticles[i].vx += 0xc00;
                else
                    self->mParticles[i].vx -= 0xc00;
            }
            self->mParticles[i].tick += 1;
            if (self->mParticles[i].tick >= 4) {
                self->mParticles[i].frame += 1;
                if (self->mParticles[i].frame >= 5) {
                    self->mParticles[i].active = 0;
                    self->mParticles[i].visible = 0;
                }
            }
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgPanel_c14Particle_SpawnEiii
/* Spawn one particle in the first free slot, at (x, y).  `mode` picks the
   fixed-drift variant over the random one. */
#pragma push
#pragma opt_common_subs off
void dScMgPanel_c::Particle_Spawn(int x, int y, int mode)
{
    char *scene = (char *)this;
    extern int data_0209d4b8;
    int i;
    char *q = scene;
    for (i = 0; i < 0x40; i++, q += 0x18) {
        if (*(u8 *)(q + 0x46b8) == 0) {
            int off = i * 0x18;
            int *w;
            *(u8 *)(scene + off + 0x46b8) = 1;
            *(u8 *)(scene + off + 0x46bd) = 1;
            *(u8 *)(scene + off + 0x46bc) = 0;
            *(u8 *)(scene + off + 0x46ba) = 0;
            *(u8 *)(scene + off + 0x46bb) = 0;
            *(u8 *)(scene + off + 0x46b9) = mode;
            /* The `(int)` launder keeps mwcc from common-subexpressioning this
               field address with the one taken three lines down; without it 16
               words of this function move. */
            *(int *)((char *)(((int)scene + 0x46b0)) + off) = 0;
            *(int *)(scene + off + 0x46b4) = 0;
            *(int *)(scene + off + 0x46a8) = x;
            *(int *)(scene + off + 0x46ac) = y;
            w = (int *)((char *)(((int)scene + 0x46b0)) + off);
            if (mode != 0) {
                *(u8 *)(scene + off + 0x46be) = 0;
            } else {
                int rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
                rnd = (int)((((u32)rnd) << 1) >> 15);
                *(u8 *)(scene + off + 0x46be) = rnd;
            }
            if (x >= 0x80000)
                *w = -0x100;
            else
                *w = 0x100;
            return;
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgPanel_c17Particle_ClearAllEv
/* Retire every particle in the 64-slot pool. */
void dScMgPanel_c::Particle_ClearAll()
{
    char *scene = (char *)this;
    int i;
    for (i = 0; i < 0x40; i++) {
        *(u8 *)(scene + 0x46b8) = 0;
        *(u8 *)(scene + 0x46bd) = 0;
        scene += 0x18;
    }
}

// @symbol _ZN12dScMgPanel_c13Scroll_UpdateEv
#pragma push
#pragma opt_common_subs off
void dScMgPanel_c::Scroll_Update()
{
    char *scene = (char *)this;
    extern int data_0209d4b8;
    int x;
    int y;
    u32 r;
    u32 r2;

    if (*(u8 *)(scene + 0x46a5) == 0) {
        *(u8 *)(scene + 0x46a6) += 1;
        if (((*(u8 *)(scene + 0x46a6) >> 1) & 1) != 0)
            *(int *)(scene + 0x4694) = 0x2000;
        else
            *(int *)(scene + 0x4694) = -0x2000;

        if (*(u8 *)(scene + 0x46a7) != 0) {
            *(u8 *)(scene + 0x46a7) -= 1;
        } else {
            r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            x = ((r * 7) >> 15) * 5;
            r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            y = ((r << 4) >> 15) + 0xb0;
            Particle_Spawn(x << 12, y << 12, 0);

            r = (u32)RandomIntInternal(&data_0209d4b8);
            r2 = (u32)RandomIntInternal(&data_0209d4b8);
            x = (((r >> 16 & 0x7fff) * 7) >> 15) * 5;
            y = ((((r2 >> 16) & 0x7fff) << 4) >> 15) + 0xb0;
            Particle_Spawn((0x100 - x) << 12, y << 12, 0);
            *(u8 *)(scene + 0x46a7) = 3;
        }

        if (*(u8 *)(scene + 0x46a6) >= 0x3c) {
            *(u8 *)(scene + 0x46a5) += 1;
            *(u8 *)(scene + 0x46a6) = 0;
            *(u8 *)(scene + 0x46a7) = 0;
            *(int *)(scene + 0x4694) = 0;
            *(int *)(scene + 0x46a0) = 0x1000;
        }
    } else if (*(int *)(scene + 0x4698) < 0x200000) {
        *(int *)(scene + 0x4698) += *(int *)(scene + 0x46a0);
        *(int *)(scene + 0x46a0) += 0x200;
        if (*(int *)(scene + 0x4698) >= 0x200000)
            *(int *)(scene + 0x4698) = 0x200000;

        if (*(u8 *)(scene + 0x46a7) != 0) {
            *(u8 *)(scene + 0x46a7) -= 1;
        } else {
            y = *(int *)(scene + 0x4698) >> 12;
            if (y >= 0 && y <= 0xc0)
                y = (0xc0 - y) << 12;
            else if (y >= 0x100)
                y = (-0x20 - (y - 0x100)) << 12;

            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            Particle_Spawn(x << 12, y, 1);
            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            Particle_Spawn((0x100 - x) << 12, y, 1);
            *(u8 *)(scene + 0x46a7) = 2;
        }
    }

    x = *(int *)(scene + 0x4694) >> 12;
    y = *(int *)(scene + 0x4698) >> 12;
    SetBg2Offset(x, y);
    if (y >= 0x100)
        y = 0x100;
    SetSubBg2Offset(x, y);
}
#pragma pop

// @symbol _ZN12dScMgPanel_c12Scroll_StartEv
void dScMgPanel_c::Scroll_Start()
{
    char *scene = (char *)this;
    *(u8 *)(scene + 0x46a4) = 1;
    *(int *)(scene + 0x4694) = 0;
    *(int *)(scene + 0x4698) = 0;
    *(int *)(scene + 0x469c) = 0;
    *(int *)(scene + 0x46a0) = 0;
    *(u8 *)(scene + 0x46a6) = 0;
    *(u8 *)(scene + 0x46a7) = 0;
    *(u8 *)(scene + 0x46a5) = 0;
    Sound::PlayBank2_2D(0x1fd);
}

// @symbol _ZN12dScMgPanel_c11Scroll_StopEv
void dScMgPanel_c::Scroll_Stop()
{
    char *scene = (char *)this;
    ((PanelScroll *)(scene + 0x4694))->on = 0;
    SetBg2Offset(0, 0);
    SetSubBg2Offset(0, 0);
}

// @symbol _ZN12dScMgPanel_c18EnableOverlayLayerEv
void dScMgPanel_c::EnableOverlayLayer()
{
    u8 *scene = (u8 *)this;
    extern u8 data_0209d454[];
    if (scene[0x4693] != 0) return;
    scene[0x4693] = 1;
    *(u16 *)(scene + 0x4600 + 0x90) = 0;
    data_0209d454[0] |= 1;
}

// @symbol _ZN12dScMgPanel_c18Shake_StateLoseTryEi
void dScMgPanel_c::Shake_StateLoseTry(int index) {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    extern u8 data_0209d454;
    extern int data_ov006_0212ed00[];
    int n = index * 0xc;
    char *base = scene + 0x4690;
    int cnt;
    *(u16 *)(base + n) = *(u16 *)(base + n) + 1;
    cnt = *(u16 *)(base + n);
    if ((u32)cnt >= 0x20) {
        *(u16 *)(base + n) = 0;
        *(u8 *)(scene + n + 0x4000 + 0x693) = 0;
        SetSubBg0Offset(0, 0);
        data_0209d454 &= ~1;
        func_02012790(0x12f);
        s->mCredits -= 1;
        return;
    }
    *(int *)(scene + 0x468c + n) = data_ov006_0212ed00[cnt >> 3];
    {
        char *q = scene + n + 0x4600;
        u16 t = *(u16 *)(q + 0x90);
        if (t == 1 || t == 0x11)
            Sound::PlayBank2_2D(0x1fc);
    }
    SetSubBg0Offset(0, *(int *)(scene + 0x468c + n));
}

// @symbol _ZN12dScMgPanel_c16Shake_StateShakeEi
extern "C" {
void SetSubBg0Offset(int a, int b);

}
void dScMgPanel_c::Shake_StateShake(int index) {
    char *scene = (char *)this;
    extern int sPanelShakeOffsets[];
    int m = index * 0xc;
    char *b = scene + 0x4690;
    u16 h = *(u16 *)(b + m);
    h = h + 1;
    *(u16 *)(b + m) = h;
    h = *(u16 *)(b + m);
    if (h >= 0x20) {
        *(s16 *)(b + m) = 0;
        *(u8 *)(scene + m + 0x4000 + 0x693) = 2;
        return;
    }
    {
        int t = sPanelShakeOffsets[h >> 3];
        int *dst = (int *)(scene + 0x468c + m);
        u16 st;
        *dst = t;
        st = *(u16 *)(scene + m + 0x4600 + 0x90);
        if (st == 1 || st == 0x11) Sound::PlayBank2_2D(0x1fc);
        SetSubBg0Offset(0, *dst);
    }
}

// @symbol _ZN12dScMgPanel_c15Shake_StateIdleEi
void dScMgPanel_c::Shake_StateIdle(int index) {}

// @symbol _ZN12dScMgPanel_c12Shake_UpdateEv
extern "C" {
struct PanelC_ac4 {
    char pad[0x4692];
    u8 guard;
    u8 idx;
};
}
void dScMgPanel_c::Shake_Update()
{
    PanelC_ac4 *c = (PanelC_ac4 *)this;
    extern PanelStateFnI sPanelShakeStateTable[];
    if (c->guard == 0) return;
    int j = c->idx;
    (((dScMgPanel_c *)c)->*sPanelShakeStateTable[j])(0);
}

// @symbol _ZN12dScMgPanel_c11Shake_StartEv
void dScMgPanel_c::Shake_Start()
{
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    self->mShake[0].active = 1;
    self->mShake[0].unk_00 = 0;
    self->mShake[0].offset = 0;
    self->mShake[0].timer = 0;
    self->mShake[0].state = 0;
}

// @symbol _ZN12dScMgPanel_c10Shake_StopEv
void dScMgPanel_c::Shake_Stop()
{
    char *scene = (char *)this;
    *(char *)(scene + 0x4692) = 0;
}

// @symbol _ZN12dScMgPanel_c11Banner_DrawEv
void dScMgPanel_c::Banner_Draw()
{
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    extern void *sPanelBannerSprites[];
    if (self->mBanner.visible == 0) return;
    Mg_RenderSubSpriteMtx(sPanelBannerSprites[0], self->mBanner.x >> 12, self->mBanner.y >> 12, 0);
}

// @symbol _ZN12dScMgPanel_c16Banner_StateIdleEv
void dScMgPanel_c::Banner_StateIdle() {}

// @symbol _ZN12dScMgPanel_c18Banner_StateSettleEv
void dScMgPanel_c::Banner_StateSettle() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    u16 *p = &self->mBanner.hold;
    *p = *p + 1;
    if (self->mBanner.hold < 8) return;
    self->mBanner.hold = 0;
    {
        int *q = &self->mBanner.y;
        *q = *q + 0x4000;
    }
    self->mBanner.state = 2;
}

// @symbol _ZN12dScMgPanel_c17Banner_StatePopUpEv
void dScMgPanel_c::Banner_StatePopUp() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    u16 *h = &self->mBanner.hold;
    *h = *h + 1;
    if (self->mBanner.hold < 4) return;
    self->mBanner.hold = 0;
    {
        int *w = &self->mBanner.y;
        *w = *w - 0x4000;
    }
    self->mBanner.state = 1;
}

// @symbol _ZN12dScMgPanel_c13Banner_UpdateEv
void dScMgPanel_c::Banner_Update()
{
    char *scene = (char *)this;
    extern PanelStateFn sPanelBannerStateTable[];
    u16 *ip;
    if (*(u8 *)(scene + 0x4000 + 0x684) == 0) return;
    ip = (u16 *)(scene + 0x4680);
    *ip = *ip - 1;
    if (*(s16 *)(scene + 0x4600 + 0x80) <= 0) {
        *(s16 *)(scene + 0x4600 + 0x80) = 0;
        *(u8 *)(scene + 0x4000 + 0x684) = 0;
        *(u8 *)(scene + 0x4000 + 0x685) = 0;
        return;
    }
    {
        dScMgPanel_c *cc = (dScMgPanel_c *)scene;
        (cc->*sPanelBannerStateTable[*(u8 *)(scene + 0x4000 + 0x686)])();
    }
}

// @symbol _ZN12dScMgPanel_c11Banner_ShowEi
void dScMgPanel_c::Banner_Show(int i)
{
    char *scene = (char *)this;
    char *e = scene + (i << 2);
    *(u8 *)(scene + 0x4684) = 1;
    *(u8 *)(scene + 0x4685) = 1;
    *(int *)(scene + 0x4678) = *(int *)(e + 0x4cc4);
    *(int *)(scene + 0x467c) = *(int *)(e + 0x4d54) + 0x8000;
    *(s16 *)(scene + 0x4680) = 0x30;
    *(s16 *)(scene + 0x4682) = 0;
    *(u8 *)(scene + 0x4686) = 0;
}

// @symbol _ZN12dScMgPanel_c10DrawCursorEv
void dScMgPanel_c::DrawCursor()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    extern void *sPanelCursorSprites[];
    if (*(u8 *)(scene + 0x4fe3) == 0) return;
    Hud_RenderSprite(sPanelCursorSprites[s->mCredits], 0xf0, 0x24, -1, -1);
}

// @symbol _ZN12dScMgPanel_c7DrawHudEv
void dScMgPanel_c::DrawHud()
{
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    extern void *sPanelHudSprites[];
    extern void *data_ov006_02136de8[];
    if (self->mSlideFlag == 0) return;
    if (self->mSlideStep != 4) {
        int a = self->mSlide >> 12;
        int b = self->mSlideY >> 12;
        int index;
        Mg_DrawBigNumber(a, b, self->mFlipCount, -1, -1, 0, 0);
        index = 1;
        if (self->mFlipCount >= 2) index = 0;
        Mg_RenderSpriteBoth(sPanelHudSprites[index], a + 0x2c, b, -1, -1);
    } else {
        int x = self->mSlideLeft;
        Hud_RenderSprite(data_ov006_02136de8[x],
                            self->mSlide >> 12,
                            self->mSlideY >> 12,
                            -1, -1);
    }
}

// @symbol _ZN12dScMgPanel_c16ReloadSlideDelayEv
void dScMgPanel_c::ReloadSlideDelay()
{
    u8 *scene = (u8 *)this;
    scene += 0x4000;
    scene[0x677] = scene[0xfde];
}

// @symbol _ZN12dScMgPanel_c14TickSlideDelayEv
// If the byte flag at self+0x4677 is set, decrement it. The read folds to add+ldrb
// (offset > 0xfff), while the laundered RMW pool-loads the offset, matching the ROM's
// predicated ldrbne/subne/strbne tail.
void dScMgPanel_c::TickSlideDelay()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (s->mSlideLeft)
        s->mSlideLeft -= 1;
}

// @symbol _ZN12dScMgPanel_c14ClearSlideFlagEv
void dScMgPanel_c::ClearSlideFlag()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlideFlag = 0;
}

// @symbol _ZN12dScMgPanel_c12SetSlideFlagEv
void dScMgPanel_c::SetSlideFlag()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlideFlag = 1;
}

// @symbol _ZN12dScMgPanel_c19IntroCard_StateDoneEv
void dScMgPanel_c::IntroCard_StateDone() {}

// @symbol _ZN12dScMgPanel_c23IntroCard_StateSlideOutEv
void dScMgPanel_c::IntroCard_StateSlideOut() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide += s->mSlideVel;
    s->mSlideVel -= 0x400;
    if ((s->mSlide >> 12) > -0x40) return;
    s->mIntro++;
    s->mSlideStep = 4;
    s->mSlide = 0x10000;
    s->mSlideY = 0x24000;
    FreeGfxSlotsById(0x1d);
    if (s->mPromptBlinkCount == 0) {
        s->mPromptEnabled = 1;
        s->mPromptBlinkCount = 1;
        s->mPromptBlinkTimer = 0;
    }
    *(u8 *)(scene + 0x4fe3) = 1;
    s->mSlideLeft = s->mFlipCount;
    s->mHold = 0;
    ShowAllPanels();
    Sound::PlayBank2_2D(0x1fb);
}

// @symbol _ZN12dScMgPanel_c19IntroCard_StateHoldEv
void dScMgPanel_c::IntroCard_StateHold() {
    u8 *scene = (u8 *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (s->mHold != 0) {
        u16 *p = &s->mHold;
        *p = *p - 1;
        if (*(s16 *)&s->mHold < 0) s->mHold = 0;
    } else {
        s->mSlideVel = -0x3000;
        s->mSlideStep = 3;
    }
}

// @symbol _ZN12dScMgPanel_c22IntroCard_StateSlideInEv
void dScMgPanel_c::IntroCard_StateSlideIn() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide += s->mSlideVel;
    s->mSlideVel -= 0x400;
    if (s->mSlide >> 12 > 0x58) return;
    s->mSlide = 0x58000;
    s->mSlideStep = 2;
    s->mHold = 0x80;
}

// @symbol _ZN12dScMgPanel_c20IntroCard_StateStartEv
void dScMgPanel_c::IntroCard_StateStart() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide = 0x110000;
    s->mSlideY = 0x60000;
    s->mSlideVel = -0x3000;
    s->mSlideFlag = 1;
    s->mSlideStep = 1;
}

// @symbol _ZN12dScMgPanel_c16IntroCard_UpdateEv
extern "C" {
struct PanelC_0bc { char pad[0x4674]; u8 guard; u8 idx; };
}
void dScMgPanel_c::IntroCard_Update()
{
    PanelC_0bc *c = (PanelC_0bc *)this;
    extern PanelStateFn sPanelIntroCardStateTable[];
    if (!c->guard) return;
    (((dScMgPanel_c *)c)->*sPanelIntroCardStateTable[c->idx])();
}

// @symbol _ZN12dScMgPanel_c15IntroCard_StartEv
void dScMgPanel_c::IntroCard_Start()
{
    char *scene = (char *)this;
    *(char *)(scene + 0x4674) = 1;
    *(char *)(scene + 0x4675) = 0;
    *(char *)(scene + 0x4677) = 0;
}

// @symbol _ZN12dScMgPanel_c15StartSolveCheckEv
void dScMgPanel_c::StartSolveCheck()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (mBusy != 0)
        return;
    if (mState == 6)
        return;
    if (*(u8 *)(scene + 0x4fe1) < mFlipCount)
        return;

    func_02012790(0xe);
    EnableOverlayLayer();

    if (s->mCredits > 1) {
        mState = 3;
        CountMatches();
    } else {
        *(u8 *)(scene + 0x4fe6) = 0;
        CountMatches();
        mState = 6;
        *(s16 *)(scene + 0x4ec0) = 0x50;
    }
}

// @symbol _ZN12dScMgPanel_c24Answer_StateFlipSolutionEv
void dScMgPanel_c::Answer_StateFlipSolution() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int index;
    int w;
    int col;
    int row;
    int hq;
    int wq;
    int y;
    int x;

    if ((u16)s->mDelay != 0) {
        (*(u16 *)(int)(scene + 0x4ec4))--;
        if (s->mDelay < 0)
            s->mDelay = 0;
        return;
    }

    if (*(u8 *)(scene + 0x4fe5) >= s->mFlipCount) {
        *(u16 *)(scene + 0x4ec0) = 0x50;
        s->mState = 6;
        *(u8 *)(scene + 0x4fe6) = 0;
        return;
    }

    index = *(u8 *)(scene + *(u8 *)(scene + 0x4fe5) + 0x4fae);
    w = s->mWidth;
    hq = 3;
    wq = hq;

    col = index % w;
    row = index / w;

    if (col == 0 || col == w - 1)
        wq = 2;
    if (row == 0 || row == w - 1)
        hq = 2;

    if (--col < 0)
        col = 0;
    if (--row < 0)
        row = 0;

    for (y = 0; y < hq; y++) {
        for (x = 0; x < wq; x++) {
            int cell = s->mWidth * (row + y) + (col + x);

            s->mKind[cell] = 1;
            s->mFace[cell] ^= 1;
            *(u16 *)(scene + cell * 2 + 0x4e78) = 8;
        }
    }

    Banner_Show(index);
    *(u16 *)(scene + index * 2 + 0x4e78) = 0;
    s->mDelay = 0x30;
    (*(u8 *)(int)(scene + 0x4fe5))++;
}

// @symbol _ZN12dScMgPanel_c24Answer_StateFindSolutionEv
void dScMgPanel_c::Answer_StateFindSolution() {
    Ov006M8c *scene = (Ov006M8c *)this;
    extern u8 *sPanelPuzzles[];
    extern u8 *sPanelPuzzlesAlt[];
    char *c = (char *)scene;
    int n;
    int found;
    int i;
    int j;
    int li;
    int nlights;
    int arr[36];
    int idx;
    int wq;
    int hq;
    int col;
    int row;
    int y;
    int x;
    int cell;
    u8 *p;

    if (((u16 *)(c + 0x4e00))[0x62] != 0) {
        (*(u16 *)(c + 0x4ec4))--;
        if (((s16 *)(c + 0x4e00))[0x62] < 0) {
            ((s16 *)(c + 0x4e00))[0x62] = 0;
        }
        return;
    }

    n = *(int *)(c + 0x4cb8);
    found = 0;
    i = found;
    if (n > 0) {
        do {
            if (*(u8 *)(c + i + 0x4efa) != 0) {
                found++;
                break;
            }
            i++;
        } while (i < n);
    }
    if (found) {
        return;
    }

    i = 0;
    do {
        arr[i] = 0;
        i++;
    } while (i < 0x24);
    j = 0;

    if (n > 0) {
        do {
            idx = *(int *)(c + 0x4cb4);
            if (scene->m8c() != 0) {
                arr[j] = sPanelPuzzlesAlt[idx][j];
            } else {
                arr[j] = sPanelPuzzles[idx][j];
            }
            n = *(int *)(c + 0x4cb8);
            j++;
        } while (j < n);
    }

    nlights = *(u8 *)(c + 0x4fde);
    li = 0;
    if (nlights > 0) {
        do {
            idx = *(u8 *)(c + li + 0x4fae);
            wq = 3;
            hq = 3;
            col = idx % scene->width;
            row = idx / scene->width;
            if (col == 0 || col == scene->width - 1) {
                wq = 2;
            }
            if (row == 0 || row == scene->width - 1) {
                hq = 2;
            }
            if (--col < 0) {
                col = 0;
            }
            if (--row < 0) {
                row = 0;
            }
            y = 0;
            if (hq > 0) {
                do {
                    x = 0;
                    if (wq > 0) {
                        do {
                            cell = scene->width * (row + y) + (col + x);
                            arr[cell] ^= 1;
                            x++;
                        } while (x < wq);
                    }
                    y++;
                } while (y < hq);
            }
            li++;
        } while (li < nlights);
    }

    j = 0;
    if (n > 0) {
        do {
            p = (u8 *)(c + j + 0x4f1e);
            if (*p != arr[j]) {
                *(u8 *)(c + j + 0x4efa) = 1;
                *p ^= 1;
                *(u8 *)(c + j + 0x4f8a) = 1;
            }
            j++;
        } while (j < *(int *)(c + 0x4cb8));
    }

    ((u16 *)(c + 0x4e00))[0x62] = 0x40;
    *(u8 *)(c + 0x4fe0) = 3;
    *(u8 *)(c + 0x4fe5) = 0;
}

// @symbol _ZN12dScMgPanel_c22Answer_StateResetBoardEv
void dScMgPanel_c::Answer_StateResetBoard() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    if (((u16 *)(scene + 0x4e00))[0x62] != 0) {
        (*(u16 *)(scene + 0x4ec4))--;
        if (((s16 *)(scene + 0x4e00))[0x62] < 0) {
            ((s16 *)(scene + 0x4e00))[0x62] = 0;
        }
        return;
    } else {
        int k;
        for (k = 0; k < self->mCount; k++) {
            *(u8 *)(scene + k + 0x4efa) = 1;
            *(u8 *)(scene + k + 0x4f1e) ^= 1;
            *(u8 *)(scene + k + 0x4f8a) = 1;
        }
        *(u8 *)(scene + 0x4fe0) = 2;
        ((u16 *)(scene + 0x4e00))[0x62] = 0x20;
    }
}

// @symbol _ZN12dScMgPanel_c22Answer_StateWaitButtonEv
extern "C" {
typedef struct WarpEntry_730 {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
} WarpEntry_730;

}
void dScMgPanel_c::Answer_StateWaitButton() {
    char *c = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)c;
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
    extern u8 gActivePlayerSlot;
// local extern: see above.
    extern WarpEntry_730 gTouchHeld[];
    u8 idx;
    s32 b;
    s32 dx;
    s32 dy;

    if (s->mBusy != 0) {
        return;
    }
    idx = gActivePlayerSlot;
    b = 0;
    if (gTouchHeld[idx].a != 0) {
        b = (gTouchHeld[idx].b != 0);
    }
    if (b == 0) {
        return;
    }
    dx = gTouchHeld[idx].c - 0xe8;
    dy = gTouchHeld[idx].d - 0x10;
    if (dx < -15) {
        return;
    }
    if (dx > 15) {
        return;
    }
    if (dy < -12) {
        return;
    }
    if (dy > 12) {
        return;
    }
    *(u8 *)(c + 0x4fe0) = 1;
    s->mState = 5;
    s->mDelay = 0x20;
}

// @symbol _ZN12dScMgPanel_c13Answer_UpdateEv
extern "C" {
struct PanelB_7f0 {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int m();
};
struct PanelC_7f0 : PanelB_7f0 { char pad[0x4fdc]; u8 idx; };
}
void dScMgPanel_c::Answer_Update() {
    PanelC_7f0 *c = (PanelC_7f0 *)this;
    extern PanelStateFn sPanelAnswerStateTable[];
    if (c->m() != 0) return;
    (((dScMgPanel_c *)c)->*sPanelAnswerStateTable[c->idx])();
    Banner_Update();
}

// @symbol _ZN12dScMgPanel_c11UpdateMovesEv
void dScMgPanel_c::UpdateMoves()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (*(u16 *)(scene + 0x4ec4) != 0) {
        (*(u16 *)(int)(scene + 0x4ec4))--;
        if (mDelay < 0)
            *(u16 *)(scene + 0x4ec4) = 0;
        return;
    }

    {
        int found = 0;
        int i;
        int n = s->mCount;
        for (i = 0; i < n; i++) {
            if (*(u8 *)(scene + i + 0x4efa) != 0) {
                found++;
                break;
            }
        }
        if (found)
            return;
    }

    if (*(u8 *)(scene + 0x4fe4) == 0xff) {
        *(u8 *)(scene + 0x4fe4) = 0;
        mState = 2;
        SetSlideFlag();
        ReloadSlideDelay();
        return;
    }

    (*(u8 *)(int)(scene + 0x4fe4))--;

    if (*(u8 *)(scene + 0x4fe4) == 0xff) {
        *(u8 *)(scene + 0x4fe4) = 0;
        mState = 2;
        *(u8 *)(scene + 0x4fe1) = 0;
        SetSlideFlag();
        ReloadSlideDelay();
        return;
    }

    {
        u8 slot = *(u8 *)(scene + 0x4fe4);
        int hq = 3;
        int index = *(u8 *)(scene + slot + 0x4fce);
        int w = mWidth;
        int wq = 3;
        int col = index % w;
        int row = index / w;
        int x, y;

        if (col == 0 || col == w - 1)
            wq = 2;
        if (row == 0 || row == w - 1)
            hq = 2;
        if (--col < 0)
            col = 0;
        if (--row < 0)
            row = 0;

        for (y = 0; y < hq; y++) {
            for (x = 0; x < wq; x++) {
                int cell = mWidth * (row + y) + (col + x);
                *(u8 *)(scene + cell + 0x4efa) = 1;
                s->mFace[cell] ^= 1;
                *(u16 *)(scene + cell * 2 + 0x4e78) = 8;
                *(u8 *)(scene + cell + 0x4f8a) = 1;
            }
        }
        *(u16 *)(scene + index * 2 + 0x4e78) = 0;
        Sound::PlayBank2_2D(0x1fb);
    }
}

// @symbol _ZN12dScMgPanel_c10DrawPanelsEv
extern "C" {
typedef struct PanelObj_ab4
{
    char _p0[0x4ca8];
    int mode;                                  /* 0x4ca8 */
    char _p1[0x4cb8 - 0x4ca8 - 4];
    int count;                                 /* 0x4cb8 */
    char _p2[0x4cc4 - 0x4cb8 - 4];
    int xarr[(0x4d54 - 0x4cc4) / 4];           /* 0x4cc4 */
    int yarr[(0x4de8 - 0x4d54) / 4];           /* 0x4d54 */
    u16 harr[(0x4efa - 0x4de8) / 2];/* 0x4de8 */
    u8 tarr[0x4f1e - 0x4efa];       /* 0x4efa */
    u8 karr[0x4f42 - 0x4f1e];       /* 0x4f1e */
    u8 parr[0x4f66 - 0x4f42];       /* 0x4f42 */
    u8 flagarr[0x4fe8 - 0x4f66];    /* 0x4f66 */
    u8 q;                           /* 0x4fe8 */
} PanelObj_ab4;

}
void dScMgPanel_c::DrawPanels()
{
    PanelObj_ab4 *scene = (PanelObj_ab4 *)this;
    extern void *sPanelSprites[];
    extern u8 sPanelFlipFrameSprites[];
    int x;
    int y;
    int v;
    int t;
    int k;
    int index;
    int i;

    for (i = 0; i < scene->count; i++) {
        if (scene->flagarr[i] != 0) {
            v = 0;

            if (scene->mode == 6) {
                v = 1;
            }

            t = scene->tarr[i];
            k = scene->karr[i];
            x = scene->xarr[i] >> 12;
            y = scene->yarr[i] >> 12;

            index = k * 3;
            if (t == 1) {
                int h = scene->harr[i];
                index = sPanelFlipFrameSprites[k * 5 + h];
            }

            Mg_RenderSubSpritePalMtx(sPanelSprites[index], x, y, -1, v, 0);

            if (scene->mode == 1) {
                int p = scene->parr[i];
                int q = scene->q;
                DrawOamSprite(sPanelSprites[p * 3 + q], (void *)x, y, 0);
            } else {
                int p = scene->parr[i];
                DrawOamSprite(sPanelSprites[p * 3], (void *)x, y, 0);
            }
        }
    }
}

// @symbol _ZN12dScMgPanel_c12CountMatchesEv
void dScMgPanel_c::CountMatches()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int i = 0;

    if (s->mCount <= 0) return;
    do {
        if (s->mFace[i] != s->mGoal[i]) {
            s->mKind[i] = 2;
            *(u16 *)(scene + (int)(((long long)i)) * 2 + 0x4e30) = 0;
        }
        i++;
    } while (i < s->mCount);
}

// @symbol _ZN12dScMgPanel_c13CountFlippingEv
void dScMgPanel_c::CountFlipping()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int cnt;
    int i;
    cnt = 0;
    if (s->mBusy != 0) return;

    for (i = 0; i < s->mCount; i++) {
        if (s->mFace[i] != s->mGoal[i]) {
            cnt++;
            break;
        }
    }
    if (cnt != 0) return;

    s->mState = 6;
    *(u8 *)(scene + 0x4fe6) = 1;
    *(s16 *)(scene + 0x4ec0) = 0x40;
}

// @symbol _ZN12dScMgPanel_c11CheckSolvedEv
void dScMgPanel_c::CheckSolved()
{
    char *scene = (char *)this;
    int found;
    int i;
    int n;
    int tmp;
    if (mBusy == 0)
        return;
    if (*(u8 *)(scene + 0x4feb) == 0) return;
    /* Laundered volatile round trip: the `(int)` cast and the volatile are both
       load-bearing -- they are what turns this decrement into the ROM's
       ldrb/sub/strb instead of a folded one. */
    tmp = *(volatile u8 *)(int *)(((int)scene + 0x4feb));
    --tmp;
    *(volatile u8 *)(int *)(((int)scene + 0x4feb)) = tmp;
    if (*(u8 *)(scene + 0x4feb) != 0) return;
    found = 0;
    i = 0;
    n = ((dScMgPanel_c *)scene)->mCount;
    while (i < n) {
        if (*(u8 *)(scene + i + 0x4f1e) != *(u8 *)(scene + i + 0x4f42)) {
            found++;
            break;
        }
        i++;
    }
    if (found != 0) return;
    Sound::PlayBank2_2D(0x1cd);
}

// @symbol _ZN12dScMgPanel_c11UpdateTouchEv
extern "C" {
/* Find the cursor slot whose fixed-point position is
 * within 16px of the level's target cell, mark it captured, flip a 2x2 or 3x3
 * quad of grid cells around it and start their flash timers.
 */
typedef struct PanelObj_de4 {
    char _p0[0x4cb8];
    int count;                       /* 0x4cb8 */
    int w;                           /* 0x4cbc */
    char _p1[4];
    int xarr[(0x4d54 - 0x4cc4) / 4]; /* 0x4cc4 */
    int yarr[(0x4de8 - 0x4d54) / 4]; /* 0x4d54 */
} PanelObj_de4;

}
void dScMgPanel_c::UpdateTouch()
{
    char *scene = (char *)this;
// local extern: see above.
    extern u8 gActivePlayerSlot;
// local extern: see above.
    extern u8 gTouchHeld[];
// local extern: see above.
    extern u8 gTouchEdge[];
    u8 lvl;
    int n;
    int i;
    int tx, ty;

    if (mBusy != 0)
        return;

    lvl = gActivePlayerSlot;
    n = 0;
    if (gTouchHeld[lvl * 4] != 0)
        n = gTouchEdge[lvl * 4] != 0;
    if (n == 0)
        return;

    i = 0;
    n = ((PanelObj_de4 *)scene)->count;
    if (n <= 0)
        return;

    {
        u8 *e = &gTouchHeld[lvl * 4];
        tx = e[2];
        ty = e[3];
    }

    do {
        int dx = tx - (((PanelObj_de4 *)scene)->xarr[i] >> 12);
        int dy = ty - (((PanelObj_de4 *)scene)->yarr[i] >> 12);
        if (dx >= -0x10 && dx <= 0x10 && dy >= -0x10 && dy <= 0x10) {
            int wq = 3;
            int hq = 3;
            int w, col, row;

            *(u8 *)(scene + *(u8 *)(scene + 0x4fe4) + 0x4fce) = i;
            (*(u8 *)(int)(scene + 0x4fe4))++;
            w = mWidth;
            col = i % w;
            row = i / w;

            if (col == 0 || col == w - 1)
                wq = 2;
            if (row == 0 || row == w - 1)
                hq = 2;

            if (--col < 0)
                col = 0;
            if (--row < 0)
                row = 0;

            {
                int y;
                for (y = 0; y < hq; y++) {
                    int x;
                    for (x = 0; x < wq; x++) {
                        int index = mWidth * (row + y) + (col + x);
                        ((dScMgPanel_c *)scene)->mKind[index] = 1;
                        ((dScMgPanel_c *)scene)->mFace[index] ^= 1;
                        *(u16 *)(scene + index * 2 + 0x4e78) = 8;
                    }
                }
            }

            *(u16 *)(scene + i * 2 + 0x4e78) = 0;
            *(u8 *)(scene + 0x4feb) = 0x28;
            (*(u8 *)(int)(scene + 0x4fe1))++;
            Sound::PlayBank2_2D(0x1fa);
            TickSlideDelay();
            return;
        }
    } while (++i < n);
}

// @symbol _ZN12dScMgPanel_c13ShowAllPanelsEv
/* Show every panel (intro end). */
void dScMgPanel_c::ShowAllPanels()
{
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int i;
    for (i = 0; i < s->mCount; i++) {
        s->mVisible[i] = 1;
    }
}

// @symbol _ZN12dScMgPanel_c6FlipAtEi
extern "C" {
/* Flip the faces in the 3x3 neighbourhood centred on panel `idx`, clipped at
   the board edges.  The same walk appears in Answer_StateFlipSolution,
   UpdateMoves and UpdateTouch; only this one touches the face
   table alone. */
}
void dScMgPanel_c::FlipAt(int index)
{
    char *scene = (char *)this;
    int w = mWidth;
    int wq = 3;
    int hq = 3;
    int col = index % w;
    int row = index / w;
    if (col == 0 || col == w - 1) wq = 2;
    if (row == 0 || row == w - 1) hq = 2;
    col = col - 1;
    if (col < 0) col = 0;
    row = row - 1;
    if (row < 0) row = 0;
    int y, x;
    for (y = 0; y < hq; y++) {
        for (x = 0; x < wq; x++) {
            u8 *p = (u8 *)((((int)scene + ((mWidth) * (row + y) + (col + x))) + 0x4f1e));
            *p ^= 1;
        }
    }
}

// @symbol _ZN12dScMgPanel_c10DealPuzzleEv
extern "C" {
/* Layout pass: reads the panel layout (x and y from the layout tables,
   face from the face table picked by IsAltVariant), draws mFlips random picks
   through FlipAt, and retries the whole thing until
   HasDuplicateRows accepts it, at least one panel face differs from its
   copy, and no two picks are the same panel.

   Register shape: the loop-1 counter and the loop-2 counter are DIFFERENT
   locals (i, j); a single `i` for both loops colours out of the callee-saved
   band (r5) no matter where it is declared. With them split, the declaration
   order i, v, cnt2, j, cnt1, pd, pe is what puts i and j on r7, v on sb, cnt2
   on r8, cnt1 on r6 and the ded0 table pointer on r5; every other slot for j
   rotates loop 2 or loop 1. The pd/pe pointer locals keep the byte tables
   hoisted above the do-loop and the [idx4] reload inside it. */

}
void dScMgPanel_c::DealPuzzle()
{
    extern int data_0209d4b8;
    extern u16 *sPanelPuzzleXPerMode[];
    extern u16 *sPanelPuzzleYPerMode[];
    extern u8 *sPanelPuzzlesAlt[];
    extern u8 *sPanelPuzzles[];
    int i;
    int v;
    int cnt2;
    int j;
    int cnt1;
    u8 **pd;
    u8 **pe;
    int mismatch, dup, a, b, k;
    do {
        mFlipCount = 0;
        for (i = 0; i < (cnt1 = mCount); i++) {
            int m = mWidth - 4;
            int idx4 = mFaceSet;
            mX[i] = sPanelPuzzleXPerMode[m][i] << 12;
            mY[i] = sPanelPuzzleYPerMode[m][i] << 12;
            pe = sPanelPuzzles;
            pd = sPanelPuzzlesAlt;
            if (IsAltVariant()) {
                mFace[i] = pd[idx4][i];
                mGoal[i] = pd[idx4][i];
            } else {
                mFace[i] = pe[idx4][i];
                mGoal[i] = pe[idx4][i];
            }
        }
        cnt2 = mFlips;
        for (j = 0; j < cnt2; j++) {
            u32 rnd = (u32)RandomIntInternal(&data_0209d4b8);
            u32 s = (rnd >> 16) & 0x7fff;
            v = (cnt1 * s) >> 15;
            FlipAt(v);
            mFlipAt[mFlipCount] = v;
            mFlipCount++;
        }
        mismatch = 0;
        for (k = 0; k < mCount; k++) {
            if (mFace[k] != mGoal[k]) {
                mismatch++;
                break;
            }
        }
        dup = 0;
        for (a = 0; a < cnt2; a++) {
            for (b = a + 1; b < cnt2; b++) {
                if (mFlipAt[a] == mFlipAt[b]) {
                    dup++;
                    break;
                }
            }
        }
        if (dup != 0) mismatch = 0;
    } while (HasDuplicateRows() == 0 || mismatch == 0);
}

// @symbol _ZN12dScMgPanel_c14ScramblePanelsEv
#pragma push
#pragma opt_common_subs off
extern "C" {
/* Address launder and the 15-bit random draw, both used a dozen times below.
   The launder is what stops mwcc folding the field address into the
   surrounding expression; Particle_Spawn spells out the same lever. */
#define PANEL_AT(p, off) ((void *)(int)((char *)(p) + (off)))
#define PANEL_RAND ((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)

struct PanelO_3a0
{
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int m8c();
};

}
void dScMgPanel_c::ScramblePanels()
{
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    extern int data_0209d4b8;
    extern u8 sPanelScrambleFlipEnabled[];
    extern u8 sPanelScrambleFlips[];
    extern u8 sPanelScrambleWidths[];
    extern u8 sPanelScrambleFlipEnabledAlt[];
    extern u8 sPanelScrambleFlipsAlt[];
    extern u8 sPanelPuzzleSizes[];
    int t = (int)self->mLevel;
    if (((PanelO_3a0 *)scene)->m8c() != 0) {
        int nv;
        if (t >= 0xf) {
            t = (PANEL_RAND * 0xf) >> 15;
            if (t == *(int *)(scene + 0x4de4)) {
                t += ((PANEL_RAND * 0xe) >> 15) + 1;
                if (t >= 0xf) t -= 0xf;
            }
        }
        if (sPanelScrambleFlipEnabled[t] != 0 || self->mDealt == 0) {
            if (self->mDealt == 0) *(int *)PANEL_AT(scene, 0x4cb0) += 1;
            nv = (PANEL_RAND * 8) >> 15;
            if (nv == self->mFaceSet) {
                nv = (nv + (((PANEL_RAND * 7) >> 15) + 1)) & 7;
            }
            self->mFaceSet = nv;
        }
        ((dScMgPanel_c *)scene)->mWidth = sPanelScrambleWidths[t];
        { int q = ((dScMgPanel_c *)scene)->mWidth; ((dScMgPanel_c *)scene)->mCount = q * q; }
        self->mFlips = sPanelScrambleFlips[t];
        *(int *)(scene + 0x4de4) = t;
    } else {
        int nv;
        int k;
        u8 *p;
        while (t >= 0x19) t -= 0x19;
        if (sPanelScrambleFlipEnabledAlt[t] != 0 || self->mDealt == 0) {
            if (self->mDealt == 0) *(int *)PANEL_AT(scene, 0x4cb0) += 1;
            nv = (PANEL_RAND * 4) >> 15;
            k = sPanelPuzzleSizes[t];
            p = &sPanelPuzzleSizes[t];
            if (k == 5) {
                nv = ((PANEL_RAND * 5) >> 15) + 4;
            }
            if (nv == self->mFaceSet) {
                if (*p == 4) {
                    nv = (nv + (((PANEL_RAND * 3) >> 15) + 1)) & 3;
                } else {
                    nv = nv + (((PANEL_RAND * 4) >> 15) + 1);
                    if (nv >= 9) nv -= 5;
                }
            }
            self->mFaceSet = nv;
        }
        ((dScMgPanel_c *)scene)->mWidth = sPanelPuzzleSizes[t];
        { int q = ((dScMgPanel_c *)scene)->mWidth; ((dScMgPanel_c *)scene)->mCount = q * q; }
        self->mFlips = sPanelScrambleFlipsAlt[t];
    }
}
#undef PANEL_AT
#undef PANEL_RAND
#pragma pop

// @symbol _ZN12dScMgPanel_c16HasDuplicateRowsEv
#pragma push
#pragma opt_strength_reduction off
int dScMgPanel_c::HasDuplicateRows()
{
    char *scene = (char *)this;
    int i, bad, j;
    char *row = scene;

    /* Refuse a pick set that repeats any of the last five. */
    for (i = 0; i < 5; i++) {
        bad = 0;
        for (j = 0; j < 0xa; j++) {
            if (*(u8 *)(scene + 0x4fae + j) != *(u8 *)(row + 0x4ec8 + j)) {
                bad = 1;
                break;
            }
        }
        if (bad == 0) return 0;
        row += 0xa;
    }
    /* Shift the history down one row, then record this pick set as row 0. */
    {
        int r, k;
        char *dst;
        char *src = scene;
        for (r = 0; r < 4; r++) {
            dst = scene + (r + 1) * 0xa;
            for (k = 0; k < 0xa; k++) {
                *(u8 *)(dst + 0x4ec8) = *(u8 *)(src + 0x4ec8 + k);
                dst++;
            }
            src += 0xa;
        }
    }
    for (i = 0; i < 0xa; i++) {
        *(u8 *)(scene + 0x4ec8 + i) = *(u8 *)(scene + 0x4fae + i);
    }
    return 1;
}
#pragma pop

// @symbol _ZN12dScMgPanel_c9ClearGridEv
void dScMgPanel_c::ClearGrid()
{
    char *scene = (char *)this;
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 0xa; j++) {
            *(u8 *)(scene + j + 0x4ec8) = 0xff;
        }
        scene += 0xa;
    }
}

// @symbol _ZN12dScMgPanel_c10ResetRoundEv
/* Clear the whole board back to its start-of-round state: the 36 parallel
   panel arrays, the state word, the pick list (0xff = no pick) and the
   cursor, then hand off to the three subsystem resets. */
#pragma push
#pragma opt_strength_reduction off
void dScMgPanel_c::ResetRound()
{
    char *scene = (char *)this;
    int i;
    for (i = 0; i < 0x24; i++) {
        *(int *)(scene + i * 4 + 0x4cc4) = 0;
        *(int *)(scene + i * 4 + 0x4d54) = 0;
        *(s16 *)(scene + i * 2 + 0x4de8) = 0;
        *(s16 *)(scene + i * 2 + 0x4e30) = 0;
        *(s16 *)(scene + i * 2 + 0x4e78) = 0;
        ((dScMgPanel_c *)scene)->mFace[i] = 0;
        ((dScMgPanel_c *)scene)->mKind[i] = 0;
        ((dScMgPanel_c *)scene)->mGoal[i] = 0;
        ((dScMgPanel_c *)scene)->mVisible[i] = 0;
        *(char *)(scene + i + 0x4f8a) = 0;
    }
    mState = 0;
    *(char *)(scene + 0x4fe9) = 0;
    *(s16 *)(scene + 0x4ec0) = 0;
    *(s16 *)(scene + 0x4ec2) = 0;
    *(char *)(scene + 0x4fde) = 0;
    *(char *)(scene + 0x4fe1) = 0;
    ((dScMgPanel_c *)scene)->mCredits = 0;
    for (i = 0; i < 0x20; i++) {
        *(u8 *)(scene + i + 0x4fae) = 0xff;
    }
    for (i = 0; i < 0x10; i++) {
        *(char *)(scene + i + 0x4fce) = 0;
    }
    *(char *)(scene + 0x4fe4) = 0;
    *(char *)(scene + 0x4fe0) = 0;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide = 0;
    s->mSlideY = 0;
    s->mSlideVel = 0;
    *(int *)(scene + 0x466c) = 0;
    s->mHold = 0;
    s->mSlideStep = 0;
    s->mSlideFlag = 0;
    s->mSlideOn = 0;
    *(int *)(scene + 0x4678) = 0;
    *(int *)(scene + 0x467c) = 0;
    *(s16 *)(scene + 0x4680) = 0;
    *(char *)(scene + 0x4684) = 0;
    *(char *)(scene + 0x4685) = 0;
    *(char *)(scene + 0x4686) = 0;
    *(char *)(scene + 0x4fe6) = 0;
    *(s16 *)(scene + 0x4ec6) = 0;
    Shake_Stop();
    Scroll_Stop();
    Particle_ClearAll();
}
#pragma pop

// @symbol _ZN12dScMgPanel_c14Panel_StartAllEv
void dScMgPanel_c::Panel_StartAll()
{
    char *scene = (char *)this;
    int i;
    for (i = 0; i < mWidth; i++) {
        *(u8 *)(scene + i + 0x4efa) = 3;
    }
}

// @symbol _ZN12dScMgPanel_c15Panel_StateFallEi
void dScMgPanel_c::Panel_StateFall(int index) {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int old = s->mY[index];
    s->mY[index] = old + 0x10000;
    if ((s->mY[index] >> 12) >= 0xd0) {
        s->mKind[index] = 0;
        s->mVisible[index] = 0;
    }

    /* Hand the fall on to the panel directly below, unless this one is already
       on the bottom row or the one below has not come far enough down yet.
       The row/column split is a subtract loop, not a divide, and the nested
       blocks are what keep it that way -- flattening them costs 18 words. */
    {
        u8 *flag = (u8 *)(scene + 0x4f8a + index);
        if (*flag != 0) return;
        {
            int col = index;
            int row = 0;
            int div = s->mWidth;
            if (index >= div) {
                do {
                    col -= div;
                    row++;
                } while (col >= div);
            }
            if (row == div - 1) {
                *flag = 1;
                return;
            }
            {
                int nxt = (row + 1) * div + col;
                if (((s->mY[nxt] - *(int *)(scene + (u32)index * 4 + 0x4d54)) >> 12) > 0) return;
                *flag = 1;
                s->mKind[nxt] = 3;
            }
        }
    }
}

// @symbol _ZN12dScMgPanel_c16Panel_StateBlinkEi
void dScMgPanel_c::Panel_StateBlink(int idx) {
    char *c = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)c;
    u16 *timers = (u16 *)(int)(c + 0x4e30);
    u8 *q;
    timers[idx] = timers[idx] + 1;
    if ((((int)*(u16 *)(c + idx * 2 + 0x4e30) >> 3) & 1) != 0) {
        q = s->mVisible;
        q[idx] = 1;
    } else {
        q = s->mVisible;
        q[idx] = 0;
    }
    {
        /* The `(long long)` round trip on the index is load-bearing here. */
        int idxL = (int)((long long)idx);
        char *b = c + idxL * 2;
        b += 0x4e00;
        if (((u16 *)b)[0x18] < 0x40)
            return;
        ((u16 *)b)[0x18] = 0;
        s->mKind[idx] = 0;
        q[idx] = 1;
    }
}

// @symbol _ZN12dScMgPanel_c15Panel_StateFlipEi
#pragma push
#pragma opt_common_subs off
/*
 * Per-slot tick for one of the tracked entries (idx).
 * Bumps the global tick byte at +0x4fe9.  If the slot's hold counter
 * (+0x4e78[idx]) is running, count it down and clamp at zero; otherwise
 * advance the slot's frame counter (+0x4e30[idx]) and, once it reaches
 * the limit (4 while the +0x4f8a flag is set, else 8), reset it, step the
 * slot's phase (+0x4de8[idx]) and wrap the phase past 5 with the +0x4efa
 * flag cleared.
 *
 * Shape notes: the pool-constant + register-offset form is what the
 * RMW sites (`*p = *p +- 1`) produce, the split base+0xNN00/#off form is
 * what the plain reads produce, and opt_common_subs off keeps the
 * else-branch recomputing base+idx*2 (its own extended basic block) the
 * way the ROM does.  The frame counter must be bumped before the flag
 * byte is read, and the limit compare is unsigned.
 */

void dScMgPanel_c::Panel_StateFlip(int index) {
    char *scene = (char *)this;
    u8 *cnt = (u8 *)(scene + 0x4fe9);

    *cnt = *cnt + 1;

    if (*(u16 *)(scene + index * 2 + 0x4e78) != 0) {
        u16 *q = (u16 *)(scene + 0x4e78 + index * 2);
        *q = *q - 1;
        if (*(s16 *)(scene + index * 2 + 0x4e78) < 0) {
            *(u16 *)(scene + index * 2 + 0x4e78) = 0;
        }
    } else {
        u16 *r = (u16 *)(scene + 0x4e30 + index * 2);
        u8 flag;
        u32 lim;
        *r = *r + 1;
        flag = *(u8 *)(scene + index + 0x4000 + 0xf8a);
        lim = (flag != 0) ? 4 : 8;
        if (*(u16 *)(scene + index * 2 + 0x4e30) < lim) {
            return;
        }
        *(u16 *)(scene + index * 2 + 0x4e30) = 0;
        {
            u16 *s = (u16 *)(scene + 0x4de8 + index * 2);
            *s = *s + 1;
        }
        if (*(u16 *)(scene + index * 2 + 0x4d00 + 0xe8) >= 5) {
            *(u16 *)(scene + index * 2 + 0x4d00 + 0xe8) = 0;
            *(u8 *)(scene + index + 0x4000 + 0xefa) = 0;
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgPanel_c15Panel_StateIdleEi
void dScMgPanel_c::Panel_StateIdle(int index) {
    char *scene = (char *)this;
    char *base = scene + index + 0x4000;
    base[0xf8a] = 0;
}

// @symbol _ZN12dScMgPanel_c19Scene_StateGameOverEv
extern "C" {
struct PanelPmfRec_bc0 { int off; int adj; };
typedef void (*PanelPmfFn_bc0)(void *, int);

}
void dScMgPanel_c::Scene_StateGameOver() {
    char *scene = (char *)this;
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    for (i = 0; i < mCount; i++) {
        u8 index = *(u8 *)(scene + i + 0x4efa);
        struct PanelPmfRec_bc0 *e = (struct PanelPmfRec_bc0 *)&sPanelPanelStateTable[index];
        int adj = e->adj;
        char *thisp = scene + (adj >> 1);
        PanelPmfFn_bc0 fn;
        if (adj & 1) {
            fn = *(PanelPmfFn_bc0 *)(*(char **)thisp + e->off);
        } else {
            fn = (PanelPmfFn_bc0)e->off;
        }
        fn(thisp, i);
    }
    if (*(u16 *)(scene + 0x4ec0) == 0) return;
    *(u16 *)(scene + 0x4ec0) -= 1;
    if (*(s16 *)(scene + 0x4ec0) > 0) return;
    *(u16 *)(scene + 0x4ec0) = 0;
    Mg_SetSharedState(MG_STATE_GAME_OVER);
    mPromptEnabled = 0;
    *(u8 *)(scene + 0x4fe3) = 0;
    ClearSlideFlag();
}

// @symbol _ZN12dScMgPanel_c17Scene_StateResultEv
#pragma push
#pragma opt_propagation off
extern "C" {

}
void dScMgPanel_c::Scene_StateResult() {
    char *scene = (char *)this;
    extern PanelStateFnI sPanelPanelStateTable[];
    // local extern: see above.
    extern u8 gActivePlayerSlot;
// local extern: see above.
    extern u8 gTouchHeld[];
// local extern: see above.
    extern u8 gTouchEdge[];
    int found;
    int i;
    int n;

    IntroCard_Update();
    found = 0;
    i = found;
    n = mCount;
    if (n > 0) {
        do {
            u8 *p = (u8 *)(scene + i + 0x4efa);
            (((dScMgPanel_c *)scene)->*sPanelPanelStateTable[*p])(i);
            i++;
            if (*p != 0)
                found++;
            n = mCount;
        } while (i < n);
    }
    if (found != 0)
        return;

    if (*(u16 *)(scene + 0x4e00 + 0xc0) != 0) {
        int off;
        u16 *tp;
        u8 *flags;
        off = 0x4ec0;
        tp = (u16 *)(int)((long long)(int)(scene + off) & 0xFFFFFFFFFFFFFFFFLL);
        flags = (u8 *)(scene + 0x4000);
        *tp = *tp - 1;
        if (flags[0xfe6] == 0) {
            int hit;
            u8 index;
            hit = 0;
            index = gActivePlayerSlot;
            if (gTouchHeld[index * 4] != 0) {
                if (gTouchEdge[index * 4] != 0)
                    hit = 1;
            }
            if (hit)
                *(u16 *)(scene + 0x4e00 + 0xc0) = 0;
        }
        if (*(s16 *)(scene + 0x4e00 + 0xc0) > 0)
            return;
        *(u16 *)(scene + 0x4e00 + 0xc0) = 0;
        if (*(u8 *)(scene + 0x4000 + 0xfe6) != 0) {
            char *g;
            Scroll_Start();
            Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
            ClearSlideFlag();
            *(u8 *)(scene + 0x4000 + 0xfe3) = 0;
            mPromptEnabled = 0;
            g = (char *)gMgScene;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 9999)
                    (*(int *)(int)(g + 0xb4))++;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
            Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
            return;
        }
        *(u16 *)(scene + 0x4e00 + 0xc0) = 0x10;
        mState = 7;
        Panel_StartAll();
        return;
    }
    Scroll_Update();
    {
        int inc;
        u16 *ip;
        inc = 0x4ec6;
        ip = (u16 *)(int)((long long)(unsigned)(scene + inc) & 0xFFFFFFFFFFFFFFFFLL);
        *ip = *ip + 1;
    }
}
#pragma pop

// @symbol _ZN12dScMgPanel_c21Scene_StateShowAnswerEv
void dScMgPanel_c::Scene_StateShowAnswer() {
    char *scene = (char *)this;
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    IntroCard_Update();
    Answer_Update();
    for (i = 0; i < mCount; i++) {
        u8 index = *(u8 *)(scene + i + 0x4efa);
        (((dScMgPanel_c *)scene)->*sPanelPanelStateTable[index])(i);
    }
}

// @symbol _ZN12dScMgPanel_c17Scene_StateRewindEv
void dScMgPanel_c::Scene_StateRewind() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    IntroCard_Update();
    self->mBusy = 1;
    UpdateMoves();
    for (i = 0; i < self->mCount; i++) {
        u8 index = *(u8 *)(scene + i + 0x4efa);
        (((dScMgPanel_c *)scene)->*sPanelPanelStateTable[index])(i);
    }
}

// @symbol _ZN12dScMgPanel_c21Scene_StateShowMissesEv
void dScMgPanel_c::Scene_StateShowMisses() {
    dScMgPanel_c *scene = this;
    extern PanelStateFnI sPanelPanelStateTable[];
    u8 index;
    int found;
    int i;
    IntroCard_Update();
    found = 0;
    scene->mBusy = 1;
    for (i = 0; i < scene->mCount; i++) {
        index = scene->mKind[i];
        (scene->*sPanelPanelStateTable[index])(i);
        if (index != 0)
            found++;
    }
    if (found == 0) {
        scene->mState = 4;
        scene->mClear = 1;
        scene->mDelay = 0x40;
    }
}

// @symbol _ZN12dScMgPanel_c15Scene_StatePlayEv
void dScMgPanel_c::Scene_StatePlay() {
    dScMgPanel_c *scene = this;
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    char *b = (char *)scene;
    UpdateTouch();
    IntroCard_Update();
    *(u8 *)(b + 0x4fe9) = 0;
    for (i = 0; i < *(int *)(b + 0x4cb8); i++) {
        u8 index = *(u8 *)(b + i + 0x4efa);
        (scene->*sPanelPanelStateTable[index])(i);
    }
    CheckSolved();
    CountFlipping();
    StartSolveCheck();
}

// @symbol _ZN12dScMgPanel_c16Scene_StateIntroEv
void dScMgPanel_c::Scene_StateIntro() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    if (self->mIntro == 0) {
        IntroCard_Update();
        *(u8 *)(scene + 0x4fe7) = 0;
        *(u8 *)(scene + 0x4fe8) = 2;
        return;
    }
    *(u8 *)(scene + 0x4fe7) += 1;
    if (*(u8 *)(scene + 0x4fe7) >= 8) {
        *(u8 *)(scene + 0x4fe7) = 0;
        *(u8 *)(scene + 0x4fe8) -= 1;
    }
    if (*(u8 *)(scene + 0x4fe8) == 0)
    self->mState = 2;
}

// @symbol _ZN12dScMgPanel_c15Scene_StateInitEv
void dScMgPanel_c::Scene_StateInit() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    IntroCard_Start();
    self->mState = 1;
    self->mIntro = 0;
}

/* The four class methods can't sit in an `extern "C"` region, so a
 * declaration inside them would get C++ linkage and name a symbol the ROM
 * doesn't have. Their callees are declared here instead, below every helper,
 * so the helpers' own declarations are untouched. Data declarations are not
 * mangled, so those stay in the bodies. */
extern "C" {

extern void  Deallocate(void *);
extern int   GetOwnerLanguage(void);
extern int   LoadFile(int handle);
extern void  DecompressLZ16(int src, void *dst);
extern void  MultiStore16(u16 val, char *dst, int nbytes);
extern void  FreeGfxSlotsById(int arg);
extern void  SetSubBg0Offset(int, int);
extern int   func_02054d88(void);
extern void  func_020562b4(const void *, u32, u32);
extern void  func_02056314(void *, u32, u32);
extern void  func_02056374(const void *, u32, u32);
extern void  func_020563d4(const void *, u32, u32);
extern void  func_020564f4(const void *, int, int);
extern void  Mg_SubmitScore(int);
extern void  Mg_SetTopScreenYOffset(int);
extern void  Mg_ShowBanner(int, int, int, int, int, s16);
extern void  Mg_DrawScoreTally(void *a, int b, int c, int d);

}  /* extern "C" */

// @symbol _ZN12dScMgPanel_c10StartRoundEi
/* Vtable slot 18, an override of dScMgBase_c::StartRound(int).  The
   signature has to repeat the base declaration exactly or mwcc appends a new
   slot instead of overriding it. */

void dScMgPanel_c::StartRound(int flag)
{
    extern u8 data_0209d454;
    char *self = (char *)this;

    char *p;

    if (flag == 0) {
        int *q = (int *)(self + 0xbc);
        *q += 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        unk_4fea = 0;
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;

        if (gMgScene != 0)
            gMgScene->mHudScore = 0;

        p = (char *)gMgScene;
        Mg_SubmitScore(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    FreeGfxSlotsById(0x1d);
    ResetRound();
    ScramblePanels();
    DealPuzzle();
    Shake_Start();

    mCredits = 3;
    mState = 0;
    Mg_ShowBanner(0xd, 0x80, 0x40, 0, -1, 0xd);
    SetSubBg0Offset(0, 0);

    data_0209d454 &= ~1;
}

// @symbol _ZN12dScMgPanel_c6RenderEv
/* Vtable slot 9. */
s32 dScMgPanel_c::Render()
{
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
    DrawCursor();
    DrawHud();
    Particle_DrawAll();
    Banner_Draw();
    DrawPanels();
    return 1;
}

// @symbol _ZN12dScMgPanel_c8BehaviorEv
/* Vtable slot 6.  One frame of the current state, dispatched through the
   descriptor pair at sPanelSceneStateTable. */
/* One row of the state-dispatch table at sPanelSceneStateTable: `a` is either a
   function address or a vtable byte offset, and bit 0 of `b` says which. */
struct Ent_358 { int a; int b; };
s32 dScMgPanel_c::Behavior()
{
    extern PanelStateFn sPanelSceneStateTable[];
    char *c = (char *)this;
    int idx = mState;
    Ent_358 *e = (Ent_358 *)&sPanelSceneStateTable[idx];
    int adj = e->b;
    char *obj = c + (adj >> 1);
    int fn;
    if (adj & 1) {
        fn = *(int *)(*(int *)obj + e->a);
    } else {
        fn = e->a;
    }
    ((void (*)(void *))fn)(obj);
    Shake_Update();
    Particle_UpdateAll();
    return 1;
}

// @symbol _ZN12dScMgPanel_c13InitResourcesEv
/* Vtable slot 0.  Brings up both screens' backgrounds, loads the panel and
   message graphics for the owner's language, then resets the board. */

s32 dScMgPanel_c::InitResources()
{
    extern u8 data_0209d45c;
    extern u8 data_0209d454;
    char *c = (char *)this;
    char *b;
    volatile u16 mainFill;
    volatile u16 subFill;
    int file;
    int msgFile;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    file = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCG_BIN);
    DecompressLZ16(file, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)file);

    file = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCL_BIN);
    GX::LoadBGPltt((const void *)file, 0x1a0, 0x60);
    Deallocate((void *)file);

    file = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_UP_NSC_BIN);
    func_02056314((void *)file, 0, 0x800);
    Deallocate((void *)file);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9410;

    b = G2::GetBG2ScrPtr();
    mainFill = 0xf361;
    MultiStore16(mainFill, b, 0x1000);

    file = LoadFile(ASSET_D_2D_YOSIFINX_NORMAL_NSC_BIN);
    func_020563d4((const void *)file, 0x800, 0x800);
    Deallocate((void *)file);

    msgFile = LoadFile(ASSET_D_2D_MG_PAZZLE_PANEL_NCG_BIN);
    if (GetOwnerLanguage() == 5) {
        msgFile = LoadFile(ASSET_D_2D_MG_PAZZLE_PANEL_S_NCG_BIN);
    } else if (GetOwnerLanguage() == 4) {
        msgFile = LoadFile(ASSET_D_2D_MG_PAZZLE_PANEL_I_NCG_BIN);
    } else if (GetOwnerLanguage() == 3) {
        msgFile = LoadFile(ASSET_D_2D_MG_PAZZLE_PANEL_G_NCG_BIN);
    } else if (GetOwnerLanguage() == 2) {
        msgFile = LoadFile(ASSET_D_2D_MG_PAZZLE_PANEL_F_NCG_BIN);
    }

    file = LoadFile(ASSET_D_2D_MG_PAZZLE_PANEL_NCL_BIN);
    DecompressLZ16(msgFile, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)file, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x210;

    {
        int subFile = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCG_BIN);
        DecompressLZ16(subFile, (void *)(G2S::GetBG3CharPtr() + 0x4000));
        Deallocate((void *)subFile);

        subFile = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCL_BIN);
        GXS::LoadBGPltt((const void *)subFile, 0x1a0, 0x60);
        Deallocate((void *)subFile);

        subFile = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_DOWN_NSC_BIN);
        func_020562b4((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);

        data_0209d454 |= 4;
        *(volatile u16 *)0x400100c &= ~0x40;
        *(volatile u32 *)0x4001018 = 0;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x8410;

        b = G2S::GetBG2ScrPtr();
        subFill = 0xf361;
        MultiStore16(subFill, b, 0x1000);

        subFile = LoadFile(ASSET_D_2D_YOSIFINX_NORMAL_NSC_BIN);
        func_02056374((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);

        *(volatile u16 *)0x4001008 &= ~3;
        *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x610;

        subFile = LoadFile(ASSET_D_2D_YOSIFINX_BIKKURI_NSC_BIN);
        func_020564f4((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);
    }

    DecompressLZ16(msgFile, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)file, 0, 0x100);
    Deallocate((void *)msgFile);
    Deallocate((void *)file);

    FreeGfxSlotsById(0x1d);
    ResetRound();
    ClearGrid();

    mDealt = 0;
    unk_4fea = 0;
    mFaceSet = 0xff;

    ScramblePanels();
    DealPuzzle();
    IntroCard_Start();
    Shake_Start();

    mState = 1;
    mIntro = 0;
    mCredits = 3;

    Mg_SetTopScreenYOffset(0x20);
    Mg_ShowBanner(0xd, 0x80, 0x40, 0, -1, 0xd);
    mHudScore = 0;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgPanel_c through RTTI,
 * allocation size, vtable identity, and the MG_PANEL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgPuzzlePanelPuzzlePanic_Spawn. */
// @symbol dScMgPanel_c_classInit
extern "C" dScMgPanel_c *dScMgPanel_c_classInit() {
    return new dScMgPanel_c;
}

/* The state tables, in the order the static initializer fills them. Each
   entry's {function, 0} record is a compiler temporary in .data. The two
   sprite-handle triples are copied whole from data_ov006_02136cf0 and
   data_ov006_02136d1c. */
extern "C" void *data_ov006_02136cf0[3];
extern "C" void *data_ov006_02136d1c[3];

void *sPanelSprites[6] = {
    data_ov006_02136cf0[0], data_ov006_02136cf0[1], data_ov006_02136cf0[2],
    data_ov006_02136d1c[0], data_ov006_02136d1c[1], data_ov006_02136d1c[2],
};
PanelStateFn sPanelSceneStateTable[8] = {
    &dScMgPanel_c::Scene_StateInit,   &dScMgPanel_c::Scene_StateIntro,
    &dScMgPanel_c::Scene_StatePlay,   &dScMgPanel_c::Scene_StateShowMisses,
    &dScMgPanel_c::Scene_StateRewind, &dScMgPanel_c::Scene_StateShowAnswer,
    &dScMgPanel_c::Scene_StateResult, &dScMgPanel_c::Scene_StateGameOver,
};
PanelStateFnI sPanelPanelStateTable[4] = {
    &dScMgPanel_c::Panel_StateIdle,
    &dScMgPanel_c::Panel_StateFlip,
    &dScMgPanel_c::Panel_StateBlink,
    &dScMgPanel_c::Panel_StateFall,
};
PanelStateFn sPanelAnswerStateTable[4] = {
    &dScMgPanel_c::Answer_StateWaitButton,
    &dScMgPanel_c::Answer_StateResetBoard,
    &dScMgPanel_c::Answer_StateFindSolution,
    &dScMgPanel_c::Answer_StateFlipSolution,
};
PanelStateFn sPanelIntroCardStateTable[5] = {
    &dScMgPanel_c::IntroCard_StateStart, &dScMgPanel_c::IntroCard_StateSlideIn,
    &dScMgPanel_c::IntroCard_StateHold,  &dScMgPanel_c::IntroCard_StateSlideOut,
    &dScMgPanel_c::IntroCard_StateDone,
};
PanelStateFn sPanelBannerStateTable[3] = {
    &dScMgPanel_c::Banner_StatePopUp,
    &dScMgPanel_c::Banner_StateSettle,
    &dScMgPanel_c::Banner_StateIdle,
};
PanelStateFnI sPanelShakeStateTable[3] = {
    &dScMgPanel_c::Shake_StateIdle,
    &dScMgPanel_c::Shake_StateShake,
    &dScMgPanel_c::Shake_StateLoseTry,
};

/* MG_PANEL's base profile: the factory, then the same priority in both
   halfwords. */
BaseSpawnInfo<dScMgPanel_c> g_profile_MG_PANEL = {
    dScMgPanel_c_classInit,
    0x17c,
    0x17c,
};
unsigned int data_ov006_0213dc94[2] = { 0x01fc00fc, 0xffff2099 };
int data_ov006_02142804[7];
