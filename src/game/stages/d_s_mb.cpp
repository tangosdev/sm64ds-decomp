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
// LoadMultiBootResources stays free (no receiver). Its guarded store to
// data_020a0c5c under data_020a0c60 is a function-local static
// (`static mHeap::Heap_t *sPrevHeap = heap->SetDefault();` reproduces it), but
// folding it moves this TU's .bss start to 0x020a0c5c, which takes a
// config/arm9/delinks.txt change. The loader-job word data_020a0c64 is an int
// that decl_common.h spells u8[].
#include "dScMB_c.h"
#include "nitro/hw/registers.h"
#include "AsyncJob.h"
#include "decl_common.h"
#include "Heap.h"
#include "OAM.h"
#include "OamAttr.h"
#include "Sound.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_load.h"
#include "nitro/fx/fx_trig.h"

extern "C" {
void MultiStore16(u16 val, char *dst, int nbytes);
void DecompressLZ16(void *src, void *dst);

void func_020308b4(void);

void func_0201a5f8(int t);
void *func_0201a3e4(void);
extern u32 data_020a0c60;
extern void *data_020a0c5c;
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

/* NitroSDK's main-engine OBJ character base getter (nitro/gx/gx_bgcnt.h).
   Read through the inline, the ROM builds 0x06401280 as `mov #0x6400000 /
   add #0x1280` where a literal address would load it from the pool.
   local copy: include/nitro/gx/gx_bgcnt.h carries only the sub-engine
   G2S_GetOBJCharPtr, and adding a second inline body there renumbers
   d_a_demo.cpp's pinned @NNN symbols. */
static inline void *G2_GetOBJCharPtr(void) {
    return (void *)0x06400000;
}

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
        DecompressLZ16(data_0208a0e4[GetLanguageIndex()], (void *)0x6400000);
        DecompressLZ16(data_0208a0e4[GetLanguageIndex()], (void *)0x6600000);
        gMainScreenLayers = DISPLAY_LAYER_OBJ;
        gSubScreenLayers = 0x10;
        REG_DISPCNT = (REG_DISPCNT & ~0x1f00) | 0x1000;
        REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~0x1f00) | 0x1000;
        mStep++;
        break;

    case 1: {
        int done = func_0201a1bc();
        *(int *)data_020a0c64 = done;
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
    if (!(data_020a0c60 & 1)) {
        data_020a0c5c = heap->SetDefault();
        data_020a0c60 |= 1;
    }
    LoadTextNarcs();
    LoadArchive(1);
    ((mHeap::Heap_t *)data_020a0c5c)->SetDefault();
    func_0201a5f8(6);
    LoadOverlay((int)&OVERLAY_100_ID);
    LoadOverlay((int)&OVERLAY_102_ID);
    data_020a0c5c = ((mHeap::Heap_t *)func_0201a3e4())->SetDefault();
    LoadArchive(7);
    ((mHeap::Heap_t *)data_020a0c5c)->SetDefault();
}

s32 dScMB_c::InitResources() {
    volatile u16 fillMain;
    volatile u16 fillSub;

    func_02053b98();
    REG_DISPCNT_SUB &= ~0x10000;
    func_02030aa4(2);

    REG_POWCNT1 = (REG_POWCNT1 & ~0x20e) | 0x20e;
    func_0200f2cc();

    REG_BG1CNT = (REG_BG1CNT & 0x43) | 0x704;
    REG_BG1CNT &= ~0x40;
    REG_DB_BG1CNT = (REG_DB_BG1CNT & 0x43) | 0x408;
    REG_DB_BG1CNT &= ~0x40;

    fillMain = 0xffff;
    MultiStore16(fillMain, (char *)0x5000000, 2);
    fillSub = 0xffff;
    MultiStore16(fillSub, (char *)0x5000400, 2);

    /* From OBJ tile 0x94 onwards; a 4bpp tile is 0x20 bytes. */
    DecompressLZ16(&data_0209446c, (u8 *)G2_GetOBJCharPtr() + 0x94 * 0x20);
    GX_LoadOBJPltt((const void *)&data_0209444c, 0, 0x20);
    GX_LoadOBJPltt((const void *)&data_020945d0, 0xa0, 0x20);
    GXS_LoadOBJPltt((const void *)&data_0209444c, 0, 0x20);

    REG_BG1CNT &= ~3;
    REG_DB_BG1CNT &= ~3;
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    gMainScreenLayers = 0;
    gSubScreenLayers = 0;
    REG_DISPCNT &= ~0x1f00;
    REG_DISPCNT_SUB &= ~0x1f00;
    GX_DispOn();

    REG_DISPCNT_SUB |= 0x10000;
    func_02034b1c(&sMBGraphCallback, this);

    gGraphCallback = &sMBGraphCallback;
    mStep = 0;
    mTimeout = 0;
    dScene_c::SetFaders(&fader);

    fader.color = 0x7fff;
    data_0208ee44 = 1;
    *(int *)data_020a0c64 = func_0201a244(func_0201a2f8, 0, 0xf, 0, 0x1000);

    return 1;
}

extern "C" void *dScMB_c_classInit(void) {
    return new dScMB_c;
}
