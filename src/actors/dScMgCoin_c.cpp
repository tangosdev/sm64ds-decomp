//cpp
/**
 * Coincentration. Twenty-four coins fall into the blocks. A coin
 * keeps bouncing until it has bounced as many times as it is worth,
 * then sits and pops that value. Sparkles burst off it. The moneybag
 * hops down six stages and drops off the bottom. Phase 5 is the
 * result: a countdown, then this score against the other player's.
 *
 * This file is the whole ov006 unit 0x020dbe40..0x020de940: 62
 * functions in ROM order, from the destructor to InitResources. The
 * destructor is the key function, so the vtable and RTTI are emitted
 * here too. dScMgCoin_c_classInit, just above at 0x020de940, is still
 * its own file (src/d_s_mg_coin.cpp).
 *
 * The functions from func_ov006_020dd0e0 up, and the three below
 * func_ov006_020dbf7c, came from one-function files. Their bodies are
 * kept as they matched there. Where two of them spelled a type or a
 * declaration differently, the tag is renamed per function and the
 * call casts. func_ov006_020dd880 and func_ov006_020ddeb0 called
 * slot 35 through a stand-in vtable; they now call Virtual8C.
 *
 * The five state tables (the coin handlers at data_ov006_021417b0, the
 * moneybag's at 021417c8, the caption's at 021417e8, the phases' at
 * 02141810 and the blocks' at 02141840) are {pmf, delta} records on
 * dScMgCoin_c. __sinit_ov006_0213014c copies them into .bss from the
 * data_ov006_0213be** literals.
 *
 * Leftover: func_ov006_020dd594 and func_ov006_020dde28 still use
 *   local layout structs instead of the class members.
 * Leftover: func_ov006_020ddeb0 (from func_ov006_020dc154 and
 *   func_ov006_020dc1c4) and func_ov006_020dd4b0 (from
 *   func_ov006_020dd000) are still unnamed.
 * Leftover: RenderOamMainScreen and func_ov004_020b0380 are redeclared
 *   extern "C" at the call — the ROM symbols are unmangled, and the
 *   sites disagree on the types.
 * Leftover: func_ov006_020dcd74 reads the other score at
 *   data_ov004_020beb68 + 0xac. That word is not a field of dScMgBase_c.
 * Leftover: func_ov006_020dc6d0 casts the scene through unsigned long
 *   long before clearing the caption frameTime. Five words without it.
 * Leftover: func_ov006_020dc900, func_ov006_020dc960 and
 *   func_ov006_020dcb1c stay on byte offsets. The popup and sparkle
 *   structs miss (timer address reused, 0x18 kept as both stride and
 *   timer, literal pool swapped).
 * Leftover: GetGameLanguage is called twice in func_ov006_020dccb8
 *   and again in func_ov006_020dcd74. Hoisting the second call misses.
 */

#include "types.h"
#include "decl_common.h"
#include "Sound.h"
#include "dScMgCoin_c.h"

#pragma defer_codegen off /* ROM-ascending emission; dropping this inverts it */

/* Indexed from the object base. A pointer to the element misses. */
struct BagView {
    u8 pad[0x51a8];
    dScMgCoin_Bouncer bag[1];
};
struct CapView {
    u8 pad[0x5194];
    dScMgCoin_Caption cap[1];
};
struct CoinView {
    u8 pad[0x4ac0];
    dScMgCoin_Coin coin[1];
};
struct SparkView {
    u8 pad[0x4d14];
    dScMgCoin_Sparkle spark[1];
};

enum {
    kCoinCount = 24,
    kSparkleCount = 32,
    kPopupCount = 24,
    kResultPhase = 5,
    kBouncerDone = 6,
    kCoinSettled = 2,
    kGravity = 0x400,
    kFallStep = 0x100,
    kHopVy = -0x3000,
    kKickVy = -0x4000,
    kParkedY = -0xf0000,
    kParkRow = -0xf0,
    kOffBottom = 0xd0,
    kSpinStep = 0x400,
    kSparkleFall = 0x180,
    kSparkleDrag = 0x180,
    kSparkleCap = 0x300,
    kCaptionHold = 0x3c,
    kCountInDelay = 0xb4,
    kStartDelay = 0x28,
    kChimeAt = 0x18,
    kBagRestX = 0x8e000,
    kBagRestY = -0x43000,
    kCaptionX = 0x70000,
    kCaptionY = 0x98000,
    kSndBounce = 0xef,
    kSndReady = 0xf0,
    kSndChime = 0xf1,
    kTileRows = 3,
    kTileCols = 0x10,
    kTileStep = 0x20
};

extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212e370[];
extern int data_ov006_0212e388[];
extern int data_ov006_0212e344[];
extern int data_ov006_0213a9fc[];
extern unsigned char data_ov006_0212e324[];
extern unsigned char data_ov006_0212e31c[];
extern unsigned char data_ov006_0212e308[];
extern unsigned char data_ov006_0212e30c[];
extern u16 data_ov006_0212e33c[];
extern u8 data_ov006_0212e310[];
extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern int data_ov006_0212e430[];
extern u16 data_ov006_0212e334[];
extern s16 data_02082214[];
extern int data_ov006_021341ec;
extern int data_ov006_0212e364[];
extern int *data_ov006_0213bf34[];
extern int GetGameLanguage(void);
extern void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
extern void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void *data_ov006_02133f10[];
extern void *data_ov006_02136e24[];
extern void *data_ov006_02134b4c[];
extern void func_ov004_020b023c(void *obj, int x, int y, int w, int *vec);
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void func_020127a4(int a, int b, int c, int d);
extern int data_ov006_0212e358[];
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void RenderOamMainScreen(int a, int b, int c, int d, int e);
extern u16 data_ov006_0212e314[];
extern void *data_ov006_0213bf0c[];
extern int data_ov006_0212e418[];
extern int data_ov006_0212e400[];
extern int data_ov006_0212e3d0[];
extern int data_ov006_0212e3e8[];
extern int data_ov006_0212e3a0[];
extern int data_ov006_0212e3b8[];
extern int data_ov006_0212e34c[];
extern u16 data_ov006_0212e32c[];
extern void FreeGfxSlotsById(int n);
extern void func_ov004_020ae274(void *c);
extern int func_ov004_020adc1c(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;
}

/* The per-state handler tables for the coins, the blocks, the caption and
   the moneybag: {pmf, delta} records indexed by the record's own state byte.
   __sinit_ov006_0213014c copies them from data_ov006_0213be** literals. */
typedef void (dScMgCoin_c::*CoinPmf)(int);
struct CoinPmfEntry { CoinPmf pmf; };
extern "C" CoinPmfEntry data_ov006_021417b0[];
extern "C" CoinPmfEntry data_ov006_021417c8[];
extern "C" CoinPmfEntry data_ov006_021417e8[];
extern "C" CoinPmfEntry data_ov006_02141840[];

// @symbol _ZN11dScMgCoin_cD1Ev
// @symbol _ZN11dScMgCoin_cD0Ev
/* Nothing to destroy by hand: D1 stores the vptr and calls dScMgBase_c's
   D2, and D0 adds the delete. */
dScMgCoin_c::~dScMgCoin_c()
{
}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgCoin_c() above,
 * so this spells out what the deleting destructor does in terms of it: the
 * D1 body, called qualified so it is a direct call, then the class-specific
 * operator delete. mwccarm never defines _MSC_VER, so nothing here reaches
 * the cartridge object. */
extern "C" dScMgCoin_c *_ZN11dScMgCoin_cD0Ev(dScMgCoin_c *thiz)
{
    thiz->dScMgCoin_c::~dScMgCoin_c();
    dScMgCoin_c::operator delete(thiz);
    return thiz;
}
#endif

#define FX_MUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

