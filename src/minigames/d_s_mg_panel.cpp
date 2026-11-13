//cpp
/* Puzzle Panic, the whole run 0x0210428c..0x0210788c, 72 functions, closing
 * with the factory dScMgPanel_c_classInit, then the state tables and the
 * registry profile, from which mwcc emits __sinit_d_s_mg_panel.cpp. Touch a
 * panel to flip it and its neighbours until every mFace matches mGoal.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; do not
 * reorder or drop the pragma.
 *
 * The destructor is out of line and written first, so this file owns the
 * key function and emits the vtable and typeinfo; the compiler emits D1 and
 * D0 from the one body in cartridge order.
 *
 * The 62 helpers that take the scene as their first argument are members
 * (the address is the method name; no ROM spelling survives), declared in
 * dScMgPanel_c.h. The three empty handlers Shake_StateIdle, Banner_StateIdle
 * and IntroCard_StateDone are members too; the state tables point at them.
 *
 * Every helper reads the scene through plain member access: mFace[i],
 * mShake[index].timer, (this->*table[mKind[i]])(i). Earlier drafts reached
 * these through stepped scene pointers, layout views, volatile and long-long
 * launders and hand-expanded member-pointer calls; each was re-measured and
 * the member spelling is byte-identical (#821). The named locals those drafts
 * carried shifted the per-TU @NNN uniquifier, which the manifest re-keys.
 *
 * Leftover raw site: ResetRound zeroes 0x4ec2 and 0x466c, pads that nothing
 * else reads or writes, so there is no evidence to name them.
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

#pragma defer_codegen off /* .text in source order */

#include "types.h"
#include "nitro/hw/registers.h"
#include "dScMgPanel_c.h"
#include "LevelRng.h"
#include "SpawnInfo.h"

#include "Sound.h"
#include "OamAttr.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_load.h"
#include "nitro/mi/memory.h"
#include "math/CalcLib.h"

/* Callees of the member helpers. Declared here, not in the bodies: a
   declaration inside a member body gets C++ linkage and names a symbol the ROM
   doesn't have. */
extern "C" {

void SetBg2Offset(int x, int y);

void SetSubBg2Offset(int x, int y);

void SetSubBg0Offset(int a, int b);

void FreeGfxSlotsById(int arg);}

struct Matrix2x2;
extern "C" {

void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx);

void Hud_RenderSprite(void *a, int b, int c, int d, int e);

void Mg_SubmitScore(int a);

void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *d);

void Mg_RenderSpriteBoth(void *a, int b, int c, int d, int e);

void Mg_SetSharedState(int a);

void Mg_RenderSubSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5);

void Mg_DrawBigNumber(int a0, int a1, int a2, int a3, int a4, int a5, int a6);}


/* The state tables are arrays of member-function pointers, defined at the end
   of this file. Each dispatcher calls through one of them. */
typedef void (dScMgPanel_c::*PanelStateFn)();
typedef void (dScMgPanel_c::*PanelStateFnI)(int);

/* The complete-object destructor (D1).  Nothing this class owns needs
   destroying, so the whole 0x24 bytes are the compiler's own-vtable write and
   the tail call into ~dScMgBase_c -- the same empty body dScMgCurling_c has. */
dScMgPanel_c::~dScMgPanel_c() {}

/* dScMgPanel_c::~dScMgPanel_c (D0, deleting destructor).  There is no second
   definition to write: mwcc emits D0 from the ONE definition above, and
   dScMgBase_c's own operator delete covers the deallocation.  Under
   `#pragma defer_codegen off` the variants come out D1, D0, D2, which is the
   cartridge order; the trailing D2 is homeless and is deadstripped. */

/* Draw the live particles: 64 slots of 0x18 bytes, sprite chosen by the
   slot's kind byte and positioned from its Fix12 x/y. */
void dScMgPanel_c::Particle_DrawAll() {
    extern int sPanelParticleSprites[];
    int i;
    for (i = 0; i < 0x40; i++) {
        if (mParticles[i].visible != 0) {
            Mg_RenderSpriteBoth((void *)sPanelParticleSprites[mParticles[i].kind],
                                mParticles[i].x >> 12, mParticles[i].y >> 12, -1, -1);
        }
    }
}

/* Advance the 64 particles one frame: gravity on the free ones, a spin/fade
   counter on all of them, and retire a slot once its counter runs out. */
void dScMgPanel_c::Particle_UpdateAll() {
    int i;
    for (i = 0; i < 0x40; i++) {
        if (mParticles[i].active != 0) {
            if (mParticles[i].stuck == 0) {
                mParticles[i].x += mParticles[i].vx;
                mParticles[i].y += mParticles[i].vy;
                mParticles[i].vy -= 0x300;
                if (mParticles[i].vx > 0)
                    mParticles[i].vx += 0xc00;
                else
                    mParticles[i].vx -= 0xc00;
            }
            mParticles[i].tick += 1;
            if (mParticles[i].tick >= 4) {
                mParticles[i].frame += 1;
                if (mParticles[i].frame >= 5) {
                    mParticles[i].active = 0;
                    mParticles[i].visible = 0;
                }
            }
        }
    }
}

