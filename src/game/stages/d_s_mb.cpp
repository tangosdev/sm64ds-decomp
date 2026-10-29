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

// leftovers: OAM::Render stays mangled (Fix12 by value -- this TU's render
// calls). func_02034b1c initialises the global graph callback, retaining
// the raw store to its unmodelled byte at +0x0f. func_02034fbc stays free
// (no receiver).
#include "dScMB_c.h"
#include "AsyncJob.h"
#include "decl_common.h"
#include "Heap.h"
#include "OAM.h"
#include "Sound.h"
#include "GX.h"

extern "C" {
void MultiStore16(u16 val, char *dst, int nbytes);
void DecompressLZ16(int src, void *dst);

void func_020308b4(void);

void func_0201a5f8(int t);
void *func_0201a3e4(void);
extern u32 data_020a0c60;
extern void *data_020a0c5c;
/* Linker-defined overlay IDs: the address is the overlay number. */
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

void OamRender10_d_s_mb(int sub, void *attr, int x, int y, int a, int cc, int fx0, int fy0, int rot,
                        int mode);
extern struct OamAttr *data_0208a0f8[];
extern s16 data_02082214[];

extern u8 data_0209d45c;
extern u8 data_0209d454;
extern void *data_0209d4a8;
extern int data_0208ee44;
}

/* The scene's graph callback. A global, not a member: InitResources points
   it at the scene and hands it to the graph hook. Its constructor is inline,
   so mwcc emits __sinit_d_s_mb.cpp to run it (was the handwritten
   __sinit_02074fb8). */
dScMB_c::graphCallback_c data_020a0c68;

#pragma defer_codegen off

// @symbol _ZN7dScMB_cD1Ev
dScMB_c::~dScMB_c() {}

// @symbol func_02034b1c
void func_02034b1c(void *c, int owner) {
    dScMB_c::graphCallback_c *callback = (dScMB_c::graphCallback_c *)c;
    callback->mOwner = (void *)owner;
    callback->mSpinAngle = 0;
    callback->mStillTimer = 0;
    callback->mEnabled = 1;
    callback->mAdvancing = 1;
    *(u8 *)((char *)c + 0xf) = 0;
}

// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback3Ev
int dScMB_c::graphCallback_c::GraphCallback3() {
    if (mOwner != 0) {
        ((dScMB_c *)mOwner)->func_02034d34();
    }
    OAM::Reset();

    if (mEnabled != 0) {
        int language = func_0200f0bc();
        void *sprite = data_0208a0f8[language];

        if (mStillTimer == 0) {
            int spriteAngle = mSpinAngle;
            int x = 0x80;
            int one = 1;
            int drawFlags[3];
            drawFlags[0] = 0;
            drawFlags[1] = 0;
            drawFlags[2] = 0;

            do {
                int y;
                if (spriteAngle >= 0 && spriteAngle < 0x10000) {
                    // Preserve the narrowing sequence: collapsing it removes 16 ROM bytes.
                    u16 angleBits = (u16)spriteAngle;
                    s16 signedAngle = (s16)angleBits;
                    u16 tableAngle = (u16)signedAngle;
                    y = data_02082214[(tableAngle >> 4) * 2];
                    y = (y >> 10) + 0x80;
                    drawFlags[0] = one;
                } else {
                    y = x;
                }

                OAM::Render(drawFlags[1], (OamAttr *)sprite, x, y, -1, -1, 0x1000, drawFlags[1]);
                OAM::Render(one, (OamAttr *)sprite, x, y, -1, -1, 0x1000, drawFlags[2]);

                if (*(u16 *)((char *)sprite + 6) == 0xffff)
                    break;
                sprite = (char *)sprite + 8;
                spriteAngle -= 0x2000;
            } while (1);

            if (mAdvancing != 0) {
                mSpinAngle += (data_0208ee44 << 10);
                if (drawFlags[0] == 0)
                    mStillTimer = 0x78;
            }
        } else {
            OAM::Render(0, (OamAttr *)sprite, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            OAM::RenderSub((OamAttr *)sprite, 0x80, 0x80);
            mStillTimer -= data_0208ee44;
            mSpinAngle = 0;
        }
    }

    func_0203083c();
    OAM::Flush();
    OAM::Load();
    return 1;
}

// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback2Ev
int dScMB_c::graphCallback_c::GraphCallback2() {
    return 0;
}

// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback0Ev
int dScMB_c::graphCallback_c::GraphCallback0() {
    return 0;
}

// @symbol _ZN7dScMB_c13func_02034d34Ev
// Its sole caller ignores the result; AdvanceFade itself returns void.
void dScMB_c::func_02034d34() {
    if (fader.IsAtStart())
        return;
    fader.AdvanceFade();
}

// @symbol _ZN7dScMB_c16CleanupResourcesEv
s32 dScMB_c::CleanupResources() {
    data_0209d4a8 = 0;
    dScene_c::SetAndStopColorFader();
    return 1;
}

// @symbol _ZN7dScMB_c6RenderEv
s32 dScMB_c::Render() {
    return 1;
}

// @symbol _ZN7dScMB_c8BehaviorEv
s32 dScMB_c::Behavior() {
    switch (mStep) {
    case 0:
        if (func_0203d8fc() == 0)
            break;
        DecompressLZ16((int)data_0208a0e4[func_0200f0bc()], (void *)0x6400000);
        DecompressLZ16((int)data_0208a0e4[func_0200f0bc()], (void *)0x6600000);
        data_0209d45c = 0x10;
        data_0209d454 = 0x10;
        *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x1000;
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x1000;
        mStep++;
        break;

    case 1: {
        int r = func_0201a1bc();
        *(int *)&data_020a0c64 = r;
        if (r == 0)
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
        func_0201a244(func_02034fbc, 0, 0xf, 0, 0x1000);
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

// @symbol func_02034fbc
void func_02034fbc(int unused) {
    void *h;

    LoadArchive(0);
    h = (void *)func_0201a458();
    if (!(data_020a0c60 & 1)) {
        data_020a0c5c = ((mHeap::Heap_t *)h)->SetDefault();
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

// @symbol _ZN7dScMB_c13InitResourcesEv
s32 dScMB_c::InitResources() {
    volatile u16 fillMain;
    volatile u16 fillSub;

    func_02053b98();
    *(volatile u32 *)0x4001000 &= ~0x10000;
    func_02030aa4(2);

    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & ~0x20e) | 0x20e;
    func_0200f2cc();

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x704;
    *(volatile u16 *)0x400000a &= ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x408;
    *(volatile u16 *)0x400100a &= ~0x40;

    fillMain = 0xffff;
    MultiStore16(fillMain, (char *)0x5000000, 2);
    fillSub = 0xffff;
    MultiStore16(fillSub, (char *)0x5000400, 2);

    {
        char *vram = (char *)0x6400000;
        vram += 0x1280;
        DecompressLZ16((int)&data_0209446c, vram);
    }
    GX::LoadOBJPltt((const void *)&data_0209444c, 0, 0x20);
    GX::LoadOBJPltt((const void *)&data_020945d0, 0xa0, 0x20);
    GXS::LoadOBJPltt((const void *)&data_0209444c, 0, 0x20);

    *(volatile u16 *)0x400000a &= ~3;
    *(volatile u16 *)0x400100a &= ~3;
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    *(u8 *)&data_0209d45c = 0;
    *(u8 *)&data_0209d454 = 0;
    *(volatile u32 *)0x4000000 &= ~0x1f00;
    *(volatile u32 *)0x4001000 &= ~0x1f00;
    GX::DispOn();

    *(volatile u32 *)0x4001000 |= 0x10000;
    func_02034b1c(&data_020a0c68, (int)this);

    *(void **)&data_0209d4a8 = &data_020a0c68;
    mStep = 0;
    mTimeout = 0;
    dScene_c::SetFaders(&fader);

    fader.color = 0x7fff;
    *(int *)&data_0208ee44 = 1;
    *(int *)&data_020a0c64 = func_0201a244(func_0201a2f8, 0, 0xf, 0, 0x1000);

    return 1;
}

// @symbol dScMB_c_classInit
extern "C" void *dScMB_c_classInit(void) {
    return new dScMB_c;
}
