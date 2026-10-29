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
 * dScMgPanel_c.h. Some helpers retain local layout views and raw offset accesses where native member loads change the matching build. The three empty handlers Shake_StateIdle, Banner_StateIdle and IntroCard_StateDone are members too; the state tables point at them.
 *
 * Stepped member reads. Many per-panel sites index a member through a
 * stepped scene pointer, `((dScMgPanel_c *)(scene + i))->mFace[0]`, rather
 * than `mFace[i]`. The two name the same byte, but the plain index moves code
 * or renumbers @NNN at each of these sites. This applies to:
 * - Particle_Spawn / Particle_ClearAll's slot scan
 * - Banner_Show
 * - CountMatches
 * - Answer_StateFindSolution
 * - CheckSolved
 * - FlipAt
 * - HasDuplicateRows / ClearGrid
 * - ResetRound
 * - Panel_StateIdle
 * - Scene_StateResult
 * The shake helpers address mShake[index] as &mShake[0].field + index * 0xc
 * for the same reason.
 *
 * Leftover raw sites, each measured against the matching build:
 * - Shake_StateLoseTry: the dead `q` (dropping it renumbers @NNN).
 * - Panel_StateBlink: the mAnimTimer[idx] reset through a +0x4e00 split base.
 * - Scene_StateResult: mSolved through a +0x4000 byte base. mResultTimer and
 *   mScrollFrames go through laundered `scene + off` pointers.
 * - ResetRound: 0x4ec2 and 0x466c are pads that only ResetRound writes.
 * - Answer_StateFindSolution keeps the PanelInputView width view, and
 *   DrawPanels a local layout view. Its native-member rewrite grows the body
 *   from 360 to 388 bytes.
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

namespace G2 {
char *GetBG2ScrPtr();
}
namespace G2S {
char *GetBG2ScrPtr();
char *GetBG3CharPtr();
}
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace GXS {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}

/* Callees of the member helpers. Declared here, not in the bodies: a
   declaration inside a member body gets C++ linkage and names a symbol the ROM
   doesn't have. */
extern "C" {
int RandomIntInternal(int *seed);
void SetBg2Offset(int x, int y);
void SetSubBg2Offset(int x, int y);
void SetSubBg0Offset(int a, int b);
void FreeGfxSlotsById(int arg);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void Hud_RenderSprite(void *a, int b, int c, int d, int e);
void Mg_SubmitScore(int a);
void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *d);
void Mg_RenderSpriteBoth(void *a, int b, int c, int d, int e);
void Mg_SetSharedState(int a);
void Mg_RenderSubSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5);
int __aeabi_idiv(int a, int b);
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
dScMgPanel_c::~dScMgPanel_c() {}

// @symbol _ZN12dScMgPanel_cD0Ev
/* dScMgPanel_c::~dScMgPanel_c (D0, deleting destructor).  There is no second
   definition to write: mwcc emits D0 from the ONE definition above, and
   dScMgBase_c's own operator delete covers the deallocation.  Under
   `#pragma defer_codegen off` the variants come out D1, D0, D2, which is the
   cartridge order; the trailing D2 is homeless and is deadstripped. */

// @symbol _ZN12dScMgPanel_c16Particle_DrawAllEv
/* Draw the live particles: 64 slots of 0x18 bytes, sprite chosen by the
   slot's kind byte and positioned from its Fix12 x/y. */