/* Spawn one particle in the first free slot, at (x, y).  `mode` picks the
   fixed-drift variant over the random one. */
void dScMgPanel_c::Particle_Spawn(int x, int y, int mode) {
    int i;
    for (i = 0; i < 0x40; i++) {
        if (mParticles[i].active == 0) {
            mParticles[i].active = 1;
            mParticles[i].visible = 1;
            mParticles[i].tick = 0;
            mParticles[i].kind = 0;
            mParticles[i].frame = 0;
            mParticles[i].stuck = mode;
            mParticles[i].vx = 0;
            mParticles[i].vy = 0;
            mParticles[i].x = x;
            mParticles[i].y = y;
            if (mode != 0) {
                mParticles[i].drift = 0;
            } else {
                mParticles[i].drift =
                    ((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) << 1) >> 15;
            }
            if (x >= 0x80000)
                mParticles[i].vx = -0x100;
            else
                mParticles[i].vx = 0x100;
            return;
        }
    }
}

/* Retire every particle in the 64-slot pool. */
void dScMgPanel_c::Particle_ClearAll() {
    int i;
    for (i = 0; i < 0x40; i++) {
        mParticles[i].active = 0;
        mParticles[i].visible = 0;
    }
}

void dScMgPanel_c::Scroll_Update() {
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
            r = ((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
            x = ((r * 7) >> 15) * 5;
            r = ((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
            y = ((r << 4) >> 15) + 0xb0;
            Particle_Spawn(x << 12, y << 12, 0);

            r = (u32)RandomIntInternal(&data_0209d4b8.value);
            r2 = (u32)RandomIntInternal(&data_0209d4b8.value);
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

            x = (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * 7) >> 15) * 5;
            Particle_Spawn(x << 12, y, 1);
            x = (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * 7) >> 15) * 5;
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

void dScMgPanel_c::Scroll_Start() {
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

void dScMgPanel_c::Scroll_Stop() {
    mScroll.on = 0;
    SetBg2Offset(0, 0);
    SetSubBg2Offset(0, 0);
}

void dScMgPanel_c::EnableOverlayLayer() {
    extern u8 gSubScreenLayers[];
    if (mShake[0].state != 0)
        return;
    mShake[0].state = 1;
    mShake[0].timer = 0;
    gSubScreenLayers[0] |= 1;
}

void dScMgPanel_c::Shake_StateLoseTry(int index) {
    extern u8 gSubScreenLayers;
    extern int data_ov006_0212ed00[];
    mShake[index].timer++;
    if (mShake[index].timer >= 0x20) {
        mShake[index].timer = 0;
        mShake[index].state = 0;
        SetSubBg0Offset(0, 0);
        gSubScreenLayers &= ~1;
        func_02012790(0x12f);
        mCredits--;
        return;
    }
    mShake[index].offset = data_ov006_0212ed00[mShake[index].timer >> 3];
    if (mShake[index].timer == 1 || mShake[index].timer == 0x11)
        Sound::PlayBank2_2D(0x1fc);
    SetSubBg0Offset(0, mShake[index].offset);
}

extern "C" {

void SetSubBg0Offset(int a, int b);}

void dScMgPanel_c::Shake_StateShake(int index) {
    extern int sPanelShakeOffsets[];
    mShake[index].timer++;
    if (mShake[index].timer >= 0x20) {
        mShake[index].timer = 0;
        mShake[index].state = 2;
        return;
    }
    mShake[index].offset = sPanelShakeOffsets[mShake[index].timer >> 3];
    if (mShake[index].timer == 1 || mShake[index].timer == 0x11)
        Sound::PlayBank2_2D(0x1fc);
    SetSubBg0Offset(0, mShake[index].offset);
}

void dScMgPanel_c::Shake_StateIdle(int index) {}

void dScMgPanel_c::Shake_Update() {
    extern PanelStateFnI sPanelShakeStateTable[];
    if (mShake[0].active == 0)
        return;
    int state = mShake[0].state;
    (this->*sPanelShakeStateTable[state])(0);
}

void dScMgPanel_c::Shake_Start() {
    mShake[0].active = 1;
    mShake[0].unk_00 = 0;
    mShake[0].offset = 0;
    mShake[0].timer = 0;
    mShake[0].state = 0;
}

void dScMgPanel_c::Shake_Stop() {
    mShake[0].active = 0;
}

void dScMgPanel_c::Banner_Draw() {
    extern void *sPanelBannerSprites[];
    if (mBanner.visible == 0)
        return;
    Mg_RenderSubSpriteMtx(sPanelBannerSprites[0], mBanner.x >> 12, mBanner.y >> 12, 0);
}

void dScMgPanel_c::Banner_StateIdle() {}

void dScMgPanel_c::Banner_StateSettle() {
    u16 *p = &mBanner.hold;
    *p = *p + 1;
    if (mBanner.hold < 8)
        return;
    mBanner.hold = 0;
    {
        int *q = &mBanner.y;
        *q = *q + 0x4000;
    }
    mBanner.state = 2;
}

void dScMgPanel_c::Banner_StatePopUp() {
    u16 *h = &mBanner.hold;
    *h = *h + 1;
    if (mBanner.hold < 4)
        return;
    mBanner.hold = 0;
    {
        int *w = &mBanner.y;
        *w = *w - 0x4000;
    }
    mBanner.state = 1;
}

void dScMgPanel_c::Banner_Update() {
    extern PanelStateFn sPanelBannerStateTable[];
    if (mBanner.active == 0)
        return;
    mBanner.timer--;
    if ((s16)mBanner.timer <= 0) {
        mBanner.timer = 0;
        mBanner.active = 0;
        mBanner.visible = 0;
        return;
    }
    (this->*sPanelBannerStateTable[mBanner.state])();
}

void dScMgPanel_c::Banner_Show(int i) {
    mBanner.active = 1;
    mBanner.visible = 1;
    mBanner.x = mX[i];
    mBanner.y = mY[i] + 0x8000;
    mBanner.timer = 0x30;
    mBanner.hold = 0;
    mBanner.state = 0;
}

void dScMgPanel_c::DrawCursor() {
    extern void *sPanelCursorSprites[];
    if (mCreditsOn == 0)
        return;
    Hud_RenderSprite(sPanelCursorSprites[mCredits], 0xf0, 0x24, -1, -1);
}

void dScMgPanel_c::DrawHud() {
    extern void *sPanelHudSprites[];
    extern void *data_ov006_02136de8[];
    if (mSlideFlag == 0)
        return;
    if (mSlideStep != 4) {
        int a = mSlide >> 12;
        int b = mSlideY >> 12;
        int index;
        Mg_DrawBigNumber(a, b, mFlipCount, -1, -1, 0, 0);
        index = 1;
        if (mFlipCount >= 2)
            index = 0;
        Mg_RenderSpriteBoth(sPanelHudSprites[index], a + 0x2c, b, -1, -1);
    } else {
        int x = mSlideLeft;
        Hud_RenderSprite(data_ov006_02136de8[x], mSlide >> 12, mSlideY >> 12, -1, -1);
    }
}

void dScMgPanel_c::ReloadSlideDelay() {
    mSlideLeft = mFlipCount;
}

// If mSlideLeft (self+0x4677) is set, decrement it: the ROM's predicated
// ldrbne/subne/strbne tail.
void dScMgPanel_c::TickSlideDelay() {
    if (mSlideLeft)
        mSlideLeft -= 1;
}

void dScMgPanel_c::ClearSlideFlag() {
    mSlideFlag = 0;
}

void dScMgPanel_c::SetSlideFlag() {
    mSlideFlag = 1;
}

void dScMgPanel_c::IntroCard_StateDone() {}

void dScMgPanel_c::IntroCard_StateSlideOut() {
    mSlide += mSlideVel;
    mSlideVel -= 0x400;
    if ((mSlide >> 12) > -0x40)
        return;
    mIntro++;
    mSlideStep = 4;
    mSlide = 0x10000;
    mSlideY = 0x24000;
    FreeGfxSlotsById(0x1d);
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    mCreditsOn = 1;
    mSlideLeft = mFlipCount;
    mHold = 0;
    ShowAllPanels();
    Sound::PlayBank2_2D(0x1fb);
}

void dScMgPanel_c::IntroCard_StateHold() {
    if (mHold != 0) {
        u16 *p = &mHold;
        *p = *p - 1;
        if (*(s16 *)&mHold < 0)
            mHold = 0;
    } else {
        mSlideVel = -0x3000;
        mSlideStep = 3;
    }
}

void dScMgPanel_c::IntroCard_StateSlideIn() {
    mSlide += mSlideVel;
    mSlideVel -= 0x400;
    if (mSlide >> 12 > 0x58)
        return;
    mSlide = 0x58000;
    mSlideStep = 2;
    mHold = 0x80;
}

void dScMgPanel_c::IntroCard_StateStart() {
    mSlide = 0x110000;
    mSlideY = 0x60000;
    mSlideVel = -0x3000;
    mSlideFlag = 1;
    mSlideStep = 1;
}

void dScMgPanel_c::IntroCard_Update() {
    extern PanelStateFn sPanelIntroCardStateTable[];
    if (!mSlideOn)
        return;
    (this->*sPanelIntroCardStateTable[mSlideStep])();
}

void dScMgPanel_c::IntroCard_Start() {
    mSlideOn = 1;
    mSlideStep = 0;
    mSlideLeft = 0;
}

void dScMgPanel_c::StartSolveCheck() {
    if (mBusy != 0)
        return;
    if (mState == 6)
        return;
    if (mMovesUsed < mFlipCount)
        return;

    func_02012790(0xe);
    EnableOverlayLayer();

    if (mCredits > 1) {
        mState = 3;
        CountMatches();
    } else {
        mSolved = 0;
        CountMatches();
        mState = 6;
        mResultTimer = 0x50;
    }
}

void dScMgPanel_c::Answer_StateFlipSolution() {
    int index;
    int w;
    int col;
    int row;
    int hq;
    int wq;
    int y;
    int x;

    if (mDelay != 0) {
        mDelay--;
        if ((s16)mDelay < 0)
            mDelay = 0;
        return;
    }

    if (mReplayStep >= mFlipCount) {
        mResultTimer = 0x50;
        mState = 6;
        mSolved = 0;
        return;
    }

    index = mFlipAt[mReplayStep];
    w = mWidth;
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
            int cell = mWidth * (row + y) + (col + x);

            mKind[cell] = 1;
            mFace[cell] ^= 1;
            mFlipDelay[cell] = 8;
        }
    }

    Banner_Show(index);
    mFlipDelay[index] = 0;
    mDelay = 0x30;
    mReplayStep++;
}

void dScMgPanel_c::Answer_StateFindSolution() {
    extern u8 *sPanelPuzzles[];
    extern u8 *sPanelPuzzlesAlt[];
    int found;
    int i;
    int j;
    int li;
    int arr[36];
    int idx;
    int wq;
    int hq;
    int col;
    int row;
    int y;
    int x;

    if (mDelay != 0) {
        mDelay--;
        if ((s16)mDelay < 0) {
            mDelay = 0;
        }
        return;
    }

    found = 0;
    for (i = 0; i < mCount; i++) {
        if (mKind[i] != 0) {
            found++;
            break;
        }
    }
    if (found) {
        return;
    }

    for (i = 0; i < 0x24; i++) {
        arr[i] = 0;
    }

    for (j = 0; j < mCount; j++) {
        idx = mFaceSet;
        if (IsAltVariant() != 0) {
            arr[j] = sPanelPuzzlesAlt[idx][j];
        } else {
            arr[j] = sPanelPuzzles[idx][j];
        }
    }

    for (li = 0; li < mFlipCount; li++) {
        idx = mFlipAt[li];
        wq = 3;
        hq = 3;
        col = idx % mWidth;
        row = idx / mWidth;
        if (col == 0 || col == mWidth - 1) {
            wq = 2;
        }
        if (row == 0 || row == mWidth - 1) {
            hq = 2;
        }
        if (--col < 0) {
            col = 0;
        }
        if (--row < 0) {
            row = 0;
        }
        for (y = 0; y < hq; y++) {
            for (x = 0; x < wq; x++) {
                arr[mWidth * (row + y) + (col + x)] ^= 1;
            }
        }
    }

    for (j = 0; j < mCount; j++) {
        if (mFace[j] != arr[j]) {
            mKind[j] = 1;
            mFace[j] ^= 1;
            mFastFlip[j] = 1;
        }
    }

    mDelay = 0x40;
    mInputState = 3;
    mReplayStep = 0;
}

void dScMgPanel_c::Answer_StateResetBoard() {
    if (mDelay != 0) {
        mDelay--;
        if ((s16)mDelay < 0) {
            mDelay = 0;
        }
        return;
    } else {
        int k;
        for (k = 0; k < mCount; k++) {
            mKind[k] = 1;
            mFace[k] ^= 1;
            mFastFlip[k] = 1;
        }
        mInputState = 2;
        mDelay = 0x20;
    }
}


typedef struct WarpEntry_730 {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
}
 WarpEntry_730;

void dScMgPanel_c::Answer_StateWaitButton() {
    // local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
    extern u8 gActivePlayerSlot;
    // local extern: see above.
    extern WarpEntry_730 gTouchHeld[];
    u8 idx;
    s32 b;
    s32 dx;
    s32 dy;

    if (mBusy != 0) {
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
    mState = 5;
    mDelay = 0x20;
}

void dScMgPanel_c ::Answer_Update() {
    extern PanelStateFn sPanelAnswerStateTable[];
    if (IsAltVariant() != 0)
        return;
    (this->*sPanelAnswerStateTable[mInputState])();
    Banner_Update();
}

void dScMgPanel_c::UpdateMoves() {
    if (mDelay != 0) {
        mDelay--;
        if ((s16)mDelay < 0)
            mDelay = 0;
        return;
    }

    {
        int found = 0;
        int i;
        int n = mCount;
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
                mFace[cell] ^= 1;
                mFlipDelay[cell] = 8;
                mFastFlip[cell] = 1;
            }
        }
        mFlipDelay[index] = 0;
        Sound::PlayBank2_2D(0x1fb);
    }
}

void dScMgPanel_c::DrawPanels() {
    extern OamAttr *sPanelSprites[];
    extern u8 sPanelFlipFrameSprites[];
    int x;
    int y;
    int spritePriority;
    int kind;
    int face;
    int spriteIndex;
    int panel;

    for (panel = 0; panel < mCount; panel++) {
        if (mVisible[panel] != 0) {
            spritePriority = 0;

            if (mState == 6) {
                spritePriority = 1;
            }

            kind = mKind[panel];
            face = mFace[panel];
            x = mX[panel] >> 12;
            y = mY[panel] >> 12;

            spriteIndex = face * 3;
            if (kind == 1) {
                int flipFrame = mFlipFrame[panel];
                spriteIndex = sPanelFlipFrameSprites[face * 5 + flipFrame];
            }

            Mg_RenderSubSpritePalMtx(sPanelSprites[spriteIndex], x, y, -1, spritePriority, 0);

            if (mState == 1) {
                int goalFace = mGoal[panel];
                int goalFrame = mGoalFrame;
                DrawOamSprite(sPanelSprites[goalFace * 3 + goalFrame], x, y, 0);
            } else {
                int goalFace = mGoal[panel];
                DrawOamSprite(sPanelSprites[goalFace * 3], x, y, 0);
            }
        }
    }
}

void dScMgPanel_c::CountMatches() {
    int i;

    for (i = 0; i < mCount; i++) {
        if (mFace[i] != mGoal[i]) {
            mKind[i] = 2;
            mAnimTimer[i] = 0;
        }
    }
}

void dScMgPanel_c::CountFlipping() {
    int cnt;
    int i;
    cnt = 0;
    if (mBusy != 0)
        return;

    for (i = 0; i < mCount; i++) {
        if (mFace[i] != mGoal[i]) {
            cnt++;
            break;
        }
    }
    if (cnt != 0)
        return;

    mState = 6;
    mSolved = 1;
    mResultTimer = 0x40;
}

void dScMgPanel_c::CheckSolved() {
    int found;
    int i;
    if (mBusy == 0)
        return;
    if (mCheckDelay == 0)
        return;
    mCheckDelay--;
    if (mCheckDelay != 0)
        return;
    found = 0;
    for (i = 0; i < mCount; i++) {
        if (mFace[i] != mGoal[i]) {
            found++;
            break;
        }
    }
    if (found != 0)
        return;
    Sound::PlayBank2_2D(0x1cd);
}

/* Accept a new touch within 16 pixels of a panel centre, flip its clipped
   3x3 neighbourhood, and record the move. */
void dScMgPanel_c::UpdateTouch() {
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
                        mKind[index] = 1;
                        mFace[index] ^= 1;
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

/* Show every panel (intro end). */
void dScMgPanel_c::ShowAllPanels() {
    int i;
    for (i = 0; i < mCount; i++) {
        mVisible[i] = 1;
    }
}

/* Flip the faces in the 3x3 neighbourhood centred on panel `idx`, clipped at
   the board edges.  The same walk appears in Answer_StateFlipSolution,
   UpdateMoves and UpdateTouch; only this one touches the face
   table alone. */
void dScMgPanel_c::FlipAt(int index) {
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
            mFace[mWidth * (row + y) + (col + x)] ^= 1;
        }
    }
}

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
void dScMgPanel_c::DealPuzzle() {
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
            u32 rnd = (u32)RandomIntInternal(&data_0209d4b8.value);
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

/* The 15-bit random draw, used a dozen times below. */
#define PANEL_RAND ((((u32)RandomIntInternal(&data_0209d4b8.value)) >> 16) & 0x7fff)

void dScMgPanel_c::ScramblePanels() {
    extern u8 sPanelScrambleFlipEnabled[];
    extern u8 sPanelScrambleFlips[];
    extern u8 sPanelScrambleWidths[];
    extern u8 sPanelScrambleFlipEnabledAlt[];
    extern u8 sPanelScrambleFlipsAlt[];
    extern u8 sPanelPuzzleSizes[];
    int t = (int)mLevel;
    if (IsAltVariant() != 0) {
        int nv;
        if (t >= 0xf) {
            t = (PANEL_RAND * 0xf) >> 15;
            if (t == mLastPuzzle) {
                t += ((PANEL_RAND * 0xe) >> 15) + 1;
                if (t >= 0xf)
                    t -= 0xf;
            }
        }
        if (sPanelScrambleFlipEnabled[t] != 0 || mDealt == 0) {
            if (mDealt == 0)
                mDealt += 1;
            nv = (PANEL_RAND * 8) >> 15;
            if (nv == mFaceSet) {
                nv = (nv + (((PANEL_RAND * 7) >> 15) + 1)) & 7;
            }
            mFaceSet = nv;
        }
        mWidth = sPanelScrambleWidths[t];
        mCount = mWidth * mWidth;
        mFlips = sPanelScrambleFlips[t];
        mLastPuzzle = t;
    } else {
        int nv;
        while (t >= 0x19)
            t -= 0x19;
        if (sPanelScrambleFlipEnabledAlt[t] != 0 || mDealt == 0) {
            if (mDealt == 0)
                mDealt += 1;
            nv = (PANEL_RAND * 4) >> 15;
            if (sPanelPuzzleSizes[t] == 5) {
                nv = ((PANEL_RAND * 5) >> 15) + 4;
            }
            if (nv == mFaceSet) {
                if (sPanelPuzzleSizes[t] == 4) {
                    nv = (nv + (((PANEL_RAND * 3) >> 15) + 1)) & 3;
                } else {
                    nv = nv + (((PANEL_RAND * 4) >> 15) + 1);
                    if (nv >= 9)
                        nv -= 5;
                }
            }
            mFaceSet = nv;
        }
        mWidth = sPanelPuzzleSizes[t];
        mCount = mWidth * mWidth;
        mFlips = sPanelScrambleFlipsAlt[t];
    }
}
#undef PANEL_RAND

int dScMgPanel_c::HasDuplicateRows() {
    int i, bad, j;

    /* Refuse a pick set that repeats any of the last five. */
    for (i = 0; i < 5; i++) {
        bad = 0;
        for (j = 0; j < 0xa; j++) {
            if (mFlipAt[j] != mPastFlips[i][j]) {
                bad = 1;
                break;
            }
        }
        if (bad == 0)
            return 0;
    }
    /* Shift the history down one row, then record this pick set as row 0. */
    {
        int r, k;
        for (r = 0; r < 4; r++) {
            for (k = 0; k < 0xa; k++) {
                mPastFlips[r + 1][k] = mPastFlips[r][k];
            }
        }
    }
    for (i = 0; i < 0xa; i++) {
        mPastFlips[0][i] = mFlipAt[i];
    }
    return 1;
}

void dScMgPanel_c::ClearGrid() {
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 0xa; j++) {
            mPastFlips[i][j] = 0xff;
        }
    }
}

