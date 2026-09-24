//cpp
/* Wanted. A poster names one character; pictures of the cast drift around
 * the touch screen and the player taps the match. A hit adds time and pops
 * a reward, a miss drops a mark and costs ten seconds, and curtains plus an
 * iris close the round. The timer running out fails it. dScMgLuigi_c is the
 * scene.
 *
 * Keep source order. The local optimization pragmas stay with the functions
 * that need them.
 *
 * Member access changed the code in four places, so those stay raw:
 * UpdatePenalties (a pointer at mPenalty is 0x24 bytes short),
 * TickPictureFlash (one spelling of the blink timer collapses the two
 * exits), RestartPicture (the three slot bytes no longer share a base),
 * and the grid cell in PlaceNextPicture. InitResources and ChooseTarget
 * still copy tiles through a volatile u16; a plain local is shorter.
 *
 * The measurements behind the source forms of MovePictureSway,
 * MovePictureDriftRandom, PlaceBoardLayout and PlaceNextPicture (for one,
 * opt_propagation off takes PlaceNextPicture from 219 to 330 aligned
 * instructions) are in notes/experiments/pr2876-source-repair-0920.json.
 */

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "dScMgLuigi_c.h"
#include "Sound.h"
#include "IRQ.h"

typedef void (dScMgLuigi_c::*LuigiSlotHandler)(int);
typedef void (dScMgLuigi_c::*LuigiStateHandler)();

/* Same spelling the matched IRQ::EnableIRQs / IRQ::DisableIRQs shards use. */
#define IME (*(volatile u16 *)0x04000208)

/* Scanline window record pushed to the iris hardware. */
struct Record_efdac { char _pad[0x300]; };

/* One row of the iris window table: left and right edges. */
struct Px_efdf0 {
    u8 a, b, c2, d2;
};

#define BEB68_f0274 ((char *)data_ov004_020beb68)

/* C-linkage engine calls and the remaining local helpers. */
extern "C" {
extern int func_02053c10(int);
/* Functions the members call need C linkage at file scope. Data names do
   not mangle, so the data_* declarations stay next to their users. */
extern int RandomIntInternal(int *seed);
extern s32 GetGameLanguage(void);
extern void func_ov004_020af948(void *a, int b, int c, void *matrix);
extern char *func_ov004_020afb20(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern void DrawOamSprite(int p, int a, int b, int c);
extern void func_ov004_020b2220(int a, int b, int c, int d, int e, int f, int g);
extern unsigned int func_02012790(unsigned int x);
extern void func_020127a4(int a0, int a1, int a2, int a3);
extern void Hud_RenderSprite(void *sprite, int b, int c, int d, int e);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void FreeGfxSlotsById(int arg);
extern void func_ov004_020b0a54(int c);

extern void func_ov006_020efcf8(void);
extern void func_ov006_020efdac(void);
extern int *dScMgLuigi_c_classInit(void);
}
namespace GXS {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace G2S {
unsigned GetBG2CharPtr();
char *GetBG3ScrPtr();
}
namespace G2 {
char *GetBG2CharPtr();
char *GetBG2ScrPtr();
char *GetBG3ScrPtr();
}
namespace CP15 { void FlushAndInvalidateDataCache(u32 addr, u32 size); }

namespace cstd { int sqrt(unsigned long long x); }

// @symbol _ZN12dScMgLuigi_cD1Ev
// @symbol _ZN12dScMgLuigi_cD0Ev
/* One definition, so D1 and D0 come out in that order under defer_codegen
   off. The extra D2 is not in the ROM; the manifest lets it be deadstripped. */
dScMgLuigi_c::~dScMgLuigi_c()
{
}

// @symbol _ZN12dScMgLuigi_c21AfterCleanupResourcesEj
/* The base cleanup returns void; call it, don't return it. */
void dScMgLuigi_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2 && IRQ::GetIRQHandler(2) == func_ov006_020efcf8) {
        u16 ime;
        do {
            ime = IME;
            IME = 0;
        } while (ime != 0);
        IRQ::DisableIRQs(2);
        func_02053c10(0);
        IRQ::SetIRQHandler(2, 0);
        ime = IME;
        IME = 1;
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol func_ov006_020efcf8
/* IRQ 2's handler uses the void(*)() callback installed by StartIris.
 * It accesses shared scanline state and remains a free function. */
extern "C" {
void func_ov006_020efcf8(void)
{
    extern char data_023c0000[];
    extern int data_0209f608;
    extern int data_0209f648[][192];
    extern void MultiCopy_Int(int *dst, int *src, int len);
    int line;
    /* The (int) cast is what keeps this handler the same size. It is the
       only one of 24 launders this TU inherited from its shards that pays;
       do not re-add the other 23. */
    *(int *)(((int)data_023c0000 + 0x3ff8)) |= 2;
    line = REG_VCOUNT + 1;
    if (line >= 0xc0) {
        func_ov006_020efdac();
    } else if (line <= 0xc0) {
        if (*(volatile u16 *)0x04000004 & 2) {
            MultiCopy_Int(&data_0209f648[data_0209f608][line], (int *)0x04000040, 4);
        }
    }
}
}

// @symbol func_ov006_020efdac
extern "C" {
/* Latches the pending scanline-table index and pushes that 0x300-byte record
   at 0x04000040. */
void func_ov006_020efdac(void)
{
    extern u32 data_0209f60c;
    extern volatile u32 data_0209f608;
    extern struct Record_efdac data_0209f648[];
    extern void MultiCopy_Int(struct Record_efdac *src, void *dst, u32 count);
    data_0209f608 = data_0209f60c;
    MultiCopy_Int(&data_0209f648[data_0209f608], (void *)0x04000040, 4);
}
}

// @symbol _ZN12dScMgLuigi_c14BuildIrisTableEi
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::BuildIrisTable(int i)
{
    extern int data_0209f608;
    extern struct Px_efdf0 data_0209f648[][0xc0];
    extern int data_0209f60c;
    int toggle;
    int y;
    int k, n, m;
    u8 mirrored;

    if (data_0209f608 == 0)
        toggle = 1;
    else
        toggle = 0;
    y = mIris[i].radius >> 12;

    for (k = 0; k < 0x60; k++) {
        if (0x60 - y <= k) {
            int t = k - 0x60 + y;
            int s = cstd::sqrt((s64)(t * (y * 2 - t)));
            int lo = (mIris[0].x >> 12) - s;
            int hi = s + (mIris[0].x >> 12);
            if (lo < 0)
                lo = 0;
            if (hi >= 0xff)
                hi = 0xff;
            data_0209f648[toggle][k].b = lo;
            data_0209f648[toggle][k].a = hi;
        } else {
            data_0209f648[toggle][k].a = 0;
            data_0209f648[toggle][k].b = 0;
        }
    }
    for (n = 0x5f, m = 0; m < 0x60; m++, n--) {
        mirrored = data_0209f648[toggle][n].b;
        data_0209f648[toggle][m + 0x60].b = mirrored;
        data_0209f648[toggle][m + 0x60].a = data_0209f648[toggle][n].a;
    }
    CP15::FlushAndInvalidateDataCache((u32)data_0209f648, 0x600);
    data_0209f60c = toggle;
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c8IrisStopEi
extern "C" {
extern unsigned char data_0209d454;
}
void dScMgLuigi_c::IrisStop(int idx) {
    volatile unsigned short *ime = &IME;
    unsigned short saved;
    mIris[idx].active = 0;
    saved = *ime;
    *ime = 0;
    IRQ::DisableIRQs(2);
    func_02053c10(0);
    IRQ::SetIRQHandler(2, 0);
    if (saved != 0) { *ime; *ime = 1; }
    REG_DISPCNT &= ~0xe000;
    REG_DISPCNT_SUB &= ~0xe000;
    data_0209d454 |= 4;
}

// @symbol _ZN12dScMgLuigi_c8IrisGrowEi
extern "C" {
extern unsigned char data_0209d45c;
}

void dScMgLuigi_c::IrisGrow(int idx) {
    mIris[idx].radius += 0x2000;
    BuildIrisTable(idx);
    if ((mIris[idx].radius >> 12) < 0xa0) return;
    mIris[idx].state += 1;
    data_0209d45c &= ~4;
}

// @symbol _ZN12dScMgLuigi_c8IrisHoldEi
void dScMgLuigi_c::IrisHold(int idx) {
    mIris[idx].scale = 0x1000;
}

// @symbol _ZN12dScMgLuigi_c10UpdateIrisEv
void dScMgLuigi_c::UpdateIris()
{
    extern LuigiSlotHandler data_ov006_021421ec[];
    if (mIris[0].active == 0) return;
    int j = mIris[0].state;
    (this->*data_ov006_021421ec[j])(0);
}

// @symbol _ZN12dScMgLuigi_c9StartIrisEv
void dScMgLuigi_c::StartIris()
{
    extern unsigned int data_0209f608;
    extern unsigned char data_0209d460;
    unsigned short saved;
    mIris[0].active = 1;
    mIris[0].x = 0x80000;
    mIris[0].y = 0x60000;
    mIris[0].state = 0;
    mIris[0].radius = 0x40000;
    mIris[0].scale = 0x1000;
    data_0209f608 = 0;
    BuildIrisTable(0);

    data_0209f608 = 1;
    BuildIrisTable(0);

    data_0209f608 = 0;
    saved = IME;
    IME = 0;
    IRQ::SetIRQHandler(2, func_ov006_020efcf8);

    REG_DISPCNT = (REG_DISPCNT & ~0xe000) | 0x2000;
    data_0209d460 = 1;
    { unsigned int v = *(volatile unsigned short *)0x04000048; v = (v & ~0x3f) | 0x18; v = v | 0x20; *(volatile unsigned short *)0x04000048 = v; }
    *(volatile unsigned short *)0x0400004a = (*(volatile unsigned short *)0x0400004a & ~0x3f) | 0x14;
    *(volatile unsigned short *)0x04000040 = 0;
    *(volatile unsigned short *)0x04000044 = 0xc0;
    IRQ::EnableIRQs(2);
    func_02053c10(1);
    if (saved != 0) {
        IME;
        IME = 1;
    }
}

// @symbol _ZN12dScMgLuigi_c10DrawRewardEv
void dScMgLuigi_c::DrawReward()
{
    extern void *data_ov006_0213ce70[];
    extern char data_ov006_02137cd8[];
    if (mReward.shown == 0) return;
    int x = mReward.x >> 0xc;
    int y = mReward.y >> 0xc;
    if (x <= 8) x = 8;
    if (x >= 0xf0) x = 0xf0;
    if (y <= 8) y = 8;
    if (y >= 0xb8) y = 0xb8;
    s32 r = GetGameLanguage();
    func_ov004_020af948(*(void **)((char *)data_ov006_0213ce70[r] + 0x34), x - 8, y, 0);
    func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0x64), x + 8, y, 0);
}

// @symbol _ZN12dScMgLuigi_c12UpdateRewardEv
extern "C" {
extern void func_ov004_020adb1c(int self);
}

void dScMgLuigi_c::UpdateReward()
{
    if (mReward.active == 0)
        return;

    if (mReward.phase == 0) {
        mReward.y += mReward.velY;
        mReward.velY -= 0x100;
        if (mReward.timer != 0) {
            int v;
            mReward.timer--;
            v = mReward.timer;
            if (v < 0)
                mReward.timer = 0;
            return;
        }
        mReward.timer = 0x40;
        mReward.phase += 1;
        return;
    }

    if (mReward.phase == 1) {
        if (mReward.timer != 0) {
            int v;
            mReward.timer--;
            v = mReward.timer;
            if (v < 0)
                mReward.timer = 0;
            return;
        }
        mReward.shown = 0;
        mReward.phase += 1;
        {
            char *g = BEB68_f0274;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 0x270f)
                    *(int *)(g + 0xb4) += 1;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
        }
        func_ov004_020adb1c(BEB68_f0274 != 0 ? *(int *)(BEB68_f0274 + 0xb4) : 0);
        return;
    }

    if (mReward.sparks != 0) {
        mReward.timer += 1;
        if (mReward.timer < 4)
            return;
        Sound::PlayBank2_2D(0x1bc);
        mReward.timer = 0;
        mReward.sparks -= 1;
        *(u16 *)&mTime += 1;
        if (*(u16 *)&mTime >= 0x32) {
            *(u16 *)&mTime = 0x32;
            mTimeTick = 0;
        }
        return;
    }

    mReward.active = 0;
}

