//cpp
/* Wanted. A poster names one character; pictures of the cast drift around
 * the touch screen and the player taps the match. A hit adds time and pops
 * a reward, a miss drops a mark and costs ten seconds, and curtains plus an
 * iris close the round. The timer running out fails it. dScMgLuigi_c is the
 * scene. Its state tables and registry profile close the file, and mwcc
 * builds the tables in __sinit_d_s_mg_luigi.cpp.
 *
 * Keep source order. The local optimization pragmas stay with the functions
 * that need them.
 *
 * Four places only match through a stepped base: UpdatePenalties walks a
 * char * by 0x14 (a pointer at mPenalty is 0x24 bytes short),
 * RestartPicture steps by idx (direct mShown[idx] and friends change the
 * code), the grid cell in PlaceNextPicture steps by col * 9 + row (direct
 * &mGrid[col][row] changes it), and TickPictureFlash keeps its blink and
 * shown base pointers (one spelling of the blink timer collapses the two
 * exits). Each spells the member through the stepped base. InitResources
 * and ChooseTarget still copy tiles through a volatile u16; a plain local
 * is shorter.
 *
 * The measurements behind the source forms of MovePictureSway,
 * MovePictureDriftRandom, PlaceBoardLayout and PlaceNextPicture (for one,
 * opt_propagation off takes PlaceNextPicture from 219 to 330 aligned
 * instructions) are in notes/experiments/pr2876-source-repair-0920.json.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgLuigi_c_AssetId {
    ASSET_D_2D_MG_BG_LUIGI1_NCL_BIN = 0x0048,
    ASSET_D_2D_MG_BG_LUIGI1_NSC_BIN = 0x0049,
    ASSET_D_2D_MG_BG_LUIGI2_BLACK_NSC_BIN = 0x004a,
    ASSET_D_2D_MG_BG_LUIGI2_NCG_BIN = 0x004b,
    ASSET_D_2D_MG_BG_LUIGI2_NCL_BIN = 0x004c,
    ASSET_D_2D_MG_BG_LUIGI2_NSC_BIN = 0x004d,
    ASSET_D_2D_MG_LUIGI_NCG_BIN = 0x00d3,
    ASSET_D_2D_MG_LUIGI_NCL_BIN = 0x00d4
};

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "dScMgLuigi_c.h"
#include "Sound.h"
#include "IRQ.h"
#include "nitro/fx/fx.h"
#include "SpawnInfo.h"
#include "OamAttr.h"

typedef void (dScMgLuigi_c::*LuigiSlotHandler)(int);
typedef void (dScMgLuigi_c::*LuigiStateHandler)();

/* Same spelling the matched IRQ::EnableIRQs / IRQ::DisableIRQs shards use. */
#define IME (*(volatile u16 *)0x04000208)

/* Scanline window record pushed to the iris hardware. */
struct IrisScanlineTable {
    char _pad[0x300];
};

/* One row of the iris window table: left and right edges. */
struct IrisScanline {
    u8 right, left, reserved2, reserved3;
};

/* C-linkage engine calls and the remaining local helpers. */
extern "C" {
extern int func_02053c10(int);
/* Functions the members call need C linkage at file scope. Data names do
   not mangle, so the data_* declarations stay next to their users. */
extern int RandomIntInternal(int *seed);
extern s32 GetGameLanguage(void);
extern void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *matrix);
extern char *Mg_RenderMainSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5,
                                       unsigned short a6);
extern void DrawOamSprite(int p, int a, int b, int c);
extern void Mg_DrawNumberRotated(int a, int b, int c, int d, int e, int f, int g);
extern void func_020127a4(int a0, int a1, int a2, int a3);
extern void Hud_RenderSprite(void *sprite, int b, int c, int d, int e);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void FreeGfxSlotsById(int arg);
extern void Mg_SetSharedState(int c);

extern void Luigi_PrepareScanlines(void);
extern void Luigi_PushScanlineRecord(void);
extern dScMgLuigi_c *dScMgLuigi_c_classInit(void);
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
namespace CP15 {
void FlushAndInvalidateDataCache(u32 addr, u32 size);
}

namespace cstd {
int sqrt(unsigned long long x);
}

// @symbol _ZN12dScMgLuigi_cD1Ev
// @symbol _ZN12dScMgLuigi_cD0Ev
/* One definition, so D1 and D0 come out in that order under defer_codegen
   off. The extra D2 is not in the ROM; the manifest lets it be deadstripped. */
dScMgLuigi_c::~dScMgLuigi_c() {}