/* Clear the whole board back to its start-of-round state: the 36 parallel
   panel arrays, the state word, the pick list (0xff = no pick) and the
   cursor, then hand off to the three subsystem resets. */
void dScMgPanel_c::ResetRound() {
    char *scene = (char *)this;
    /* 0x4ec2 and 0x466c are pads that only this function writes. */
    int i;
    for (i = 0; i < 0x24; i++) {
        mX[i] = 0;
        mY[i] = 0;
        mFlipFrame[i] = 0;
        mAnimTimer[i] = 0;
        mFlipDelay[i] = 0;
        mFace[i] = 0;
        mKind[i] = 0;
        mGoal[i] = 0;
        mVisible[i] = 0;
        mFastFlip[i] = 0;
    }
    mState = 0;
    mBusy = 0;
    mResultTimer = 0;
    *(s16 *)(scene + 0x4ec2) = 0;
    mFlipCount = 0;
    mMovesUsed = 0;
    mCredits = 0;
    for (i = 0; i < 0x20; i++) {
        mFlipAt[i] = 0xff;
    }
    for (i = 0; i < 0x10; i++) {
        mTouchLog[i] = 0;
    }
    mTouchCount = 0;
    mInputState = 0;
    mSlide = 0;
    mSlideY = 0;
    mSlideVel = 0;
    *(int *)(scene + 0x466c) = 0;
    mHold = 0;
    mSlideStep = 0;
    mSlideFlag = 0;
    mSlideOn = 0;
    mBanner.x = 0;
    mBanner.y = 0;
    mBanner.timer = 0;
    mBanner.active = 0;
    mBanner.visible = 0;
    mBanner.state = 0;
    mSolved = 0;
    mScrollFrames = 0;
    Shake_Stop();
    Scroll_Stop();
    Particle_ClearAll();
}

