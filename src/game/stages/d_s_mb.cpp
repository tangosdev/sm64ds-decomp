//cpp
/* dScMB_c -- the multi-boot download scene, arm9.
 *
 * .text 0x02034a78..0x02035354. Twelve class functions plus the factory
 * dScMB_c_classInit, which abuts InitResources and stores this class's
 * vtable. The next symbol, dBgCh::IsOwnedBy, opens the collision-query
 * run and is not part of the scene.
 *
 * _ZTS7dScMB_c 0x02094364. _ZTI7dScMB_c 0x02094370, base word _ZTI8dScene_c.
 * Nested _ZTIN7dScMB_c15graphCallback_cE 0x0209437c,
 * _ZTSN7dScMB_c15graphCallback_cE 0x020943a0. _ZTV7dScMB_c 0x020943c4.
 * The out-of-line destructor is the key function. Under
 * `#pragma defer_codegen off` it emits D1 (0x02034a78), D0 (0x02034ac0),
 * then a base-object D2 the cartridge does not carry. That pragma also
 * makes emission ROM-ascending, so this file is written in ROM order.
 */

// leftovers: func_02034b1c initialises the global graph callback, retaining
// the raw store to its unmodelled byte at +0x0f; it keeps its placeholder name
// and its untyped parameters because include/decl_common.h declares it.
// LoadMultiBootResources stays free (no receiver).
#include "dScMB_c.h"
#include "nitro/hw/registers.h"
#include "AsyncJob.h"
#include "decl_common.h"
#include "Heap.h"
#include "OAM.h"
#include "OamAttr.h"
#include "Sound.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/mi/memory.h"

extern "C" {
void DecompressLZ16(void *src, void *dst);

void func_020308b4(void);

void func_0201a5f8(int t);
/* Linker-defined overlay IDs: the address is the overlay number. */
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

extern struct OamAttr *data_0208a0f8[];

extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
/* The scene's graph callback: the scene registers its own object here and
   clears it on the way out. The frame calls its four slots: GraphCallback0 at
   the frame start (func_02019390), GraphCallback1 before the frame wait
   (func_02019404), GraphCallback2 and GraphCallback3 in the VBlank IRQ
   (func_02019144, func_02019100). dGraph_c::callback_c is the ROM-proven base
   of every scene's nested graphCallback_c (include/dGraph_c.h). */
extern dGraph_c::callback_c *gGraphCallback;
extern int data_0208ee44;
}
void *func_0201a3e4(void);

/* Whether the loader job Behavior waits on has finished: InitResources seeds
   it from func_0201a244's return value (1 when the job ran synchronously) and
   Behavior's step 1 polls func_0201a1bc into it. It is this TU's .bss, sorted
   between LoadMultiBootResources's static and sMBGraphCallback. d_s_boot keeps
   the same flag for its own job as sBootJobDone. */
int sMBJobDone;

/* The scene's graph callback. A global, not a member: InitResources points
   it at the scene and hands it to the graph hook. Its constructor is inline,
   so mwcc emits __sinit_d_s_mb.cpp to run it (was the handwritten
   __sinit_02074fb8). */
dScMB_c::graphCallback_c sMBGraphCallback;

#pragma defer_codegen off

dScMB_c::~dScMB_c() {}

/* The untyped parameter pair is measured, not sloppy: spelling `c` as
   dScMB_c::graphCallback_c * (or `owner` as dScMB_c *, with `this` at the one
   call site) moves this function's bytes. The casts stay. */
void func_02034b1c(void *c, void *owner) {
    dScMB_c::graphCallback_c *callback = (dScMB_c::graphCallback_c *)c;
    callback->mOwner = owner;
    callback->mSpinAngle = 0;
    callback->mStillTimer = 0;
    callback->mEnabled = 1;
    callback->mAdvancing = 1;
    callback->mUnusedFlag = 0;
}

