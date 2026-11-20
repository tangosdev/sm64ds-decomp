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

// leftovers: graphCallback_c::Init initialises the global graph callback,
// retaining the raw store to its unmodelled byte at +0x0f; its untyped
// parameter is measured (see the note on it below).
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
/* Multiboot OBJ palette and LZ16 tile blob. */
u8 data_0209444c[32] = {
    0x7f, 0x1a, 0x00, 0x00, 0x9c, 0x7f, 0xb5, 0x7e, 0x31, 0x7e, 0xac, 0x7d, 0x28, 0x7d, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
u8 data_0209446c[356] = {
    0x10, 0x40, 0x05, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x4f, 0x44, 0x44, 0x44, 0x82, 0x10, 0x07,
    0x5f, 0x55, 0x55, 0x55, 0x5f, 0x00, 0x05, 0x5f, 0x63, 0x4f, 0x00, 0x03, 0x10, 0x07, 0xf5, 0x55,
    0x55, 0x10, 0x17, 0x00, 0x1e, 0x74, 0xf4, 0x10, 0x07, 0x00, 0x1e, 0x00, 0x11, 0x66, 0x20, 0x03,
    0x65, 0x56, 0xc2, 0x30, 0x03, 0x10, 0x17, 0x1f, 0x11, 0x11, 0x11, 0xa0, 0x3f, 0x1f, 0xd8, 0xb0,
    0x3f, 0x00, 0x1e, 0xf1, 0xf0, 0x3f, 0x70, 0x3f, 0xbf, 0xbb, 0xbb, 0x5a, 0xbb, 0xa0, 0x3f, 0xbf,
    0xb0, 0x3f, 0x00, 0x1e, 0xfb, 0xd0, 0x3f, 0xf5, 0x4f, 0x5f, 0x30, 0x03, 0x00, 0x00, 0xf0, 0x01,
    0xf0, 0x01, 0xf0, 0x01, 0x50, 0x01, 0xe9, 0xf0, 0x7f, 0xf0, 0x7f, 0xb0, 0x7f, 0xff, 0x20, 0x03,
    0xf5, 0x5f, 0x30, 0x03, 0xf2, 0xf0, 0x51, 0xf0, 0x01, 0xf0, 0x01, 0x90, 0x01, 0xb1, 0xaa, 0x70,
    0x03, 0xa1, 0x00, 0x99, 0x11, 0x11, 0xa1, 0x99, 0xa1, 0xaa, 0xaa, 0x00, 0x99, 0x81, 0x99, 0x99,
    0x99, 0x11, 0x98, 0x99, 0x04, 0x99, 0xaa, 0x19, 0x00, 0x00, 0x50, 0x03, 0x99, 0x18, 0x81, 0x10,
    0x03, 0x11, 0x11, 0x99, 0xaa, 0xaa, 0x1a, 0x00, 0x1e, 0x00, 0x18, 0x99, 0x99, 0x89, 0x11, 0x11,
    0x71, 0x88, 0x02, 0x88, 0x10, 0x11, 0x87, 0x88, 0x00, 0x00, 0x08, 0x00, 0xae, 0x00, 0x08, 0x00,
    0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x58, 0x10, 0x03, 0x00, 0x00, 0x88, 0x88, 0x17, 0x11, 0x88,
    0x78, 0x11, 0x01, 0xab, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x30,
    0x1c, 0xff, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01,
    0xf0, 0x01, 0xff, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0,
    0x01, 0xf0, 0x01, 0xff, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01,
    0xf0, 0x01, 0xf0, 0x01, 0xfd, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0x80, 0x01, 0x02,
    0xe3, 0x66, 0xa0, 0x03, 0xff, 0x02, 0xe4, 0x94, 0x13, 0x53, 0xa7, 0x50, 0x03, 0x13, 0x03, 0x94,
    0x13, 0x00, 0x33, 0xb0, 0x03, 0xff, 0x40, 0x3f, 0x54, 0x13, 0xf0, 0x3f, 0x30, 0x3f, 0x54, 0x13,
    0xf0, 0x3f, 0x30, 0x3f, 0x53, 0x93, 0xff, 0x53, 0xe7, 0x50, 0x03, 0x50, 0x3f, 0x53, 0x93, 0xf0,
    0xd1, 0xf0, 0x01, 0xf0, 0x01, 0xa0, 0xff, 0xf8, 0xf0, 0x7f, 0xa0, 0x7f, 0x73, 0xeb, 0x10, 0x03,
    0xd0, 0x7f, 0x00, 0x00,
};
u8 data_020945d0[32] = {
    0x44, 0x76, 0xff, 0x02, 0x7f, 0x1e, 0x7b, 0x18, 0x7f, 0x39, 0xff, 0x7f, 0x9c, 0x73, 0xd6, 0x5a,
    0xc6, 0x18, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x26, 0x67, 0x06, 0x8c, 0x31, 0x00, 0x00, 0x00, 0x00,
};
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
extern int gFrameStep;
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
void dScMB_c::graphCallback_c::Init(void *owner) {
    dScMB_c::graphCallback_c *callback = (dScMB_c::graphCallback_c *)this;
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
                    y = (FX_SinIdx((u16)(s16)angle >> 4) >> 10) + 128;
                    anyOnArc = 1;
                } else {
                    y = 128;
                }

                OAM::Render(false, sprite, 128, y, -1, -1, 0x1000, 0);
                OAM::Render(true, sprite, 128, y, -1, -1, 0x1000, 0);

                if (sprite->attr3 == 0xffff)
                    break;
                sprite++;
                spriteAngle -= 0x2000;
            } while (1);

            if (mAdvancing != 0) {
                mSpinAngle += (gFrameStep << 10);
                if (anyOnArc == 0)
                    mStillTimer = 120;
            }
        } else {
            OAM::Render(0, sprite, 128, 128, -1, -1, 0x1000, 0x1000, 0, -1);
            OAM::RenderSub(sprite, 128, 128);
            mStillTimer -= gFrameStep;
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
        if (mTimeout >= 5400) {
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
    sMBGraphCallback.Init(this);

    gGraphCallback = &sMBGraphCallback;
    mStep = 0;
    mTimeout = 0;
    dScene_c::SetFaders(&fader);

    fader.color = 0x7fff;
    gFrameStep = 1;
    sMBJobDone = func_0201a244(func_0201a2f8, 0, 0xf, 0, 0x1000);

    return 1;
}

void *dScMB_c_classInit(void) {
    return new dScMB_c;
}