void dScMgPanel_c::Panel_StartAll() {
    int i;
    for (i = 0; i < mWidth; i++) {
        mKind[i] = 3;
    }
}

void dScMgPanel_c::Panel_StateFall(int index) {
    int old = mY[index];
    mY[index] = old + 0x10000;
    if ((mY[index] >> 12) >= 0xd0) {
        mKind[index] = 0;
        mVisible[index] = 0;
    }

    /* Hand the fall on to the panel directly below, unless this one is already
       on the bottom row or the one below has not come far enough down yet.
       The row/column split is a subtract loop, not a divide, and the nested
       blocks are what keep it that way -- flattening them costs 18 words. */
    {
        u8 *flag = &mFastFlip[index];
        if (*flag != 0)
            return;
        {
            int col = index;
            int row = 0;
            int div = mWidth;
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
                if (((mY[nxt] - mY[index]) >> 12) > 0)
                    return;
                *flag = 1;
                mKind[nxt] = 3;
            }
        }
    }
}

void dScMgPanel_c::Panel_StateBlink(int idx) {
    mAnimTimer[idx]++;
    if ((mAnimTimer[idx] >> 3) & 1) {
        mVisible[idx] = 1;
    } else {
        mVisible[idx] = 0;
    }
    if (mAnimTimer[idx] < 0x40)
        return;
    mAnimTimer[idx] = 0;
    mKind[idx] = 0;
    mVisible[idx] = 1;
}

