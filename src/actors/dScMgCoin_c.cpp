//cpp
/**
 * Coincentration. Twenty-four coins fall into the blocks. A coin
 * keeps bouncing until it has bounced as many times as it is worth,
 * then sits and pops that value. Sparkles burst off it. The moneybag
 * hops down six stages and drops off the bottom. Phase 5 is the
 * result: a countdown, then this score against the other player's.
 *
 * Leftover: func_ov006_020ddeb0 (from func_ov006_020dc154 and
 *   func_ov006_020dc1c4) and func_ov006_020dd4b0 (from
 *   func_ov006_020dd000) are still unnamed.
 * Leftover: RenderOamMainScreen and func_ov004_020b0380 are redeclared
 *   at the call. The sites disagree on the types.
 * Leftover: func_ov006_020dc298 and func_ov006_020dc754 call through
 *   data_ov006_021417c8 and data_ov006_021417e8. Those records are
 *   filled by __sinit_ov006_0213014c.
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
extern void func_ov006_020dd4b0(char *raw, int index);
}

// @symbol func_ov006_020dbf7c
extern "C" void func_ov006_020dbf7c(BagView *scene, int i)
{
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

// @symbol func_ov006_020dc154
extern "C" void func_ov006_020dc154(BagView *scene, int index)
{
    extern void func_ov006_020ddeb0(char *rec);

    if (scene->bag[index].countdown != 0) {
        scene->bag[index].countdown = scene->bag[index].countdown - 1;
        return;
    }
    scene->bag[index].state = 3;
    scene->bag[index].vy = 0;
    scene->bag[index].bouncesLeft = 1;
    scene->bag[index].stage = 0;
    func_ov006_020ddeb0((char *)scene);
}

// @symbol func_ov006_020dc1c4
extern "C" void func_ov006_020dc1c4(BagView *scene, int index)
{
    extern void func_ov006_020ddeb0(void *self);

    scene->bag[index].y = scene->bag[index].y + scene->bag[index].vy;
    scene->bag[index].vy = scene->bag[index].vy - kFallStep;
    scene->bag[index].spin = scene->bag[index].spin + kSpinStep;
    if ((scene->bag[index].y >> 12) > kParkRow)
        return;
    scene->bag[index].y = kParkedY;
    scene->bag[index].state = 2;
    scene->bag[index].countdown = 0x30;
    func_ov006_020ddeb0(scene);
}

// @symbol func_ov006_020dc26c
extern "C" void func_ov006_020dc26c(dScMgCoin_c *scene)
{
    scene->mBouncer.state = 1;
    scene->mBouncer.sprite = 1;
    scene->mBouncer.vx = 0;
    scene->mBouncer.vy = kKickVy;
}

// @symbol func_ov006_020dc294
extern "C" void func_ov006_020dc294(void)
{
}

// @symbol func_ov006_020dc298
extern "C" {
struct C_c298;
typedef void (C_c298::*PMF_c298)(int);
struct Entry_c298 { PMF_c298 pmf[1]; };
extern Entry_c298 data_ov006_021417c8[];
struct C_c298 {
    char pad[0x51a8];
    dScMgCoin_Bouncer bouncer;
};

/* Dispatch the moneybag while mBouncer.active is set. Not a vtable:
   the records live in data_ov006_021417c8. */
void func_ov006_020dc298(C_c298 *scene)
{
    if (scene->bouncer.active == 0)
        return;

    int state = scene->bouncer.state;
    (scene->*data_ov006_021417c8[state].pmf[0])(0);
}
}

// @symbol func_ov006_020dc2f8
extern "C" void func_ov006_020dc2f8(char *raw)
{
    dScMgCoin_c *scene = (dScMgCoin_c *)raw;
    scene->mBouncer.active = 1;
    scene->mBouncer.shown = 1;
    scene->mBouncer.sprite = 0;
    scene->mBouncer.state = 0;
    scene->mBouncer.spin = 0;
    scene->mBouncer.x = kBagRestX;
    scene->mBouncer.y = kBagRestY;
}