void dScMgPanel_c::Particle_DrawAll() {
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
void dScMgPanel_c::Particle_UpdateAll() {
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
void dScMgPanel_c::Particle_Spawn(int x, int y, int mode) {
    char *scene = (char *)this;
    extern int data_0209d4b8;
    int i;
    char *q = scene;
    for (i = 0; i < 0x40; i++, q += 0x18) {
        if (((dScMgPanel_c *)q)->mParticles[0].active == 0) {
            int off = i * 0x18;
            int *w;
            mParticles[i].active = 1;
            mParticles[i].visible = 1;
            mParticles[i].tick = 0;
            mParticles[i].kind = 0;
            mParticles[i].frame = 0;
            mParticles[i].stuck = mode;
            /* The `(int)` launder keeps mwcc from common-subexpressioning this
               field address with the one taken three lines down; without it 16
               words of this function move. */
            *(int *)((char *)(((int)&mParticles[0].vx)) + off) = 0;
            mParticles[i].vy = 0;
            mParticles[i].x = x;
            mParticles[i].y = y;
            w = (int *)((char *)(((int)&mParticles[0].vx)) + off);
            if (mode != 0) {
                mParticles[i].drift = 0;
            } else {
                int rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
                rnd = (int)((((u32)rnd) << 1) >> 15);
                /* Strided on purpose: `mParticles[i].drift = rnd` moves code. */
                ((dScMgPanel_c *)(scene + off))->mParticles[0].drift = rnd;
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
void dScMgPanel_c::Particle_ClearAll() {
    char *scene = (char *)this;
    int i;
    for (i = 0; i < 0x40; i++) {
        ((dScMgPanel_c *)scene)->mParticles[0].active = 0;
        ((dScMgPanel_c *)scene)->mParticles[0].visible = 0;
        scene += 0x18;
    }
}

// @symbol _ZN12dScMgPanel_c13Scroll_UpdateEv
#pragma push
#pragma opt_common_subs off
void dScMgPanel_c::Scroll_Update() {
    char *scene = (char *)this;
    extern int data_0209d4b8;
    int x;
    int y;
    u32 r;
    u32 r2;

    if (mScroll.phase == 0) {
        mScroll.timer += 1;
        if (((mScroll.timer >> 1) & 1) != 0)
            mScroll.x = 0x2000;
        else
            mScroll.x = -0x2000;

        if (mScroll.spawnDelay != 0) {
            mScroll.spawnDelay -= 1;
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
            mScroll.spawnDelay = 3;
        }

        if (mScroll.timer >= 0x3c) {
            mScroll.phase += 1;
            mScroll.timer = 0;
            mScroll.spawnDelay = 0;
            mScroll.x = 0;
            mScroll.vel = 0x1000;
        }
    } else if (mScroll.y < 0x200000) {
        mScroll.y += mScroll.vel;
        mScroll.vel += 0x200;
        if (mScroll.y >= 0x200000)
            mScroll.y = 0x200000;

        if (mScroll.spawnDelay != 0) {
            mScroll.spawnDelay -= 1;
        } else {
            y = mScroll.y >> 12;
            if (y >= 0 && y <= 0xc0)
                y = (0xc0 - y) << 12;
            else if (y >= 0x100)
                y = (-0x20 - (y - 0x100)) << 12;

            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            Particle_Spawn(x << 12, y, 1);
            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            Particle_Spawn((0x100 - x) << 12, y, 1);
            mScroll.spawnDelay = 2;
        }
    }

    x = mScroll.x >> 12;
    y = mScroll.y >> 12;
    SetBg2Offset(x, y);
    if (y >= 0x100)
        y = 0x100;
    SetSubBg2Offset(x, y);
}
#pragma pop

// @symbol _ZN12dScMgPanel_c12Scroll_StartEv
void dScMgPanel_c::Scroll_Start() {
    char *scene = (char *)this;
    mScroll.on = 1;
    mScroll.x = 0;
    mScroll.y = 0;
    mScroll.z = 0;
    mScroll.vel = 0;
    mScroll.timer = 0;
    mScroll.spawnDelay = 0;
    mScroll.phase = 0;
    Sound::PlayBank2_2D(0x1fd);
}

// @symbol _ZN12dScMgPanel_c11Scroll_StopEv
void dScMgPanel_c::Scroll_Stop() {
    mScroll.on = 0;
    SetBg2Offset(0, 0);
    SetSubBg2Offset(0, 0);
}

// @symbol _ZN12dScMgPanel_c18EnableOverlayLayerEv
void dScMgPanel_c::EnableOverlayLayer() {
    u8 *scene = (u8 *)this; /* dead, but dropping it renumbers @NNN */
    extern u8 data_0209d454[];
    if (mShake[0].state != 0)
        return;
    mShake[0].state = 1;
    mShake[0].timer = 0;
    data_0209d454[0] |= 1;
}

// @symbol _ZN12dScMgPanel_c18Shake_StateLoseTryEi
void dScMgPanel_c::Shake_StateLoseTry(int index) {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    extern u8 data_0209d454;
    extern int data_ov006_0212ed00[];
    int n = index * 0xc;
    /* base + n is mShake[index].timer and &mShake[0].offset + n is
       mShake[index].offset: `s->mShake[index].timer` / `.offset` move this
       function's code.  The dead `q` below stays: dropping it renumbers
       @NNN. */
    char *base = (char *)&s->mShake[0].timer;
    int cnt;
    *(u16 *)(base + n) = *(u16 *)(base + n) + 1;
    cnt = *(u16 *)(base + n);
    if ((u32)cnt >= 0x20) {
        *(u16 *)(base + n) = 0;
        ((dScMgPanel_c *)(scene + n))->mShake[0].state = 0;
        SetSubBg0Offset(0, 0);
        data_0209d454 &= ~1;
        func_02012790(0x12f);
        s->mCredits -= 1;
        return;
    }
    *(int *)((char *)&s->mShake[0].offset + n) = data_ov006_0212ed00[cnt >> 3];
    {
        char *q = scene + n + 0x4600;
        u16 t = ((dScMgPanel_c *)(scene + n))->mShake[0].timer;
        if (t == 1 || t == 0x11)
            Sound::PlayBank2_2D(0x1fc);
    }
    SetSubBg0Offset(0, *(int *)((char *)&s->mShake[0].offset + n));
}

// @symbol _ZN12dScMgPanel_c16Shake_StateShakeEi
extern "C" {
void SetSubBg0Offset(int a, int b);
}
void dScMgPanel_c::Shake_StateShake(int index) {
    char *scene = (char *)this;
    extern int sPanelShakeOffsets[];
    int m = index * 0xc;
    /* b + m is mShake[index].timer and dst mShake[index].offset: the
       indexed member spellings move this function's code. */
    char *b = (char *)&((dScMgPanel_c *)scene)->mShake[0].timer;
    u16 h = *(u16 *)(b + m);
    h = h + 1;
    *(u16 *)(b + m) = h;
    h = *(u16 *)(b + m);
    if (h >= 0x20) {
        *(s16 *)(b + m) = 0;
        ((dScMgPanel_c *)(scene + m))->mShake[0].state = 2;
        return;
    }
    {
        int t = sPanelShakeOffsets[h >> 3];
        int *dst = (int *)((char *)&((dScMgPanel_c *)scene)->mShake[0].offset + m);
        u16 st;
        *dst = t;
        st = ((dScMgPanel_c *)(scene + m))->mShake[0].timer;
        if (st == 1 || st == 0x11)
            Sound::PlayBank2_2D(0x1fc);
        SetSubBg0Offset(0, *dst);
    }
}

// @symbol _ZN12dScMgPanel_c15Shake_StateIdleEi
void dScMgPanel_c::Shake_StateIdle(int index) {}

// @symbol _ZN12dScMgPanel_c12Shake_UpdateEv
void dScMgPanel_c::Shake_Update() {
    extern PanelStateFnI sPanelShakeStateTable[];
    if (mShake[0].active == 0)
        return;
    int state = mShake[0].state;
    (this->*sPanelShakeStateTable[state])(0);
}

// @symbol _ZN12dScMgPanel_c11Shake_StartEv
void dScMgPanel_c::Shake_Start() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    self->mShake[0].active = 1;
    self->mShake[0].unk_00 = 0;
    self->mShake[0].offset = 0;
    self->mShake[0].timer = 0;
    self->mShake[0].state = 0;
}

// @symbol _ZN12dScMgPanel_c10Shake_StopEv
void dScMgPanel_c::Shake_Stop() {
    mShake[0].active = 0;
}

// @symbol _ZN12dScMgPanel_c11Banner_DrawEv
void dScMgPanel_c::Banner_Draw() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    extern void *sPanelBannerSprites[];
    if (self->mBanner.visible == 0)
        return;
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
    if (self->mBanner.hold < 8)
        return;
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
    if (self->mBanner.hold < 4)
        return;
    self->mBanner.hold = 0;
    {
        int *w = &self->mBanner.y;
        *w = *w - 0x4000;
    }
    self->mBanner.state = 1;
}

// @symbol _ZN12dScMgPanel_c13Banner_UpdateEv
void dScMgPanel_c::Banner_Update() {
    char *scene = (char *)this;
    extern PanelStateFn sPanelBannerStateTable[];
    u16 *ip;
    if (mBanner.active == 0)
        return;
    ip = &mBanner.timer;
    *ip = *ip - 1;
    if ((s16)mBanner.timer <= 0) {
        mBanner.timer = 0;
        mBanner.active = 0;
        mBanner.visible = 0;
        return;
    }
    {
        dScMgPanel_c *cc = (dScMgPanel_c *)scene;
        (cc->*sPanelBannerStateTable[mBanner.state])();
    }
}

// @symbol _ZN12dScMgPanel_c11Banner_ShowEi
void dScMgPanel_c::Banner_Show(int i) {
    char *scene = (char *)this;
    char *e = scene + (i << 2);
    mBanner.active = 1;
    mBanner.visible = 1;
    mBanner.x = mX[i];
    /* Stepped: `mY[i]` moves this function's code. */
    mBanner.y = ((dScMgPanel_c *)e)->mY[0] + 0x8000;
    mBanner.timer = 0x30;
    mBanner.hold = 0;
    mBanner.state = 0;
}

// @symbol _ZN12dScMgPanel_c10DrawCursorEv
void dScMgPanel_c::DrawCursor() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    extern void *sPanelCursorSprites[];
    if (mCreditsOn == 0)
        return;
    Hud_RenderSprite(sPanelCursorSprites[s->mCredits], 0xf0, 0x24, -1, -1);
}

// @symbol _ZN12dScMgPanel_c7DrawHudEv
void dScMgPanel_c::DrawHud() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    extern void *sPanelHudSprites[];
    extern void *data_ov006_02136de8[];
    if (self->mSlideFlag == 0)
        return;
    if (self->mSlideStep != 4) {
        int a = self->mSlide >> 12;
        int b = self->mSlideY >> 12;
        int index;
        Mg_DrawBigNumber(a, b, self->mFlipCount, -1, -1, 0, 0);
        index = 1;
        if (self->mFlipCount >= 2)
            index = 0;
        Mg_RenderSpriteBoth(sPanelHudSprites[index], a + 0x2c, b, -1, -1);
    } else {
        int x = self->mSlideLeft;
        Hud_RenderSprite(data_ov006_02136de8[x], self->mSlide >> 12, self->mSlideY >> 12, -1, -1);
    }
}

// @symbol _ZN12dScMgPanel_c16ReloadSlideDelayEv
void dScMgPanel_c::ReloadSlideDelay() {
    u8 *scene = (u8 *)this;
    /* Through `scene`: the plain `mSlideLeft = mFlipCount` moves code. */
    ((dScMgPanel_c *)scene)->mSlideLeft = ((dScMgPanel_c *)scene)->mFlipCount;
}

// @symbol _ZN12dScMgPanel_c14TickSlideDelayEv
// If mSlideLeft (self+0x4677) is set, decrement it: the ROM's predicated
// ldrbne/subne/strbne tail.
void dScMgPanel_c::TickSlideDelay() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (s->mSlideLeft)
        s->mSlideLeft -= 1;
}

// @symbol _ZN12dScMgPanel_c14ClearSlideFlagEv
void dScMgPanel_c::ClearSlideFlag() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlideFlag = 0;
}