int dScMB_c::graphCallback_c::GraphCallback3() {
    if (mOwner != 0) {
        ((dScMB_c *)mOwner)->UpdateFader();
    }
    OAM::Reset();

    if (mEnabled != 0) {
        int language = GetLanguageIndex();
        OamAttr *sprite = data_0208a0f8[language];

        if (mStillTimer == 0) {
            int spriteAngle = mSpinAngle;
            int anyOnArc = 0;

            do {
                int y;
                if (spriteAngle >= 0 && spriteAngle < 0x10000) {
                    u16 angle = spriteAngle;
                    y = (FX_SinIdx((u16)(s16)angle >> 4) >> 10) + 0x80;
                    anyOnArc = 1;
                } else {
                    y = 0x80;
                }

                OAM::Render(false, sprite, 0x80, y, -1, -1, 0x1000, 0);
                OAM::Render(true, sprite, 0x80, y, -1, -1, 0x1000, 0);

                if (sprite->attr3 == 0xffff)
                    break;
                sprite++;
                spriteAngle -= 0x2000;
            } while (1);

            if (mAdvancing != 0) {
                mSpinAngle += (data_0208ee44 << 10);
                if (anyOnArc == 0)
                    mStillTimer = 0x78;
            }
        } else {
            OAM::Render(0, sprite, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            OAM::RenderSub(sprite, 0x80, 0x80);
            mStillTimer -= data_0208ee44;
            mSpinAngle = 0;
        }
    }

    func_0203083c();
    OAM::Flush();
    OAM::Load();
    return 1;
}

int dScMB_c::graphCallback_c::GraphCallback2() {
    return 0;
}

int dScMB_c::graphCallback_c::GraphCallback0() {
    return 0;
}

// Its sole caller ignores the result; AdvanceFade itself returns void.
void dScMB_c::UpdateFader() {
    if (fader.IsAtStart())
        return;
    fader.AdvanceFade();
}

s32 dScMB_c::CleanupResources() {
    gGraphCallback = 0;
    dScene_c::SetAndStopColorFader();
    return 1;
}

s32 dScMB_c::Render() {
    return 1;
}

s32 dScMB_c::Behavior() {
    switch (mStep) {
    case 0:
        if (func_0203d8fc() == 0)
            break;
        DecompressLZ16(data_0208a0e4[GetLanguageIndex()], G2_GetOBJCharPtr());
        DecompressLZ16(data_0208a0e4[GetLanguageIndex()], G2S_GetOBJCharPtr());
        gMainScreenLayers = DISPLAY_LAYER_OBJ;
        gSubScreenLayers = 0x10;
        GX_SetVisiblePlane(DISPLAY_LAYER_OBJ);
        GXS_SetVisiblePlane(DISPLAY_LAYER_OBJ);
        mStep++;
        break;

    case 1: {
        int done = func_0201a1bc();
        sMBJobDone = done;
        if (done == 0)
            break;
    }
        mStep++;
        /* fallthrough */
    case 2:
        mStep++;
        /* fallthrough */
    case 3:
        mStep++;
        /* fallthrough */
    case 4:
        func_0201a244(LoadMultiBootResources, 0, 0xf, 0, 0x1000);
        mStep++;
        /* fallthrough */
    case 5:
        if (func_0201a1bc() == 0)
            break;
        mStep++;
        /* fallthrough */
    case 6:
        if (func_0203d7b8() != 0)
            break;
        func_0200f220();
        func_0200f13c();
        func_0203d930();
        func_020308b4();
        mStep++;
        /* fallthrough */
    case 7:
        if (func_020308a8() == 0)
            break;
        dScene_c::StartSceneFade(6, 0, 0x7fff);
        func_02012790(0x11f);
        mStep++;
        break;
    }

    if (mStep < 8) {
        mTimeout++;
        if (mTimeout >= 0x1518) {
            data_0209fc54 = 1;
        }
    }

    return 1;
}

void LoadMultiBootResources(int unused) {
    LoadArchive(0);
    mHeap::Heap_t *heap = (mHeap::Heap_t *)func_0201a458();
    /* The default heap to restore after each load. */
    static mHeap::Heap_t *sPrevHeap = heap->SetDefault();
    LoadTextNarcs();
    LoadArchive(1);
    sPrevHeap->SetDefault();
    func_0201a5f8(6);
    LoadOverlay((int)&OVERLAY_100_ID);
    LoadOverlay((int)&OVERLAY_102_ID);
    sPrevHeap = ((mHeap::Heap_t *)func_0201a3e4())->SetDefault();
    LoadArchive(7);
    sPrevHeap->SetDefault();
}

s32 dScMB_c::InitResources() {
    volatile u16 fillMain;
    volatile u16 fillSub;

    GX_DispOff();
    GXS_DispOff();
    func_02030aa4(2);

    GX_SetPower(GX_POWER_ALL);
    func_0200f2cc();

    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800,
                     GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2S_BG1Mosaic(0);

    fillMain = 0xffff;
    MI_CpuFill16((char *)0x5000000, fillMain, 2);
    fillSub = 0xffff;
    MI_CpuFill16((char *)0x5000400, fillSub, 2);

    /* From OBJ tile 0x94 onwards; a 4bpp tile is 0x20 bytes. */
    DecompressLZ16(&data_0209446c, (u8 *)G2_GetOBJCharPtr() + 0x94 * 0x20);
    GX_LoadOBJPltt(&data_0209444c, 0, 0x20);
    GX_LoadOBJPltt(&data_020945d0, 0xa0, 0x20);
    GXS_LoadOBJPltt(&data_0209444c, 0, 0x20);

    G2_SetBG1Priority(0);
    G2S_SetBG1Priority(0);
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    gMainScreenLayers = 0;
    gSubScreenLayers = 0;
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    GX_DispOn();

    GXS_DispOn();
    func_02034b1c(&sMBGraphCallback, this);

    gGraphCallback = &sMBGraphCallback;
    mStep = 0;
    mTimeout = 0;
    dScene_c::SetFaders(&fader);

    fader.color = 0x7fff;
    data_0208ee44 = 1;
    sMBJobDone = func_0201a244(func_0201a2f8, 0, 0xf, 0, 0x1000);

    return 1;
}

void *dScMB_c_classInit(void) {
    return new dScMB_c;
}