// @symbol func_ov006_020dc334
extern "C" void func_ov006_020dc334(dScMgCoin_c *scene)
{
    scene->mBouncer.active = 0;
    scene->mBouncer.shown = 0;
}

// @symbol func_ov006_020dc348
extern "C" void func_ov006_020dc348(dScMgCoin_c *scene)
{
    scene->mCaption.state = 4;
    scene->mCaption.frame = 0;
    scene->mCaption.frameTime = 0;
    scene->mCaptionLatch = 1;
}

// @symbol func_ov006_020dc370
extern "C" void func_ov006_020dc370(dScMgCoin_c *scene)
{
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

// @symbol func_ov006_020dc3bc
extern "C" void func_ov006_020dc3bc(dScMgCoin_c *scene)
{
    extern int RenderOamMainScreen(int a, int b, int raw, int d, int e);

    if (scene->mCaption.visible == 0)
        return;

    RenderOamMainScreen(data_ov006_0213a9fc[scene->mCaption.sprite],
        scene->mCaption.x >> 12, scene->mCaption.y >> 12, -1, -1);
}

// @symbol func_ov006_020dc414
extern "C" void func_ov006_020dc414(CapView *scene, int index)
{
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

// @symbol func_ov006_020dc4b0
extern "C" void func_ov006_020dc4b0(dScMgCoin_c *scene, int index)
{
    (&scene->mCaption)[index].sprite = 0;
}

// @symbol func_ov006_020dc4c8
extern "C" void func_ov006_020dc4c8(CapView *scene, int index)
{
    u16 frameTime = scene->cap[index].frameTime;
    scene->cap[index].frameTime = frameTime + 1;
    u8 frame = scene->cap[index].frame;
    if (scene->cap[index].frameTime >= data_ov006_0212e308[frame]) {
        scene->cap[index].frameTime = 0;
        scene->cap[index].frame = scene->cap[index].frame + 1;
        scene->cap[index].frame = scene->cap[index].frame & 1;
    }
    scene->cap[index].sprite = data_ov006_0212e30c[scene->cap[index].frame];
    if (((dScMgCoin_c *)scene)->unk_51c8 == kResultPhase)
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

// @symbol func_ov006_020dc5c4
extern "C" void func_ov006_020dc5c4(CapView *scene, int index)
{
    extern void func_ov006_020dc26c(dScMgCoin_c *scene);

    u16 *frameTime = &scene->cap[index].frameTime;
    u8 *frame = &scene->cap[index].frame;

    *frameTime = (u16)(*frameTime + 1);
    if (*frameTime < data_ov006_0212e33c[*frame])
        return;
    *frameTime = 0;
    *frame = (u8)(*frame + 1);
    if (*frame == 2) {
        func_ov006_020dc26c((dScMgCoin_c *)scene);
        Sound::PlayBank2_2D(kSndReady);
    }
    if (*frame >= 4) {
        scene->cap[index].delay = kCountInDelay;
        scene->cap[index].state = 2;
        {
            int *phase = &((dScMgCoin_c *)scene)->unk_51c8;
            *frame = 0;
            *frameTime = 0;
            *phase = *phase + 1;
        }
    } else {
        scene->cap[index].sprite = data_ov006_0212e310[*frame];
    }
}

// @symbol func_ov006_020dc6d0
extern "C" void func_ov006_020dc6d0(int raw, int index)
{
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

// @symbol func_ov006_020dc754
extern "C" {
struct C_c754;
typedef void (C_c754::*PMF_c754)(int);
struct Entry_c754 { PMF_c754 pmf[1]; };
extern Entry_c754 data_ov006_021417e8[];
struct C_c754 {
    char pad[0x5194];
    dScMgCoin_Caption caption;
};

/* Same dispatch as the moneybag, for the caption. */
void func_ov006_020dc754(C_c754 *scene)
{
    if (scene->caption.active == 0)
        return;

    int state = scene->caption.state;
    (scene->*data_ov006_021417e8[state].pmf[0])(0);
}
}

// @symbol func_ov006_020dc7b4
extern "C" void func_ov006_020dc7b4(char *raw)
{
    dScMgCoin_c *scene = (dScMgCoin_c *)raw;
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

// @symbol func_ov006_020dc7fc
extern "C" void func_ov006_020dc7fc(dScMgCoin_c *scene)
{
    scene->mCaption.active = 0;
    scene->mCaption.visible = 1;
}

// @symbol func_ov006_020dc814
extern "C" void func_ov006_020dc814(dScMgCoin_c *scene)
{
    if (scene->unk_51c8 != kResultPhase)
        return;

    int left = scene->mCountdown;
    if (left > 0x80)
        return;
    if (left == 0)
        return;

    func_ov004_020b0d8c(scene, 0xe0, 0xa0);
}

// @symbol func_ov006_020dc870
extern "C" void func_ov006_020dc870(dScMgCoin_c *scene)
{
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

// @symbol func_ov006_020dc900
extern "C" void func_ov006_020dc900(char *raw)
{
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

// @symbol func_ov006_020dc960
extern "C" void func_ov006_020dc960(char *raw, int index)
{
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

// @symbol func_ov006_020dc99c
extern "C" void func_ov006_020dc99c(SparkView *scene)
{
    extern void func_ov004_020b0380(void *fn, int x, int y, int d);

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

// @symbol func_ov006_020dca04
extern "C" void func_ov006_020dca04(SparkView *scene)
{
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

// @symbol func_ov006_020dcb1c
extern "C" void func_ov006_020dcb1c(char *raw, int coinIndex)
{
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

// @symbol func_ov006_020dcc48
extern "C" void func_ov006_020dcc48(void)
{
    extern void func_ov004_020b0380(int a, int b, int c, int d);

    int i;
    int x, y, j;
    for (i = 0; i < kTileRows; i++) {
        y = data_ov006_0212e364[i];
        x = 0x10;
        for (j = 0; j < kTileCols; j++) {
            func_ov004_020b0380(data_ov006_021341ec, x, y, 0);
            x += kTileStep;
        }
    }
}

// @symbol func_ov006_020dccb8
extern "C" void func_ov006_020dccb8(dScMgCoin_c *scene)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

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

// @symbol func_ov006_020dcd74
extern "C" void func_ov006_020dcd74(dScMgCoin_c *scene)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

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

// @symbol func_ov006_020dce3c
extern "C" void func_ov006_020dce3c(dScMgCoin_c *scene)
{
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

// @symbol func_ov006_020dcea8
extern "C" void func_ov006_020dcea8(dScMgCoin_c *scene)
{
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

// @symbol func_ov006_020dcffc
extern "C" void func_ov006_020dcffc(void)
{
}

// @symbol func_ov006_020dd000
extern "C" void func_ov006_020dd000(CoinView *scene, int index)
{
    extern void func_ov006_020dc370(dScMgCoin_c *scene);
    extern void func_ov006_020dc960(char *raw, int index);
    extern void func_ov006_020dd4b0(char *raw, int index);

    scene->coin[index].y = scene->coin[index].y + scene->coin[index].vy;
    scene->coin[index].vy = scene->coin[index].vy + kGravity;
    if (scene->coin[index].y < scene->coin[index].landY)
        return;
    scene->coin[index].y = scene->coin[index].landY;
    scene->coin[index].bounces = scene->coin[index].bounces + 1;
    func_ov006_020dc370((dScMgCoin_c *)scene);
    if (scene->coin[index].value == scene->coin[index].bounces) {
        scene->coin[index].state = kCoinSettled;
        func_ov006_020dc960((char *)scene, index);
        return;
    }
    scene->coin[index].vy = kHopVy;
    func_ov006_020dd4b0((char *)scene, index);
}