// @symbol _ZN12dScMgPanel_c12SetSlideFlagEv
void dScMgPanel_c::SetSlideFlag() {
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
    if ((s->mSlide >> 12) > -0x40)
        return;
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
    mCreditsOn = 1;
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
        if (*(s16 *)&s->mHold < 0)
            s->mHold = 0;
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
    if (s->mSlide >> 12 > 0x58)
        return;
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
void dScMgPanel_c::IntroCard_Update() {
    extern PanelStateFn sPanelIntroCardStateTable[];
    if (!mSlideOn)
        return;
    (this->*sPanelIntroCardStateTable[mSlideStep])();
}

// @symbol _ZN12dScMgPanel_c15IntroCard_StartEv
void dScMgPanel_c::IntroCard_Start() {
    mSlideOn = 1;
    mSlideStep = 0;
    mSlideLeft = 0;
}

// @symbol _ZN12dScMgPanel_c15StartSolveCheckEv
void dScMgPanel_c::StartSolveCheck() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (mBusy != 0)
        return;
    if (mState == 6)
        return;
    if (mMovesUsed < mFlipCount)
        return;

    func_02012790(0xe);
    EnableOverlayLayer();

    if (s->mCredits > 1) {
        mState = 3;
        CountMatches();
    } else {
        mSolved = 0;
        CountMatches();
        mState = 6;
        mResultTimer = 0x50;
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
        (*(u16 *)&s->mDelay)--;
        if (s->mDelay < 0)
            s->mDelay = 0;
        return;
    }

    if (s->mReplayStep >= s->mFlipCount) {
        s->mResultTimer = 0x50;
        s->mState = 6;
        s->mSolved = 0;
        return;
    }

    index = s->mFlipAt[s->mReplayStep];
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
            s->mFlipDelay[cell] = 8;
        }
    }

    Banner_Show(index);
    s->mFlipDelay[index] = 0;
    s->mDelay = 0x30;
    s->mReplayStep++;
}