// @symbol _ZN11dScMgCoin_c19func_ov006_020dbe9cEv
/* The hand cursor while the stylus is down: the sine table entry for the
   scene's angle becomes a unit-scale 2x2 rotation matrix [cos, sin; -sin,
   cos] for the shared sprite call. The four matrix words are written
   straight into vec[], in slot order, after the icon index is read. A named
   rx/ry pair leaves the two 64-bit products in the wrong order or the sine
   base in the wrong register, and reading the icon index any later costs
   fifteen words. The pragma is part of that match. */
#pragma push
#pragma opt_propagation off
void dScMgCoin_c::func_ov006_020dbe9c()
{
    char *c = (char *)this;
    char *s = c + 0x5000;

    if (*(u8 *)(s + 0x1bd) == 0)
        return;
    {
        u16 idx_h = *(u16 *)(c + 0x5100 + 0xb8);
        s32 xr = *(s32 *)(s + 0x1a8);
        s32 yr = *(s32 *)(s + 0x1ac);
        int i = (idx_h >> 4) * 2;
        int vec[4];
        u8 idx_l = *(u8 *)(s + 0x1be);

        vec[0] = FX_MUL(data_02082214[i + 1], 0x1000);
        vec[1] = FX_MUL(data_02082214[i], 0x1000);
        vec[2] = -FX_MUL(data_02082214[i], 0x1000);
        vec[3] = FX_MUL(data_02082214[i + 1], 0x1000);
        func_ov004_020b023c(data_ov006_02134b4c[idx_l], xr >> 12, yr >> 12, -1, vec);
    }
}
#pragma pop