// @symbol _ZN12dScMgLuigi_c11StartRewardEi
void dScMgLuigi_c::StartReward(int i) {
    mReward.active = 1;
    mReward.x = mPosX[i];
    mReward.y = mPosY[i];
    mReward.timer = 16;
    mReward.phase = 0;
    mReward.shown = 1;
    mReward.sparks = 5;
    mReward.velY = -3840;
}

// @symbol _ZN12dScMgLuigi_c12DrawCurtainsEv
void dScMgLuigi_c::DrawCurtains()
{
    extern char *data_ov006_021350d8;
    int j;
    for (j = 0; j < 2; j++) {
        if (mCurtain[j].shown != 0) {
            int x = mCurtain[j].x >> 0xc;
            int y = mCurtain[j].y >> 0xc;
            char *e = data_ov006_021350d8;
            for (;;) {
                char *r = func_ov004_020afb20((int)e, x, y, -1, 1, 0x1000, 0);
                if (r != 0) {
                    int v = (int)((unsigned int)(*(int *)(r + 4) << 0x10) >> 0x1c);
                    *(int *)r = (*(int *)r & ~0xc00) | 0x800;
                    *(unsigned short *)(r + 4) = (unsigned short)((*(unsigned short *)(r + 4) & ~0xf000) | (v << 12));
                }
                if (*(unsigned short *)(e + 6) == 0xffff) break;
                e += 8;
            }
        }
    }
}

// @symbol _ZN12dScMgLuigi_c16CurtainStateSlowEi
void dScMgLuigi_c::CurtainStateSlow(int i)
{
    int *b;
    int *a;
    int v;
    mCurtain[i].count = mCurtain[i].count + 1;
    if (mCurtain[i].count == 0x1a)
        Sound::PlayBank2_2D(0x1b9);
    b = &mCurtain[i].velX;
    a = &mCurtain[i].x;
    *a = *a + *b;
    v = *a >> 12;
    if (i == 0) {
        *b = *b - 0x140;
        if (v < 0x80) return;
        if (*b <= 0) return;
        *a = 0x80000;
        mCurtain[i].active = 0;
        mCurtain[i].phase = 0;
        FreeGfxSlotsById(0xd);
    } else {
        *b = *b + 0x140;
        if (v > 0x80) return;
        if (*b >= 0) return;
        *a = 0x80000;
        mCurtain[i].active = 0;
        mCurtain[i].phase = 0;
    }
}