// @symbol _ZN12dScMgPanel_c24Answer_StateFindSolutionEv

/* The width view preserves the measured field-load form without a dummy vtable. */
struct PanelInputView {
    char pad[0x4cbc];
    int width;
};
void dScMgPanel_c ::Answer_StateFindSolution() {
    PanelInputView *scene = (PanelInputView *)this;
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

    if ((u16)mDelay != 0) {
        (*(u16 *)&mDelay)--;
        if (mDelay < 0) {
            mDelay = 0;
        }
        return;
    }

    n = mCount;
    found = 0;
    i = found;
    if (n > 0) {
        do {
            if (((dScMgPanel_c *)(c + i))->mKind[0] != 0) {
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
            idx = mFaceSet;
            if (IsAltVariant() != 0) {
                arr[j] = sPanelPuzzlesAlt[idx][j];
            } else {
                arr[j] = sPanelPuzzles[idx][j];
            }
            n = mCount;
            j++;
        } while (j < n);
    }

    nlights = mFlipCount;
    li = 0;
    if (nlights > 0) {
        do {
            idx = ((dScMgPanel_c *)(c + li))->mFlipAt[0];
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
            p = &((dScMgPanel_c *)(c + j))->mFace[0];
            if (*p != arr[j]) {
                ((dScMgPanel_c *)(c + j))->mKind[0] = 1;
                *p ^= 1;
                ((dScMgPanel_c *)(c + j))->mFastFlip[0] = 1; /* mFastFlip[j] renumbers @NNN */
            }
            j++;
        } while (j < mCount);
    }

    mDelay = 0x40;
    mInputState = 3;
    mReplayStep = 0;
}

// @symbol _ZN12dScMgPanel_c22Answer_StateResetBoardEv
void dScMgPanel_c::Answer_StateResetBoard() {
    char *scene = (char *)this;
    dScMgPanel_c *self = (dScMgPanel_c *)scene;
    if ((u16)mDelay != 0) {
        (*(u16 *)&mDelay)--;
        if (mDelay < 0) {
            mDelay = 0;
        }
        return;
    } else {
        int k;
        for (k = 0; k < self->mCount; k++) {
            self->mKind[k] = 1;
            self->mFace[k] ^= 1;
            self->mFastFlip[k] = 1;
        }
        mInputState = 2;
        mDelay = 0x20;
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
    mInputState = 1;
    s->mState = 5;
    s->mDelay = 0x20;
}

// @symbol _ZN12dScMgPanel_c13Answer_UpdateEv
void dScMgPanel_c ::Answer_Update() {
    extern PanelStateFn sPanelAnswerStateTable[];
    if (IsAltVariant() != 0)
        return;
    (this->*sPanelAnswerStateTable[mInputState])();
    Banner_Update();
}

// @symbol _ZN12dScMgPanel_c11UpdateMovesEv
void dScMgPanel_c::UpdateMoves() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if ((u16)mDelay != 0) {
        (*(u16 *)&mDelay)--;
        if (mDelay < 0)
            mDelay = 0;
        return;
    }

    {
        int found = 0;
        int i;
        int n = s->mCount;
        for (i = 0; i < n; i++) {
            if (mKind[i] != 0) {
                found++;
                break;
            }
        }
        if (found)
            return;
    }

    if (mTouchCount == 0xff) {
        mTouchCount = 0;
        mState = 2;
        SetSlideFlag();
        ReloadSlideDelay();
        return;
    }

    mTouchCount--;

    if (mTouchCount == 0xff) {
        mTouchCount = 0;
        mState = 2;
        mMovesUsed = 0;
        SetSlideFlag();
        ReloadSlideDelay();
        return;
    }

    {
        u8 slot = mTouchCount;
        int hq = 3;
        int index = mTouchLog[slot];
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
                mKind[cell] = 1;
                s->mFace[cell] ^= 1;
                s->mFlipDelay[cell] = 8;
                s->mFastFlip[cell] = 1;
            }
        }
        s->mFlipDelay[index] = 0;
        Sound::PlayBank2_2D(0x1fb);
    }
}