/*
 * Per-panel flip tick for one panel (index).
 * Bumps mBusy.  If the panel's mFlipDelay is running, count it down and
 * clamp at zero; otherwise advance its mAnimTimer and, once it reaches
 * the limit (4 while mFastFlip is set, else 8), reset it, step mFlipFrame
 * and end the flip past 5 by clearing mKind.
 */
void dScMgPanel_c::Panel_StateFlip(int index) {
    mBusy++;
    if (mFlipDelay[index] != 0) {
        mFlipDelay[index]--;
        if ((s16)mFlipDelay[index] < 0) {
            mFlipDelay[index] = 0;
        }
    } else {
        mAnimTimer[index]++;
        u32 limit = mFastFlip[index] != 0 ? 4 : 8;
        if (mAnimTimer[index] < limit) {
            return;
        }
        mAnimTimer[index] = 0;
        mFlipFrame[index]++;
        if (mFlipFrame[index] >= 5) {
            mFlipFrame[index] = 0;
            mKind[index] = 0;
        }
    }
}

void dScMgPanel_c::Panel_StateIdle(int index) {
    mFastFlip[index] = 0;
}

void dScMgPanel_c::Scene_StateGameOver() {
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    for (i = 0; i < mCount; i++) {
        (this->*sPanelPanelStateTable[mKind[i]])(i);
    }
    if (mResultTimer == 0)
        return;
    mResultTimer--;
    if ((s16)mResultTimer > 0)
        return;
    mResultTimer = 0;
    Mg_SetSharedState(MG_STATE_GAME_OVER);
    mPromptEnabled = 0;
    mCreditsOn = 0;
    ClearSlideFlag();
}