// @symbol _ZN12dScMgLuigi_c16CurtainStateFastEi
/* The tests on i inside the arm that already tested i stay. Folding them
   drops code this compiler still emits. */
void dScMgLuigi_c::CurtainStateFast(int i)
{
    int *pf8 = &mCurtain[i].velX;
    int *pf0 = &mCurtain[i].x;
    int v;

    *pf0 = *pf0 + *pf8;
    v = *pf0 >> 12;

    if (i == 0) {
        *pf8 += 0x140;
        if (v >= 0x80 && *pf8 > 0) {
            *pf0 = 0x80000;
            mCurtain[i].active = 0;
            mCurtain[i].phase = 0;
            if (i == 0)
                Sound::PlayBank2_2D(0x1b9);
            return;
        }
        if (i != 0)
            return;
        mCurtain[i].sound = Sound_PlayIfNotActive(mCurtain[i].sound, 2, 0x1b8, 0);
    } else {
        *pf8 -= 0x140;
        if (v <= 0x80 && *pf8 < 0) {
            *pf0 = 0x80000;
            mCurtain[i].active = 0;
            mCurtain[i].phase = 0;
            if (i != 0)
                return;
            Sound::PlayBank2_2D(0x1b9);
            return;
        }
        if (i != 0)
            return;
        mCurtain[i].sound = Sound_PlayIfNotActive(mCurtain[i].sound, 2, 0x1b8, 0);
    }
}

// @symbol _ZN12dScMgLuigi_c16CurtainStateHoldEi
void dScMgLuigi_c::CurtainStateHold(int i)
{
    int *b;
    int *a;
    int v;
    mCurtain[i].count = mCurtain[i].count + 1;
    if (mCurtain[i].count == 0x35)
        Sound::PlayBank2_2D(0x1b9);
    if (i == 0) {
        if (mCurtain[i].count <= 0x35) {
            int *p = &mCurtain[i].sound;
            *p = Sound_PlayIfNotActive(*p, 2, 0x1b8, 0);
        }
    }
    {
        unsigned short *q = (unsigned short *)&mCurtain[i].delay;
        if (*q != 0) {
            *q = *q - 1;
            if ((short)*q < 0) *q = 0;
            return;
        }
    }
    b = &mCurtain[i].velX;
    a = &mCurtain[i].x;
    *a = *a + *b;
    v = *a >> 12;
    if (i == 0) {
        *b = *b + 0x140;
        if (v > 0x80) return;
        *a = 0x80000;
        mCurtain[i].active = 0;
        mCurtain[i].phase = 0;
        FreeGfxSlotsById(0x1d);
        if (mPromptBlinkCount != 0) return;
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    } else {
        *b = *b - 0x140;
        if (v < 0x80) return;
        *a = 0x80000;
        mCurtain[i].active = 0;
        mCurtain[i].phase = 0;
        FreeGfxSlotsById(0x1d);
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
    }
}

// @symbol _ZN12dScMgLuigi_c18CurtainStateBounceEi
void dScMgLuigi_c::CurtainStateBounce(int i)
{
    int *pf8 = &mCurtain[i].velX;
    int *pf0 = &mCurtain[i].x;
    int v;
    *pf0 = *pf0 + *pf8;
    v = *pf0 >> 12;
    if (i == 0) {
        *pf8 -= 0x100;
        if (v >= 0x140) {
            *pf0 = 0x140000;
            mCurtain[i].phase++;
            mCurtain[i].delay = 0x10;
            *pf8 = -0x6000;
        }
    } else {
        *pf8 += 0x100;
        if (v <= -0x40) {
            *pf0 = -0x40000;
            mCurtain[i].phase++;
            mCurtain[i].delay = 0x10;
            *pf8 = 0x6000;
        }
    }
    if (i == 0) {
        mCurtain[i].sound = Sound_PlayIfNotActive(mCurtain[i].sound, 2, 0x1b8, 0);
    }
}

// @symbol _ZN12dScMgLuigi_c13CurtainUpdateEi
void dScMgLuigi_c::CurtainUpdate(int i) {
    extern LuigiSlotHandler data_ov006_0214221c[];
    (this->*(data_ov006_0214221c[mCurtain[i].phase]))(i);
}

// @symbol _ZN12dScMgLuigi_c12CurtainStartEi
void dScMgLuigi_c::CurtainStart(int i) {
    extern unsigned char data_0209d460;
    extern int data_ov006_0212e850[];
    unsigned short t;
    int x;
    int v;

    t = *(unsigned short *)&mCurtain[i].delay;
    if (t != 0) {
        mCurtain[i].delay = t - 1;
        if (mCurtain[i].delay < 0)
            mCurtain[i].delay = 0;
        return;
    }

    x = *(unsigned short *)0x0400004a;
    x &= ~0x3f00;
    x |= 0x1800;
    x &= ~0x2000;
    x |= 0x2000;
    *(unsigned short *)0x0400004a = x;
    x = (*(unsigned short *)0x0400004a & ~0x3f) | 0x14;
    *(unsigned short *)0x0400004a = x;
    *(volatile int *)0x04000000 = (*(volatile int *)0x04000000 & ~0xe000) | 0x8000;
    data_0209d460 = 4;

    mCurtain[i].x = data_ov006_0212e850[i] << 12;
    mCurtain[i].y = 0x60000;
    mCurtain[i].shown = 1;
    mCurtain[i].state = 1;
    if (i != 0)
        mCurtain[i].velX = -0x8000;
    else
        mCurtain[i].velX = 0x8000;

    v = unk_0bc;
    while (v >= 5)
        v -= 5;
    if (v != 0) {
        mCurtain[i].state = 2;
        if (i != 0)
            mCurtain[i].velX = -0x7000;
        else
            mCurtain[i].velX = 0x7000;
    }
    mCurtain[i].sound = 0;
}

// @symbol _ZN12dScMgLuigi_c12CurtainsDoneEv
int dScMgLuigi_c::CurtainsDone()
{
    extern LuigiSlotHandler data_ov006_02142204[];
    int count = 0;
    int i = 0;
    do {
        if (mCurtain[i].active != 0) {
            (this->*data_ov006_02142204[mCurtain[i].state])(i);
            count++;
        }
        i++;
    } while (i < 2);
    return count == 0;
}

// @symbol _ZN12dScMgLuigi_c13ResetCurtainsEv
void dScMgLuigi_c::ResetCurtains() {
    int i;
    for (i = 0; i < 2; i++) {
        mCurtain[i].active = 1;
        mCurtain[i].state = 0;
        mCurtain[i].delay = 0;
        mCurtain[i].count = 0;
        mCurtain[i].shown = 0;
        mCurtain[i].phase = 0;
    }
}

// @symbol _ZN12dScMgLuigi_c9DrawTimerEv
void dScMgLuigi_c::DrawTimer() {
    extern int *data_ov006_0213ce70[];
    int idx;
    if (mState < 2)
        return;
    idx = GetGameLanguage();
    DrawOamSprite(*(int *)((char *)data_ov006_0213ce70[idx] + 0xc), 0x80, 0x10, 0);
    func_ov004_020b2220(0x80, 0x28, static_cast<u16>(mTime), 1, -1, 0x800, 0);
}