// @symbol _ZN12dScMgPanel_c10DrawPanelsEv
/* Native member loads change DrawPanels code generation; retain this layout view. */
extern "C" {
typedef struct PanelDrawView {
    char _p0[0x4ca8];
    int state; /* 0x4ca8 */
    char _p1[0x4cb8 - 0x4ca8 - 4];
    int count; /* 0x4cb8 */
    char _p2[0x4cc4 - 0x4cb8 - 4];
    int x[(0x4d54 - 0x4cc4) / 4];         /* 0x4cc4 */
    int y[(0x4de8 - 0x4d54) / 4];         /* 0x4d54 */
    u16 flipFrame[(0x4efa - 0x4de8) / 2]; /* 0x4de8 */
    u8 kind[0x4f1e - 0x4efa];             /* 0x4efa */
    u8 face[0x4f42 - 0x4f1e];             /* 0x4f1e */
    u8 goal[0x4f66 - 0x4f42];             /* 0x4f42 */
    u8 visible[0x4fe8 - 0x4f66];          /* 0x4f66 */
    u8 goalFrame;                         /* 0x4fe8 */
} PanelDrawView;
}
void dScMgPanel_c::DrawPanels() {
    PanelDrawView *scene = (PanelDrawView *)this;
    extern void *sPanelSprites[];
    extern u8 sPanelFlipFrameSprites[];
    int x;
    int y;
    int spritePriority;
    int kind;
    int face;
    int spriteIndex;
    int panel;

    for (panel = 0; panel < scene->count; panel++) {
        if (scene->visible[panel] != 0) {
            spritePriority = 0;

            if (scene->state == 6) {
                spritePriority = 1;
            }

            kind = scene->kind[panel];
            face = scene->face[panel];
            x = scene->x[panel] >> 12;
            y = scene->y[panel] >> 12;

            spriteIndex = face * 3;
            if (kind == 1) {
                int flipFrame = scene->flipFrame[panel];
                spriteIndex = sPanelFlipFrameSprites[face * 5 + flipFrame];
            }

            Mg_RenderSubSpritePalMtx(sPanelSprites[spriteIndex], x, y, -1, spritePriority, 0);

            if (scene->state == 1) {
                int goalFace = scene->goal[panel];
                int goalFrame = scene->goalFrame;
                DrawOamSprite(sPanelSprites[goalFace * 3 + goalFrame], (void *)x, y, 0);
            } else {
                int goalFace = scene->goal[panel];
                DrawOamSprite(sPanelSprites[goalFace * 3], (void *)x, y, 0);
            }
        }
    }
}

// @symbol _ZN12dScMgPanel_c12CountMatchesEv
void dScMgPanel_c::CountMatches() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int i = 0;

    if (s->mCount <= 0)
        return;
    do {
        if (s->mFace[i] != s->mGoal[i]) {
            s->mKind[i] = 2;
            /* Stepped: `mAnimTimer[i]` moves code. */
            ((dScMgPanel_c *)(scene + (int)(((long long)i)) * 2))->mAnimTimer[0] = 0;
        }
        i++;
    } while (i < s->mCount);
}

// @symbol _ZN12dScMgPanel_c13CountFlippingEv
void dScMgPanel_c::CountFlipping() {
    char *scene = (char *)this;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int cnt;
    int i;
    cnt = 0;
    if (s->mBusy != 0)
        return;

    for (i = 0; i < s->mCount; i++) {
        if (s->mFace[i] != s->mGoal[i]) {
            cnt++;
            break;
        }
    }
    if (cnt != 0)
        return;

    s->mState = 6;
    s->mSolved = 1;
    s->mResultTimer = 0x40;
}

// @symbol _ZN12dScMgPanel_c11CheckSolvedEv
void dScMgPanel_c::CheckSolved() {
    char *scene = (char *)this;
    int found;
    int i;
    int n;
    int tmp;
    if (mBusy == 0)
        return;
    if (mCheckDelay == 0)
        return;
    /* Laundered volatile round trip: the `(int)` cast and the volatile are both
       load-bearing -- they are what turns this decrement into the ROM's
       ldrb/sub/strb instead of a folded one. */
    tmp = *(volatile u8 *)(int *)((int)&((dScMgPanel_c *)scene)->mCheckDelay);
    --tmp;
    *(volatile u8 *)(int *)((int)&((dScMgPanel_c *)scene)->mCheckDelay) = tmp;
    if (mCheckDelay != 0)
        return;
    found = 0;
    i = 0;
    n = ((dScMgPanel_c *)scene)->mCount;
    while (i < n) {
        if (((dScMgPanel_c *)(scene + i))->mFace[0] != ((dScMgPanel_c *)(scene + i))->mGoal[0]) {
            found++;
            break;
        }
        i++;
    }
    if (found != 0)
        return;
    Sound::PlayBank2_2D(0x1cd);
}

// @symbol _ZN12dScMgPanel_c11UpdateTouchEv
/* Accept a new touch within 16 pixels of a panel centre, flip its clipped
   3x3 neighbourhood, and record the move. */