void dScMgPanel_c::Scene_StateResult() {
    extern PanelStateFnI sPanelPanelStateTable[];
    // local extern: see above.
    extern u8 gActivePlayerSlot;
    // local extern: see above.
    extern u8 gTouchHeld[];
    // local extern: see above.
    extern u8 gTouchEdge[];
    int found;
    int i;

    IntroCard_Update();
    found = 0;
    for (i = 0; i < mCount; i++) {
        (this->*sPanelPanelStateTable[mKind[i]])(i);
        if (mKind[i] != 0)
            found++;
    }
    if (found != 0)
        return;

    if (mResultTimer != 0) {
        mResultTimer--;
        if (mSolved == 0) {
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
    mScrollFrames++;
}

void dScMgPanel_c::Scene_StateShowAnswer() {
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    IntroCard_Update();
    Answer_Update();
    for (i = 0; i < mCount; i++) {
        (this->*sPanelPanelStateTable[mKind[i]])(i);
    }
}

void dScMgPanel_c::Scene_StateRewind() {
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    IntroCard_Update();
    mBusy = 1;
    UpdateMoves();
    for (i = 0; i < mCount; i++) {
        (this->*sPanelPanelStateTable[mKind[i]])(i);
    }
}

void dScMgPanel_c::Scene_StateShowMisses() {
    extern PanelStateFnI sPanelPanelStateTable[];
    u8 index;
    int found;
    int i;
    IntroCard_Update();
    found = 0;
    mBusy = 1;
    for (i = 0; i < mCount; i++) {
        index = mKind[i];
        (this->*sPanelPanelStateTable[index])(i);
        if (index != 0)
            found++;
    }
    if (found == 0) {
        mState = 4;
        mClear = 1;
        mDelay = 0x40;
    }
}

void dScMgPanel_c::Scene_StatePlay() {
    extern PanelStateFnI sPanelPanelStateTable[];
    int i;
    UpdateTouch();
    IntroCard_Update();
    mBusy = 0;
    for (i = 0; i < mCount; i++) {
        (this->*sPanelPanelStateTable[mKind[i]])(i);
    }
    CheckSolved();
    CountFlipping();
    StartSolveCheck();
}

void dScMgPanel_c::Scene_StateIntro() {
    if (mIntro == 0) {
        IntroCard_Update();
        mGoalTimer = 0;
        mGoalFrame = 2;
        return;
    }
    mGoalTimer += 1;
    if (mGoalTimer >= 8) {
        mGoalTimer = 0;
        mGoalFrame -= 1;
    }
    if (mGoalFrame == 0)
        mState = 2;
}

void dScMgPanel_c::Scene_StateInit() {
    IntroCard_Start();
    mState = 1;
    mIntro = 0;
}

/* The four class methods can't sit in an `extern "C"` region, so a
 * declaration inside them would get C++ linkage and name a symbol the ROM
 * doesn't have. Their callees are declared here instead, below every helper,
 * so the helpers' own declarations are untouched. Data declarations are not
 * mangled, so those stay in the bodies. */



extern void Deallocate(void *);

extern int GetOwnerLanguage(void);extern "C" {

extern void *LoadFile(int handle);

extern void DecompressLZ16(void *src, void *dst);

extern void FreeGfxSlotsById(int arg);

extern void SetSubBg0Offset(int, int);}

/* local extern: nitro/gx/gx_load.h leaves GX_LoadBG3Scr out until
 * decl_common.h's non-const spelling of it goes. */
extern void GX_LoadBG3Scr(const void *, u32, u32);extern "C" {

extern void Mg_SubmitScore(int);

extern void Mg_SetTopScreenYOffset(int);

extern void Mg_ShowBanner(int, int, int, int, int, s16);

extern void Mg_DrawScoreTally(void *a, int b, int c, int d);}
 /* extern "C" */

/* Vtable slot 18, an override of dScMgBase_c::StartRound(int).  The
   signature has to repeat the base declaration exactly or mwcc appends a new
   slot instead of overriding it. */

void dScMgPanel_c::StartRound(int flag) {
    extern u8 gSubScreenLayers;

    if (flag == 0) {
        mLevel++;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        unk_4fea = 0;
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;

        if (gMgScene != 0)
            gMgScene->mHudScore = 0;

        Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
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

    gSubScreenLayers &= ~1;
}

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

/* Vtable slot 6.  One frame of the current state, dispatched through the
   descriptor pair at sPanelSceneStateTable. */
s32 dScMgPanel_c::Behavior() {
    extern PanelStateFn sPanelSceneStateTable[];
    (this->*sPanelSceneStateTable[mState])();
    Shake_Update();
    Particle_UpdateAll();
    return 1;
}

/* Vtable slot 0.  Brings up both screens' backgrounds, loads the panel and
   message graphics for the owner's language, then resets the board. */

s32 dScMgPanel_c::InitResources() {
    extern u8 gMainScreenLayers;
    extern u8 gSubScreenLayers;
    char *b;
    volatile u16 mainFill;
    volatile u16 subFill;
    void *file;
    void *msgFile;

    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    G2_SetBG3Priority(1);
    G2_BG3Mosaic(false);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3Priority(1);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);

    file = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCG_BIN);
    DecompressLZ16(file, (void *)((int)G2_GetBG3CharPtr() + 0x4000));
    Deallocate(file);

    file = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCL_BIN);
    GX_LoadBGPltt(file, 0x1a0, 0x60);
    Deallocate(file);

    file = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_UP_NSC_BIN);
    GX_LoadBG3Scr(file, 0, 0x800);
    Deallocate(file);

    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    G2_BG2Mosaic(false);
    G2_SetBG2Offset(0, 0);
    G2_SetBG2Priority(1);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                         GX_BG_CHARBASE_0x10000);

    b = (char *)G2_GetBG2ScrPtr();
    mainFill = 0xf361;
    MI_CpuFill16(b, mainFill, 0x1000);

    file = LoadFile(ASSET_D_2D_YOSIFINX_NORMAL_NSC_BIN);
    GX_LoadBG2Scr(file, 0x800, 0x800);
    Deallocate(file);

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
    DecompressLZ16(msgFile, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(file, 0, 0x100);

    gSubScreenLayers |= 8;
    G2S_SetBG3Priority(1);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(0, 0);
    G2S_SetBG3Priority(1);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x10000);

    {
        void *subFile = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCG_BIN);
        DecompressLZ16(subFile, (void *)((char *)G2S_GetBG3CharPtr() + 0x4000));
        Deallocate(subFile);

        subFile = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_NCL_BIN);
        GXS_LoadBGPltt(subFile, 0x1a0, 0x60);
        Deallocate(subFile);

        subFile = LoadFile(ASSET_D_2D_MG_BG_PAZZLE_PANEL_DOWN_NSC_BIN);
        GXS_LoadBG3Scr(subFile, 0, 0x800);
        Deallocate(subFile);

        gSubScreenLayers |= 4;
        G2S_BG2Mosaic(false);
        G2S_SetBG2Offset(0, 0);
        G2S_SetBG2Priority(1);
        G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                              GX_BG_CHARBASE_0x10000);

        b = (char *)G2S_GetBG2ScrPtr();
        subFill = 0xf361;
        MI_CpuFill16(b, subFill, 0x1000);

        subFile = LoadFile(ASSET_D_2D_YOSIFINX_NORMAL_NSC_BIN);
        GXS_LoadBG2Scr(subFile, 0, 0x800);
        Deallocate(subFile);

        G2S_SetBG0Priority(0);
        G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                          GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);

        subFile = LoadFile(ASSET_D_2D_YOSIFINX_BIKKURI_NSC_BIN);
        GXS_LoadBG0Scr(subFile, 0, 0x800);
        Deallocate(subFile);
    }

    DecompressLZ16(msgFile, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(file, 0, 0x100);
    Deallocate(msgFile);
    Deallocate(file);

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
 dScMgPanel_c *dScMgPanel_c_classInit() {
    return new dScMgPanel_c;
}extern /* The state tables, in the order the static initializer fills them. Each
   entry's {function, 0} record is a compiler temporary in .data. The two
   sprite triples (OamAttr lists) are copied whole from data_ov006_02136cf0 and
   data_ov006_02136d1c. */
 OamAttr *data_ov006_02136cf0[3];
extern OamAttr *data_ov006_02136d1c[3];


OamAttr *sPanelSprites[6] = {
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