// @symbol _ZN12dScMgLuigi_c21AfterCleanupResourcesEj
/* The base cleanup returns void; call it, don't return it. */
void dScMgLuigi_c::AfterCleanupResources(u32 vfSuccess) {
    if (vfSuccess == 2 && IRQ::GetIRQHandler(2) == Luigi_PrepareScanlines) {
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

// @symbol Luigi_PrepareScanlines
/* IRQ 2's handler uses the void(*)() callback installed by StartIris.
 * It accesses shared scanline state and remains a free function. */
extern "C" {
void Luigi_PrepareScanlines(void) {
    extern char data_023c0000[];
    extern int data_0209f608;
    extern int data_0209f648[][192];
    extern void MultiCopy_Int(int *src, int *dst, int byteCount);
    int nextLine;
    /* The (int) cast is what keeps this handler the same size. It is the
       only one of 24 launders this TU inherited from its shards that pays;
       do not re-add the other 23. */
    *(int *)(((int)data_023c0000 + 0x3ff8)) |= 2;
    nextLine = REG_VCOUNT + 1;
    if (nextLine >= 0xc0) {
        Luigi_PushScanlineRecord();
    } else if (nextLine <= 0xc0) {
        if (*(volatile u16 *)0x04000004 & 2) {
            MultiCopy_Int(&data_0209f648[data_0209f608][nextLine], (int *)0x04000040, 4);
        }
    }
}
}

// @symbol Luigi_PushScanlineRecord
extern "C" {
/* Latches the pending table index and copies its first four-byte scanline
   entry to 0x04000040. The record type retains the table stride. */
void Luigi_PushScanlineRecord(void) {
    extern u32 data_0209f60c;
    extern volatile u32 data_0209f608;
    extern struct IrisScanlineTable data_0209f648[];
    extern void MultiCopy_Int(int *src, int *dst, int byteCount);
    data_0209f608 = data_0209f60c;
    MultiCopy_Int((int *)&data_0209f648[data_0209f608], (int *)0x04000040, 4);
}
}

// @symbol _ZN12dScMgLuigi_c14BuildIrisTableEi
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::BuildIrisTable(int irisIndex) {
    extern int data_0209f608;
    extern struct IrisScanline data_0209f648[][0xc0];
    extern int data_0209f60c;
    int writeBuffer;
    int radius;
    int scanline, sourceLine, mirrorLine;
    u8 mirroredLeft;

    if (data_0209f608 == 0)
        writeBuffer = 1;
    else
        writeBuffer = 0;
    radius = mIris[irisIndex].radius >> 12;

    for (scanline = 0; scanline < 0x60; scanline++) {
        if (0x60 - radius <= scanline) {
            int circleOffset = scanline - 0x60 + radius;
            int halfWidth = cstd::sqrt((s64)(circleOffset * (radius * 2 - circleOffset)));
            int left = (mIris[0].x >> 12) - halfWidth;
            int right = halfWidth + (mIris[0].x >> 12);
            if (left < 0)
                left = 0;
            if (right >= 0xff)
                right = 0xff;
            data_0209f648[writeBuffer][scanline].left = left;
            data_0209f648[writeBuffer][scanline].right = right;
        } else {
            data_0209f648[writeBuffer][scanline].right = 0;
            data_0209f648[writeBuffer][scanline].left = 0;
        }
    }
    for (sourceLine = 0x5f, mirrorLine = 0; mirrorLine < 0x60; mirrorLine++, sourceLine--) {
        mirroredLeft = data_0209f648[writeBuffer][sourceLine].left;
        data_0209f648[writeBuffer][mirrorLine + 0x60].left = mirroredLeft;
        data_0209f648[writeBuffer][mirrorLine + 0x60].right =
            data_0209f648[writeBuffer][sourceLine].right;
    }
    CP15::FlushAndInvalidateDataCache((u32)data_0209f648, 0x600);
    data_0209f60c = writeBuffer;
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
    if (saved != 0) {
        *ime;
        *ime = 1;
    }
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
    if ((mIris[idx].radius >> 12) < 0xa0)
        return;
    mIris[idx].state += 1;
    data_0209d45c &= ~4;
}

// @symbol _ZN12dScMgLuigi_c8IrisHoldEi
void dScMgLuigi_c::IrisHold(int idx) {
    mIris[idx].scale = 0x1000;
}

// @symbol _ZN12dScMgLuigi_c10UpdateIrisEv
void dScMgLuigi_c::UpdateIris() {
    extern LuigiSlotHandler sLuigiIrisTable[];
    if (mIris[0].active == 0)
        return;
    int irisState = mIris[0].state;
    (this->*sLuigiIrisTable[irisState])(0);
}

// @symbol _ZN12dScMgLuigi_c9StartIrisEv
void dScMgLuigi_c::StartIris() {
    extern unsigned int data_0209f608;
    extern unsigned char data_0209d460;
    unsigned short saved;
    mIris[0].active = 1;
    mIris[0].x = 0x80000;
    mIris[0].y = 0x60000;
    mIris[0].state = IRIS_HOLD;
    mIris[0].radius = 0x40000;
    mIris[0].scale = 0x1000;
    data_0209f608 = 0;
    BuildIrisTable(0);

    data_0209f608 = 1;
    BuildIrisTable(0);

    data_0209f608 = 0;
    saved = IME;
    IME = 0;
    IRQ::SetIRQHandler(2, Luigi_PrepareScanlines);

    REG_DISPCNT = (REG_DISPCNT & ~0xe000) | 0x2000;
    data_0209d460 = 1;
    {
        unsigned int v = *(volatile unsigned short *)0x04000048;
        v = (v & ~0x3f) | 0x18;
        v = v | 0x20;
        *(volatile unsigned short *)0x04000048 = v;
    }
    *(volatile unsigned short *)0x0400004a =
        (*(volatile unsigned short *)0x0400004a & ~0x3f) | 0x14;
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
void dScMgLuigi_c::DrawReward() {
    extern void *sLuigiTextSpritesByLanguage[];
    extern char sMgDigitSprites[];
    if (mReward.shown == 0)
        return;
    int x = mReward.x >> 0xc;
    int y = mReward.y >> 0xc;
    if (x <= 8)
        x = 8;
    if (x >= 0xf0)
        x = 0xf0;
    if (y <= 8)
        y = 8;
    if (y >= 0xb8)
        y = 0xb8;
    s32 language = GetGameLanguage();
    Mg_RenderSubSpriteMtx(((void **)sLuigiTextSpritesByLanguage[language])[13], x - 8, y, 0);
    Mg_RenderSubSpriteMtx(((void **)sMgDigitSprites)[25], x + 8, y, 0);
}

// @symbol _ZN12dScMgLuigi_c12UpdateRewardEv
extern "C" {
extern void Mg_SubmitScore(int self);
}

void dScMgLuigi_c::UpdateReward() {
    if (mReward.active == 0)
        return;

    if (mReward.phase == 0) {
        mReward.y += mReward.velY;
        mReward.velY -= 0x100;
        if (mReward.timer != 0) {
            int remainingTicks;
            mReward.timer--;
            remainingTicks = mReward.timer;
            if (remainingTicks < 0)
                mReward.timer = 0;
            return;
        }
        mReward.timer = 0x40;
        mReward.phase += 1;
        return;
    }

    if (mReward.phase == 1) {
        if (mReward.timer != 0) {
            int remainingTicks;
            mReward.timer--;
            remainingTicks = mReward.timer;
            if (remainingTicks < 0)
                mReward.timer = 0;
            return;
        }
        mReward.shown = 0;
        mReward.phase += 1;
        {
            char *g = (char *)gMgScene;
            if (g != 0) {
                if (((dScMgBase_c *)g)->mHudScore < 0x270f)
                    ((dScMgBase_c *)g)->mHudScore += 1;
                if (((dScMgBase_c *)g)->mHudScore > ((dScMgBase_c *)g)->mBestHudScore)
                    ((dScMgBase_c *)g)->mBestHudScore = ((dScMgBase_c *)g)->mHudScore;
            }
        }
        Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
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
void dScMgLuigi_c::StartReward(int pictureIndex) {
    mReward.active = 1;
    mReward.x = mPosX[pictureIndex];
    mReward.y = mPosY[pictureIndex];
    mReward.timer = 16;
    mReward.phase = 0;
    mReward.shown = 1;
    mReward.sparks = 5;
    mReward.velY = -3840;
}

// @symbol _ZN12dScMgLuigi_c12DrawCurtainsEv
void dScMgLuigi_c::DrawCurtains() {
    extern char *sLuigiCurtainSprite;
    int curtainIndex;
    for (curtainIndex = 0; curtainIndex < 2; curtainIndex++) {
        if (mCurtain[curtainIndex].shown != 0) {
            int x = mCurtain[curtainIndex].x >> 0xc;
            int y = mCurtain[curtainIndex].y >> 0xc;
            char *entry = sLuigiCurtainSprite;
            for (;;) {
                char *sprite = Mg_RenderMainSpriteScaled((int)entry, x, y, -1, 1, 0x1000, 0);
                if (sprite != 0) {
                    /* attr2's palette, read with the ROM's word load at +4:
                       spelled `->attr2` it becomes a halfword load and moves bytes. */
                    int palette = (int)((unsigned int)(*(int *)(sprite + 4) << 0x10) >> 0x1c);
                    *(int *)sprite = (*(int *)sprite & ~0xc00) | 0x800;
                    ((OamAttr *)sprite)->attr2 =
                        (unsigned short)((((OamAttr *)sprite)->attr2 & ~0xf000) | (palette << 12));
                }
                if (((OamAttr *)entry)->attr3 == 0xffff)
                    break;
                entry += 8;
            }
        }
    }
}

// @symbol _ZN12dScMgLuigi_c16CurtainStateSlowEi
void dScMgLuigi_c::CurtainStateSlow(int curtainIndex) {
    int *velocity;
    int *position;
    int screenX;
    mCurtain[curtainIndex].count = mCurtain[curtainIndex].count + 1;
    if (mCurtain[curtainIndex].count == 0x1a)
        Sound::PlayBank2_2D(0x1b9);
    velocity = &mCurtain[curtainIndex].velX;
    position = &mCurtain[curtainIndex].x;
    *position = *position + *velocity;
    screenX = *position >> 12;
    if (curtainIndex == 0) {
        *velocity = *velocity - 0x140;
        if (screenX < 0x80)
            return;
        if (*velocity <= 0)
            return;
        *position = 0x80000;
        mCurtain[curtainIndex].active = 0;
        mCurtain[curtainIndex].phase = CURTAIN_BOUNCE;
        FreeGfxSlotsById(0xd);
    } else {
        *velocity = *velocity + 0x140;
        if (screenX > 0x80)
            return;
        if (*velocity >= 0)
            return;
        *position = 0x80000;
        mCurtain[curtainIndex].active = 0;
        mCurtain[curtainIndex].phase = CURTAIN_BOUNCE;
    }
}

// @symbol _ZN12dScMgLuigi_c16CurtainStateFastEi
/* The tests on i inside the arm that already tested i stay. Folding them
   drops code this compiler still emits. */
void dScMgLuigi_c::CurtainStateFast(int curtainIndex) {
    int *velocity = &mCurtain[curtainIndex].velX;
    int *position = &mCurtain[curtainIndex].x;
    int screenX;

    *position = *position + *velocity;
    screenX = *position >> 12;

    if (curtainIndex == 0) {
        *velocity += 0x140;
        if (screenX >= 0x80 && *velocity > 0) {
            *position = 0x80000;
            mCurtain[curtainIndex].active = 0;
            mCurtain[curtainIndex].phase = CURTAIN_BOUNCE;
            if (curtainIndex == 0)
                Sound::PlayBank2_2D(0x1b9);
            return;
        }
        if (curtainIndex != 0)
            return;
        mCurtain[curtainIndex].sound =
            Sound_PlayIfNotActive(mCurtain[curtainIndex].sound, 2, 0x1b8, 0);
    } else {
        *velocity -= 0x140;
        if (screenX <= 0x80 && *velocity < 0) {
            *position = 0x80000;
            mCurtain[curtainIndex].active = 0;
            mCurtain[curtainIndex].phase = CURTAIN_BOUNCE;
            if (curtainIndex != 0)
                return;
            Sound::PlayBank2_2D(0x1b9);
            return;
        }
        if (curtainIndex != 0)
            return;
        mCurtain[curtainIndex].sound =
            Sound_PlayIfNotActive(mCurtain[curtainIndex].sound, 2, 0x1b8, 0);
    }
}

// @symbol _ZN12dScMgLuigi_c16CurtainStateHoldEi
void dScMgLuigi_c::CurtainStateHold(int curtainIndex) {
    int *velocity;
    int *position;
    int screenX;
    mCurtain[curtainIndex].count = mCurtain[curtainIndex].count + 1;
    if (mCurtain[curtainIndex].count == 0x35)
        Sound::PlayBank2_2D(0x1b9);
    if (curtainIndex == 0) {
        if (mCurtain[curtainIndex].count <= 0x35) {
            int *sound = &mCurtain[curtainIndex].sound;
            *sound = Sound_PlayIfNotActive(*sound, 2, 0x1b8, 0);
        }
    }
    {
        unsigned short *delay = (unsigned short *)&mCurtain[curtainIndex].delay;
        if (*delay != 0) {
            *delay = *delay - 1;
            if ((short)*delay < 0)
                *delay = 0;
            return;
        }
    }
    velocity = &mCurtain[curtainIndex].velX;
    position = &mCurtain[curtainIndex].x;
    *position = *position + *velocity;
    screenX = *position >> 12;
    if (curtainIndex == 0) {
        *velocity = *velocity + 0x140;
        if (screenX > 0x80)
            return;
        *position = 0x80000;
        mCurtain[curtainIndex].active = 0;
        mCurtain[curtainIndex].phase = CURTAIN_BOUNCE;
        FreeGfxSlotsById(0x1d);
        if (mPromptBlinkCount != 0)
            return;
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    } else {
        *velocity = *velocity - 0x140;
        if (screenX < 0x80)
            return;
        *position = 0x80000;
        mCurtain[curtainIndex].active = 0;
        mCurtain[curtainIndex].phase = CURTAIN_BOUNCE;
        FreeGfxSlotsById(0x1d);
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
    }
}

// @symbol _ZN12dScMgLuigi_c18CurtainStateBounceEi
void dScMgLuigi_c::CurtainStateBounce(int curtainIndex) {
    int *velocity = &mCurtain[curtainIndex].velX;
    int *position = &mCurtain[curtainIndex].x;
    int screenX;
    *position = *position + *velocity;
    screenX = *position >> 12;
    if (curtainIndex == 0) {
        *velocity -= 0x100;
        if (screenX >= 0x140) {
            *position = 0x140000;
            mCurtain[curtainIndex].phase++;
            mCurtain[curtainIndex].delay = 0x10;
            *velocity = -0x6000;
        }
    } else {
        *velocity += 0x100;
        if (screenX <= -0x40) {
            *position = -0x40000;
            mCurtain[curtainIndex].phase++;
            mCurtain[curtainIndex].delay = 0x10;
            *velocity = 0x6000;
        }
    }
    if (curtainIndex == 0) {
        mCurtain[curtainIndex].sound =
            Sound_PlayIfNotActive(mCurtain[curtainIndex].sound, 2, 0x1b8, 0);
    }
}

// @symbol _ZN12dScMgLuigi_c13CurtainUpdateEi
void dScMgLuigi_c::CurtainUpdate(int i) {
    extern LuigiSlotHandler sLuigiCurtainStateTable[];
    (this->*(sLuigiCurtainStateTable[mCurtain[i].phase]))(i);
}

// @symbol _ZN12dScMgLuigi_c12CurtainStartEi
void dScMgLuigi_c::CurtainStart(int curtainIndex) {
    extern unsigned char data_0209d460;
    extern int sLuigiCurtainX[];
    unsigned short delay;
    int windowMask;
    int roundInSet;

    delay = *(unsigned short *)&mCurtain[curtainIndex].delay;
    if (delay != 0) {
        mCurtain[curtainIndex].delay = delay - 1;
        if (mCurtain[curtainIndex].delay < 0)
            mCurtain[curtainIndex].delay = 0;
        return;
    }

    windowMask = *(unsigned short *)0x0400004a;
    windowMask &= ~0x3f00;
    windowMask |= 0x1800;
    windowMask &= ~0x2000;
    windowMask |= 0x2000;
    *(unsigned short *)0x0400004a = windowMask;
    windowMask = (*(unsigned short *)0x0400004a & ~0x3f) | 0x14;
    *(unsigned short *)0x0400004a = windowMask;
    *(volatile int *)0x04000000 = (*(volatile int *)0x04000000 & ~0xe000) | 0x8000;
    data_0209d460 = 4;

    mCurtain[curtainIndex].x = sLuigiCurtainX[curtainIndex] << 12;
    mCurtain[curtainIndex].y = 0x60000;
    mCurtain[curtainIndex].shown = 1;
    mCurtain[curtainIndex].state = CURTAIN_UPDATE;
    if (curtainIndex != 0)
        mCurtain[curtainIndex].velX = -0x8000;
    else
        mCurtain[curtainIndex].velX = 0x8000;

    roundInSet = mLevel;
    while (roundInSet >= 5)
        roundInSet -= 5;
    if (roundInSet != 0) {
        mCurtain[curtainIndex].state = CURTAIN_SLOW;
        if (curtainIndex != 0)
            mCurtain[curtainIndex].velX = -0x7000;
        else
            mCurtain[curtainIndex].velX = 0x7000;
    }
    mCurtain[curtainIndex].sound = 0;
}

// @symbol _ZN12dScMgLuigi_c12CurtainsDoneEv
int dScMgLuigi_c::CurtainsDone() {
    extern LuigiSlotHandler sLuigiCurtainTable[];
    int activeCurtains = 0;
    int curtainIndex = 0;
    do {
        if (mCurtain[curtainIndex].active != 0) {
            (this->*sLuigiCurtainTable[mCurtain[curtainIndex].state])(curtainIndex);
            activeCurtains++;
        }
        curtainIndex++;
    } while (curtainIndex < 2);
    return activeCurtains == 0;
}

// @symbol _ZN12dScMgLuigi_c13ResetCurtainsEv
void dScMgLuigi_c::ResetCurtains() {
    int i;
    for (i = 0; i < 2; i++) {
        mCurtain[i].active = 1;
        mCurtain[i].state = CURTAIN_START;
        mCurtain[i].delay = 0;
        mCurtain[i].count = 0;
        mCurtain[i].shown = 0;
        mCurtain[i].phase = CURTAIN_BOUNCE;
    }
}

// @symbol _ZN12dScMgLuigi_c9DrawTimerEv
void dScMgLuigi_c::DrawTimer() {
    extern int *sLuigiTextSpritesByLanguage[];
    int language;
    if (mState < STATE_PLAY)
        return;
    language = GetGameLanguage();
    DrawOamSprite(*(int *)((char *)sLuigiTextSpritesByLanguage[language] + 0xc), 0x80, 0x10, 0);
    Mg_DrawNumberRotated(0x80, 0x28, static_cast<u16>(mTime), 1, -1, 0x800, 0);
}

// @symbol _ZN12dScMgLuigi_c9TickTimerEv
void dScMgLuigi_c::TickTimer() {
    if (*(u16 *)&mTime != 0) {
        {
            u8 *tick = &mTimeTick;
            *tick = *tick + 1;
        }
        if (mTimeTick < 0x3c)
            return;
        mTimeTick = 0;
        {
            u16 *seconds = (u16 *)&mTime;
            *seconds = *seconds - 1;
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
    extern void *sLuigiPictureSprites[];
    int i;
    if (mState == STATE_SETUP)
        return;
    for (i = 0; i < 0x78; i++) {
        if (mShown[i] == 1) {
            Hud_RenderSprite(sLuigiPictureSprites[mCharacter[i]], mPosX[i] >> 12, mPosY[i] >> 12,
                             -1, 0);
        }
    }
}

// @symbol _ZN12dScMgLuigi_c13DrawPenaltiesEv
void dScMgLuigi_c::DrawPenalties() {
    extern void *sLuigiTextSpritesByLanguage[];
    extern char sMgDigitSprites[];
    int penaltyIndex;
    for (penaltyIndex = 0; penaltyIndex < 0x10; penaltyIndex++) {
        if (mPenalty[penaltyIndex].shown != 0) {
            int x = mPenalty[penaltyIndex].x >> 0xc;
            int y = mPenalty[penaltyIndex].y >> 0xc;
            s32 language;
            if (x <= 0x10)
                x = 0x10;
            if (x >= 0xe0)
                x = 0xe0;
            if (y <= 8)
                y = 8;
            if (y >= 0xb8)
                y = 0xb8;
            language = GetGameLanguage();
            Mg_RenderSubSpriteMtx(((void **)sLuigiTextSpritesByLanguage[language])[14], x - 0x10, y,
                                  0);
            Mg_RenderSubSpriteMtx(((void **)sMgDigitSprites)[41], x, y, 0);
            Mg_RenderSubSpriteMtx(((void **)sMgDigitSprites)[40], x + 0x10, y, 0);
        }
    }
}

// @symbol _ZN12dScMgLuigi_c15UpdatePenaltiesEv
#pragma push
#pragma opt_common_subs off
/* Walks from this: q steps one dScMgLuigi_Penalty at a time and PEN reads
   mPenalty[0] through it. A pointer at mPenalty is 0x24 bytes short of this
   body. timer is s16, but the ROM tests and decrements it as a u16; the s16
   spelling of either changes the code. */
#define PEN(q) (((dScMgLuigi_c *)(q))->mPenalty[0])
void dScMgLuigi_c::UpdatePenalties() {
    char *q = (char *)this;
    int penaltyIndex;
    for (penaltyIndex = 0; penaltyIndex < 0x10; penaltyIndex++, q += 0x14) {
        if (PEN(q).active != 0) {
            if (PEN(q).phase == 0) {
                if (*(u16 *)&PEN(q).timer != 0) {
                    *(u16 *)&PEN(q).timer -= 1;
                    if (PEN(q).timer < 0)
                        PEN(q).timer = 0;
                    PEN(q).y += PEN(q).velY;
                    PEN(q).velY += 0x100;
                } else {
                    PEN(q).timer = 0x40;
                    PEN(q).phase += 1;
                }
            } else {
                if (*(u16 *)&PEN(q).timer != 0) {
                    *(u16 *)&PEN(q).timer -= 1;
                    if (PEN(q).timer < 0)
                        PEN(q).timer = 0;
                } else {
                    PEN(q).timer = 0;
                    PEN(q).active = 0;
                    PEN(q).shown = 0;
                }
            }
        }
    }
}
#undef PEN
#pragma pop

// @symbol _ZN12dScMgLuigi_c10AddPenaltyEi
/* First free miss mark, at the picture that was tapped, and ten seconds off. */
void dScMgLuigi_c::AddPenalty(int pictureIndex) {
    int penaltyIndex;
    for (penaltyIndex = 0; penaltyIndex < 0x10; penaltyIndex++) {
        if (mPenalty[penaltyIndex].active != 0)
            continue;
        mPenalty[penaltyIndex].active = 1;
        mPenalty[penaltyIndex].shown = 1;
        mPenalty[penaltyIndex].timer = 0x10;
        mPenalty[penaltyIndex].x = mPosX[pictureIndex];
        mPenalty[penaltyIndex].y = mPosY[pictureIndex];
        mPenalty[penaltyIndex].velY = 0x1100;
        mPenalty[penaltyIndex].phase = 0;
        *(u16 *)&mTime -= 0xa;
        if (mTime < 0)
            mTime = 0;
        return;
    }
}

// @symbol _ZN12dScMgLuigi_c14DrawWantedIconEv
void dScMgLuigi_c::DrawWantedIcon() {
    extern void *sLuigiPictureSprites[];
    if (mIconOn == 0)
        return;
    Mg_RenderSubSpriteMtx(sLuigiPictureSprites[mWanted], (unsigned short)mIconX,
                          (unsigned short)mIconY, 0);
}

// @symbol _ZN12dScMgLuigi_c16TickPictureFlashEi
#pragma push
#pragma opt_common_subs off
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::TickPictureFlash(int pictureIndex) {
    char *raw = (char *)this; /* unread; deleting it renumbers the pinned @NNN */
    char *blinkBase = (char *)mBlink;
    int byteOffset = pictureIndex * 2;
    unsigned short timer = *(unsigned short *)(blinkBase + byteOffset);
    unsigned char *shown;

    *(short *)(blinkBase + byteOffset) = (short)(timer - 1);

    if ((short)mBlink[pictureIndex] < 0)
        mBlink[pictureIndex] = 0;

    /* Two exits, not one if/else: folding them changes the code. */
    if ((((unsigned short)mBlink[pictureIndex] >> 2) & 1) != 0) {
        char *shownBase = (char *)mShown;
        shownBase[pictureIndex] = 0;
        shown = (unsigned char *)(shownBase + pictureIndex);
        goto after_flag;
    }
    {
        char *shownBase = (char *)mShown;
        shownBase[pictureIndex] = 1;
        shown = (unsigned char *)(shownBase + pictureIndex);
    }
after_flag:
    if (mBlink[pictureIndex] != 0)
        return;
    mKind[pictureIndex] = 0;
    *shown = 1;
}
#pragma pop
#pragma pop

// @symbol _ZN12dScMgLuigi_c17MovePictureBounceEi
#pragma push
#pragma opt_common_subs off
void dScMgLuigi_c::MovePictureBounce(int pictureIndex) {
    extern int data_0209d4b8;
    extern int sLuigiBounceSpeeds[];
    extern s16 data_02082214[];
    u8 *started = &mStarted[pictureIndex];

    if (*started == 0) {
        u32 randomValue = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        u16 initialAngle = (u16)((((randomValue << 4) >> 15) << 12));

        mMovePhase[pictureIndex] = initialAngle;
        *started += 1;
        return;
    }

    {
        u8 *characters = mCharacter;
        int *posX = mPosX;
        int *posY = mPosY;
        u16 phase;
        int angleIndex;
        int cosine, sine;
        int speed;
        s64 scaledStep;
        int round12 = 0x800;

        phase = mMovePhase[pictureIndex];
        angleIndex = phase >> 4;
        cosine = data_02082214[angleIndex * 2 + 1];
        speed = sLuigiBounceSpeeds[characters[pictureIndex]];
        scaledStep = (s64)cosine * speed;
        posX[pictureIndex] += (int)((scaledStep + round12) >> 12);

        phase = mMovePhase[pictureIndex];
        angleIndex = phase >> 4;
        sine = data_02082214[angleIndex * 2];
        speed = sLuigiBounceSpeeds[characters[pictureIndex]];
        scaledStep = (s64)sine * speed;
        posY[pictureIndex] += (int)((scaledStep + round12) >> 12);

        {
            int x = mPosX[pictureIndex] >> 0xc;
            int y = mPosY[pictureIndex] >> 0xc;

            if (x > 0xf8) {
                mMovePhase[pictureIndex] = 0x8000 - mMovePhase[pictureIndex];
                mPosX[pictureIndex] = 0xf8000;
            } else if (x < 8) {
                mMovePhase[pictureIndex] = 0x8000 - mMovePhase[pictureIndex];
                mPosX[pictureIndex] = 0x8000;
            }

            if (y > 0xb8) {
                mMovePhase[pictureIndex] = -mMovePhase[pictureIndex];
                mPosY[pictureIndex] = 0xb8000;
                return;
            }

            if (y < 8) {
                mMovePhase[pictureIndex] = -mMovePhase[pictureIndex];
                mPosY[pictureIndex] = 0x8000;
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
void dScMgLuigi_c::MovePictureSway(int idx) {
    extern int sLuigiSwaySpeed[];
    extern int sLuigiSwayStartVY[];
    extern int sLuigiSwayAccel[];
    if (mStarted[idx] == 0) {
        mMovePhase[idx] = 0;
        mStarted[idx]++;
        if (mCharacter[idx] == 0) {
            mVelX[idx] = sLuigiSwaySpeed[mCharacter[idx]];
        } else {
            mVelX[idx] = -sLuigiSwaySpeed[mCharacter[idx]];
        }
        mVelY[idx] = sLuigiSwayStartVY[mCharacter[idx]];
        return;
    }
    mPosX[idx] += mVelX[idx];
    mPosY[idx] += mVelY[idx];
    if (mMovePhase[idx] == 0) {
        if (mVelX[idx] > 0) {
            mVelX[idx] -= sLuigiSwayAccel[mCharacter[idx]];
            if (mVelX[idx] <= 0) {
                mVelX[idx] = 0;
                mMovePhase[idx] = 2;
            }
        } else if (mVelX[idx] < 0) {
            mVelX[idx] += sLuigiSwayAccel[mCharacter[idx]];
            if (mVelX[idx] >= 0) {
                mVelX[idx] = 0;
                mMovePhase[idx] = 1;
            }
        }
    } else if (mMovePhase[idx] == 1) {
        mVelX[idx] += sLuigiSwayAccel[mCharacter[idx]];
        if (mVelX[idx] >= sLuigiSwaySpeed[mCharacter[idx]]) {
            mMovePhase[idx] = 0;
        }
    } else {
        mVelX[idx] -= sLuigiSwayAccel[mCharacter[idx]];
        if (mVelX[idx] <= -sLuigiSwaySpeed[mCharacter[idx]]) {
            mMovePhase[idx] = 0;
        }
    }
    WrapPicture(idx);
}

// @symbol _ZN12dScMgLuigi_c22MovePictureDriftRandomEi
/* Drift on a random angle, one of sixteen steps of 0x1000. */
void dScMgLuigi_c::MovePictureDriftRandom(int pictureIndex) {
    extern int data_0209d4b8;
    extern int sLuigiDriftRandomSpeed[];
    extern s16 data_02082214[];
    if (mStarted[pictureIndex] == 0) {
        unsigned int randomValue = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        unsigned short initialAngle = (unsigned short)(((randomValue << 4) >> 15) << 12);
        mMovePhase[pictureIndex] = initialAngle;
        mStarted[pictureIndex]++;
        return;
    }
    {
        int angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosX[pictureIndex] =
            mPosX[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2 + 1],
                                         sLuigiDriftRandomSpeed[mCharacter[pictureIndex]]);
        angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosY[pictureIndex] =
            mPosY[pictureIndex] +
            FX_MUL(data_02082214[angleIndex * 2], sLuigiDriftRandomSpeed[mCharacter[pictureIndex]]);
    }
    WrapPicture(pictureIndex);
}

// @symbol _ZN12dScMgLuigi_c21MovePictureDriftByRowEi
/* Drift left or right from mRowDir of the slot's row. */
void dScMgLuigi_c::MovePictureDriftByRow(int pictureIndex) {
    extern int sLuigiDriftByRowSpeed[];
    extern s16 data_02082214[];
    int rowIndex;
    int columnIndex;
    if (mStarted[pictureIndex] == 0) {
        mStarted[pictureIndex]++;
        rowIndex = 0;
        columnIndex = pictureIndex;
        if (pictureIndex >= 8) {
            do {
                columnIndex -= 8;
                rowIndex++;
            } while (columnIndex >= 8);
        }
        if (mRowDir[rowIndex] != 0) {
            mMovePhase[pictureIndex] = 0x8000;
        } else {
            mMovePhase[pictureIndex] = 0;
        }
        return;
    }
    {
        int angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosX[pictureIndex] =
            mPosX[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2 + 1],
                                         sLuigiDriftByRowSpeed[mCharacter[pictureIndex]]);
        angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosY[pictureIndex] =
            mPosY[pictureIndex] +
            FX_MUL(data_02082214[angleIndex * 2], sLuigiDriftByRowSpeed[mCharacter[pictureIndex]]);
    }
    WrapPicture(pictureIndex);
}

// @symbol _ZN12dScMgLuigi_c24MovePictureDriftByColumnEi
/* Drift up or down from mColDir of the slot's column. */
void dScMgLuigi_c::MovePictureDriftByColumn(int pictureIndex) {
    extern int sLuigiDriftByColumnSpeed[];
    extern s16 data_02082214[];
    if (mStarted[pictureIndex] == 0) {
        mStarted[pictureIndex]++;
        if (mColDir[pictureIndex & 7] != 0) {
            mMovePhase[pictureIndex] = 0x4000;
        } else {
            mMovePhase[pictureIndex] = 0xc000;
        }
        return;
    }
    {
        int angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosX[pictureIndex] =
            mPosX[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2 + 1],
                                         sLuigiDriftByColumnSpeed[mCharacter[pictureIndex]]);
        angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosY[pictureIndex] =
            mPosY[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2],
                                         sLuigiDriftByColumnSpeed[mCharacter[pictureIndex]]);
    }
    WrapPicture(pictureIndex);
}

// @symbol _ZN12dScMgLuigi_c23MovePictureDriftByLevelEi
/* Drift on the angle stored for this picture's speed. */
void dScMgLuigi_c::MovePictureDriftByLevel(int pictureIndex) {
    extern int sLuigiDriftByLevelSpeed[];
    extern s16 data_02082214[];
    if (mStarted[pictureIndex] == 0) {
        u8 character = mCharacter[pictureIndex];
        mMovePhase[pictureIndex] = mLevelPhase[character];
        mStarted[pictureIndex]++;
    } else {
        int angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosX[pictureIndex] =
            mPosX[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2 + 1],
                                         sLuigiDriftByLevelSpeed[mCharacter[pictureIndex]]);
        angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosY[pictureIndex] =
            mPosY[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2],
                                         sLuigiDriftByLevelSpeed[mCharacter[pictureIndex]]);
        WrapPicture(pictureIndex);
    }
}

// @symbol _ZN12dScMgLuigi_c21MovePictureDriftFixedEi
/* Drift on a fixed angle of 0x6000. */
void dScMgLuigi_c::MovePictureDriftFixed(int pictureIndex) {
    extern int sLuigiDriftFixedSpeed[];
    extern s16 data_02082214[];
    if (mStarted[pictureIndex] == 0) {
        mMovePhase[pictureIndex] = 0x6000;
        mStarted[pictureIndex]++;
        return;
    }
    {
        int angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosX[pictureIndex] =
            mPosX[pictureIndex] + FX_MUL(data_02082214[angleIndex * 2 + 1],
                                         sLuigiDriftFixedSpeed[mCharacter[pictureIndex]]);
        angleIndex = mMovePhase[pictureIndex] >> 4;
        mPosY[pictureIndex] =
            mPosY[pictureIndex] +
            FX_MUL(data_02082214[angleIndex * 2], sLuigiDriftFixedSpeed[mCharacter[pictureIndex]]);
    }
    WrapPicture(pictureIndex);
}

// @symbol _ZN12dScMgLuigi_c11WrapPictureEi
/* Wraps one picture's 20.12 position round the 0x110 x 0xd0 screen. Leaf. */
void dScMgLuigi_c::WrapPicture(int i) {
    int x = mPosX[i] >> 12;
    int y = mPosY[i] >> 12;
    if (x > 0x110)
        mPosX[i] = -0x10000;
    if (x < -0x10)
        mPosX[i] = 0x110000;
    if (y > 0xd0)
        mPosY[i] = -0x10000;
    if (y < -0x10)
        mPosY[i] = 0xd0000;
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
    ((dScMgLuigi_c *)self)->mShown[0] = 1;
    ((dScMgLuigi_c *)self)->mStarted[0] = 0;
    ((dScMgLuigi_c *)self)->mKind[0] = data_ov006_0213ceac[board] + 1;
}

// @symbol _ZN12dScMgLuigi_c14UpdatePicturesEv
void dScMgLuigi_c::UpdatePictures() {
    extern LuigiSlotHandler sLuigiSlotHandlers[];
    int pictureIndex;
    for (pictureIndex = 0; pictureIndex < 0x78; pictureIndex++) {
        if (mActive[pictureIndex] != 0) {
            unsigned char pictureState = mKind[pictureIndex];
            (this->*sLuigiSlotHandlers[pictureState])(pictureIndex);
        }
    }
}

// @symbol _ZN12dScMgLuigi_c10BeginCatchEi
void dScMgLuigi_c::BeginCatch(int found) {
    int roundInSet;
    mIconX = (short)(mPosX[mTarget - 1] >> 0xc);
    mIconY = (short)(mPosY[mTarget - 1] >> 0xc);
    mIconOn = 0x60;
    mState = STATE_CATCH;
    mCatchWait = 0xc8;
    roundInSet = mLevel;
    while (roundInSet >= 5)
        roundInSet -= 5;
    if (roundInSet != 4)
        *(unsigned short *)&mCatchWait += 8;
    if (found == 0)
        mCatchWait = 0x80;
    mFound = (unsigned char)found;
    StartIris();
    mIris[0].state = IRIS_GROW;
}

// @symbol _ZN12dScMgLuigi_c10CheckTouchEv
void dScMgLuigi_c::CheckTouch() {
    // local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
    extern unsigned char gActivePlayerSlot;
    // local extern: see above.
    extern unsigned char gTouchHeld[][4];
    // local extern: see above.
    extern unsigned char gTouchEdge[][4];
    // local extern: see above.
    extern unsigned char gTouchX[][4];
    // local extern: see above.
    extern unsigned char gTouchY[][4];
    extern unsigned short sLuigiTouchSfx[];
    int playerSlot = gActivePlayerSlot;
    int touchPressed = 0;
    int target;
    int pictureIndex;

    if (gTouchHeld[playerSlot][0] != 0) {
        if (gTouchEdge[playerSlot][0] != 0)
            touchPressed = 1;
    }
    if (touchPressed == 0)
        return;

    target = mTarget;
    {
        int dx = gTouchX[playerSlot][0] - (mPosX[target - 1] >> 12);
        int dy = gTouchY[playerSlot][0] - (mPosY[target - 1] >> 12);
        if (dx <= 0x10 && dx >= -0x10 && dy <= 0x10 && dy >= -0x10) {
            int level, soundTier;

            BeginCatch(1);
            target = mTarget;
            StartReward(target - 1);

            level = mLevel;
            soundTier = 0;
            if (level >= 0xa)
                soundTier = 2;
            else if (level >= 5)
                soundTier = 1;
            func_02012790(0x26);

            {
                int soundBase = sLuigiTouchSfx[soundTier];
                func_02012790(soundBase + (mWanted << 3));
            }
            return;
        }
    }

    for (pictureIndex = 0; pictureIndex < 0x78; pictureIndex++) {
        if (mActive[pictureIndex] == 1) {
            if (mKind[pictureIndex] != PICTURE_FLASH) {
                int dx2 = gTouchHeld[playerSlot][2] - (mPosX[pictureIndex] >> 12);
                int dy2 = gTouchHeld[playerSlot][3] - (mPosY[pictureIndex] >> 12);
                if (dx2 <= 0x10 && dx2 >= -0x10 && dy2 <= 0x10 && dy2 >= -0x10) {
                    AddPenalty(pictureIndex);
                    mFound = 0;
                    mKind[pictureIndex] = PICTURE_FLASH;
                    mBlink[pictureIndex] = 0x40;
                    if (*(u16 *)&mTime != 0) {
                        int targetIndex = mTarget;
                        int targetOffsetX = (mPosX[targetIndex - 1] >> 12) - 0x80;
                        int pan = (targetOffsetX * 0x30) >> 7;
                        if (pan >= 0x30)
                            pan = 0x30;
                        if (pan <= -0x30)
                            pan = -0x30;
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
/* One of four fixed boards, placed in a single call. The randomValue temporary and
   the per-arm loop locals are the spelling that matches; the tail loops
   keep the random call inline. */
void dScMgLuigi_c::PlaceBoardLayout() {
    extern int data_0209d4b8;
    extern u8 sLuigiBoardModes[];
    int layout;
    int randomValue;
    int i;

    layout = sLuigiBoardModes[mBoard];
    if (layout == 1) {
        int i;
        int pictureIndex;
        int column;
        int firstCharacter;
        pictureIndex = 0;
        randomValue = RandomIntInternal(&data_0209d4b8);
        firstCharacter = (((u32)randomValue >> 16) & 0x7fff) * 4 >> 15;
        randomValue = RandomIntInternal(&data_0209d4b8);
        mTarget = (((u32)randomValue >> 16) & 0x7fff) * 4 >> 15;
        for (i = 0; i < 2; i++) {
            for (column = 0; column < 2; column++) {
                mPosX[pictureIndex] = (0x70 + column * 0x20) << 12;
                mPosY[pictureIndex] = (0x50 + i * 0x20) << 12;
                mActive[pictureIndex] = 1;
                mKind[pictureIndex] = PICTURE_RESTART;
                mCharacter[pictureIndex] = (firstCharacter + pictureIndex) & 3;
                if (mWanted == mCharacter[pictureIndex])
                    mTarget = pictureIndex + 1;
                pictureIndex++;
            }
        }
    } else if (layout == 2) {
        int i;
        int pictureIndex;
        int column;
        pictureIndex = 0;
        randomValue = RandomIntInternal(&data_0209d4b8);
        mTarget = ((((u32)randomValue >> 16) & 0x7fff) * 16 >> 15) + 1;
        for (i = 0; i < 4; i++) {
            for (column = 0; column < 4; column++) {
                u8 characterOffset;
                mPosX[pictureIndex] = (0x50 + column * 0x20) << 12;
                mPosY[pictureIndex] = (0x30 + i * 0x20) << 12;
                mActive[pictureIndex] = 1;
                mKind[pictureIndex] = PICTURE_RESTART;
                randomValue = RandomIntInternal(&data_0209d4b8);
                mCharacter[pictureIndex] = (((u32)randomValue >> 16) & 0x7fff) * 4 >> 15;
                if (mWanted == mCharacter[pictureIndex]) {
                    randomValue = RandomIntInternal(&data_0209d4b8);
                    characterOffset = (((u32)randomValue >> 16) & 0x7fff) * 3 >> 15;
                    mCharacter[pictureIndex] += characterOffset + 1;
                    mCharacter[pictureIndex] &= 3;
                }
                if (pictureIndex == mTarget - 1)
                    mCharacter[pictureIndex] = mWanted;
                pictureIndex++;
            }
        }
    } else if (layout == 3) {
        int i;
        int pictureIndex;
        int column;
        pictureIndex = 0;
        randomValue = RandomIntInternal(&data_0209d4b8);
        mTarget = ((((u32)randomValue >> 16) & 0x7fff) * 0x30 >> 15) + 1;
        for (i = 0; i < 6; i++) {
            for (column = 0; column < 8; column++) {
                u8 characterOffset;
                mPosX[pictureIndex] = (0x10 + column * 0x20) << 12;
                mPosY[pictureIndex] = (0x10 + i * 0x20) << 12;
                mActive[pictureIndex] = 1;
                mKind[pictureIndex] = PICTURE_RESTART;
                randomValue = RandomIntInternal(&data_0209d4b8);
                mCharacter[pictureIndex] = (((u32)randomValue >> 16) & 0x7fff) * 4 >> 15;
                if (mWanted == mCharacter[pictureIndex]) {
                    randomValue = RandomIntInternal(&data_0209d4b8);
                    characterOffset = (((u32)randomValue >> 16) & 0x7fff) * 3 >> 15;
                    mCharacter[pictureIndex] += characterOffset + 1;
                    mCharacter[pictureIndex] &= 3;
                }
                if (pictureIndex == mTarget - 1)
                    mCharacter[pictureIndex] = mWanted;
                pictureIndex++;
            }
        }
    } else {
        int pictureIndex;
        int i;
        pictureIndex = 0;
        randomValue = RandomIntInternal(&data_0209d4b8);
        mTarget = ((((u32)randomValue >> 16) & 0x7fff) * 8 >> 15) + 1;
        for (i = 0; i < 8; i++) {
            u8 characterOffset;
            mPosX[pictureIndex] = (0x10 + i * 0x20) << 12;
            mPosY[pictureIndex] = -(5 << 12);
            mActive[pictureIndex] = 1;
            mKind[pictureIndex] = PICTURE_RESTART;
            randomValue = RandomIntInternal(&data_0209d4b8);
            mCharacter[pictureIndex] = (((u32)randomValue >> 16) & 0x7fff) * 3 >> 15;
            if (mWanted == mCharacter[pictureIndex]) {
                randomValue = RandomIntInternal(&data_0209d4b8);
                characterOffset = ((((u32)randomValue >> 16) & 0x7fff) * 2 >> 15) + 1;
                mCharacter[pictureIndex] += characterOffset;
                if (mCharacter[pictureIndex] >= 3)
                    mCharacter[pictureIndex] = 0;
            }
            if (pictureIndex == mTarget - 1)
                mCharacter[pictureIndex] = mWanted;
            pictureIndex++;
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
/* One picture per call, or a whole fixed board. character and targetFirst
   stay as pointers: character selection and the final capacity test reload them.
   opt_propagation off keeps this in the right registers. */
void dScMgLuigi_c::PlaceNextPicture() {
    extern int data_0209d4b8;
    extern u8 sLuigiBoardFlags[];
    extern u8 sLuigiBoardModes[];
    extern u16 sLuigiPlaceLimits[];
    int boardIndex;
    int pictureIndex;
    int capacity;
    int col;
    int row;
    int attempts;
    int randomValue;

    if (mFilled != 0)
        return;

    boardIndex = mBoard;
    if (sLuigiBoardModes[boardIndex] != 0) {
        PlaceBoardLayout();
        return;
    }

    capacity = sLuigiPlaceLimits[boardIndex];
    pictureIndex = mTarget;

    randomValue = RandomIntInternal(&data_0209d4b8);
    mCharacter[pictureIndex] = (((u32)randomValue >> 16) & 0x7fff) * 4 >> 15;
    {
        u8 *character = &mCharacter[pictureIndex];
        if (mWanted == mCharacter[pictureIndex]) {
            randomValue = RandomIntInternal(&data_0209d4b8);
            *character += ((((u32)randomValue >> 16) & 0x7fff) * 3 >> 15) + 1;
            *character &= 3;
        }
    }

    if (pictureIndex == 0) {
        int jitterX;
        int jitterY;
        do {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        } while (col >= 6 && col <= 10 && row >= 4 && row <= 8);

        jitterY = -2;
        jitterX = jitterY;
        jitterX += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
        jitterY += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;

        if (sLuigiBoardFlags[boardIndex] != 0) {
            mPosX[0] = (col * 20 + (jitterX + 8)) << 12;
            mPosY[0] = (row * 20 + (jitterY + 16)) << 12;
            mActive[0] = 1;
            mKind[0] = PICTURE_RESTART;
            mCharacter[0] = mWanted;
            mGrid[col][row] = 1;
            mTarget++;
            return;
        } else {
            int targetIndex = capacity - 1;
            mPosX[targetIndex] = (col * 20 + 8) << 12;
            mPosY[targetIndex] = (row * 20 + 16) << 12;
            mActive[targetIndex] = 1;
            mKind[targetIndex] = PICTURE_RESTART;
            mCharacter[targetIndex] = mWanted;
            mGrid[col][row] = 1;
        }
    }

    attempts = 0;
    while (1) {
        u8 *cell;
        if (attempts >= 100) {
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
        cell = ((dScMgLuigi_c *)((char *)this + col * 9 + row))->mGrid[0];
        if (*cell == 0) {
            int jitterX;
            int jitterY;
            jitterY = -2;
            jitterX = jitterY;
            jitterX += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            jitterY += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            mPosX[pictureIndex] = (col * 20 + (jitterX + 8)) << 12;
            mPosY[pictureIndex] = (row * 20 + (jitterY + 16)) << 12;
            *cell = 1;
            break;
        }
        attempts++;
    }

    mActive[pictureIndex] = 1;
    mKind[pictureIndex] = PICTURE_RESTART;
    mTarget++;

    const u8 *targetFirst = &sLuigiBoardFlags[boardIndex];
    if (mTarget >= (sLuigiBoardFlags[boardIndex] != 0 ? capacity : capacity - 1)) {
        if (boardIndex == 0xe || boardIndex == 0x10 || boardIndex == 0x11 || boardIndex == 0x13) {
            mPosX[pictureIndex] = mPosX[capacity - 1];
            mPosY[pictureIndex] = mPosY[capacity - 1] - 0x14000;
        }
        mFilled = 1;
        if (*targetFirst != 0)
            mTarget = 1;
        else
            mTarget = capacity;
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
void dScMgLuigi_c::ChooseTarget() {
    extern int data_0209d4b8;
    extern u8 sLuigiWantedPicks[];
    extern u16 sLuigiWantedPosterTiles[];
    s32 tileRow;
    s32 choiceOffset;
    u16 *dst;
    u16 sourceTile;
    u16 t;
    s32 tileColumn;
    s32 rowOffset;
    s32 tileBytes;
    /* A plain u16 here is 8 bytes short of the tile copy. */
    volatile u16 tile;

    t = mTime;
    choiceOffset = 0;
    if (t >= 0x14) {
        choiceOffset = 0x14;
    } else if (t >= 0xa) {
        choiceOffset = 0xa;
    }

    mWanted =
        (sLuigiWantedPicks +
         choiceOffset)[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >>
                       15];

    t = mBoard;
    if (t == 9) {
        mWanted = 1;
    }
    if (t == 0xe || t == 0x10 || t == 0x11 || t == 0x13) {
        if (mWanted == 3) {
            mWanted =
                (u8)((((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3)) >> 15);
        }
    }

    tileBytes = 2;
    for (tileRow = 0, rowOffset = 0; tileRow < 8; tileRow++, rowOffset += 0x20) {
        dst = (u16 *)(G2::GetBG3ScrPtr() + 0x1d8) + rowOffset;
        sourceTile = sLuigiWantedPosterTiles[mWanted];
        sourceTile += rowOffset;
        for (tileColumn = 0; tileColumn < 8; tileColumn++) {
            tile = ((u16 *)G2::GetBG3ScrPtr())[sourceTile];
            MultiStore16(tile, (char *)dst, tileBytes);
            sourceTile++;
            dst++;
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c11ChooseBoardEv
void dScMgLuigi_c::ChooseBoard() {
    extern int data_0209d4b8;
    int boardIndex;
    mPrevBoard = mBoard;
    boardIndex = mLevel;
    if (boardIndex >= 0x14) {
        int pick =
            (int)(((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 0xf);
        if (pick + 0xa == mPrevBoard) {
            int step =
                (int)((((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9) >>
                       0xf)) +
                1;
            pick += step;
            if (pick >= 0xa)
                pick -= 0xa;
        }
        boardIndex = pick + 0xa;
    }
    mBoard = boardIndex;
}

// @symbol _ZN12dScMgLuigi_c10ResetBoardEv
void dScMgLuigi_c::ResetBoard() {
    s32 pictureIndex;
    s32 penaltyIndex;
    s32 curtainIndex;
    s32 column;
    s32 row;

    for (pictureIndex = 0; pictureIndex < 120; pictureIndex++) {
        mPosX[pictureIndex] = 0;
        mPosY[pictureIndex] = 0;
        mVelX[pictureIndex] = 0;
        mVelY[pictureIndex] = 0;
        mKind[pictureIndex] = PICTURE_RESTART;
        mActive[pictureIndex] = 0;
        mCharacter[pictureIndex] = 0;
        mBlink[pictureIndex] = 0;
        mMovePhase[pictureIndex] = 0;
        mShown[pictureIndex] = 0;
    }

    for (penaltyIndex = 0; penaltyIndex < 16; penaltyIndex++) {
        mPenalty[penaltyIndex].x = 0;
        mPenalty[penaltyIndex].y = 0;
        mPenalty[penaltyIndex].timer = 0;
        mPenalty[penaltyIndex].active = 0;
        mPenalty[penaltyIndex].shown = 0;
    }

    for (curtainIndex = 0; curtainIndex < 2; curtainIndex++) {
        mCurtain[curtainIndex].active = 0;
        mCurtain[curtainIndex].shown = 0;
    }

    mReward.active = 0;
    mReward.shown = 0;
    mIris[0].active = 0;
    mIris[0].x = 0;
    mIris[0].y = 0;
    mIris[0].state = IRIS_HOLD;
    mIris[0].radius = 0;

    for (column = 0; column < 13; column++) {
        for (row = 0; row < 9; row++) {
            mGrid[column][row] = 0;
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

void dScMgLuigi_c::StateCatch() {
    extern LuigiSlotHandler sLuigiSlotHandlers[];
    int flashingCount;
    int pictureIndex;

    UpdateIris();
    UpdatePenalties();
    UpdateReward();

    flashingCount = 0;
    for (pictureIndex = 0; pictureIndex < 0x78; pictureIndex++) {
        if (mActive[pictureIndex] != 0) {
            int pictureState = mKind[pictureIndex];
            if (pictureState == PICTURE_FLASH) {
                flashingCount++;
                (this->*sLuigiSlotHandlers[pictureState])(pictureIndex);
            }
        }
    }
    if (flashingCount != 0)
        return;

    for (pictureIndex = 0; pictureIndex < 0x78; pictureIndex++) {
        if (mShown[pictureIndex] == 1)
            mShown[pictureIndex] = 0;
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
        int roundInSet = mLevel;
        while (roundInSet >= 5)
            roundInSet -= 5;
        if (roundInSet != 4)
            StartRound(-1);
        else
            Mg_SetSharedState(MG_STATE_NEXT_ROUND);
    } else {
        Mg_SetSharedState(MG_STATE_GAME_OVER);
    }
    mPromptEnabled = 0;
}
#pragma pop

// @symbol _ZN12dScMgLuigi_c9StatePlayEv
void dScMgLuigi_c::StatePlay() {
    TickTimer();
    if (mState == STATE_CATCH)
        return;
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
    mState = STATE_PLAY;
    mBoardPending = 0;
    unk_516c = 0xa;
    unk_516e = 0;
}

// @symbol _ZN12dScMgLuigi_c10StateSetupEv
void dScMgLuigi_c::StateSetup() {
    mIconOn = 0;
    ResetCurtains();
    mState = STATE_PLACE;
    ChooseTarget();
}

/* Engine calls used by scene setup and rendering. */

extern "C" {
/* dScMgLuigi_c::StartRound */
extern void FreeGfxSlotsById(int arg);
extern void Mg_SubmitScore(int);
extern void Mg_ShowBanner(int, int, int, int, int, short);
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
/* dScMgLuigi_c::Render */
extern void Mg_DrawScoreTally(void *c, int a, int b, int d);
/* dScMgLuigi_c::Behavior */
extern LuigiStateHandler sLuigiStateTable[];
/* dScMgLuigi_c::InitResources */
extern void *sLuigiBg1NcgPath;
extern void *Ov004_LoadFileByPath(const char *path);
extern void DecompressLZ16(void *src, void *dst);
extern int LoadFile(int handle);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void Ov004_Deallocate(void *ptr);
extern int func_02054d88(void);
extern void func_02056314(void *, u32, u32);
extern void func_02056374(const void *, u32, u32);
extern void Deallocate(void *);
extern void Mg_SetTopScreenYOffset(int);
extern int sMgResultsBannerX;
extern int sMgResultsBannerY;
}

// @symbol _ZN12dScMgLuigi_c10StartRoundEi
/* Same signature as dScMgBase_c::StartRound, or this stops overriding it.
   mLevel is re-read through an int* so the compare is not folded away. */
void dScMgLuigi_c::StartRound(int mode) {
    char *p;
    int *level;

    if (mFound != 0) {
        mClears += 1;
        level = (int *)&mLevel;
        *level += 1;
        if ((unsigned int)*(int *)&mLevel > 0x270e)
            *(int *)&mLevel = 0x270e;
    }

    if (mode == 0x12) {
        mTime = 0xa;
        *(int *)&mLevel = 0;
        if ((unsigned int)*(int *)&mLevel > 0x270e)
            *(int *)&mLevel = 0x270e;

        if (gMgScene != 0)
            gMgScene->mHudScore = 0;

        p = (char *)gMgScene;
        Mg_SubmitScore(p != 0 ? ((dScMgBase_c *)p)->mHudScore : 0);
    }

    FreeGfxSlotsById(0x1d);
    ResetBoard();
    ChooseBoard();

    mState = STATE_SETUP;
    data_0209d45c |= 4;
    data_0209d454 &= ~4;

    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
}

// @symbol _ZN12dScMgLuigi_c6RenderEv
s32 dScMgLuigi_c::Render() {
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
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
s32 dScMgLuigi_c::Behavior() {
    int state = mState;
    (this->*sLuigiStateTable[state])();
    return 1;
}

// @symbol _ZN12dScMgLuigi_c13InitResourcesEv
s32 dScMgLuigi_c::InitResources() {
    char *screenMap;
    /* Plain u16 fills are 16 bytes short of these two tile copies. */
    volatile u16 fillMain;
    volatile u16 fillSub;
    void *arc;
    int fileData;
    int objectTiles;

    arc = Ov004_LoadFileByPath((const char *)&sLuigiBg1NcgPath);
    if (arc == 0)
        return 0;

    data_0209d45c |= 8;
    *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & ~3) | 2;
    *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & 0x43) | 0x1218;

    DecompressLZ16(arc, (void *)func_02054d88());

    fileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI1_NCL_BIN);
    GX::LoadBGPltt((const void *)fileData, 0x60, 0x1a0);
    Deallocate((void *)fileData);

    fileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI1_NSC_BIN);
    func_02056314((void *)fileData, 0, 0x800);
    Deallocate((void *)fileData);

    data_0209d45c |= 4;
    *(volatile u16 *)0x0400000c &= ~3;
    *(volatile u16 *)0x0400000c &= ~0x40;
    *(volatile u32 *)0x04000018 = 0;
    *(volatile u16 *)0x0400000c = (*(volatile u16 *)0x0400000c & 0x43) | 0x1410;

    fileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI2_NCG_BIN);
    DecompressLZ16((void *)fileData, (void *)(G2::GetBG2CharPtr() + 0x4000));
    Deallocate((void *)fileData);

    fileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI2_NCL_BIN);
    GX::LoadBGPltt((const void *)fileData, 0x40, 0x20);
    Deallocate((void *)fileData);

    screenMap = G2::GetBG2ScrPtr();
    fillMain = 0x23e1;
    MultiStore16(fillMain, screenMap, 0x800);

    objectTiles = LoadFile(ASSET_D_2D_MG_LUIGI_NCG_BIN);
    fileData = LoadFile(ASSET_D_2D_MG_LUIGI_NCL_BIN);
    DecompressLZ16((void *)objectTiles, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)fileData, 0, 0x100);

    *(volatile u16 *)0x0400100c &= ~3;
    *(volatile u16 *)0x0400100c &= ~0x40;
    *(volatile u32 *)0x04001018 = 0;
    *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & 0x43) | 0x1210;

    {
        int subFileData;
        subFileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI2_NCG_BIN);
        DecompressLZ16((void *)subFileData, (void *)(G2S::GetBG2CharPtr() + 0x4000));
        Deallocate((void *)subFileData);

        subFileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI2_NCL_BIN);
        GXS::LoadBGPltt((const void *)subFileData, 0x1e0, 0x20);
        Deallocate((void *)subFileData);

        subFileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI2_NSC_BIN);
        func_02056374((const void *)subFileData, 0, 0x800);
        Deallocate((void *)subFileData);

        data_0209d454 |= 8;
        *(volatile u16 *)0x0400100e &= ~3;
        *(volatile u16 *)0x0400100e = (*(volatile u16 *)0x0400100e & 0x43) | 0x1410;

        subFileData = LoadFile(ASSET_D_2D_MG_BG_LUIGI2_BLACK_NSC_BIN);
        screenMap = G2S::GetBG3ScrPtr();
        fillSub = 0xf3e1;
        MultiStore16(fillSub, screenMap, 0x800);
        Deallocate((void *)subFileData);
    }

    DecompressLZ16((void *)objectTiles, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)fileData, 0, 0x100);
    Deallocate((void *)objectTiles);
    Deallocate((void *)fileData);

    mClears = 0;
    ResetBoard();
    mBoard = 0xff;
    ChooseBoard();

    mState = STATE_SETUP;
    Ov004_Deallocate(arc);
    Mg_SetTopScreenYOffset(0x30);
    mTime = 0xa;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    sMgResultsBannerX = 0x80;
    sMgResultsBannerY = ~0x1b;
    mHudScore = 0;
    return 1;
}

// @symbol dScMgLuigi_c_classInit
extern "C" {
/* Allocates the scene. The ROM has no spelling for this factory. */
dScMgLuigi_c *dScMgLuigi_c_classInit(void) {
    return new dScMgLuigi_c;
}
}

/* The state tables, in the order the static initializer fills them. Each
   entry's {function, 0} record is a compiler temporary in .data. */
LuigiStateHandler sLuigiStateTable[4] = {
    &dScMgLuigi_c::StateSetup,
    &dScMgLuigi_c::StatePlace,
    &dScMgLuigi_c::StatePlay,
    &dScMgLuigi_c::StateCatch,
};
LuigiSlotHandler sLuigiSlotHandlers[10] = {
    &dScMgLuigi_c::RestartPicture,           &dScMgLuigi_c::StopPicture,
    &dScMgLuigi_c::MovePictureDriftFixed,    &dScMgLuigi_c::MovePictureDriftByLevel,
    &dScMgLuigi_c::MovePictureDriftByColumn, &dScMgLuigi_c::MovePictureDriftByRow,
    &dScMgLuigi_c::MovePictureDriftRandom,   &dScMgLuigi_c::MovePictureSway,
    &dScMgLuigi_c::MovePictureBounce,        &dScMgLuigi_c::TickPictureFlash,
};
LuigiSlotHandler sLuigiCurtainTable[3] = {
    &dScMgLuigi_c::CurtainStart,
    &dScMgLuigi_c::CurtainUpdate,
    &dScMgLuigi_c::CurtainStateSlow,
};
LuigiSlotHandler sLuigiCurtainStateTable[3] = {
    &dScMgLuigi_c::CurtainStateBounce,
    &dScMgLuigi_c::CurtainStateHold,
    &dScMgLuigi_c::CurtainStateFast,
};
LuigiSlotHandler sLuigiIrisTable[3] = {
    &dScMgLuigi_c::IrisHold,
    &dScMgLuigi_c::IrisGrow,
    &dScMgLuigi_c::IrisStop,
};

/* MG_LUIGI's base profile: the factory, then the same priority in both
   halfwords. */
BaseSpawnInfo<dScMgLuigi_c> g_profile_MG_LUIGI = {
    dScMgLuigi_c_classInit,
    0x16e,
    0x16e,
};

/* ChooseTarget's tile-number table, indexed by the wanted character. */
u16 sLuigiWantedPosterTiles[4] = {0x0308, 0x0300, 0x0318, 0x0310};