void dScMgPanel_c::UpdateTouch() {
    char *scene = (char *)this;
    // local extern: see above.
    extern u8 gActivePlayerSlot;
    // local extern: see above.
    extern u8 gTouchHeld[];
    // local extern: see above.
    extern u8 gTouchEdge[];
    u8 playerSlot;
    int n;
    int i;
    int touchX, touchY;

    if (mBusy != 0)
        return;

    playerSlot = gActivePlayerSlot;
    n = 0;
    if (gTouchHeld[playerSlot * 4] != 0)
        n = gTouchEdge[playerSlot * 4] != 0;
    if (n == 0)
        return;

    i = 0;
    n = mCount;
    if (n <= 0)
        return;

    {
        u8 *e = &gTouchHeld[playerSlot * 4];
        touchX = e[2];
        touchY = e[3];
    }

    do {
        int dx = touchX - (mX[i] >> 12);
        int dy = touchY - (mY[i] >> 12);
        if (dx >= -0x10 && dx <= 0x10 && dy >= -0x10 && dy <= 0x10) {
            int flipWidth = 3;
            int flipHeight = 3;
            int gridWidth, col, row;

            mTouchLog[mTouchCount] = i;
            mTouchCount++;
            gridWidth = mWidth;
            col = i % gridWidth;
            row = i / gridWidth;

            if (col == 0 || col == gridWidth - 1)
                flipWidth = 2;
            if (row == 0 || row == gridWidth - 1)
                flipHeight = 2;

            if (--col < 0)
                col = 0;
            if (--row < 0)
                row = 0;

            {
                int y;
                for (y = 0; y < flipHeight; y++) {
                    int x;
                    for (x = 0; x < flipWidth; x++) {
                        int index = mWidth * (row + y) + (col + x);
                        ((dScMgPanel_c *)scene)->mKind[index] = 1;
                        ((dScMgPanel_c *)scene)->mFace[index] ^= 1;
                        mFlipDelay[index] = 8;
                    }
                }
            }

            mFlipDelay[i] = 0;
            mCheckDelay = 0x28;
            mMovesUsed++;
            Sound::PlayBank2_2D(0x1fa);
            TickSlideDelay();
            return;
        }
    } while (++i < n);
}

// @symbol _ZN12dScMgPanel_c13ShowAllPanelsEv
/* Show every panel (intro end). */
void dScMgPanel_c::ShowAllPanels() {
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
void dScMgPanel_c::FlipAt(int index) {
    char *scene = (char *)this;
    int w = mWidth;
    int wq = 3;
    int hq = 3;
    int col = index % w;
    int row = index / w;
    if (col == 0 || col == w - 1)
        wq = 2;
    if (row == 0 || row == w - 1)
        hq = 2;
    col = col - 1;
    if (col < 0)
        col = 0;
    row = row - 1;
    if (row < 0)
        row = 0;
    int y, x;
    for (y = 0; y < hq; y++) {
        for (x = 0; x < wq; x++) {
            u8 *p = ((dScMgPanel_c *)((int)scene + ((mWidth) * (row + y) + (col + x))))->mFace;
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
void dScMgPanel_c::DealPuzzle() {
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
        if (dup != 0)
            mismatch = 0;
    } while (HasDuplicateRows() == 0 || mismatch == 0);
}

// @symbol _ZN12dScMgPanel_c14ScramblePanelsEv
#pragma push
#pragma opt_common_subs off
/* The 15-bit random draw, used a dozen times below. */
#define PANEL_RAND ((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)

void dScMgPanel_c::ScramblePanels() {
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
    if (self->IsAltVariant() != 0) {
        int nv;
        if (t >= 0xf) {
            t = (PANEL_RAND * 0xf) >> 15;
            if (t == self->mLastPuzzle) {
                t += ((PANEL_RAND * 0xe) >> 15) + 1;
                if (t >= 0xf)
                    t -= 0xf;
            }
        }
        if (sPanelScrambleFlipEnabled[t] != 0 || self->mDealt == 0) {
            if (self->mDealt == 0)
                self->mDealt += 1;
            nv = (PANEL_RAND * 8) >> 15;
            if (nv == self->mFaceSet) {
                nv = (nv + (((PANEL_RAND * 7) >> 15) + 1)) & 7;
            }
            self->mFaceSet = nv;
        }
        ((dScMgPanel_c *)scene)->mWidth = sPanelScrambleWidths[t];
        {
            int q = ((dScMgPanel_c *)scene)->mWidth;
            ((dScMgPanel_c *)scene)->mCount = q * q;
        }
        self->mFlips = sPanelScrambleFlips[t];
        self->mLastPuzzle = t;
    } else {
        int nv;
        int k;
        u8 *p;
        while (t >= 0x19)
            t -= 0x19;
        if (sPanelScrambleFlipEnabledAlt[t] != 0 || self->mDealt == 0) {
            if (self->mDealt == 0)
                self->mDealt += 1;
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
                    if (nv >= 9)
                        nv -= 5;
                }
            }
            self->mFaceSet = nv;
        }
        ((dScMgPanel_c *)scene)->mWidth = sPanelPuzzleSizes[t];
        {
            int q = ((dScMgPanel_c *)scene)->mWidth;
            ((dScMgPanel_c *)scene)->mCount = q * q;
        }
        self->mFlips = sPanelScrambleFlipsAlt[t];
    }
}
#undef PANEL_RAND
#pragma pop

// @symbol _ZN12dScMgPanel_c16HasDuplicateRowsEv
#pragma push
#pragma opt_strength_reduction off
int dScMgPanel_c::HasDuplicateRows() {
    char *scene = (char *)this;
    int i, bad, j;
    char *row = scene;

    /* Refuse a pick set that repeats any of the last five. */
    for (i = 0; i < 5; i++) {
        bad = 0;
        for (j = 0; j < 0xa; j++) {
            if (((dScMgPanel_c *)scene)->mFlipAt[j] != ((dScMgPanel_c *)row)->mPastFlips[0][j]) {
                bad = 1;
                break;
            }
        }
        if (bad == 0)
            return 0;
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
                ((dScMgPanel_c *)dst)->mPastFlips[0][0] = ((dScMgPanel_c *)src)->mPastFlips[0][k];
                dst++;
            }
            src += 0xa;
        }
    }
    /* mPastFlips[0][i] = mFlipAt[i]; the indexed store renumbers @NNN. */
    for (i = 0; i < 0xa; i++) {
        ((dScMgPanel_c *)(scene + i))->mPastFlips[0][0] = ((dScMgPanel_c *)scene)->mFlipAt[i];
    }
    return 1;
}
#pragma pop