// @symbol _ZN11dScMgCoin_c19func_ov006_020dbf7cEi
void dScMgCoin_c::func_ov006_020dbf7c(int i)
{
    BagView *scene = (BagView *)this;
    int height;
    int stageIndex;

    scene->bag[i].x += scene->bag[i].vx;
    scene->bag[i].y += scene->bag[i].vy;
    scene->bag[i].vy += kGravity;
    if (scene->bag[i].vx > 0)
        scene->bag[i].spin += kSpinStep;
    else
        scene->bag[i].spin -= kSpinStep;

    height = scene->bag[i].y >> 12;
    stageIndex = scene->bag[i].stage;

    if (stageIndex >= kBouncerDone) {
        if (height >= kOffBottom) {
            scene->bag[i].shown = 0;
            scene->bag[i].active = 0;
        }
        return;
    }

    if (scene->bag[i].bouncesLeft != 0) {
        unsigned int roll;
        if (height < (data_ov006_0212e370[stageIndex] - data_ov006_0212e388[stageIndex]))
            return;
        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        scene->bag[i].vx = data_ov006_0212e344[((roll >> 16 & 0x7fff) << 1) >> 15];
        scene->bag[i].vy = kHopVy;
        scene->bag[i].bouncesLeft--;
        func_02012718(kSndBounce, scene->bag[i].x);
        if (scene->bag[i].bouncesLeft == 0)
            scene->bag[i].stage++;
    } else {
        scene->bag[i].bouncesLeft = 1;
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc154Ei
void dScMgCoin_c::func_ov006_020dc154(int index)
{
    BagView *scene = (BagView *)this;
    if (scene->bag[index].countdown != 0) {
        scene->bag[index].countdown = scene->bag[index].countdown - 1;
        return;
    }
    scene->bag[index].state = 3;
    scene->bag[index].vy = 0;
    scene->bag[index].bouncesLeft = 1;
    scene->bag[index].stage = 0;
    this->func_ov006_020ddeb0();
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc1c4Ei
void dScMgCoin_c::func_ov006_020dc1c4(int index)
{
    BagView *scene = (BagView *)this;
    scene->bag[index].y = scene->bag[index].y + scene->bag[index].vy;
    scene->bag[index].vy = scene->bag[index].vy - kFallStep;
    scene->bag[index].spin = scene->bag[index].spin + kSpinStep;
    if ((scene->bag[index].y >> 12) > kParkRow)
        return;
    scene->bag[index].y = kParkedY;
    scene->bag[index].state = 2;
    scene->bag[index].countdown = 0x30;
    this->func_ov006_020ddeb0();
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc26cEv
void dScMgCoin_c::func_ov006_020dc26c()
{
    dScMgCoin_c *scene = this;
    scene->mBouncer.state = 1;
    scene->mBouncer.sprite = 1;
    scene->mBouncer.vx = 0;
    scene->mBouncer.vy = kKickVy;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc294Ev
void dScMgCoin_c::func_ov006_020dc294()
{
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc298Ev
/* Dispatch the moneybag while mBouncer.active is set. Not a vtable:
   the records live in data_ov006_021417c8. */
void dScMgCoin_c::func_ov006_020dc298()
{
    if (this->mBouncer.active == 0)
        return;

    int state = this->mBouncer.state;
    (this->*data_ov006_021417c8[state].pmf)(0);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc2f8Ev
void dScMgCoin_c::func_ov006_020dc2f8()
{
    dScMgCoin_c *scene = this;
    scene->mBouncer.active = 1;
    scene->mBouncer.shown = 1;
    scene->mBouncer.sprite = 0;
    scene->mBouncer.state = 0;
    scene->mBouncer.spin = 0;
    scene->mBouncer.x = kBagRestX;
    scene->mBouncer.y = kBagRestY;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc334Ev
void dScMgCoin_c::func_ov006_020dc334()
{
    dScMgCoin_c *scene = this;
    scene->mBouncer.active = 0;
    scene->mBouncer.shown = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc348Ev
void dScMgCoin_c::func_ov006_020dc348()
{
    dScMgCoin_c *scene = this;
    scene->mCaption.state = 4;
    scene->mCaption.frame = 0;
    scene->mCaption.frameTime = 0;
    scene->mCaptionLatch = 1;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc370Ev
void dScMgCoin_c::func_ov006_020dc370()
{
    dScMgCoin_c *scene = this;
    if (scene->mCaptionLatch != 0)
        return;

    if (scene->mCaption.state == 2) {
        scene->mCaption.delay = kCaptionHold;
        return;
    }

    scene->mCaption.delay = kCaptionHold;
    scene->mCaption.state = 2;
    scene->mCaption.frame = 0;
    scene->mCaption.frameTime = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc3bcEv
void dScMgCoin_c::func_ov006_020dc3bc()
{
    dScMgCoin_c *scene = this;

    if (scene->mCaption.visible == 0)
        return;

    RenderOamMainScreen(data_ov006_0213a9fc[scene->mCaption.sprite],
        scene->mCaption.x >> 12, scene->mCaption.y >> 12, -1, -1);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc414Ei
void dScMgCoin_c::func_ov006_020dc414(int index)
{
    CapView *scene = (CapView *)this;
    u16 *frameTime = &scene->cap[index].frameTime;
    u8 *frame = &scene->cap[index].frame;

    *frameTime = *frameTime + 1;
    if (*frameTime < data_ov006_0212e324[*frame])
        return;

    *frameTime = 0;
    *frame = *frame + 1;
    if (*frame >= 8)
        *frame = 1;
    else
        scene->cap[index].sprite = data_ov006_0212e31c[*frame];
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc4b0Ei
void dScMgCoin_c::func_ov006_020dc4b0(int index)
{
    dScMgCoin_c *scene = this;
    (&scene->mCaption)[index].sprite = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc4c8Ei
void dScMgCoin_c::func_ov006_020dc4c8(int index)
{
    CapView *scene = (CapView *)this;
    u16 frameTime = scene->cap[index].frameTime;
    scene->cap[index].frameTime = frameTime + 1;
    u8 frame = scene->cap[index].frame;
    if (scene->cap[index].frameTime >= data_ov006_0212e308[frame]) {
        scene->cap[index].frameTime = 0;
        scene->cap[index].frame = scene->cap[index].frame + 1;
        scene->cap[index].frame = scene->cap[index].frame & 1;
    }
    scene->cap[index].sprite = data_ov006_0212e30c[scene->cap[index].frame];
    if (this->unk_51c8 == kResultPhase)
        return;
    u16 delay = scene->cap[index].delay;
    if (delay != 0) {
        scene->cap[index].delay = delay - 1;
        return;
    }
    scene->cap[index].state = 3;
    scene->cap[index].frame = 0;
    scene->cap[index].frameTime = 0;
    scene->cap[index].delay = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc5c4Ei
void dScMgCoin_c::func_ov006_020dc5c4(int index)
{
    CapView *scene = (CapView *)this;

    u16 *frameTime = &scene->cap[index].frameTime;
    u8 *frame = &scene->cap[index].frame;

    *frameTime = (u16)(*frameTime + 1);
    if (*frameTime < data_ov006_0212e33c[*frame])
        return;
    *frameTime = 0;
    *frame = (u8)(*frame + 1);
    if (*frame == 2) {
        this->func_ov006_020dc26c();
        Sound::PlayBank2_2D(kSndReady);
    }
    if (*frame >= 4) {
        scene->cap[index].delay = kCountInDelay;
        scene->cap[index].state = 2;
        {
            int *phase = &this->unk_51c8;
            *frame = 0;
            *frameTime = 0;
            *phase = *phase + 1;
        }
    } else {
        scene->cap[index].sprite = data_ov006_0212e310[*frame];
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc6d0Ei
void dScMgCoin_c::func_ov006_020dc6d0(int index)
{
    int raw = (int)this;
    CapView *scene = (CapView *)raw;

    if (scene->cap[index].delay != 0) {
        scene->cap[index].delay = scene->cap[index].delay - 1;
        if (scene->cap[index].delay == kChimeAt)
            Sound::PlayBank2_2D(kSndChime);
        return;
    }
    scene->cap[index].state = 1;
    ((CapView *)(unsigned long long)raw)->cap[index].frameTime = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc754Ev
/* Same dispatch as the moneybag, for the caption. */
void dScMgCoin_c::func_ov006_020dc754()
{
    if (this->mCaption.active == 0)
        return;

    int state = this->mCaption.state;
    (this->*data_ov006_021417e8[state].pmf)(0);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc7b4Ev
void dScMgCoin_c::func_ov006_020dc7b4()
{
    dScMgCoin_c *scene = this;
    scene->mCaption.active = 1;
    scene->mCaption.state = 0;
    scene->mCaption.visible = 1;
    scene->mCaption.delay = kStartDelay;
    scene->mCaption.frameTime = 0;
    scene->mCaption.sprite = 3;
    scene->mCaption.frame = 0;
    scene->mCaption.x = kCaptionX;
    scene->mCaption.y = kCaptionY;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc7fcEv
void dScMgCoin_c::func_ov006_020dc7fc()
{
    dScMgCoin_c *scene = this;
    scene->mCaption.active = 0;
    scene->mCaption.visible = 1;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc814Ev
void dScMgCoin_c::func_ov006_020dc814()
{
    dScMgCoin_c *scene = this;
    if (scene->unk_51c8 != kResultPhase)
        return;

    int left = scene->mCountdown;
    if (left > 0x80)
        return;
    if (left == 0)
        return;

    func_ov004_020b0d8c(scene, 0xe0, 0xa0);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc870Ev
void dScMgCoin_c::func_ov006_020dc870()
{
    dScMgCoin_c *scene = this;
    int i;
    if (scene->unk_51c8 == kResultPhase) {
        if (scene->mCountdown == 0)
            return;
    }
    for (i = 0; i < kPopupCount; i++) {
        if (scene->mPopups[i].flag != 0) {
            func_ov004_020b2444(scene->mPopups[i].x >> 12, scene->mPopups[i].y >> 12,
                scene->mPopups[i].value, 0, -1, 0, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc900Ev
void dScMgCoin_c::func_ov006_020dc900()
{
    char *raw = (char *)this;
    /* Popup struct misses: the (int) cast is what makes the timer
       read-modify-write use a fresh address. */
    int i;
    for (i = 0; i < 0x18; i++) {
        char *popup = raw + (int)((long long)i) * 0x10;
        if (*(u8 *)(popup + 0x5020) != 0 && *(u16 *)(popup + 0x501c) != 0) {
            *(u16 *)((int)(popup + 0x501c)) -= 1;
            if (*(s16 *)((int)(popup + 0x501c)) <= 0)
                *(u8 *)(popup + 0x5021) = 1;
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc960Ei
void dScMgCoin_c::func_ov006_020dc960(int index)
{
    char *raw = (char *)this;
    /* Two strides, 0x18 and 0x10. The struct form keeps 0x18 alive
       past the coin and pushes a register. */
    char *coin = raw + index * 0x18;
    char *popup = raw + index * 0x10;
    *(int *)(popup + 0x5014) = *(int *)(coin + 0x4ac0);
    *(int *)(popup + 0x5018) = *(int *)(coin + 0x4ac4);
    *(short *)(popup + 0x501e) = *(unsigned char *)(coin + 0x4ad3);
    *(unsigned char *)(popup + 0x5020) = 1;
    *(short *)(popup + 0x501c) = 0x18;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dc99cEv
void dScMgCoin_c::func_ov006_020dc99c()
{
    SparkView *scene = (SparkView *)this;

    int i;
    for (i = 0; i < kSparkleCount; i++) {
        if (scene->spark[i].visible != 0) {
            int sprite = scene->spark[i].sprite;
            int x = scene->spark[i].x;
            int y = scene->spark[i].y;
            func_ov004_020b0380(data_ov006_02136e24[sprite], x >> 12, y >> 12, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dca04Ev
void dScMgCoin_c::func_ov006_020dca04()
{
    SparkView *scene = (SparkView *)this;
    int i;
    for (i = 0; i < kSparkleCount; i++) {
        if (scene->spark[i].active != 0) {
            if (scene->spark[i].life != 0) {
                scene->spark[i].life -= 1;
            } else {
                scene->spark[i].active = 0;
                scene->spark[i].visible = 0;
                return;
            }
            scene->spark[i].x += scene->spark[i].vx;
            scene->spark[i].y += scene->spark[i].vy;
            scene->spark[i].vy += kSparkleFall;
            if (scene->spark[i].vx > kSparkleCap)
                scene->spark[i].vx -= kSparkleDrag;
            else if (scene->spark[i].vx < -kSparkleCap)
                scene->spark[i].vx += kSparkleDrag;
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dcb1cEi
void dScMgCoin_c::func_ov006_020dcb1c(int coinIndex)
{
    char *raw = (char *)this;
    /* Sparkle struct reorders the literal pool (0212e430 before 0212e334). */
    int i, j, k;
    char *coin = raw + coinIndex * 0x18;
    int *coinX = (int *)(coin + 0x4ac0);
    int *coinY = (int *)(coin + 0x4ac4);
    for (i = 0, j = 0, k = 1; i < 4; i++, raw += 0x18, j += 2, k += 2) {
        if (*(u8 *)(raw + 0x4d28) == 0) {
            *(u8 *)(raw + 0x4d28) = 1;
            *(u8 *)(raw + 0x4d29) = 1;
            *(int *)(raw + 0x4d14) = *coinX + (data_ov006_0212e430[j] << 12);
            *(int *)(raw + 0x4d18) = *coinY + (data_ov006_0212e430[k] << 12);
            {
                u16 ang = data_ov006_0212e334[i];
                int sine = (ang >> 4) * 2;
                *(int *)(raw + 0x4d1c) = (int)(((s64)data_02082214[sine + 1] * 0x2000 + 0x800) >> 12);
                *(int *)(raw + 0x4d20) = (int)(((s64)data_02082214[sine] * 0x2000 + 0x800) >> 12);
            }
            *(u16 *)(raw + 0x4d24) = 0x18;
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dcc48Ev
void dScMgCoin_c::func_ov006_020dcc48()
{
    dScMgCoin_c *scene = this;

    int i;
    int x, y, j;
    for (i = 0; i < kTileRows; i++) {
        y = data_ov006_0212e364[i];
        x = 0x10;
        for (j = 0; j < kTileCols; j++) {
            func_ov004_020b0380((void *)data_ov006_021341ec, x, y, (void *)0);
            x += kTileStep;
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dccb8Ev
void dScMgCoin_c::func_ov006_020dccb8()
{
    dScMgCoin_c *scene = this;

    int lang;
    if (scene->unk_51c8 < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[lang][8], 0x80, 0x18, -1, -1);
    func_ov004_020b1de8(0x68, 0x28, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite((void *)data_ov006_0213bf34[lang][1], (void *)0x7a, 0x28, (void *)0);
    func_ov004_020b2444(0x8c, 0x28, scene->unk_51d4, 1, -1, 2, 0);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dcd74Ev
void dScMgCoin_c::func_ov006_020dcd74()
{
    dScMgCoin_c *scene = this;

    int lang;
    int count;
    if (scene->unk_51c8 < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[lang][4], 0x80, 0x50, -1, -1);
    func_ov004_020b1de8(0x6c, 0x60, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite((void *)data_ov006_0213bf34[lang][1], (void *)0x7e, 0x60, (void *)0);
    if (data_ov004_020beb68 != 0)
        count = *(int *)((char *)data_ov004_020beb68 + 0xac);
    else
        count = 0;
    func_ov004_020b2444(0x90, 0x60, count, 1, -1, 2, 0);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dce3cEv
void dScMgCoin_c::func_ov006_020dce3c()
{
    dScMgCoin_c *scene = this;
    if (scene->mScore.running == 0)
        return;
    if (scene->mScore.total == scene->mScore.shown)
        return;
    {
        u16 *tick = &scene->mScore.tick;
        *tick = *tick + 1;
    }
    if (scene->mScore.tick < 8)
        return;
    scene->mScore.tick = 0;
    {
        u16 *shown = &scene->mScore.shown;
        *shown = *shown + 1;
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dcea8Ev
void dScMgCoin_c::func_ov006_020dcea8()
{
    dScMgCoin_c *scene = this;
    int i;
    int v1, v2;
    if (scene->unk_51c8 == kResultPhase && scene->mCountdown == 0)
        return;

    v1 = 0;
    v2 = 0;
    for (i = 0; i < kCoinCount; i++) {
        int st = scene->mCoins[i].state;
        int sprite = (st == kCoinSettled) ? 1 : v1;
        int x = scene->mCoins[i].x >> 12;
        int y = scene->mCoins[i].y >> 12;
        int highlight = (scene->unk_51c8 == kResultPhase) ? 1 : v2;
        if (st == kCoinSettled)
            highlight = 1;
        if (scene->unk_51c8 != kResultPhase) {
            if (scene->mCoins[i].visible != 0)
                func_ov004_020afdd0(data_ov006_02133f10[sprite], x, y, -1, highlight);
        } else {
            if (sprite != 0 && scene->mCoins[i].value != 0)
                func_ov004_020afdd0(data_ov006_02133f10[sprite], x, y, -1, highlight);
            if (scene->mCoins[i].collected == 0 && scene->mCoins[i].value != 0)
                func_ov004_020b2444(x, y, scene->mCoins[i].value, 0, -1, 0, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dcffcEv
void dScMgCoin_c::func_ov006_020dcffc()
{
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd000Ei
void dScMgCoin_c::func_ov006_020dd000(int index)
{
    CoinView *scene = (CoinView *)this;

    scene->coin[index].y = scene->coin[index].y + scene->coin[index].vy;
    scene->coin[index].vy = scene->coin[index].vy + kGravity;
    if (scene->coin[index].y < scene->coin[index].landY)
        return;
    scene->coin[index].y = scene->coin[index].landY;
    scene->coin[index].bounces = scene->coin[index].bounces + 1;
    this->func_ov006_020dc370();
    if (scene->coin[index].value == scene->coin[index].bounces) {
        scene->coin[index].state = kCoinSettled;
        this->func_ov006_020dc960(index);
        return;
    }
    scene->coin[index].vy = kHopVy;
    this->func_ov006_020dd4b0(index);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd0e0Ei
/* The touch test for one coin. In phase 2, with the stylus down and newly
   pressed, a touch within 0x10 of the coin takes it: a coin still worth
   something starts its hop, otherwise the round ends (phase 3, a 0x40 timer,
   the panned reveal jingle, and the win flag from the other score).
   Four spellings carry the match: p1 is assigned before p0; the touch point
   is re-read through data_020a0e40 rather than the i already in hand; each
   of the five coin stores spells its own self + n + 0x4000 + ... with no
   hoisted base; and the hop target re-reads its address instead of *p1. */
void dScMgCoin_c::func_ov006_020dd0e0(int idx)
{
    char *self = (char *)this;
    int i;
    int ok;
    int n;
    int v;
    int w;
    int *p0;
    int *p1;
    int ax;
    int ay;
    int q0;
    int q1;
    int ang;
    int stars;
    int need;

    if (*(int *)(self + 0x5000 + 0x1c8) != 2)
        return;

    i = data_020a0e40;
    ok = 0;
    if (data_020a0de8[i * 4] != 0 && data_020a0de9[i * 4] != 0)
        ok = 1;
    if (ok == 0)
        return;

    n = idx * 0x18;
    p1 = (int *)(self + 0x4ac4 + n);
    p0 = (int *)(self + 0x4ac0 + n);
    q1 = *p1;
    ax = data_020a0dea[data_020a0e40 * 4];
    ay = data_020a0deb[data_020a0e40 * 4];
    q0 = *p0;
    v = ax - (q0 >> 12);
    w = ay - (q1 >> 12);

    if (v <= -0x10)
        return;
    if (v >= 0x10)
        return;
    if (w <= -0x10)
        return;
    if (w >= 0x10)
        return;

    *(u8 *)(self + n + 0x4000 + 0xad5) = 1;
    if (*(u8 *)(self + n + 0x4000 + 0xad3) != 0) {
        *(u8 *)(self + n + 0x4000 + 0xad0) = 1;
        *(int *)(self + n + 0x4000 + 0xac8) = *(int *)(self + 0x4ac4 + n);
        *(int *)(self + n + 0x4000 + 0xacc) = -0x3000;
        *(u8 *)(self + n + 0x4000 + 0xad6) = 0;
        this->func_ov006_020dd4b0(idx);
        return;
    }

    this->func_ov006_020dcb1c(idx);
    *(u8 *)(self + idx * 0x18 + 0x4000 + 0xad2) = 0;
    *(int *)(self + 0x5000 + 0x1c8) = 3;
    *(int *)(self + 0x5000 + 0x1cc) = 0x40;

    ang = (*p0 >> 12) - 0x80;
    ang >>= 1;
    if (ang >= 0x3c)
        ang = 0x3c;
    if (ang <= -0x3c)
        ang = -0x3c;
    func_020127a4(2, 0xee, 0xffff, ang);
    Sound::PlayBank2_2D(0xf2);

    *(u8 *)(self + 0x4000 + idx * 0x18 + 0xad0) = 2;
    stars = (data_ov004_020beb68 != 0) ? *(int *)((char *)data_ov004_020beb68 + 0xa8) : 0;
    need = *(int *)(self + 0x5000 + 0x1d4);
    if (stars > need)
        *(u8 *)(self + 0x5000 + 0x1db) = 1;
    else
        *(u8 *)(self + 0x5000 + 0x1db) = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd2ccEv
/* Runs each coin's state handler from data_ov006_021417b0. */
void dScMgCoin_c::func_ov006_020dd2cc()
{
    int i;
    for (i = 0; i < 0x18; i++) {
        unsigned char k = this->mCoins[i].state;
        (this->*data_ov006_021417b0[k].pmf)(i);
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd334Ev
/* Lays the 24 coins out in three rows of eight and marks two coins in each
   row, picked at random, at +0x14. */
void dScMgCoin_c::func_ov006_020dd334()
{
    char *c = (char *)this;
    int a[3];
    int b[3];
    int k;
    int r3;
    int r2;
    int i;
    char *p;

    for (k = 0; k < 3; k++) {
        a[k] = (int)((((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) << 3) >> 15);
    }

    for (k = 0; k < 3; k++) {
        unsigned int w = ((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 6;
        b[k] = (a[k] + (int)(w >> 15) + 1) & 7;
    }

    r2 = 0;
    r3 = 0;
    i = 0;
    p = c;
    for (; i < 0x18; i++, p += 0x18) {
        *(int *)(p + 0x4ac0) = (((r3 << 5) + 0x10) << 12);
        *(int *)(p + 0x4ac4) = data_ov006_0212e358[r2] << 12;
        r3++;
        *(unsigned char *)(p + 0x4ad1) = 1;
        *(unsigned char *)(p + 0x4ad2) = 1;
        if (r3 >= 8) { r3 = 0; r2++; }
    }

    *(unsigned char *)(c + a[0] * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + b[0] * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (a[1] + 8) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (b[1] + 8) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (a[2] + 0x10) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (b[2] + 0x10) * 0x18 + 0x4ad4) = 1;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd4b0Ei
/* Finds the first resting block (state 4) that belongs to coin id, kicks it
   up from under the coin (state 5) and plays the panned hop sound. */
void dScMgCoin_c::func_ov006_020dd4b0(int id)
{
    char *base = (char *)this;
    int i;
    char *p = base;
    for (i = 0; i < 0x28; i++) {
        if (*(unsigned char *)(p + 0x4677) != 0 &&
            *(unsigned char *)(p + 0x4675) == 4 &&
            id == *(unsigned char *)(p + 0x467b)) {
            int v = *(int *)(base + id * 0x18 + 0x4ac4);
            *(int *)(base + i * 0x1c + 0x4664) = v - 0x20000;
            *(unsigned char *)(base + i * 0x1c + 0x4676) = 1;
            *(unsigned char *)(base + i * 0x1c + 0x4675) = 5;
            *(int *)(base + i * 0x1c + 0x466c) = -0x4800;
            *(short *)(base + i * 0x1c + 0x4670) = 0;
            int w = (*(int *)(base + id * 0x18 + 0x4ac0) >> 12) - 0x80;
            int r = w >> 1;
            if (r >= 0x3c) r = 0x3c;
            if (r <= -0x3c) r = -0x3c;
            func_020127a4(2, 0xef, 0xffff, r);
            return;
        }
        p += 0x1c;
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd594Ev
/* Draws the 40 blocks on both screens. */
typedef struct {
    int f0;
    int f4;
    char pad8[0xd];
    unsigned char b15;
    unsigned char b16;
    char pad17;
    unsigned char b18;
    char pad19[3];
} E_dd594;

typedef struct {
    char pad0[0x4660];
    E_dd594 e[40];
    char pad1[0x708];
    int f51c8;
    int f51cc;
} Self_dd594;

void dScMgCoin_c::func_ov006_020dd594()
{
    Self_dd594 *self = (Self_dd594 *)this;
    int i;
    if (self->f51c8 == 5 && self->f51cc == 0) return;
    for (i = 0; i < 0x28; i++) {
        if (self->e[i].b16 == 0) continue;
        {
            unsigned int flag = self->e[i].b15;
            int v0 = self->e[i].f0;
            int v4 = self->e[i].f4;
            int a1, a2, a4;
            unsigned short idx;
            a1 = v0 >> 12;
            a2 = v4 >> 12;
            a4 = (flag >= 3) ? 1 : 0;
            idx = data_ov006_0212e314[self->e[i].b18];
            RenderOamBothScreens(data_ov006_0213bf0c[idx], a1, a2, -1, a4, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd658Ei
/* A block falling into place. The panned landing sound plays on the frame
   vy turns from rising to falling; once it passes its row it rests (state
   4), bumps the running total and bursts an effect. */
struct Pair_dd658 { int a; int b; };
extern "C" void func_ov004_020adfc4(int a, int b, struct Pair_dd658 *p2, struct Pair_dd658 *p3,
                                    struct Pair_dd658 *p4);

void dScMgCoin_c::func_ov006_020dd658(int i)
{
    char *self = (char *)this;
    int n = i * 0x1c;
    int old466c;
    struct Pair_dd658 pa;
    struct Pair_dd658 pb;

    old466c = *(s32 *)(self + 0x466c + n);
    *(s32 *)(self + 0x4664 + n) += old466c;
    *(s32 *)(self + 0x466c + n) += 0x600;

    if (old466c <= 0 && *(s32 *)(self + 0x466c + n) >= 0) {
        int v = ((*(s32 *)(self + 0x4660 + n) >> 12) - 0x80) >> 1;
        if (v >= 0x3c)
            v = 0x3c;
        if (v <= -0x3c)
            v = -0x3c;
        func_020127a4(2, 0xec, 0xffff, v);
    }

    {
        int y = *(s32 *)(self + 0x4664 + n) >> 12;
        u8 idx = *(u8 *)(self + 0x4674 + n);
        if (*(s32 *)(self + 0x466c + n) < 0)
            return;
        if (y <= data_ov006_0212e418[idx] - 0x20)
            return;

        *(u8 *)(self + 0x4675 + n) = 4;
        *(u8 *)(self + 0x4676 + n) = 0;
        *(u8 *)(self + 0x4677 + n) = 0;
        (*(u16 *)(self + 0x4d08))++;

        {
            int v1 = *(s32 *)(self + 0x4664 + n);
            int v0 = *(s32 *)(self + 0x4660 + n);
            pb.a = 0x6c000;
            pb.b = -0x80000;
            pa.a = v0;
            pa.b = v1;
        }
        func_ov004_020adfc4(1, 0x20, &pa, &pa, &pb);
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd7bcEv
void dScMgCoin_c::func_ov006_020dd7bc()
{
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd7c0Ei
/* A block dropping onto its coin's row. It stops at the row height from
   data_ov006_0212e400, takes the coin's x and rests (state 4). */
void dScMgCoin_c::func_ov006_020dd7c0(int index)
{
    char *thiz = (char *)this;
    int off = index * 0x1c;
    int new_var;
    int *pa = (int *)((thiz + 0x466c) + off);
    int *pb = (int *)((thiz + 0x4664) + off);
    char *f = (thiz + off) + 0x4000;
    *pb = (*pb) + (*((int *)((thiz + 0x466c) + off)));
    *((int *)((thiz + 0x466c) + off)) = (*pa) + 0x400;
    new_var = 0x674;
    int look = data_ov006_0212e400[*((unsigned char *)(((thiz + off) - -0x4000) + new_var))];
    if (((*pb) >> 12) <= look) {
        return;
    }
    new_var = (*((unsigned char *)(((thiz + off) + 0x4000) + 0x67b))) * 0x18;
    char *g = (thiz + new_var) + 0x4000;
    *((int *)(((thiz + off) + 0x4000) + 0x660)) = *((int *)(g + 0xac0));
    *pb = look << 12;
    *pa = 0;
    *((int *)(((thiz + off) + 0x4000) + 0x668)) = 0;
    *((unsigned char *)(((thiz + off) + 0x4000) + 0x675)) = 4;
    pa += 0;
    *((unsigned char *)(((thiz + off) + 0x4000) + 0x676)) = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dd880Ei
/* A block wandering along its row. When it reaches the row it picks a
   random direction, and over a coin of that row still worth at most two
   (five when Virtual8C says so) it claims the coin, adds one to its value
   and starts to rise (state 3). */
#define LB(a) (*(unsigned char *)(((long long)(int)(a))))

void dScMgCoin_c::func_ov006_020dd880(int i)
{
    char *c = (char *)this;
    int n = i * 0x1c;
    int idxb;
    int diff;
    int base_j;
    int fp;
    int j;
    int *pD;

    *(int *)(c + 0x4660 + n) += *(int *)(c + 0x4668 + n);
    *(int *)(c + 0x4664 + n) += *(int *)(c + 0x466c + n);
    *(int *)(c + 0x466c + n) += 0x400;
    this->func_ov006_020ddcf8(i);

    pD = (int *)(c + 0x466c + n);
    idxb = *(u8 *)(c + n + 0x4674);
    diff = data_ov006_0212e3d0[idxb] - data_ov006_0212e3e8[idxb];
    if ((*(int *)(c + 0x4664 + n) >> 12) > diff) {
        *(int *)(c + 0x4664 + n) = diff * 0x1000;
        *pD = 0;
        if (*(int *)(c + 0x4668 + n) == 0) {
            unsigned int rv = (unsigned int)RandomIntInternal(&data_0209d4b8);
            if (((((rv >> 16) & 0x7fff) << 1) >> 15))
                *(int *)(c + 0x4668 + n) = 0xc00;
            else
                *(int *)(c + 0x4668 + n) = -0xc00;
        }
    }
    {
        int bval = *(int *)(c + 0x4664 + n);
        if (bval != (diff << 12))
            return;
    }

    base_j = (idxb - 3) * 8;
    fp = *(int *)(c + 0x4660 + n) >> 12;
    for (j = 0; j < 8; j++) {
        int idx = base_j + j;
        int d = (*(int *)((c + idx * 0x18) + 0x4ac0) >> 12) - fp;
        int limit = 2;
        char *e = c + idx * 0x18;
        int res = this->Virtual8C();
        if (res != 0) limit = 5;
        if (d <= -6 || d >= 6) continue;
        if (*(u8 *)(e + 0x4ad4) != 0) continue;
        if (LB(e + 0x4ad3) > limit) continue;
        LB(e + 0x4ad3) += 1;
        *(u8 *)(c + n + 0x4675) = 3;
        *pD = 0x200;
        *(u8 *)(c + n + 0x467b) = (u8)idx;
        return;
    }
}

#undef LB

// @symbol _ZN11dScMgCoin_c19func_ov006_020dda94Ei
/* A block hopping down the stages. Each landing picks a new random vx and
   plays the panned hop; when its hops for a stage run out it moves down a
   stage, and past its own row it switches to state 2. */
void dScMgCoin_c::func_ov006_020dda94(int i)
{
    char *self = (char *)this;
    int n = i * 0x1c;
    char *pA = self + 0x4660;
    char *fp;
    char *pD = self + 0x466c;
    char *pB = self + 0x4664;
    char *pC = self + 0x4668;

    *(s32 *)(pA + n) += *(s32 *)(pC + n);
    *(s32 *)(pB + n) += *(s32 *)(pD + n);
    *(s32 *)(pD + n) += 0x400;

    this->func_ov006_020ddcf8(i);

    {
        u8 g = *(u8 *)(self + 0x4679 + n);
        int y = *(s32 *)(pB + n) >> 12;
        u8 h = *(u8 *)(self + 0x467a + n);

        if (g != 0) {
            int rnd, idx, a, v;
            u8 g3;

            fp = (char *)(data_ov006_0212e3a0[h] - data_ov006_0212e3b8[h]);
            if (y < (int)fp) return;

            rnd = RandomIntInternal(&data_0209d4b8);
            idx = ((((unsigned)rnd >> 16) & 0x7fff) * 3) >> 15;
            *(s32 *)(pC + n) = data_ov006_0212e34c[idx];
            *(s32 *)(pD + n) = -0x2e00;
            *(s32 *)(pB + n) = (int)fp << 12;
            *(u8 *)(self + 0x4679 + n) -= 1;

            a = (*(s32 *)(pA + n) >> 12) - 0x80;
            v = a >> 1;
            if (v >= 0x3c) v = 0x3c;
            if (v <= -0x3c) v = -0x3c;
            func_020127a4(2, 0x197, 0xffff, v);

            g3 = *(u8 *)(self + 0x4679 + n);
            if (g3 == 0) {
                *(u8 *)(self + 0x467a + n) += 1;
            }
            {
                u8 hh = *(u8 *)(self + 0x467a + n);
                u8 e = *(u8 *)(self + 0x4674 + n);
                if (hh > e) *(u8 *)(self + 0x4675 + n) = 2;
            }
        } else {
            unsigned r;
            *(u8 *)(self + 0x4679 + n) = 1;
            r = (unsigned)RandomIntInternal(&data_0209d4b8);
            r = (((r >> 16) & 0x7fff) << 3) >> 15;
            if (r == 2 || r == 5) {
                *(u8 *)(self + 0x4679 + n) += 1;
            }
            return;
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020ddca0Ei
/* A block waiting to drop: counts its delay down, then starts it (state 1). */
void dScMgCoin_c::func_ov006_020ddca0(int i)
{
    char *c = (char *)this;
    int idx = i * 0x1c;
    char *r2 = c + 0x4670;
    unsigned short *p = (unsigned short *)(r2 + idx);
    if (*p != 0) {
        *p = *p - 1;
        if (*(short *)p < 0) *(short *)p = 0;
        return;
    }
    char *r0 = c + idx;
    *(char *)(r0 + 0x4000 + 0x675) = 1;
    *(char *)(r0 + 0x4000 + 0x679) = 1;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020ddcf8Ei
/* Keeps a block between x 8 and 0xf8, reversing vx at either wall. */
void dScMgCoin_c::func_ov006_020ddcf8(int idx)
{
    char *c = (char *)this;
    int *f60 = (int *)(c + 0x4660 + idx * 0x1c);
    int v = *f60 >> 12;
    if (v < 8) {
        *f60 = 0x8000;
        *(int *)(c + 0x4668 + idx * 0x1c) = -*(int *)(c + 0x4668 + idx * 0x1c);
        return;
    }
    if (v > 0xf8) {
        *f60 = 0xf8000;
        *(int *)(c + 0x4668 + idx * 0x1c) = -*(int *)(c + 0x4668 + idx * 0x1c);
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020ddd6cEv
/* Runs every live block's state handler and animation. When none is left
   moving the scene goes to phase 2, the score starts running and the start
   sound plays. */
void dScMgCoin_c::func_ov006_020ddd6c()
{
    char *thiz = (char *)this;
    int n = 0;
    int i = 0;
    char *p = thiz;
    for (; i < 0x28; i++) {
        if (*(unsigned char *)(p + 0x4000 + 0x677) != 0) {
            (this->*data_ov006_02141840[*(unsigned char *)(p + 0x4000 + 0x675)].pmf)(i);
            if (*(unsigned char *)(p + 0x4000 + 0x675) != 4)
                n++;
            this->func_ov006_020dde28(i);
        }
        p += 0x1c;
    }
    if (n != 0)
        return;
    *(int *)(thiz + 0x5000 + 0x1c8) = 2;
    *(unsigned char *)(thiz + 0x4000 + 0xd13) = 1;
    Sound::PlayBank2_2D(0x151);
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020dde28Ei
/* Steps one block's four-frame animation; data_ov006_0212e32c holds each
   frame's length. */
typedef struct {
    char _pad0[0x12];
    u16 timer;   /* +0x12 */
    char _pad1[4];
    u8 frame;    /* +0x18 */
    char _pad2[3];
} Entry_dde28; /* 0x1c */

typedef struct {
    char _pad0[0x4660];
    Entry_dde28 entries[16];
} Work_dde28;

void dScMgCoin_c::func_ov006_020dde28(int index)
{
    char *c = (char *)this;
    Work_dde28 *w = (Work_dde28 *)c;
    w->entries[index].timer++;
    if (w->entries[index].timer < data_ov006_0212e32c[w->entries[index].frame])
        return;
    w->entries[index].frame++;
    w->entries[index].frame &= 3;
    w->entries[index].timer = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020ddeb0Ev
/* Parks the blocks above the top of the screen at random x with staggered
   delays, cycling their rows 3, 4, 5. Sixteen when Virtual8C says so,
   otherwise forty. */
void dScMgCoin_c::func_ov006_020ddeb0()
{
    int n;
    unsigned int r8 = 0;
    int i;
    char *c = (char *)this;
    n = this->Virtual8C() != 0 ? 0x10 : 0x28;
    for (i = 0; i < n; i++) {
        unsigned int v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(c + 0x4660) = ((((v << 3) >> 0xf) << 5) + 0x10) << 0xc;
        *(int *)(c + 0x4664) = -0xf0000;
        *(unsigned char *)(c + 0x4677) = 1;
        *(unsigned char *)(c + 0x4676) = 1;
        {
            unsigned int w = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(short *)(c + 0x4670) = (short)(((i & 0xf) << 3) + (((w << 4) >> 0xf) << 4));
        }
        *(unsigned char *)(c + 0x4674) = (unsigned char)(r8 + 3);
        r8 = (r8 + 1) & 0xff;
        if (r8 >= 3) r8 = 0;
        c += 0x1c;
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020ddf9cEv
/* Clears the blocks, coins, score, sparkles, popups and phase, then resets
   the caption and the moneybag. */
void dScMgCoin_c::func_ov006_020ddf9c()
{
    char *c = (char *)this;
    int i, k, j, i2, i3, i4;
    char *p = c;
    k = 0;
    for (; k < 0x28; k++) {
        i = 0;
        *((u32 *)(p + 0x4660)) = i;
        *((u32 *)(p + 0x4664)) = i;
        *((u32 *)(p + 0x4668)) = i;
        *((u32 *)(p + 0x466c)) = i;
        *((u16 *)(p + 0x4670)) = i;
        *((u16 *)(p + 0x4672)) = i;
        *((u8 *)(p + 0x4674)) = i;
        *((u8 *)(p + 0x4675)) = i;
        *((u8 *)(p + 0x4676)) = i;
        *((u8 *)(p + 0x4677)) = i;
        *((u8 *)(p + 0x4678)) = i;
        *((u8 *)(p + 0x4679)) = i;
        *((u8 *)(p + 0x467a)) = i;
        *((u8 *)(p + 0x467b)) = i;
        p += 0x1c;
    }

    p = c;
    for (i2 = 0; i2 < 0x18; i2++) {
        j = 0;
        *((u32 *)(p + 0x4ac0)) = j;
        *((u32 *)(p + 0x4ac4)) = j;
        *((u8 *)(p + 0x4ad0)) = j;
        *((u8 *)(p + 0x4ad1)) = j;
        *((u8 *)(p + 0x4ad2)) = j;
        *((u8 *)(p + 0x4ad3)) = j;
        *((u8 *)(p + 0x4ad4)) = j;
        *((u8 *)(p + 0x4ad5)) = j;
        p += 0x18;
    }

    *((u32 *)(c + 0x4d00)) = 0;
    *((u32 *)(c + 0x4d04)) = 0;
    *((u16 *)(c + 0x4d08)) = 0;
    *((u16 *)(c + 0x4d0a)) = 0;
    *((u16 *)(c + 0x4d0c)) = 0;
    *((u8 *)(c + 0x4d13)) = 0;
    p = c;
    for (i3 = 0; i3 < 0x20; i3++) {
        *((u8 *)(p + 0x4d28)) = 0;
        *((u8 *)(p + 0x4d29)) = 0;
        p += 0x18;
    }

    for (i4 = 0; i4 < 0x18; i4++) {
        char *e = c + ((i4 & 0xFFFFFFFFu) << 4);
        *((u8 *)(e + 0x5020)) = 0;
        *((u8 *)(e + 0x5021)) = 0;
    }

    *((u32 *)(c + 0x51c8)) = 0;
    *((u32 *)(c + 0x51cc)) = 0;
    *((u8 *)(c + 0x51db)) = 0;
    *((u8 *)(c + 0x51dd)) = 0;
    *((u32 *)(c + 0x51d0)) = 0;
    *((u8 *)(c + 0x51de)) = 0;
    this->func_ov006_020dc7fc();
    this->func_ov006_020dc334();
    *((u8 *)(c + 0x51df)) = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020de0e0Ev
/* The result countdown. While the stylus is held it speeds to zero with a
   sound; otherwise it holds at 0x80. At zero the scene frees its graphics
   if it loaded them and starts the fade out. */
void dScMgCoin_c::func_ov006_020de0e0()
{
    char *self = (char *)this;
    if (*(int *)(self + 0x5000 + 0x1cc) == 0) return;
    *(int *)(((int)self + 0x51cc)) -= 1;
    unsigned int idx = data_020a0e40;
    int flag = 0;
    if (data_020a0de8[idx * 4] != 0) {
        if (data_020a0de9[idx * 4] != 0) flag = 1;
    }
    if (flag != 0 && *(int *)(self + 0x51cc) <= 0x80) {
        *(int *)(self + 0x51cc) = 0;
        Sound::PlayBank2_2D(0x62);
    } else {
        *(int *)(self + 0x51cc) = 0x80;
    }
    if (*(int *)(self + 0x51cc) > 0) return;
    *(int *)(self + 0x51cc) = 0;
    if (*(unsigned char *)(self + 0x51df) != 0) {
        FreeGfxSlotsById(6);
        func_ov004_020ae20c();
    }
    func_ov004_020b0a54(0x10);
    *(unsigned char *)(self + 0xc3) = 0;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020de1d4Ev
/* Counts the countdown at 0x51cc down; at zero, re-arms the caption if the
   other player scored, then enters the result phase with 0xc0 frames. */
void dScMgCoin_c::func_ov006_020de1d4()
{
    char *c = (char *)this;
    if (*(int *)((c + 0x5000) + 0x1cc) != 0) {
        int *p = (int *)(int)(((long long)(int)(c + 0x51cc)));
        *p = *p - 1;
        if (*(int *)((c + 0x5000) + 0x1cc) < 0)
            *(int *)((c + 0x5000) + 0x1cc) = 0;
        return;
    }
    {
        char *o = (char *)data_ov004_020beb68;
        int v = o != 0 ? *(int *)(o + 0xa8) : 0;
        if (v != 0) {
            *(unsigned char *)(c + 0x51de) = 0;
            this->func_ov006_020dc370();
        }
    }
    *(int *)(c + 0x51cc) = 0xc0;
    *(int *)(c + 0x51c8) = 5;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020de26cEv
/* Phase 4 on the way to the result: runs the blocks, coins and sparkles,
   and once nothing moves counts 0x51cc down, bringing in the caption or
   the other screen at 0x20, then hands the score over and sets phase 4. */
void dScMgCoin_c::func_ov006_020de26c()
{
    char *self = (char *)this;
    int count;
    int i;
    char *r5;
    int j;
    char *r2;
    char *g;

    count = 0;
    for (i = 0, r5 = self; i < 0x28; i++) {
        if (*(unsigned char *)(r5 + 0x4677)) {
            int idx = *(unsigned char *)(r5 + 0x4675);
            (this->*data_ov006_02141840[idx].pmf)(i);
            if (*(unsigned char *)(r5 + 0x4675) != 4) count++;
            if (*(unsigned char *)(r5 + 0x4676) != 0) this->func_ov006_020dde28(i);
        }
        r5 += 0x1c;
    }
    r2 = self;
    for (j = 0; j < 0x18; j++) {
        if (*(unsigned char *)(r2 + 0x4ad0) == 1 && *(unsigned char *)(r2 + 0x4ad1) != 0)
            count++;
        r2 += 0x18;
    }
    this->func_ov006_020dd2cc();
    this->func_ov006_020dca04();
    if (count != 0) return;

    if (((int *)(self + 0x5000))[0x73] != 0) {
        *(int *)(((long long)(int)(self + 0x51cc))) -= 1;
        if (((int *)(self + 0x5000))[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] == 0)
            this->func_ov006_020dc348();
        if (((int *)(self + 0x5000))[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] != 0) {
            func_ov004_020b0cac(6, 0x80, -0x80, -1, -1, 0xd);
            func_ov004_020ae274(0);
        }
        if (((int *)(self + 0x5000))[0x73] <= 0) ((int *)(self + 0x5000))[0x73] = 0;
        return;
    }
    g = (char *)data_ov004_020beb68;
    func_ov004_020adb1c(g != 0 ? *(int *)(g + 0xa8) : 0);
    ((int *)(self + 0x5000))[0x73] = 0x70;
    ((int *)(self + 0x5000))[0x72] = 4;
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020de440Ev
/* Phase 2, play: runs the blocks, coins and sparkles. Once every coin worth
   something has been touched the round ends (phase 3, 0x40 frames) and the
   win flag is set from the other score. */
void dScMgCoin_c::func_ov006_020de440()
{
    char *c = (char *)this;
    int i;
    char *p;
    for (i = 0, p = c; i < 0x28; i++, p += 0x1c) {
        if (*(u8 *)(p + 0x4677) != 0) {
            (this->*data_ov006_02141840[*(u8 *)(p + 0x4675)].pmf)(i);
            this->func_ov006_020dde28(i);
        }
    }
    this->func_ov006_020dd2cc();
    this->func_ov006_020dca04();
    if (*(s32 *)(c + 0x51c8) == 3) {
        return;
    }
    {
        int found = 0;
        int j;
        char *q;
        for (j = 0, q = c; j < 0x18; j++, q += 0x18) {
            if (*(u8 *)(q + 0x4ad3) != 0) {
                if (*(u8 *)(q + 0x4ad5) == 0) {
                    found++;
                    break;
                }
            }
        }
        if (found != 0) {
            return;
        }
    }
    *(s32 *)(c + 0x51c8) = 3;
    *(s32 *)(c + 0x51cc) = 0x40;
    *(u8 *)(c + 0x51df) = 1;
    {
        int t;
        void *g = data_ov004_020beb68;
        if (g != 0) {
            t = *(s32 *)((char *)g + 0xa8);
        } else {
            t = 0;
        }
        if (t > *(s32 *)(c + 0x51d4)) {
            *(u8 *)(c + 0x51db) = 1;
        } else {
            *(u8 *)(c + 0x51db) = 0;
        }
    }
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020de584Ev
/* Phase 1: on the first frame, flags the scene and clears its timer, then
   runs the blocks. */
void dScMgCoin_c::func_ov006_020de584()
{
    char *p = (char *)this;
    if ((*((unsigned char *)(p + 0xc4))) == 0) {
        *((unsigned char *)(p + 0xc3)) = 1;
        *((unsigned char *)(p + 0xc4)) = 1;
        *((short *)(p + 0xc0)) = 0;
    }
    this->func_ov006_020ddd6c();
}

// @symbol _ZN11dScMgCoin_c19func_ov006_020de5acEv
void dScMgCoin_c::func_ov006_020de5ac()
{
}

// @symbol _ZN11dScMgCoin_c13OnYoshiTryEatEi
/* Slot 18. Starts the next round: counts a win if the last one was won,
   clears the scene, frees its graphics and lays the coins out again. It is
   void; the mov r0,#0 at the end is the source of the unk_51c8 store. */
void dScMgCoin_c::OnYoshiTryEat(int arg)
{
    dScMgCoin_c *self = this;

    if (self->unk_51db != 0) {
        *(unsigned char *)((int)self + 0x51da) += 1;
    } else {
        self->unk_51da = 0;
    }
    self->unk_0a8 = 0;
    *(s32 *)((char *)self + 0xac) = self->unk_0a8;
    this->func_ov006_020ddf9c();
    FreeGfxSlotsById(0x1d);
    this->func_ov006_020dd334();
    this->func_ov006_020dc7b4();
    this->func_ov006_020dc2f8();
    self->unk_51d4 = func_ov004_020adc1c();
    self->unk_51c8 = 0;
}

// @symbol _ZN11dScMgCoin_c6RenderEv
int dScMgCoin_c::Render()
{
    this->func_ov006_020dccb8();
    this->func_ov006_020dcd74();
    this->func_ov006_020dc814();
    this->func_ov006_020dd594();
    this->func_ov006_020dc3bc();
    this->func_ov006_020dbe9c();
    this->func_ov006_020dc99c();
    this->func_ov006_020dc870();
    this->func_ov006_020dcea8();
    this->func_ov006_020dcc48();
    return 1;
}

// @symbol _ZN11dScMgCoin_c8BehaviorEv
/* Runs this phase's handler from data_ov006_02141810, then the caption,
   the moneybag, the popups and the score. */
typedef void (dScMgCoin_c::*PMF_Behavior)();
struct Entry_Behavior { PMF_Behavior pmf; };
extern Entry_Behavior data_ov006_02141810[];

int dScMgCoin_c::Behavior()
{
    int idx = unk_51c8;
    (this->*data_ov006_02141810[idx].pmf)();
    this->func_ov006_020dc754();
    this->func_ov006_020dc298();
    this->func_ov006_020dc900();
    this->func_ov006_020dce3c();
    return 1;
}

// @symbol _ZN11dScMgCoin_c13InitResourcesEv
/* Loads and decompresses both screens' backgrounds, palettes and sprites,
   then sets up the first round. */
int dScMgCoin_c::InitResources()
{
    dScMgCoin_c *self = this;
    char *c = (char *)self;
    int a, b, d;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e &= ~3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    a = LoadFile(0xa9);
    DecompressLZ16(a, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)a);

    a = LoadFile(0xaa);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)a, 0x1e0, 0x20);
    Deallocate((void *)a);

    a = LoadFile(0xab);
    func_02056314((void *)a, 0, 0x800);
    Deallocate((void *)a);

    b = LoadFile(0x10a);
    a = LoadFile(0x10b);
    DecompressLZ16(b, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)a, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1210;

    d = LoadFile(0xa9);
    DecompressLZ16(d, (void *)(_ZN3G2S13GetBG3CharPtrEv() + 0x4000));
    Deallocate((void *)d);

    d = LoadFile(0xaa);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)d, 0x1e0, 0x20);
    Deallocate((void *)d);

    d = LoadFile(0xa8);
    func_020562b4((const void *)d, 0, 0x800);
    Deallocate((void *)d);

    DecompressLZ16(b, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)a, 0, 0x100);
    Deallocate((void *)b);
    Deallocate((void *)a);

    self->unk_0a8 = 0;
    *(s32 *)(c + 0xac) = self->unk_0a8;
    data_0208ee44 = 1;
    this->func_ov006_020ddf9c();
    this->func_ov006_020dd334();
    this->func_ov006_020dc7b4();
    this->func_ov006_020dc2f8();
    self->unk_51da = 0;
    self->unk_51dc = 2;
    func_ov004_020b04d0(0x20);
    self->unk_51d4 = func_ov004_020adc1c();
    self->unk_0a4 = 1;
    return 1;
}