// @symbol _ZN12dScMgLuigi_c9TickTimerEv
void dScMgLuigi_c::TickTimer()
{
    if (*(u16 *)&mTime != 0) {
        {
            u8 *q = &mTimeTick;
            *q = *q + 1;
        }
        if (mTimeTick < 0x3c)
            return;
        mTimeTick = 0;
        {
            u16 *p = (u16 *)&mTime;
            *p = *p - 1;
        }
        if (mTime <= 0)
            mTime = 0;
        if (*(u16 *)&mTime <= 2) {
            Sound::PlayBank2_2D(0xa6);
            return;
        }
        Sound::PlayBank2_2D(0xa7);
        return;
    }
    Sound::PlayBank2_2D((unsigned int)((mWanted << 3) + 0xc0));
    func_02012790(0xe);
    BeginCatch(0);
}

// @symbol _ZN12dScMgLuigi_c12DrawPicturesEv
void dScMgLuigi_c::DrawPictures() {
    extern void *data_ov006_0213abc8[];
    int i;
    if (mState == 0)
        return;
    for (i=0;i<0x78;i++) {
        if (mShown[i] == 1) {
            Hud_RenderSprite(
                data_ov006_0213abc8[mCharacter[i]],
                mPosX[i] >> 12,
                mPosY[i] >> 12,
                -1,
                0);
        }
    }
}

// @symbol _ZN12dScMgLuigi_c13DrawPenaltiesEv
void dScMgLuigi_c::DrawPenalties()
{
    extern void *data_ov006_0213ce70[];
    extern char data_ov006_02137cd8[];
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mPenalty[i].shown != 0) {
            int x = mPenalty[i].x >> 0xc;
            int y = mPenalty[i].y >> 0xc;
            s32 r;
            if (x <= 0x10) x = 0x10;
            if (x >= 0xe0) x = 0xe0;
            if (y <= 8) y = 8;
            if (y >= 0xb8) y = 0xb8;
            r = GetGameLanguage();
            func_ov004_020af948(*(void **)((char *)data_ov006_0213ce70[r] + 0x38), x - 0x10, y, 0);
            func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0xa4), x, y, 0);
            func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0xa0), x + 0x10, y, 0);
        }
    }
}