// @symbol _ZN12dScMgPanel_c9ClearGridEv
void dScMgPanel_c::ClearGrid() {
    char *scene = (char *)this;
    int i, j;
    /* Fills mPastFlips with 0xff; `mPastFlips[i][j]` shrinks it and
       renumbers @NNN. */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 0xa; j++) {
            ((dScMgPanel_c *)(scene + j))->mPastFlips[0][0] = 0xff;
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
void dScMgPanel_c::ResetRound() {
    char *scene = (char *)this;
    int i;
    /* mX, mY, mFlipFrame, mAnimTimer and mFlipDelay are stepped: indexed
       they renumber this file's @NNN labels. 0x4ec2 and 0x466c are pads
       that only this function writes. */
    for (i = 0; i < 0x24; i++) {
        ((dScMgPanel_c *)(scene + i * 4))->mX[0] = 0;
        ((dScMgPanel_c *)(scene + i * 4))->mY[0] = 0;
        ((dScMgPanel_c *)(scene + i * 2))->mFlipFrame[0] = 0;
        ((dScMgPanel_c *)(scene + i * 2))->mAnimTimer[0] = 0;
        ((dScMgPanel_c *)(scene + i * 2))->mFlipDelay[0] = 0;
        ((dScMgPanel_c *)scene)->mFace[i] = 0;
        ((dScMgPanel_c *)scene)->mKind[i] = 0;
        ((dScMgPanel_c *)scene)->mGoal[i] = 0;
        ((dScMgPanel_c *)scene)->mVisible[i] = 0;
        ((dScMgPanel_c *)scene)->mFastFlip[i] = 0;
    }
    mState = 0;
    mBusy = 0;
    mResultTimer = 0;
    *(s16 *)(scene + 0x4ec2) = 0;
    mFlipCount = 0;
    mMovesUsed = 0;
    ((dScMgPanel_c *)scene)->mCredits = 0;
    for (i = 0; i < 0x20; i++) {
        ((dScMgPanel_c *)scene)->mFlipAt[i] = 0xff;
    }
    for (i = 0; i < 0x10; i++) {
        ((dScMgPanel_c *)scene)->mTouchLog[i] = 0;
    }
    mTouchCount = 0;
    mInputState = 0;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide = 0;
    s->mSlideY = 0;
    s->mSlideVel = 0;
    *(int *)(scene + 0x466c) = 0;
    s->mHold = 0;
    s->mSlideStep = 0;
    s->mSlideFlag = 0;
    s->mSlideOn = 0;
    s->mBanner.x = 0;
    s->mBanner.y = 0;
    s->mBanner.timer = 0;
    s->mBanner.active = 0;
    s->mBanner.visible = 0;
    s->mBanner.state = 0;
    s->mSolved = 0;
    s->mScrollFrames = 0;
    Shake_Stop();
    Scroll_Stop();
    Particle_ClearAll();
}
#pragma pop

// @symbol _ZN12dScMgPanel_c14Panel_StartAllEv
void dScMgPanel_c::Panel_StartAll() {
    char *scene = (char *)this;
    int i;
    for (i = 0; i < mWidth; i++) {
        mKind[i] = 3;
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
        u8 *flag = &s->mFastFlip[index];
        if (*flag != 0)
            return;
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
                if (((s->mY[nxt] - s->mY[index]) >> 12) > 0)
                    return;
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
    u16 *timers = s->mAnimTimer;
    u8 *q;
    timers[idx] = timers[idx] + 1;
    if ((((int)s->mAnimTimer[idx] >> 3) & 1) != 0) {
        q = s->mVisible;
        q[idx] = 1;
    } else {
        q = s->mVisible;
        q[idx] = 0;
    }
    {
        /* b + 0x30 is mAnimTimer[idx]. The `(long long)` round trip on the
           index and this split base are load-bearing: `s->mAnimTimer[idxL]`
           moves code. */
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
 * Per-panel flip tick for one panel (index).
 * Bumps mBusy.  If the panel's mFlipDelay is running, count it down and
 * clamp at zero; otherwise advance its mAnimTimer and, once it reaches
 * the limit (4 while mFastFlip is set, else 8), reset it, step mFlipFrame
 * and end the flip past 5 by clearing mKind.
 *
 * Shape notes: the RMW sites (`*p = *p +- 1`) go through a pointer local,
 * the plain reads index the member directly, and opt_common_subs off
 * keeps the else-branch recomputing this+index*2 (its own extended basic
 * block) the way the ROM does.  The timer must be bumped before mFastFlip
 * is read, and the limit compare is unsigned.  The unused `scene` local
 * stays: dropping it renumbers the pinned @NNN labels.
 */

void dScMgPanel_c::Panel_StateFlip(int index) {
    char *scene = (char *)this;
    u8 *cnt = &mBusy;

    *cnt = *cnt + 1;

    if (mFlipDelay[index] != 0) {
        u16 *q = &mFlipDelay[index];
        *q = *q - 1;
        if ((s16)mFlipDelay[index] < 0) {
            mFlipDelay[index] = 0;
        }
    } else {
        u16 *r = &mAnimTimer[index];
        u8 flag;
        u32 lim;
        *r = *r + 1;
        flag = mFastFlip[index];
        lim = (flag != 0) ? 4 : 8;
        if (mAnimTimer[index] < lim) {
            return;
        }
        mAnimTimer[index] = 0;
        {
            u16 *s = &mFlipFrame[index];
            *s = *s + 1;
        }
        if (mFlipFrame[index] >= 5) {
            mFlipFrame[index] = 0;
            mKind[index] = 0;
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgPanel_c15Panel_StateIdleEi
void dScMgPanel_c::Panel_StateIdle(int index) {
    char *scene = (char *)this;
    char *base = scene + index;
    /* The separate base local is load-bearing: `mFastFlip[index]`, and the
       stepped form without the local, both move code. */
    ((dScMgPanel_c *)base)->mFastFlip[0] = 0;
}

// @symbol _ZN12dScMgPanel_c19Scene_StateGameOverEv
extern "C" {
struct PanelPmfRec_bc0 {
    int off;
    int adj;
};
typedef void (*PanelPmfFn_bc0)(void *, int);
}
void dScMgPanel_c::Scene_StateGameOver() {
    char *scene = (char *)this;
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    for (i = 0; i < mCount; i++) {
        u8 index = mKind[i];
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
    if (mResultTimer == 0)
        return;
    mResultTimer -= 1;
    if ((s16)mResultTimer > 0)
        return;
    mResultTimer = 0;
    Mg_SetSharedState(MG_STATE_GAME_OVER);
    mPromptEnabled = 0;
    mCreditsOn = 0;
    ClearSlideFlag();
}

// @symbol _ZN12dScMgPanel_c17Scene_StateResultEv
#pragma push
#pragma opt_propagation off
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
            u8 *p = &((dScMgPanel_c *)(scene + i))->mKind[0];
            (((dScMgPanel_c *)scene)->*sPanelPanelStateTable[*p])(i);
            i++;
            if (*p != 0)
                found++;
            n = mCount;
        } while (i < n);
    }
    if (found != 0)
        return;

    if (mResultTimer != 0) {
        int off;
        u16 *tp;
        u8 *flags;
        /* scene + 0x4ec0 is mResultTimer, laundered. `&mResultTimer` in place
           of `scene + off` leaves `off` dead, and dropping it renumbers @NNN. */
        off = 0x4ec0;
        tp = (u16 *)(int)((long long)(int)(scene + off) & 0xFFFFFFFFFFFFFFFFLL);
        /* flags[0xfe6] is mSolved; `&mSolved` moves this code. */
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
                mResultTimer = 0;
        }
        if ((s16)mResultTimer > 0)
            return;
        mResultTimer = 0;
        if (mSolved != 0) {
            dScMgBase_c *g;
            Scroll_Start();
            Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
            ClearSlideFlag();
            mCreditsOn = 0;
            mPromptEnabled = 0;
            g = gMgScene;
            if (g != 0) {
                if (g->mHudScore < 9999)
                    g->mHudScore++;
                if (g->mHudScore > g->mBestHudScore)
                    g->mBestHudScore = g->mHudScore;
            }
            Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
            return;
        }
        mResultTimer = 0x10;
        mState = 7;
        Panel_StartAll();
        return;
    }
    Scroll_Update();
    {
        int inc;
        u16 *ip;
        /* scene + 0x4ec6 is mScrollFrames, laundered the same way. */
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
        u8 index = mKind[i];
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
        u8 index = mKind[i];
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
    char *b = (char *)scene; /* unused; dropping it renumbers @NNN */
    UpdateTouch();
    IntroCard_Update();
    mBusy = 0;
    for (i = 0; i < mCount; i++) {
        u8 index = scene->mKind[i];
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
        self->mGoalTimer = 0;
        self->mGoalFrame = 2;
        return;
    }
    self->mGoalTimer += 1;
    if (self->mGoalTimer >= 8) {
        self->mGoalTimer = 0;
        self->mGoalFrame -= 1;
    }
    if (self->mGoalFrame == 0)
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

extern void Deallocate(void *);
extern int GetOwnerLanguage(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void FreeGfxSlotsById(int arg);
extern void SetSubBg0Offset(int, int);
extern int func_02054d88(void);
extern void func_020562b4(const void *, u32, u32);
extern void func_02056314(void *, u32, u32);
extern void func_02056374(const void *, u32, u32);
extern void func_020563d4(const void *, u32, u32);
extern void func_020564f4(const void *, int, int);
extern void Mg_SubmitScore(int);
extern void Mg_SetTopScreenYOffset(int);
extern void Mg_ShowBanner(int, int, int, int, int, s16);
extern void Mg_DrawScoreTally(void *a, int b, int c, int d);

} /* extern "C" */

// @symbol _ZN12dScMgPanel_c10StartRoundEi
/* Vtable slot 18, an override of dScMgBase_c::StartRound(int).  The
   signature has to repeat the base declaration exactly or mwcc appends a new
   slot instead of overriding it. */

void dScMgPanel_c::StartRound(int flag) {
    extern u8 data_0209d454;
    char *self = (char *)this;

    char *p;

    if (flag == 0) {
        u32 *q = &mLevel;
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
        Mg_SubmitScore(p != 0 ? ((dScMgBase_c *)p)->mHudScore : 0);
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
s32 dScMgPanel_c::Render() {
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
struct Ent_358 {
    int a;
    int b;
};
s32 dScMgPanel_c::Behavior() {
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

s32 dScMgPanel_c::InitResources() {
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
unsigned int data_ov006_0213dc94[2] = {0x01fc00fc, 0xffff2099};
int data_ov006_02142804[7];