// @symbol _ZN12dScMgLuigi_c15UpdatePenaltiesEv
#pragma push
#pragma opt_common_subs off
/* Walks from this. A pointer at mPenalty is 0x24 bytes short of this body. */
void dScMgLuigi_c::UpdatePenalties()
{
    char *q = (char *)this;
    int i;
    for (i = 0; i < 0x10; i++, q += 0x14) {
        if (*(u8 *)(q + 0x4670) != 0) {
            if (*(u8 *)(q + 0x4672) == 0) {
                if (*(u16 *)(q + 0x466c) != 0) {
                    *(u16 *)(q + 0x466c) -= 1;
                    if (*(short *)(q + 0x466c) < 0)
                        *(u16 *)(q + 0x466c) = 0;
                    *(int *)(q + 0x4664) += *(int *)(q + 0x4668);
                    *(int *)(q + 0x4668) += 0x100;
                } else {
                    *(u16 *)(q + 0x466c) = 0x40;
                    *(u8 *)(q + 0x4672) += 1;
                }
            } else {
                if (*(u16 *)(q + 0x466c) != 0) {
                    *(u16 *)(q + 0x466c) -= 1;
                    if (*(short *)(q + 0x466c) < 0)
                        *(u16 *)(q + 0x466c) = 0;
                } else {
                    *(u16 *)(q + 0x466c) = 0;
                    *(u8 *)(q + 0x4670) = 0;
                    *(u8 *)(q + 0x4671) = 0;
                }
            }
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c10AddPenaltyEi
/* First free miss mark, at the picture that was tapped, and ten seconds off. */
void dScMgLuigi_c::AddPenalty(int idx)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (mPenalty[i].active != 0)
            continue;
        mPenalty[i].active = 1;
        mPenalty[i].shown = 1;
        mPenalty[i].timer = 0x10;
        mPenalty[i].x = mPosX[idx];
        mPenalty[i].y = mPosY[idx];
        mPenalty[i].velY = 0x1100;
        mPenalty[i].phase = 0;
        *(u16 *)&mTime -= 0xa;
        if (mTime < 0)
            mTime = 0;
        return;
    }
}

// @symbol _ZN12dScMgLuigi_c14DrawWantedIconEv
void dScMgLuigi_c::DrawWantedIcon() {
    extern void *data_ov006_0213abc8[];
    if (mIconOn == 0) return;
    func_ov004_020af948(data_ov006_0213abc8[mWanted],
                        (unsigned short)mIconX,
                        (unsigned short)mIconY, 0);
}

// @symbol _ZN12dScMgLuigi_c16TickPictureFlashEi
#pragma push
#pragma opt_common_subs off
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::TickPictureFlash(int idx)
{
    char *raw = (char *)this;
    char *base = raw + 0x506c;
    int twice = idx * 2;
    unsigned short h = *(unsigned short *)(base + twice);
    unsigned char *q;

    *(short *)(base + twice) = (short)(h - 1);

    if (*(short *)(raw + (idx << 1) + 0x506c) < 0)
        *(short *)(raw + (idx << 1) + 0x506c) = 0;

    /* Two exits, not one if/else: folding them changes the code. */
    if ((((unsigned short)*(unsigned short *)(raw + (idx << 1) + 0x506c) >> 2) & 1) != 0) {
        char *p = raw + 0x53dd;
        p[idx] = 0;
        q = (unsigned char *)(p + idx);
        goto after_flag;
    }
    {
        char *p = raw + 0x53dd;
        p[idx] = 1;
        q = (unsigned char *)(p + idx);
    }
after_flag:
    if (*(unsigned short *)(raw + (idx << 1) + 0x506c) != 0)
        return;
    *(unsigned char *)(raw + idx + 0x51fd) = 0;
    *q = 1;
}
#pragma pop
#pragma pop

// @symbol _ZN12dScMgLuigi_c17MovePictureBounceEi
#pragma push
#pragma opt_common_subs off
void dScMgLuigi_c::MovePictureBounce(int i)
{
    extern int data_0209d4b8;
    extern int data_ov006_0212e8b8[];
    extern s16 data_02082214[];
    u8 *started = &mStarted[i];

    if (*started == 0) {
        u32 r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        u16 val = (u16)((((r << 4) >> 15) << 12));

        mMovePhase[i] = val;
        *started += 1;
        return;
    }

    {
        u8 *speedLevel = mCharacter;
        int *posX = mPosX;
        int *posY = mPosY;
        u16 phase;
        int a;
        int stepX, stepY;
        int speed;
        s64 prod;
        int round12 = 0x800;

        phase = mMovePhase[i];
        a = phase >> 4;
        stepX = data_02082214[a * 2 + 1];
        speed = data_ov006_0212e8b8[speedLevel[i]];
        prod = (s64)stepX * speed;
        posX[i] += (int)((prod + round12) >> 12);

        phase = mMovePhase[i];
        a = phase >> 4;
        stepY = data_02082214[a * 2];
        speed = data_ov006_0212e8b8[speedLevel[i]];
        prod = (s64)stepY * speed;
        posY[i] += (int)((prod + round12) >> 12);

        {
            int x = mPosX[i] >> 0xc;
            int y = mPosY[i] >> 0xc;

            if (x > 0xf8) {
                mMovePhase[i] = 0x8000 - mMovePhase[i];
                mPosX[i] = 0xf8000;
            } else if (x < 8) {
                mMovePhase[i] = 0x8000 - mMovePhase[i];
                mPosX[i] = 0x8000;
            }

            if (y > 0xb8) {
                mMovePhase[i] = -mMovePhase[i];
                mPosY[i] = 0xb8000;
                return;
            }

            if (y < 8) {
                mMovePhase[i] = -mMovePhase[i];
                mPosY[i] = 0x8000;
                return;
            }

            return;
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c15MovePictureSwayEi
/* Sway. A new slot takes phase 0 and a signed x speed; a running slot
   coasts, flips at zero, and wraps at the screen edge. */
void dScMgLuigi_c::MovePictureSway(int idx)
{
    extern int data_ov006_0212e888[];
    extern int data_ov006_0212e898[];
    extern int data_ov006_0212e8a8[];
    if (mStarted[idx] == 0) {
        mMovePhase[idx] = 0;
        mStarted[idx]++;
        if (mCharacter[idx] == 0) {
            mVelX[idx] = data_ov006_0212e888[mCharacter[idx]];
        } else {
            mVelX[idx] = -data_ov006_0212e888[mCharacter[idx]];
        }
        mVelY[idx] = data_ov006_0212e898[mCharacter[idx]];
        return;
    }
    mPosX[idx] += mVelX[idx];
    mPosY[idx] += mVelY[idx];
    if (mMovePhase[idx] == 0) {
        if (mVelX[idx] > 0) {
            mVelX[idx] -= data_ov006_0212e8a8[mCharacter[idx]];
            if (mVelX[idx] <= 0) {
                mVelX[idx] = 0;
                mMovePhase[idx] = 2;
            }
        } else if (mVelX[idx] < 0) {
            mVelX[idx] += data_ov006_0212e8a8[mCharacter[idx]];
            if (mVelX[idx] >= 0) {
                mVelX[idx] = 0;
                mMovePhase[idx] = 1;
            }
        }
    } else if (mMovePhase[idx] == 1) {
        mVelX[idx] += data_ov006_0212e8a8[mCharacter[idx]];
        if (mVelX[idx] >= data_ov006_0212e888[mCharacter[idx]]) {
            mMovePhase[idx] = 0;
        }
    } else {
        mVelX[idx] -= data_ov006_0212e8a8[mCharacter[idx]];
        if (mVelX[idx] <= -data_ov006_0212e888[mCharacter[idx]]) {
            mMovePhase[idx] = 0;
        }
    }
    WrapPicture(idx);
}

// @symbol _ZN12dScMgLuigi_c22MovePictureDriftRandomEi
/* Drift on a random angle, one of eight steps of 0x1000. */
void dScMgLuigi_c::MovePictureDriftRandom(int idx)
{
    extern int data_0209d4b8;
    extern int data_ov006_0212e878[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        unsigned int r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        unsigned short val = (unsigned short)(((r << 4) >> 15) << 12);
        mMovePhase[idx] = val;
        mStarted[idx]++;
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e878[mCharacter[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e878[mCharacter[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

// @symbol _ZN12dScMgLuigi_c21MovePictureDriftByRowEi
/* Drift up or down from mRowDir of the slot's row. */
void dScMgLuigi_c::MovePictureDriftByRow(int idx)
{
    extern int data_ov006_0212e868[];
    extern s16 data_02082214[];
    int cnt;
    int j;
    if (mStarted[idx] == 0) {
        mStarted[idx]++;
        cnt = 0;
        j = idx;
        if (idx >= 8) {
            do {
                j -= 8;
                cnt++;
            } while (j >= 8);
        }
        if (mRowDir[cnt] != 0) {
            mMovePhase[idx] = 0x8000;
        } else {
            mMovePhase[idx] = 0;
        }
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e868[mCharacter[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e868[mCharacter[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

// @symbol _ZN12dScMgLuigi_c24MovePictureDriftByColumnEi
/* Drift left or right from mColDir of the slot's column. */
void dScMgLuigi_c::MovePictureDriftByColumn(int idx)
{
    extern int data_ov006_0212e858[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        mStarted[idx]++;
        if (mColDir[idx & 7] != 0) {
            mMovePhase[idx] = 0x4000;
        } else {
            mMovePhase[idx] = 0xc000;
        }
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e858[mCharacter[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e858[mCharacter[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

// @symbol _ZN12dScMgLuigi_c23MovePictureDriftByLevelEi
/* Drift on the angle stored for this picture's speed. */
void dScMgLuigi_c::MovePictureDriftByLevel(int idx)
{
    extern int data_ov006_0212e8d8[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        u8 t = mCharacter[idx];
        mMovePhase[idx] = mLevelPhase[t];
        mStarted[idx]++;
    } else {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e8d8[mCharacter[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e8d8[mCharacter[idx]] + 0x800) >> 12);
        WrapPicture(idx);
    }
}

// @symbol _ZN12dScMgLuigi_c21MovePictureDriftFixedEi
/* Drift on a fixed angle of 0x6000. */
void dScMgLuigi_c::MovePictureDriftFixed(int idx)
{
    extern int data_ov006_0212e8c8[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        mMovePhase[idx] = 0x6000;
        mStarted[idx]++;
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e8c8[mCharacter[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e8c8[mCharacter[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

// @symbol _ZN12dScMgLuigi_c11WrapPictureEi
/* Wraps one picture's 20.12 position round the 0x110 x 0xd0 screen. Leaf. */
void dScMgLuigi_c::WrapPicture(int i) {
    int x = mPosX[i] >> 12;
    int y = mPosY[i] >> 12;
    if (x > 0x110) mPosX[i] = -0x10000;
    if (x < -0x10) mPosX[i] = 0x110000;
    if (y > 0xd0)  mPosY[i] = -0x10000;
    if (y < -0x10) mPosY[i] = 0xd0000;
}

// @symbol _ZN12dScMgLuigi_c11StopPictureEi
void dScMgLuigi_c::StopPicture(int idx) {
    mVelX[idx] = 0;
    mVelY[idx] = 0;
}

// @symbol _ZN12dScMgLuigi_c14RestartPictureEi
void dScMgLuigi_c::RestartPicture(int idx) {
    unsigned char *self = (unsigned char *)this;
    extern unsigned char data_ov006_0213ceac[];
    unsigned short board = mBoard;
    self += idx;
    self[0x53dd] = 1;
    self[0x5275] = 0;
    self[0x51fd] = data_ov006_0213ceac[board] + 1;
}

// @symbol _ZN12dScMgLuigi_c14UpdatePicturesEv
void dScMgLuigi_c::UpdatePictures() {
    extern LuigiSlotHandler data_ov006_02142254[];
    int i;
    for (i = 0; i < 0x78; i++) {
        if (mActive[i] != 0) {
            unsigned char k = mKind[i];
            (this->*data_ov006_02142254[k])(i);
        }
    }
}

// @symbol _ZN12dScMgLuigi_c10BeginCatchEi
void dScMgLuigi_c::BeginCatch(int p1)
{
    int v;
    mIconX = (short)(mPosX[mTarget - 1] >> 0xc);
    mIconY = (short)(mPosY[mTarget - 1] >> 0xc);
    mIconOn = 0x60;
    mState = 3;
    mCatchWait = 0xc8;
    v = unk_0bc;
    while (v >= 5) v -= 5;
    if (v != 4)
        *(unsigned short *)&mCatchWait += 8;
    if (p1 == 0)
        mCatchWait = 0x80;
    mFound = (unsigned char)p1;
    StartIris();
    mIris[0].state = 1;
}

// @symbol _ZN12dScMgLuigi_c10CheckTouchEv
void dScMgLuigi_c::CheckTouch()
{
    extern unsigned char data_020a0e40;
    extern unsigned char data_020a0de8[][4];
    extern unsigned char data_020a0de9[][4];
    extern unsigned char data_020a0dea[][4];
    extern unsigned char data_020a0deb[][4];
    extern unsigned short data_ov006_0212e848[];
    int idx = data_020a0e40;
    int flag = 0;
    int cur;
    int i;

    if (data_020a0de8[idx][0] != 0) {
        if (data_020a0de9[idx][0] != 0) flag = 1;
    }
    if (flag == 0) return;

    cur = mTarget;
    {
        int dx = data_020a0dea[idx][0] - (mPosX[cur - 1] >> 12);
        int dy = data_020a0deb[idx][0] - (mPosY[cur - 1] >> 12);
        if (dx <= 0x10 && dx >= -0x10 && dy <= 0x10 && dy >= -0x10) {
            int lvl, cat;

            BeginCatch(1);
            cur = mTarget;
            StartReward(cur - 1);

            lvl = unk_0bc;
            cat = 0;
            if (lvl >= 0xa) cat = 2;
            else if (lvl >= 5) cat = 1;
            func_02012790(0x26);

            {
                int tbl = data_ov006_0212e848[cat];
                func_02012790(tbl + (mWanted << 3));
            }
            return;
        }
    }

    for (i = 0; i < 0x78; i++) {
        if (mActive[i] == 1) {
            if (mKind[i] != 9) {
                int dx2 = data_020a0de8[idx][2] - (mPosX[i] >> 12);
                int dy2 = data_020a0de8[idx][3] - (mPosY[i] >> 12);
                if (dx2 <= 0x10 && dx2 >= -0x10 && dy2 <= 0x10 && dy2 >= -0x10) {
                    AddPenalty(i);
                    mFound = 0;
                    mKind[i] = 9;
                    mBlink[i] = 0x40;
                    if (*(u16 *)&mTime != 0) {
                        int cur2 = mTarget;
                        int t = (mPosX[cur2 - 1] >> 12) - 0x80;
                        int pan = (t * 0x30) >> 7;
                        if (pan >= 0x30) pan = 0x30;
                        if (pan <= -0x30) pan = -0x30;
                        func_020127a4(2, (mWanted << 3) + 0xbf, 0xffff, pan);
                        func_02012790(0xe);
                    }
                    return;
                }
            }
        }
    }
}

// @symbol _ZN12dScMgLuigi_c16PlaceBoardLayoutEv
/* One of four fixed boards, placed in a single call. The rnd temporary and
   the per-arm i/n/j locals are the spelling that matches; the tail loops
   keep the random call inline. */
void dScMgLuigi_c::PlaceBoardLayout()
{
    extern int data_0209d4b8;
    extern u8 data_ov006_0213ce98[];
    int mode;
    int rnd;
    int i;

    mode = data_ov006_0213ce98[mBoard];
    if (mode == 1) {
        int i;
        int n;
        int j;
        int base;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        base = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
        rnd = RandomIntInternal(&data_0209d4b8);
        mTarget = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                mPosX[n] = (0x70 + j * 0x20) << 12;
                mPosY[n] = (0x50 + i * 0x20) << 12;
                mActive[n] = 1;
                mKind[n] = 0;
                mCharacter[n] = (base + n) & 3;
                if (mWanted == mCharacter[n])
                    mTarget = n + 1;
                n++;
            }
        }
    } else if (mode == 2) {
        int i;
        int n;
        int j;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        mTarget = ((((u32)rnd >> 16) & 0x7fff) * 16 >> 15) + 1;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                u8 step;
                mPosX[n] = (0x50 + j * 0x20) << 12;
                mPosY[n] = (0x30 + i * 0x20) << 12;
                mActive[n] = 1;
                mKind[n] = 0;
                rnd = RandomIntInternal(&data_0209d4b8);
                mCharacter[n] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
                if (mWanted == mCharacter[n]) {
                    rnd = RandomIntInternal(&data_0209d4b8);
                    step = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
                    mCharacter[n] += step + 1;
                    mCharacter[n] &= 3;
                }
                if (n == mTarget - 1)
                    mCharacter[n] = mWanted;
                n++;
            }
        }
    } else if (mode == 3) {
        int i;
        int n;
        int j;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        mTarget = ((((u32)rnd >> 16) & 0x7fff) * 0x30 >> 15) + 1;
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 8; j++) {
                u8 step;
                mPosX[n] = (0x10 + j * 0x20) << 12;
                mPosY[n] = (0x10 + i * 0x20) << 12;
                mActive[n] = 1;
                mKind[n] = 0;
                rnd = RandomIntInternal(&data_0209d4b8);
                mCharacter[n] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
                if (mWanted == mCharacter[n]) {
                    rnd = RandomIntInternal(&data_0209d4b8);
                    step = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
                    mCharacter[n] += step + 1;
                    mCharacter[n] &= 3;
                }
                if (n == mTarget - 1)
                    mCharacter[n] = mWanted;
                n++;
            }
        }
    } else {
        int n;
        int i;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        mTarget = ((((u32)rnd >> 16) & 0x7fff) * 8 >> 15) + 1;
        for (i = 0; i < 8; i++) {
            u8 step;
            mPosX[n] = (0x10 + i * 0x20) << 12;
            mPosY[n] = -(5 << 12);
            mActive[n] = 1;
            mKind[n] = 0;
            rnd = RandomIntInternal(&data_0209d4b8);
            mCharacter[n] = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
            if (mWanted == mCharacter[n]) {
                rnd = RandomIntInternal(&data_0209d4b8);
                step = ((((u32)rnd >> 16) & 0x7fff) * 2 >> 15) + 1;
                mCharacter[n] += step;
                if (mCharacter[n] >= 3)
                    mCharacter[n] = 0;
            }
            if (n == mTarget - 1)
                mCharacter[n] = mWanted;
            n++;
        }
    }

    for (i = 0; i < 8; i++)
        mColDir[i] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15;
    for (i = 0; i < 6; i++)
        mRowDir[i] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15;
    mFilled++;
}

#pragma push
#pragma opt_propagation off
// @symbol _ZN12dScMgLuigi_c16PlaceNextPictureEv
/* One picture per call, or a whole fixed board. p and f stay as pointers:
   the speed fixup and the second capacity test both re-read through them.
   opt_propagation off keeps this in the right registers. */
void dScMgLuigi_c::PlaceNextPicture()
{
    extern int data_0209d4b8;
    extern u8 data_ov006_0213ce84[];
    extern u8 data_ov006_0213ce98[];
    extern u16 data_ov006_0213cee0[];
    int idx;
    int cur;
    int lim;
    int col;
    int row;
    int tries;
    int rnd;

    if (mFilled != 0)
        return;

    idx = mBoard;
    if (data_ov006_0213ce98[idx] != 0) {
        PlaceBoardLayout();
        return;
    }

    lim = data_ov006_0213cee0[idx];
    cur = mTarget;

    rnd = RandomIntInternal(&data_0209d4b8);
    mCharacter[cur] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
    {
        u8 *p = &mCharacter[cur];
        if (mWanted == mCharacter[cur]) {
            rnd = RandomIntInternal(&data_0209d4b8);
            *p += ((((u32)rnd >> 16) & 0x7fff) * 3 >> 15) + 1;
            *p &= 3;
        }
    }

    if (cur == 0) {
        int jx;
        int jy;
        do {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        } while (col >= 6 && col <= 10 && row >= 4 && row <= 8);

        jy = -2;
        jx = jy;
        jx += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
        jy += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;

        if (data_ov006_0213ce84[idx] != 0) {
            mPosX[0] = (col * 20 + (jx + 8)) << 12;
            mPosY[0] = (row * 20 + (jy + 16)) << 12;
            mActive[0] = 1;
            mKind[0] = 0;
            mCharacter[0] = mWanted;
            mGrid[col][row] = 1;
            mTarget++;
            return;
        } else {
            int n = lim - 1;
            mPosX[n] = (col * 20 + 8) << 12;
            mPosY[n] = (row * 20 + 16) << 12;
            mActive[n] = 1;
            mKind[n] = 0;
            mCharacter[n] = mWanted;
            mGrid[col][row] = 1;
        }
    }

    tries = 0;
    while (1) {
        u8 *cell;
        if (tries >= 100) {
            col++;
            if (col >= 12) {
                row++;
                col = 0;
                if (row >= 11)
                    row = 0;
            }
        } else {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        }
        cell = (u8 *)((unsigned int)((char *)this + col * 9 + row) + 0x5178);
        if (*cell == 0) {
            int jx;
            int jy;
            jy = -2;
            jx = jy;
            jx += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            jy += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            mPosX[cur] = (col * 20 + (jx + 8)) << 12;
            mPosY[cur] = (row * 20 + (jy + 16)) << 12;
            *cell = 1;
            break;
        }
        tries++;
    }

    mActive[cur] = 1;
    mKind[cur] = 0;
    mTarget++;

    const u8 *f = &data_ov006_0213ce84[idx];
    if (mTarget >= (data_ov006_0213ce84[idx] != 0 ? lim : lim - 1)) {
        if (idx == 0xe || idx == 0x10 || idx == 0x11 || idx == 0x13) {
            mPosX[cur] = mPosX[lim - 1];
            mPosY[cur] = mPosY[lim - 1] - 0x14000;
        }
        mFilled = 1;
        if (*f != 0)
            mTarget = 1;
        else
            mTarget = lim;
    }

    if (mFilled == 0)
        return;

    mLevelPhase[0] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    mLevelPhase[1] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    mLevelPhase[2] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    mLevelPhase[3] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c12ChooseTargetEv
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::ChooseTarget()
{
    extern int data_0209d4b8;
    extern u8 data_ov006_0213cec0[];
    extern u16 data_ov006_0213cdec[];
    s32 i;
    s32 k;
    u16 *dst;
    u16 idx;
    u16 t;
    s32 j;
    s32 off;
    s32 nbytes;
    /* A plain u16 here is 8 bytes short of the tile copy. */
    volatile u16 v;

    t = mTime;
    k = 0;
    if (t >= 0x14) {
        k = 0x14;
    } else if (t >= 0xa) {
        k = 0xa;
    }

    mWanted = (data_ov006_0213cec0 + k)[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 15];

    t = mBoard;
    if (t == 9) {
        mWanted = 1;
    }
    if (t == 0xe || t == 0x10 || t == 0x11 || t == 0x13) {
        if (mWanted == 3) {
            mWanted = (u8)((((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3)) >> 15);
        }
    }

    nbytes = 2;
    for (i = 0, off = 0; i < 8; i++, off += 0x20) {
        dst = (u16 *)(G2::GetBG3ScrPtr() + 0x1d8) + off;
        idx = data_ov006_0213cdec[mWanted];
        idx += off;
        for (j = 0; j < 8; j++) {
            v = ((u16 *)G2::GetBG3ScrPtr())[idx];
            MultiStore16(v, (char *)dst, nbytes);
            idx++;
            dst++;
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c11ChooseBoardEv
void dScMgLuigi_c::ChooseBoard()
{
    extern int data_0209d4b8;
    int lvl;
    mPrevBoard = mBoard;
    lvl = unk_0bc;
    if (lvl >= 0x14) {
        int pick = (int)(((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 0xf);
        if (pick + 0xa == mPrevBoard) {
            int step = (int)((((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9) >> 0xf)) + 1;
            pick += step;
            if (pick >= 0xa) pick -= 0xa;
        }
        lvl = pick + 0xa;
    }
    mBoard = lvl;
}

// @symbol _ZN12dScMgLuigi_c10ResetBoardEv
void dScMgLuigi_c::ResetBoard()
{
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 n;

    for (i = 0; i < 120; i++)
    {
        mPosX[i] = 0;
        mPosY[i] = 0;
        mVelX[i] = 0;
        mVelY[i] = 0;
        mKind[i] = 0;
        mActive[i] = 0;
        mCharacter[i] = 0;
        mBlink[i] = 0;
        mMovePhase[i] = 0;
        mShown[i] = 0;
    }

    for (j = 0; j < 16; j++)
    {
        mPenalty[j].x = 0;
        mPenalty[j].y = 0;
        mPenalty[j].timer = 0;
        mPenalty[j].active = 0;
        mPenalty[j].shown = 0;
    }

    for (k = 0; k < 2; k++)
    {
        mCurtain[k].active = 0;
        mCurtain[k].shown = 0;
    }

    mReward.active = 0;
    mReward.shown = 0;
    mIris[0].active = 0;
    mIris[0].x = 0;
    mIris[0].y = 0;
    mIris[0].state = 0;
    mIris[0].radius = 0;

    for (m = 0; m < 13; m++)
    {
        for (n = 0; n < 9; n++)
        {
            mGrid[m][n] = 0;
        }
    }

    mIconOn = 0;
    mIconX = 0x80;
    mIconY = 0x60;
    mCatchWait = 0;

    mTarget = 0;
    mBoardPending = 1;
    mFound = 0;
    mFilled = 0;
}

// @symbol _ZN12dScMgLuigi_c10StateCatchEv
#pragma push
#pragma opt_strength_reduction off

void dScMgLuigi_c::StateCatch()
{
    extern LuigiSlotHandler data_ov006_02142254[];
    int matches;
    int i;

    UpdateIris();
    UpdatePenalties();
    UpdateReward();

    matches = 0;
    for (i = 0; i < 0x78; i++) {
        if (mActive[i] != 0) {
            int x = mKind[i];
            if (x == 9) {
                matches++;
                (this->*data_ov006_02142254[x])(i);
            }
        }
    }
    if (matches != 0)
        return;

    for (i = 0; i < 0x78; i++) {
        if (mShown[i] == 1)
            mShown[i] = 0;
    }

    if (*(unsigned short *)&mCatchWait == 0)
        return;
    *(unsigned short *)&mIconOn = 1;
    *(unsigned short *)&mCatchWait -= 1;
    if (mCatchWait > 0)
        return;
    *(unsigned short *)&mCatchWait = 0;
    *(unsigned short *)&mIconOn = 0;

    if (mFound != 0) {
        int v = unk_0bc;
        while (v >= 5)
            v -= 5;
        if (v != 4)
            OnYoshiTryEat(-1);
        else
            func_ov004_020b0a54(1);
    } else {
        func_ov004_020b0a54(0x12);
    }
    mPromptEnabled = 0;
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c9StatePlayEv
void dScMgLuigi_c::StatePlay() {
    TickTimer();
    if (mState == 3) return;
    UpdatePictures();
    CheckTouch();
    UpdatePenalties();
}

// @symbol _ZN12dScMgLuigi_c10StatePlaceEv
void dScMgLuigi_c::StatePlace() {
    int i;
    for (i = 0; i < 8; i++)
        PlaceNextPicture();
    if (CurtainsDone() == 0)
        return;
    if (mFilled == 0)
        return;
    mState = 2;
    mBoardPending = 0;
    unk_516c = 0xa;
    unk_516e = 0;
}

// @symbol _ZN12dScMgLuigi_c10StateSetupEv
void dScMgLuigi_c::StateSetup()
{
    mIconOn = 0;
    ResetCurtains();
    mState = 1;
    ChooseTarget();
}

/* Engine calls used by scene setup and rendering. */

extern "C" {
/* dScMgLuigi_c::OnYoshiTryEat */
extern void FreeGfxSlotsById(int arg);
extern void func_ov004_020adb1c(int);
extern void func_ov004_020b0cac(int, int, int, int, int, short);
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
/* dScMgLuigi_c::Render */
extern void func_ov004_020b1e34(void *c, int a, int b, int d);
/* dScMgLuigi_c::Behavior */
extern LuigiStateHandler data_ov006_02142234[];
/* dScMgLuigi_c::InitResources */
extern void *data_ov006_0213cfa0;
extern void *func_ov004_020adc74(const char *path);
extern void DecompressLZ16(void *src, void *dst);
extern int LoadFile(int handle);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void Ov004_Deallocate(void *ptr);
extern int func_02054d88(void);
extern void func_02056314(void *, u32, u32);
extern void func_02056374(const void *, u32, u32);
extern void Deallocate(void *);
extern void func_ov004_020b04d0(int);
extern int data_ov004_020bc888;
extern int data_ov004_020bc864;
}

// @symbol _ZN12dScMgLuigi_c13OnYoshiTryEatEi
/* Same signature as dScMgBase_c::OnYoshiTryEat, or this stops overriding it.
   unk_0bc is re-read through an int* so the compare is not folded away. */
void dScMgLuigi_c::OnYoshiTryEat(int arg1)
{
    char *p;
    int *q;

    if (mFound != 0) {
        mClears += 1;
        q = (int *)&unk_0bc;
        *q += 1;
        if ((unsigned int)*(int *)&unk_0bc > 0x270e)
            *(int *)&unk_0bc = 0x270e;
    }

    if (arg1 == 0x12) {
        mTime = 0xa;
        *(int *)&unk_0bc = 0;
        if ((unsigned int)*(int *)&unk_0bc > 0x270e)
            *(int *)&unk_0bc = 0x270e;

        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = 0;

        p = (char *)data_ov004_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    FreeGfxSlotsById(0x1d);
    ResetBoard();
    ChooseBoard();

    mState = 0;
    data_0209d45c |= 4;
    data_0209d454 &= ~4;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}

// @symbol _ZN12dScMgLuigi_c6RenderEv
s32 dScMgLuigi_c::Render()
{
    func_ov004_020b1e34(this, 0xe0, 0x14, 1);
    DrawCurtains();
    DrawReward();
    DrawPenalties();
    DrawWantedIcon();
    DrawTimer();
    DrawPictures();
    return 1;
}

// @symbol _ZN12dScMgLuigi_c8BehaviorEv
/* One step of the round: setup, place, play, or catch. */
s32 dScMgLuigi_c::Behavior()
{
    int j = mState;
    (this->*data_ov006_02142234[j])();
    return 1;
}

// @symbol _ZN12dScMgLuigi_c13InitResourcesEv
s32 dScMgLuigi_c::InitResources()
{
    char *scr;
    /* Plain u16 fills are 16 bytes short of these two tile copies. */
    volatile u16 fillMain;
    volatile u16 fillSub;
    void *arc;
    int file;
    int objChar;

    arc = func_ov004_020adc74((const char *)&data_ov006_0213cfa0);
    if (arc == 0) return 0;

    data_0209d45c |= 8;
    *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & ~3) | 2;
    *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & 0x43) | 0x1218;

    DecompressLZ16(arc, (void *)func_02054d88());

    file = LoadFile(0x48);
    GX::LoadBGPltt((const void *)file, 0x60, 0x1a0);
    Deallocate((void *)file);

    file = LoadFile(0x49);
    func_02056314((void *)file, 0, 0x800);
    Deallocate((void *)file);

    data_0209d45c |= 4;
    *(volatile u16 *)0x0400000c &= ~3;
    *(volatile u16 *)0x0400000c &= ~0x40;
    *(volatile u32 *)0x04000018 = 0;
    *(volatile u16 *)0x0400000c = (*(volatile u16 *)0x0400000c & 0x43) | 0x1410;

    file = LoadFile(0x4b);
    DecompressLZ16((void *)file, (void *)(G2::GetBG2CharPtr() + 0x4000));
    Deallocate((void *)file);

    file = LoadFile(0x4c);
    GX::LoadBGPltt((const void *)file, 0x40, 0x20);
    Deallocate((void *)file);

    scr = G2::GetBG2ScrPtr();
    fillMain = 0x23e1;
    MultiStore16(fillMain, scr, 0x800);

    objChar = LoadFile(0xd3);
    file = LoadFile(0xd4);
    DecompressLZ16((void *)objChar, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)file, 0, 0x100);

    *(volatile u16 *)0x0400100c &= ~3;
    *(volatile u16 *)0x0400100c &= ~0x40;
    *(volatile u32 *)0x04001018 = 0;
    *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & 0x43) | 0x1210;

    {
        int subFile;
        subFile = LoadFile(0x4b);
        DecompressLZ16((void *)subFile, (void *)(G2S::GetBG2CharPtr() + 0x4000));
        Deallocate((void *)subFile);

        subFile = LoadFile(0x4c);
        GXS::LoadBGPltt((const void *)subFile, 0x1e0, 0x20);
        Deallocate((void *)subFile);

        subFile = LoadFile(0x4d);
        func_02056374((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);

        data_0209d454 |= 8;
        *(volatile u16 *)0x0400100e &= ~3;
        *(volatile u16 *)0x0400100e = (*(volatile u16 *)0x0400100e & 0x43) | 0x1410;

        subFile = LoadFile(0x4a);
        scr = G2S::GetBG3ScrPtr();
        fillSub = 0xf3e1;
        MultiStore16(fillSub, scr, 0x800);
        Deallocate((void *)subFile);
    }

    DecompressLZ16((void *)objChar, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)file, 0, 0x100);
    Deallocate((void *)objChar);
    Deallocate((void *)file);

    mClears = 0;
    ResetBoard();
    mBoard = 0xff;
    ChooseBoard();

    mState = 0;
    Ov004_Deallocate(arc);
    func_ov004_020b04d0(0x30);
    mTime = 0xa;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    data_ov004_020bc888 = 0x80;
    data_ov004_020bc864 = ~0x1b;
    mHudScore = 0;
    return 1;
}

// @symbol dScMgLuigi_c_classInit
extern "C" {
/* Allocates the scene. The ROM has no spelling for this factory. */
int *dScMgLuigi_c_classInit(void)
{
    return (int *)new dScMgLuigi_c;
}
}

