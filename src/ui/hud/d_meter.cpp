//cpp
/* dMeter_c -- the in-game HUD (METER), ov002 0x020fb8f8..0x020fe190.
 *
 * One translation unit, twenty functions, the way the cartridge's own build
 * had it: nineteen members, which replaced nineteen one-function shards, plus
 * the METER registry factory. The members' bodies are
 * unchanged apart from spelling: the four members include/dMeter_c.h did not
 * declare are now real methods rather than hand-mangled extern "C" free
 * functions, the offset casts that reached this object's fields are now the
 * field names, and the destructor is inline in the header.
 *
 * dMeter_c derives from dBase_c, and the cartridge says so itself: the
 * __si_class_type_info record _ZTI8dMeter_c at ov002:0x0210c224 points its name
 * word at _ZTS8dMeter_c (0x0210c218, the ten bytes "8dMeter_c" and a NUL) and
 * its base word at arm9:0x02086e78, which is _ZTI7dBase_c. So the class name is
 * the ROM's own, not a coinage. _ZTV8dMeter_c is an eighteen-slot table, the
 * fBase_c shape dBase_c passes down unchanged, and six of those slots plus both
 * destructor variants resolve into this range.
 *
 * THIS TU OWNS THE CLASS VTABLE. The destructor is inline in the class body, so
 * the key function is InitResources -- the first virtual this class declares out
 * of line -- and that anchors _ZTV8dMeter_c (ov002 public address point
 * 0x0210c2c8, storage 0x0210c2c0) here, together with the typeinfo and type-name
 * records it drags in for the whole ancestor chain (dMeter_c, dBase_c, fBase_c).
 * None of that data lies inside this entry's licensed .text range, and it could
 * not be licensed as one interval even if it were: the cartridge puts four
 * unnamed sprite tables between this class's typeinfo pair at 0x0210c218 and the
 * vtable storage at 0x0210c2c0, and g_profile_METER immediately below the pair.
 * So dsd keeps supplying the cartridge's own bytes and production isolation
 * discards the emitted duplicates; the manifest's compiler_only_output block
 * licenses each one at its measured ROM home.
 *
 * Inlining the destructor is also what puts D1 and D0 in cartridge order.
 * Written out of line, mwcc emits the synthesized D0 ahead of the written D1 and
 * the licensed .text is no longer ROM-ascending; inlining additionally deletes
 * the homeless D2 that no module gives a symbol to.
 *
 * The registry factory dMeter_c_classInit (0x020fe154..0x020fe190) closes the
 * unit, written as `return new dMeter_c();`. dMap_c's translation unit ends
 * immediately below, at 0x020fb8f8; src/game/stages/LevelObjects.cpp starts
 * immediately above, at 0x020fe190.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address ROM
 * function is written first. Do not reorder.
 *
 * leftovers:
 * - The HUD globals are data_ names: symbols.txt has no recovered name for them.
 * - The OAM::Render overloads that take Fix12<int> by value keep their mangled
 *   spellings: the natural static member call homes the Fix12 arguments
 *   differently under mwccarm (the Fix12 wall) and rewrites every call site.
 * - InitResources keeps one launder: the volatile pun on mVsTimer in the
 *   gCurrentSublevel == 0x2B branch (a plain member store emits different code).
 * - gGameMode is CURRENT_GAMEMODE (symbols/verified.tsv); mode 1 is the one
 *   dMap_c draws its VS icons for.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dMeter_c_AssetId {
    ASSET_D_2D_BTM_ICON_NCG_BIN = 0x0229,
    ASSET_D_2D_BTM_ICON_NCL_BIN = 0x022a,
    ASSET_D_2D_CMN_ICON_E_NCG_BIN = 0x0269,
    ASSET_D_2D_CMN_ICON_F_NCG_BIN = 0x026a,
    ASSET_D_2D_CMN_ICON_G_NCG_BIN = 0x026c,
    ASSET_D_2D_CMN_ICON_I_NCG_BIN = 0x026e,
    ASSET_D_2D_CMN_ICON_S_NCG_BIN = 0x0270
};

#include "dMeter_c.h"
#include "OAM.h"
#include "nitro/gx/gx_load.h"
#include "Timer.h"
#include "decl_common.h"
#include "dFdBrightness_c.h"
#include "types.h"
#include "decl_Player.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "dScStage_c.h"
#include "PlayerInput.h"
#include "SaveData.h"
#include "private/controller_input.h"
#include "Sound.h"
#include "Event.h"
#include "nitro/gx/gx_bgcnt.h"

/* The declarations below are the union of what the nineteen shards declared
 * locally, minus everything a shared header already supplies. What is left is
 * the sprite layer and a handful of globals the tree has not named yet; each
 * entry says why it cannot come from a shared header. */

struct OamAttr;
struct Matrix2x2;

extern "C" {

/* RenderTimeTimer draws gRaceTimer only while this is set. The writers
   start that timer with it: daPly_c on surface type 15 and the Koopa the
   Quick race (daRNk_c). InitResources clears it. */
extern unsigned char gShowRaceTimer;
extern Timer gRaceTimer;
extern void OamRender10_d_meter(int sub, struct OamAttr *attr, int x, int y, int a, int b, int sx,
                                int sy, int c, int d);
extern volatile unsigned short data_ov002_0210c208[];
extern unsigned char gLocalPlayerId;
extern unsigned char gMeterCounterSlideState;
extern signed char gNumLives;
struct OamAttr;
extern daPly_c *gPlayers[];
extern u8 gZoomButtonActiveTimer;
extern dCamera_c *gCamera;
extern ControllerInput data_0209f498[];
extern int data_0208ee44;
extern u32 gNextActorUpdateMask;
extern u8 data_0209d660;
extern signed char gNumStageStars[];
/* ov001's OAM::NUMBERS, not ov000's -- ov000 shares this load window with ov002 and
* so can never be the module this reaches. */
extern int data_ov001_020abac8[];
extern unsigned char gGameMode;
extern unsigned char gPrevReturnState;
extern unsigned char gNewStarCollected;
extern unsigned char gLevelClearStep;
extern int data_020a0db0;
}

extern unsigned char NumStars(void);
extern "C" {

extern signed char gNumRedCoins[];
extern signed char gCurrentSublevel;
}

extern int SublevelToLevel(int i);
extern "C" {

extern unsigned char gGamePaused;
extern unsigned char data_0209f20c;
extern unsigned char gVsExitScreenActive;
extern unsigned char data_0209f204;
extern struct OamAttr *data_ov002_0210ca0c[];
}

extern int GetOwnerLanguage(void);
extern "C" {

extern unsigned char data_0209f2bc;
extern unsigned short data_ov002_02111188;
extern unsigned char gSubScreenLayers;
extern unsigned char gMeterShownHealth[];
extern unsigned char gShowTouchScreenArrows;
extern dFdBrightness_c *gSceneFader;
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
}

void Deallocate(void *handle);
void func_0203da4c(void);

/* Mode 1 is versus mode. This int-returning predicate is what makes the ROM
   materialize 0/1 before testing it. */
static inline int IsVsMode(void) {
    return gGameMode == 1;
}

/* 0 on the off frames of the blink after a new star is collected
   (data_020a0db0 is FRAME_TIMER).
   Flattening the nested ifs into one && chain changes the ROM's code. */
static inline int StarCountVisible(void) {
    if (gNewStarCollected != 0) {
        if (gLevelClearStep == 3 && (data_020a0db0 & 0x18) == 0)
            return 0;
        return 1;
    }
    return 1;
}

/* ROM ordinal 19 -- dMeter_c_classInit, 0x020fe154, size 0x3c.
 * Reconstructed source-style name: SM64DS proves dMeter_c through RTTI,
 * allocation size, vtable identity, and the METER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: HUD_Spawn. It was once labelled
 * the dMeter_c constructor, but it takes no `this`: it is the registry factory. */
dMeter_c *dMeter_c_classInit() {
    return new dMeter_c();
}

/* ROM ordinal 18 -- dMeter_c::InitResources(), 0x020fda04, size 0x750 */
int dMeter_c::InitResources() {
    if (IsVsMode()) {
        void *h;
        if (GetOwnerLanguage() == 5) {
            h = LoadFile(0xB003);
            DecompressLZ16(h, G2_GetOBJCharPtr());
            DecompressLZ16(h, G2S_GetOBJCharPtr());
            Deallocate(h);
            h = LoadFile(0xB007);
            DecompressLZ16(h, (u8 *)G2_GetOBJCharPtr() + 0x2000);
            Deallocate(h);
        } else if (GetOwnerLanguage() == 4) {
            h = LoadFile(0xAC03);
            DecompressLZ16(h, G2_GetOBJCharPtr());
            DecompressLZ16(h, G2S_GetOBJCharPtr());
            Deallocate(h);
            h = LoadFile(0xAC07);
            DecompressLZ16(h, (u8 *)G2_GetOBJCharPtr() + 0x2000);
            Deallocate(h);
        } else if (GetOwnerLanguage() == 3) {
            h = LoadFile(0xA803);
            DecompressLZ16(h, G2_GetOBJCharPtr());
            DecompressLZ16(h, G2S_GetOBJCharPtr());
            Deallocate(h);
            h = LoadFile(0xA807);
            DecompressLZ16(h, (u8 *)G2_GetOBJCharPtr() + 0x2000);
            Deallocate(h);
        } else if (GetOwnerLanguage() == 2) {
            h = LoadFile(0xA403);
            DecompressLZ16(h, G2_GetOBJCharPtr());
            DecompressLZ16(h, G2S_GetOBJCharPtr());
            Deallocate(h);
            h = LoadFile(0xA407);
            DecompressLZ16(h, (u8 *)G2_GetOBJCharPtr() + 0x2000);
            Deallocate(h);
        } else {
            h = LoadFile(0xA003);
            DecompressLZ16(h, G2_GetOBJCharPtr());
            DecompressLZ16(h, G2S_GetOBJCharPtr());
            Deallocate(h);
            h = LoadFile(0xA007);
            DecompressLZ16(h, (u8 *)G2_GetOBJCharPtr() + 0x2000);
            Deallocate(h);
        }
        h = LoadFile(0x8000);
        DecompressLZ16(h, (u8 *)G2S_GetOBJCharPtr() + 0x2000);
        Deallocate(h);
        h = LoadFile(0x8002);
        GX_LoadOBJPltt(h, 0, 0x120);
        GXS_LoadOBJPltt(h, 0, 0x120);
        Deallocate(h);
        h = LoadFile(0x8003);
        GX_LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
        h = LoadFile(0x8001);
        GXS_LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
    } else {
        void *h;
        if (GetOwnerLanguage() == 5)
            h = LoadFile(ASSET_D_2D_CMN_ICON_S_NCG_BIN);
        else if (GetOwnerLanguage() == 4)
            h = LoadFile(ASSET_D_2D_CMN_ICON_I_NCG_BIN);
        else if (GetOwnerLanguage() == 3)
            h = LoadFile(ASSET_D_2D_CMN_ICON_G_NCG_BIN);
        else if (GetOwnerLanguage() == 2)
            h = LoadFile(ASSET_D_2D_CMN_ICON_F_NCG_BIN);
        else
            h = LoadFile(ASSET_D_2D_CMN_ICON_E_NCG_BIN);
        DecompressLZ16(h, G2_GetOBJCharPtr());
        DecompressLZ16(h, G2S_GetOBJCharPtr());
        Deallocate(h);
        if (GetOwnerLanguage() == 5)
            h = LoadFile(0xB00E);
        else if (GetOwnerLanguage() == 4)
            h = LoadFile(0xAC0E);
        else if (GetOwnerLanguage() == 3)
            h = LoadFile(0xA80E);
        else if (GetOwnerLanguage() == 2)
            h = LoadFile(0xA40E);
        else
            h = LoadFile(0xA00E);
        DecompressLZ16(h, (u8 *)G2_GetOBJCharPtr() + 0x2000);
        Deallocate(h);
        h = LoadFile(ASSET_D_2D_BTM_ICON_NCG_BIN);
        DecompressLZ16(h, (u8 *)G2S_GetOBJCharPtr() + 0x2000);
        Deallocate(h);
        if (GetOwnerLanguage() == 5)
            h = LoadFile(0xB00A);
        else if (GetOwnerLanguage() == 4)
            h = LoadFile(0xAC0A);
        else if (GetOwnerLanguage() == 3)
            h = LoadFile(0xA80A);
        else if (GetOwnerLanguage() == 2)
            h = LoadFile(0xA40A);
        else
            h = LoadFile(0xA00A);
        GX_LoadOBJPltt(h, 0, 0x120);
        GXS_LoadOBJPltt(h, 0, 0x120);
        Deallocate(h);
        h = LoadFile(0x980F);
        GX_LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
        h = LoadFile(ASSET_D_2D_BTM_ICON_NCL_BIN);
        GXS_LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
    }

    unk62 = 0;
    unk64 = 0xB4;
    mVsTimerY = 0xA;
    if (IsVsMode()) {
        if (gCurrentSublevel == 0x33)
            mVsTimer = 0x1E;
        else if (gCurrentSublevel == 0x2B)
            *(volatile u16 *)&mVsTimer = 0x1E;
        else
            mVsTimer = 0x1E;
    } else {
        mVsTimer = 0x1E;
    }

    data_ov002_02111188 = 0xB4;
    gShowRaceTimer = 0;
    gMeterShownHealth[0] = gPlayers[gLocalPlayerId]->GetHealth();
    if (gMeterShownHealth[0] == 8) {
        mHealthMeterY = -0x18;
        unk72 = 0;
        mHealthMeterState = 0;
    } else {
        mHealthMeterY = 0x18;
        mHealthMeterState = 1;
    }

    /* Nine pixels per digit of NumStars(). */
    int starDigitsWidth = 0;
    if (!IsVsMode()) {
        int n = NumStars();
        if (n > 0x63)
            starDigitsWidth = 0x1B;
        else if (n > 9)
            starDigitsWidth = 0x12;
        else
            starDigitsWidth = 9;
    }

    if (IsVsMode()) {
        gMeterCounterSlideState = 0;
        mLifeCountX = 0x10;
        mStarCountX = 0xF0;
        mStarCountSlideIn = 0;
    } else if (!(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE)) {
        gMeterCounterSlideState = 6;
        mLifeCountX = -0x3A;
        mStarCountX = (s16)(starDigitsWidth + 0x120);
        mStarCountSlideIn = 1;
    } else if (gPrevReturnState == 2) {
        gMeterCounterSlideState = 1;
        mLifeCountX = -0x3A;
        mStarCountX = 0xF0;
        mStarCountSlideIn = 0;
    } else if (gPrevReturnState == 1) {
        gMeterCounterSlideState = 2;
        mLifeCountX = -0x3A;
        mStarCountX = (s16)(starDigitsWidth + 0x120);
        mStarCountSlideIn = 0;
    } else {
        gMeterCounterSlideState = 0;
        mLifeCountX = 0x10;
        mStarCountX = 0xF0;
        mStarCountSlideIn = 0;
    }

    func_0203da4c();
    return 1;
}

/* ROM ordinal 17 -- dMeter_c::Behavior(), 0x020fd7a4, size 0x260 */
int dMeter_c::Behavior() {
    int vs;

    /* vs is computed only once the first test passes and is reused below. */
    if (gSceneFader->IsAtStart() == 0 ||
        ((vs = IsVsMode()) == 0 && (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0))
        return 1;
    if (vs != 0) {
        UpdateVsTimer();
    } else {
        int flags;
        int n;

        UpdateHealthMeter();
        n = NumStars();
        flags = 0;
        if (n > 0x63)
            n = 0x1b;
        else if (n > 9)
            n = 0x12;
        else
            n = 9;

        switch (gMeterCounterSlideState) {
        case 2:
            gMeterCounterSlideState = 3;
            break;
        case 3:
            if (mLifeCountX >= 0x10) {
                mLifeCountX = 0x10;
                flags = 1;
            } else {
                mLifeCountX += 4;
            }
            if (gPrevReturnState == 1) {
                if (mStarCountX <= 0xf0) {
                    mStarCountX = 0xf0;
                    flags |= 2;
                } else {
                    mStarCountX -= 4;
                }
            } else {
                flags |= 2;
            }
            if (flags == 3)
                gMeterCounterSlideState = 4;
            break;
        case 5:
            if (mHealthMeterHoldTimer == 0) {
                if (mLifeCountX < -0x38) {
                    flags = 1;
                } else {
                    mLifeCountX -= 4;
                }
                if (gPrevReturnState == 1) {
                    if (mStarCountX <= n + 0x120) {
                        mStarCountX += 4;
                    } else {
                        flags |= 2;
                    }
                } else {
                    flags |= 2;
                }
                if (flags == 3)
                    gMeterCounterSlideState = 6;
            }
            break;
        case 6:
            if (mLifeCountX >= 0x10) {
                mLifeCountX = 0x10;
                flags = 1;
            } else {
                mLifeCountX += 4;
            }
            if (gPrevReturnState == 1 || mStarCountSlideIn != 0) {
                if (mStarCountX <= 0xf0) {
                    mStarCountX = 0xf0;
                    flags |= 2;
                } else {
                    mStarCountX -= 4;
                }
            } else {
                flags |= 2;
            }
            if (flags == 3) {
                gMeterCounterSlideState = 0;
                mStarCountSlideIn = 0;
            }
            break;
        }
    }
    return 1;
}

/* ROM ordinal 16 -- dMeter_c::Render(), 0x020fd5e0, size 0x1c4 */
int dMeter_c::Render() {
    if (IsVsMode()) {
        if (data_0209fc9c == 0) {
            if (((gGamePaused | data_0209f20c | gVsExitScreenActive) & 0xff) == 0) {
                RenderVsTimer();
                RenderStarCount();
                RenderCoinCount();
                RenderCameraButtons();
            } else {
                if (data_0209f204 != 0) {
                    RenderVsTimer();
                }
            }
        }
    } else if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) != 0) {
        unsigned char v = data_0209f20c;
        if (((gGamePaused | v | gVsExitScreenActive) & 0xff) == 0) {
            RenderHealthMeter();
            if (Event::GetBit(EVENT_COLLECTING_STAR) == 0) {
                RenderCoinCount();
                RenderRedCoins();
                RenderSilverStars();
                RenderTimeTimer();
            }
            if (gShowTouchScreenArrows != 0) {
                dScStage_c::RenderBouncingArrows();
            }
        } else {
            if (v != 0) {
                RenderLifeCount();
                RenderStarCount();
            }
        }
        unsigned char v2 = data_0209f20c;
        if (((gGamePaused | v2 | gVsExitScreenActive) & 0xff) == 0 ||
            (v2 != 0 && gLevelClearStep < 3)) {
            RenderStarCount();
            RenderCameraButtons();
            RenderLifeCount();
        }
    }
    return 1;
}

/* ROM ordinal 15 -- dMeter_c::OnPendingDestroy(), 0x020fd5dc, size 0x4 */
/* dMeter_c::OnPendingDestroy() at 0x020fd5dc (ov002) -- vtable slot 12.
 * Empty override; the dMeter_c does nothing when marked for destruction.
 */
void dMeter_c::OnPendingDestroy() {}

/* ROM ordinal 14 -- dMeter_c::CleanupResources(), 0x020fd5d4, size 0x8 */
/* dMeter_c::CleanupResources() at 0x020fd5d4 (ov002) -- vtable slot 3.
 * Returns VS_FAIL (1); the dMeter_c holds no SharedFilePtr/heap resources to
 * release on death. dMeter_c : dBase_c : fBase_c.
 */
s32 dMeter_c::CleanupResources() {
    return 1;
}

/* ROM ordinal 13 -- dMeter_c::UpdateHealthMeter(), 0x020fd218, size 0x3bc */
void dMeter_c::UpdateHealthMeter() {
    int chr = gLocalPlayerId;
    daPly_c *player = gPlayers[chr];
    unsigned int health = (gPlayerHealth[chr] >> 8) & 0xff;
    if ((unsigned char)(gGamePaused | data_0209f20c | gVsExitScreenActive) != 0)
        return;
    if (mHealthMeterHoldTimer != 0)
        mHealthMeterHoldTimer -= data_0208ee44;
    if (mHealthTickTimer != 0)
        mHealthTickTimer -= data_0208ee44;
    if (Event::GetBit(EVENT_COLLECTING_STAR)) {
        unsigned char s = mHealthMeterState;
        if (s != 0) {
            if (s < 5)
                mHealthMeterState = 5;
        }
    }
    switch (mHealthMeterState) {
    case 0:
        if (player->mIsUnderwater != 0) {
            mHealthMeterY = 0x39;
            mHealthMeterState = 1;
            return;
        }
        if (gMeterShownHealth[0] <= health)
            return;
        mHealthMeterY = 0x39;
        gMeterShownHealth[0]--;
        mHealthTickTimer = 0x1e;
        mHealthMeterHoldTimer = 0x5a;
        mHealthMeterState = 1;
        return;
    case 1:
        if (mHealthMeterHoldTimer == 0) {
            if (mHealthMeterY != 0x19) {
                mHealthMeterY -= 4;
                if (mHealthMeterY <= 0x19)
                    mHealthMeterY = 0x19;
            }
        }
        if (mHealthTickTimer != 0)
            return;
        {
            unsigned char cur = gMeterShownHealth[0];
            if (cur < health) {
                if ((int)(health - cur) > 1) {
                    gMeterShownHealth[0] = cur + 1;
                    func_02012790(0xd);
                    mHealthTickTimer = 0x10;
                    return;
                }
                gMeterShownHealth[0] = health;
                func_02012790(0xd);
                if (player->mIsUnderwater != 0)
                    return;
                if (health != 8)
                    return;
                if (gMeterCounterSlideState == 1) {
                    gMeterCounterSlideState = 3;
                    mHealthMeterState = 2;
                } else {
                    mHealthMeterHoldTimer = 0x3c;
                    mHealthMeterState = 4;
                }
                return;
            }
            if (cur > health) {
                gMeterShownHealth[0] = cur - 1;
                mHealthTickTimer = 0x1e;
                return;
            }
            if (cur != 8)
                return;
            if (player->mIsUnderwater != 0)
                return;
            if (player->IsInAir())
                return;
            if (gMeterCounterSlideState == 1) {
                gMeterCounterSlideState = 3;
                mHealthMeterState = 2;
            } else {
                mHealthMeterHoldTimer = 0x3c;
                mHealthMeterState = 4;
            }
        }
        return;
    case 2:
        if (gMeterCounterSlideState == 4) {
            mHealthMeterHoldTimer = 0x3c;
            mHealthMeterState = 3;
        }
        return;
    case 3:
        if (mHealthMeterHoldTimer != 0)
            return;
        GiveLives(-1);
        func_02012790(0x40);
        gMeterCounterSlideState = 5;
        mHealthMeterHoldTimer = 0x3c;
        mHealthMeterState = 4;
        return;
    case 4:
    case 5:
        if (mHealthMeterHoldTimer != 0)
            return;
        mHealthMeterY -= 4;
        if (mHealthMeterY >= -0x18)
            return;
        mHealthMeterY = -0x18;
        if (mHealthMeterState == 4)
            mHealthMeterState = 0;
        else
            mHealthMeterState = 6;
        return;
    case 6:
        if (Event::GetBit(EVENT_COLLECTING_STAR) == 0)
            mHealthMeterState = 1;
        return;
    }
}

/* ROM ordinal 12 -- dMeter_c::RenderHealthMeter(), 0x020fcfec, size 0x22c */
void dMeter_c::RenderHealthMeter() {
    if (gPlayers[gLocalPlayerId]->IsInsideOfCannon())
        return;

    if (GetOwnerLanguage() == 5) {
        OAM::Render(0, OAM::HEALTH_METERS_ES[gMeterShownHealth[0]], 0x80, mHealthMeterY, -1, 1,
                    0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::HEALTH_METER_BG_ES, 0x80, mHealthMeterY, -1, 1, 0x1000, 0x1000, 0, -1);
    } else if (GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
        OAM::Render(0, OAM::HEALTH_METERS_FR_IT[gMeterShownHealth[0]], 0x80, mHealthMeterY, -1, 1,
                    0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::HEALTH_METER_BG_FR_IT, 0x80, mHealthMeterY, -1, 1, 0x1000, 0x1000, 0,
                    -1);
    } else {
        OAM::Render(0, OAM::HEALTH_METERS[gMeterShownHealth[0]], 0x80, mHealthMeterY, -1, 1, 0x1000,
                    0x1000, 0, -1);
        OAM::Render(0, &OAM::HEALTH_METER_BG, 0x80, mHealthMeterY, -1, 1, 0x1000, 0x1000, 0, -1);
    }

    GX_LoadOBJPltt(&data_ov002_0210c310[data_ov002_0210c29c[gMeterShownHealth[0]]], 0x60, 0x20);
}

/* ROM ordinal 11 -- dMeter_c::UpdateVsTimer(), 0x020fce9c, size 0x150 */
void dMeter_c::UpdateVsTimer() {
    unsigned int v;

    if (data_0209f2bc != 0)
        return;
    if (((gGamePaused | data_0209f20c | gVsExitScreenActive) & 0xff) != 0)
        return;

    if (data_ov002_02111188 != 0)
        data_ov002_02111188 = data_ov002_02111188 - data_0208ee44;

    if (data_0209f204 != 0)
        return;

    if (data_ov002_02111188 == 0) {
        data_ov002_02111188 = 0xb4;
        mVsTimer -= 1;
        v = mVsTimer;
        if (v <= 3 && v != 0)
            func_02012790(0x28);
        if (mVsTimer == 0) {
            data_0209f204 = 1;
            data_ov002_02111188 = 0x3c;
            func_02012790(0x29);
            gSubScreenLayers &= ~4;
        }
    }

    if (mVsTimer > 3)
        return;
    if (mVsTimerY < 0x30) {
        mVsTimerY += 4;
    } else {
        mVsTimerY = 0x38;
    }
}

/* ROM ordinal 10 -- dMeter_c::RenderVsTimer(), 0x020fca18, size 0x484 */
/* The VS-mode countdown: a localised "TIME" caption, and below it either the
 * remaining seconds or a localised "TIME UP".
 *
 * Both captions are chosen by a GetOwnerLanguage() ladder testing 5, 4, 3, 2
 * and falling through to OAM::VS_TIME / OAM::VS_TIME_UP -- so the OAM symbols
 * are the DEFAULT artwork and the _FR/_DE/_IT/_ES blocks are translations.
 * The ladder re-calls GetOwnerLanguage() at every rung rather than caching it;
 * that is the ROM's own shape and is preserved rather than tidied.
 *
 * Digit layout is not uniform, which is the interesting part:
 *   >= 10   two glyphs at x=0x74 and x=0x84, recoloured to palette 0xb once
 *           mVsTimer <= 5 -- the last-five-seconds warning
 *   4..9    ONE glyph at x=0x7c, centred, there being no tens digit
 *   0..3    not a digit at all: data_ov002_0210ca0c[3 - ones], a separate
 *           countdown artwork set, indexed backwards
 *
 * Only the caption is drawn when any of gGamePaused / f20c / f294 is set;
 * they are OR-ed and tested as a single byte.
 */
void dMeter_c::RenderVsTimer() {
    int t = mVsTimer;

    if ((unsigned char)(gGamePaused | data_0209f20c | gVsExitScreenActive) == 0) {
        if (GetOwnerLanguage() == 5) {
            OAM::Render(0, &OAM::VS_TIME_ES, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 4) {
            OAM::Render(0, &OAM::VS_TIME_IT, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 3) {
            OAM::Render(0, &OAM::VS_TIME_DE, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 2) {
            OAM::Render(0, &OAM::VS_TIME_FR, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            OAM::Render(0, &OAM::VS_TIME, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (data_0209f204 == 0) {
        int pal = (mVsTimer <= 5) ? 0xb : -1;
        unsigned short tens = t / 10;
        if (tens != 0) {
            OAM::Render(false, OAM::VS_YELLOW_NUMBERS[tens], 0x74, mVsTimerY, pal, -1, 0);
            OAM::Render(false, OAM::VS_YELLOW_NUMBERS[t % 10], 0x84, mVsTimerY, pal, -1, 0);
        } else {
            int ones = t % 10;
            if (ones > 3) {
                OAM::Render(0, OAM::VS_YELLOW_NUMBERS[ones], 0x7c, mVsTimerY, -1, -1, 0x1000, 0);
            } else {
                OAM::Render(false, data_ov002_0210ca0c[3 - ones], 0x84, mVsTimerY, -1, -1, 0);
            }
        }
    } else {
        if (GetOwnerLanguage() == 5) {
            OAM::Render(0, &OAM::VS_TIME_UP_ES, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 4) {
            OAM::Render(0, &OAM::VS_TIME_UP_IT, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 3) {
            OAM::Render(0, &OAM::VS_TIME_UP_DE, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 2) {
            OAM::Render(0, &OAM::VS_TIME_UP_FR, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            OAM::Render(0, &OAM::VS_TIME_UP, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }
}

/* ROM ordinal 9 -- dMeter_c::RenderCoinCount(), 0x020fc81c, size 0x1fc */
void dMeter_c::RenderCoinCount() {
    if (IsVsMode()) {
        int sb = 0xf0;
        CalculateDigits(gNumCoins[gLocalPlayerId]);
        for (int i = 2; i >= 0; i--) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(true, OAM::NUMBERS[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(true, &OAM::TIMES, sb, 0xa, -1, 1, 0);
        OAM::Render(true, &OAM::COIN, sb - 0x10, 2, -1, 1, 0);
    } else {
        if (SublevelToLevel(gCurrentSublevel) == 0x1d)
            return;
        int i;
        int sb = 0xf0;
        CalculateDigits(gNumCoins[gLocalPlayerId]);
        for (i = 2; i >= 0; i--) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(false, OAM::NUMBERS[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(false, &OAM::TIMES, sb, 0xa, -1, 1, 0);
        OAM::Render(false, &OAM::COIN, sb - 0x10, 2, -1, 1, 0);
    }
}

/* ROM ordinal 8 -- dMeter_c::RenderRedCoins(), 0x020fc77c, size 0xa0 */
/* Draws one sprite per red coin collected so far, stepping 0xb across from
 * x=0x10. Both the count (gNumRedCoins) and the row selector (gNumStageStars)
 * are indexed by the same gLocalPlayerId.
 *
 * That row selector is the interesting bit: it is the SAME byte
 * dMeter_c::RenderSilverStars reads as its star COUNT. Non-zero there means silver
 * stars are being drawn at y=0xa, so the red coins drop to y=0x1c to make room.
 * One byte serving as both a count and an occupancy flag for a shared row.
 *
 * The count is read through an explicit unsigned char cast off a signed char
 * array -- kept as the ROM spells it. RenderSilverStars does not read this
 * array; it reads gNumStageStars.
 *
 * TOUCHES NO FIELD; the ROM body takes no `this` and now ignores the one it is
 * passed. Byte-free.
 */
void dMeter_c::RenderRedCoins() {
    int idx = gLocalPlayerId;
    int flag = gNumStageStars[idx];
    int count = (unsigned char)gNumRedCoins[idx];
    int sl = flag ? 0x1c : 0xa;
    for (int i = 0; i < count; i++) {
        OAM::Render(false, &OAM::RED_COIN, 0x10 + i * 0xb, sl, -1, 1, (Matrix2x2 *)0);
    }
}

/* ROM ordinal 7 -- dMeter_c::RenderStarCount(), 0x020fc458, size 0x324 */
/* Draws the star counter: the star icon, an "x", and up to three digits laid
 * out RIGHT to LEFT from mStarCountX, skipping the -1 slots CalculateDigits
 * blanked.
 *
 * Three paths, and what differs between them is only WHICH count is shown and
 * on WHICH screen:
 *
 *   gGameMode == 1       a per-slot count out of gNumStageStars, main screen
 *   the middle path      NumStars(), main screen, but only while
 *                        gMeterCounterSlideState has the counter sliding onto,
 *                        resting on or sliding off the main screen, and suppressed
 *                        entirely by the gNewStarCollected / gLevelClearStep test
 *   otherwise            NumStars(), SUB screen -- the leading argument to
 *                        OAM::Render flips 0 -> 1
 *
 * The three render blocks are deliberately left duplicated rather than folded
 * into a helper: they are three separate tails in the ROM, and hoisting them
 * would be a rewrite, not a substitution.
 *
 * The digit loop runs i = 2 down to 0 -- least significant first -- which is
 * why the cursor moves left.
 */
void dMeter_c::RenderStarCount() {
    int x = mStarCountX;
    int i;

    if (IsVsMode()) {
        CalculateDigits((unsigned short)gNumStageStars[gLocalPlayerId]);
        for (i = 2; i >= 0; i--) {
            if (mDigits[i] >= 0) {
                OAM::Render(false, OAM::NUMBERS[mDigits[i]], x, 2, -1, 1, 0);
                x -= 9;
            }
        }
        OAM::Render(false, &OAM::TIMES, x, 10, -1, 1, 0);
        OAM::Render(false, &OAM::POWER_STAR, x - 16, 10, -1, 1, 0);
        return;
    }

    CalculateDigits(NumStars());

    {
        unsigned char mode = gPrevReturnState;
        if ((mode != 2 && gMeterCounterSlideState == 4) ||
            (mode == 1 && gMeterCounterSlideState >= 3 && gMeterCounterSlideState < 6)) {
            if (!StarCountVisible()) {
                return;
            }
            for (i = 2; i >= 0; i--) {
                if (mDigits[i] >= 0) {
                    OAM::Render(false, OAM::NUMBERS[mDigits[i]], x, 2, -1, 1, 0);
                    x -= 9;
                }
            }
            OAM::Render(false, &OAM::TIMES, x, 10, -1, 1, 0);
            OAM::Render(false, &OAM::POWER_STAR, x - 16, 10, -1, 1, 0);
            return;
        }
    }

    for (i = 2; i >= 0; i--) {
        if (mDigits[i] >= 0) {
            OAM::Render(true, OAM::NUMBERS[mDigits[i]], x, 2, -1, 1, 0);
            x -= 9;
        }
    }
    OAM::Render(true, &OAM::TIMES, x, 10, -1, 1, 0);
    OAM::Render(true, &OAM::POWER_STAR, x - 16, 10, -1, 1, 0);
}

/* ROM ordinal 6 -- dMeter_c::RenderSilverStars(), 0x020fc3c4, size 0x94 */
/* Draws one sprite per silver star collected, in a row starting at x=0x10 and
 * stepping 0x11 across at y=0xa. The count comes from gNumStageStars indexed by
 * gLocalPlayerId -- the same byte dMeter_c::RenderRedCoins reads, where a non-zero
 * value instead pushes the red-coin row from y=0xa down to y=0x1c. So this row
 * and that one share a slot, and the red coins move when silver stars occupy it.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this` at all; as a method it now
 * receives one in r0 and ignores it, which is byte-free here. Nothing about the
 * ROM says this was a static -- unlike OAM's methods, whose call sites pass no
 * `this` -- so it is left a normal member.
 */
void dMeter_c::RenderSilverStars() {
    int sl = 0;
    unsigned short n = (unsigned short)(short)gNumStageStars[gLocalPlayerId];
    int sb;
    if (sl < n) {
        sb = 0x10;
        do {
            OAM::Render(0, (OamAttr *)data_ov001_020abac8, sb, 0xa, -1, 1, (Matrix2x2 *)0);
            sl++;
            sb += 0x11;
        } while (sl < n);
    }
}

/* ROM ordinal 5 -- dMeter_c::RenderCameraButtons(), 0x020fc04c, size 0x378 */
/* Takes the object only because its caller in this TU passes it; the body
 * never reads it, which is why the legacy source could spell it static. */
void dMeter_c::RenderCameraButtons() {
    int idx = gActivePlayerSlot;
    u8 mode = data_0209f498[idx].mode;
    dCamera_c *cam = gCamera;
    u8 t = gZoomButtonActiveTimer;

    if (t != 0)
        gZoomButtonActiveTimer = t - data_0208ee44;

    if (gNextActorUpdateMask & ACTOR_UPDATE_MASK_STAGE_ENDING)
        return;

    if (cam->mFlags & 0x1000) {
        if (mode == 0) {
            if (data_0209f498[idx].held & 0x200) {
                OAM::RenderSub(&OAM::CAM_BUTTON_L_PRESSED, 0x19, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_L_PRESSED, 0xbf, 0xa5, -1, 1);
            } else {
                OAM::RenderSub(&OAM::CAM_BUTTON_L, 0x19, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_L, 0xbf, 0xa5, -1, 1);
            }
            if (data_0209f498[gActivePlayerSlot].held & 0x100) {
                OAM::RenderSub(&OAM::CAM_BUTTON_R_PRESSED, 0x41, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_R_PRESSED, 0xe7, 0xa5, -1, 1);
            } else {
                OAM::RenderSub(&OAM::CAM_BUTTON_R, 0x41, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_R, 0xe7, 0xa5, -1, 1);
            }
        } else if (mode == 2) {
            if (data_0209f498[idx].held & 0x200) {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_L_PRESSED, 0xbf, 0xad, -1, 1);
            } else {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_L, 0xbf, 0xad, -1, 1);
            }
            if (data_0209f498[gActivePlayerSlot].held & 0x100) {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_R_PRESSED, 0xe7, 0xad, -1, 1);
            } else {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_R, 0xe7, 0xad, -1, 1);
            }
        }
    }

    if (mode == 0)
        return;
    if (data_0209d660 != 0)
        return;

    int y = (mode == 1) ? 0xa5 : 0x85;
    if (mode == 1) {
        if (gZoomButtonActiveTimer != 0) {
            OAM::RenderSub(&OAM::CAM_ZOOM_BUTTON_PRESSED, 0xe7, y, -1, 1);
            return;
        }
        OAM::RenderSub(&OAM::CAM_ZOOM_BUTTON, 0xe7, y, -1, 1);
        return;
    }
    if (gZoomButtonActiveTimer != 0) {
        OAM::RenderSub(&OAM::S_CAM_ZOOM_BUTTON_PRESSED, 0xe7, y, -1, 1);
        return;
    }
    OAM::RenderSub(&OAM::S_CAM_ZOOM_BUTTON, 0xe7, y, -1, 1);
}

/* ROM ordinal 4 -- dMeter_c::RenderLifeCount(), 0x020fbe38, size 0x214 */
void dMeter_c::RenderLifeCount() {
    unsigned char idx = gLocalPlayerId;
    daPly_c *player = gPlayers[idx];
    unsigned char state = gMeterCounterSlideState;

    if (state >= 3 && state < 6) {
        OAM::Render(0, OAM::LIFE_ICONS[player->mCharacter], mLifeCountX, 0xa, -1, 1, 0x1000, 0x1000,
                    0, -1);
        OAM::Render(false, &OAM::TIMES, mLifeCountX + 0x10, 0xa, -1, 1, 0);
        CalculateDigits((unsigned short)gNumLives);
        int x = mLifeCountX + 0x18;
        for (int i = 0; i < 3; i++) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(false, OAM::NUMBERS[d], x, 2, -1, 1, 0);
                x += 9;
            }
        }
    } else {
        OAM::RenderSub(OAM::LIFE_ICONS[player->mCharacter], mLifeCountX, 0xa, -1, 1);
        OAM::Render(true, &OAM::TIMES, mLifeCountX + 0x10, 0xa, -1, 1, 0);
        CalculateDigits((unsigned short)gNumLives);
        int i;
        int x = mLifeCountX + 0x18;
        for (i = 0; i < 3; i++) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(true, OAM::NUMBERS[d], x, 2, -1, 1, 0);
                x += 9;
            }
        }
    }
}

/* ROM ordinal 3 -- dMeter_c::CalculateDigits(unsigned short), 0x020fbdac, size 0x8c */
/* Splits a count into the three decimal digits the dMeter_c draws, most significant
 * first, and blanks leading zeros by writing -1 rather than a digit.
 *
 * The suppression stops at i != 2, so the units digit is always drawn: a value
 * of 0 renders "0", not blank. `flag` latches on the first non-zero digit, so
 * an interior zero (as in 101) is drawn normally.
 *
 * The divisor table is volatile in the ROM's own spelling and stays that way --
 * it forces the 100/10/1 loads to be re-read per iteration instead of folded
 * into immediates, which is what makes this a loop at all.
 */
void dMeter_c::CalculateDigits(unsigned short value) {
    int flag = 0;
    int i;
    for (i = 0; i < 3; i++) {
        int digit = value / data_ov002_0210c208[i];
        if (digit == 0 && flag == 0 && i != 2) {
            mDigits[i] = -1;
        } else {
            mDigits[i] = digit;
            flag = 1;
        }
        value = value % data_ov002_0210c208[i];
    }
}

/* ROM ordinal 2 -- dMeter_c::RenderTimeTimer(), 0x020fb96c, size 0x440 */
void dMeter_c::RenderTimeTimer() {
    unsigned long long t;
    unsigned long long min;
    unsigned long long sec;
    unsigned long long centi;

    if (gShowRaceTimer == 0)
        return;

    t = gRaceTimer.GetTime() << 6;
    sec = t / 33514000;
    centi = t / 33514 % 1000 / 10;
    min = sec / 60;
    sec = sec % 60;

    if (min >= 100) {
        min = 99;
        sec = 59;
        centi = 99;
    }

    if (min / 10 != 0) {
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
            OAM::Render(0, &OAM::TIME_FR_IT_ES, 0xa4, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        } else {
            OAM::Render(0, &OAM::TIME, 0xa4, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        }
        OAM::Render(false, OAM::NUMBERS[min / 10], 0xb8, 0x16, -1, 1, 0);
    } else {
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
            OAM::Render(0, &OAM::TIME_FR_IT_ES, 0xac, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        } else {
            OAM::Render(0, &OAM::TIME, 0xac, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        }
    }

    OAM::Render(false, OAM::NUMBERS[min % 10], 0xc0, 0x16, -1, 1, 0);
    OAM::Render(false, &OAM::MINUTES, 0xc4, 0x1e, -1, 1, 0);
    OAM::Render(false, OAM::NUMBERS[sec / 10], 0xcf, 0x16, -1, 1, 0);
    OAM::Render(false, OAM::NUMBERS[sec % 10], 0xd7, 0x16, -1, 1, 0);
    OAM::Render(false, &OAM::SECONDS, 0xdb, 0x1e, -1, 1, 0);
    OAM::Render(false, OAM::NUMBERS[centi / 10], 0xe8, 0x16, -1, 1, 0);
    OAM::Render(false, OAM::NUMBERS[centi % 10], 0xf0, 0x16, -1, 1, 0);
}

/* ROM ordinal 1 -- dMeter_c::~dMeter_c() (deleting, D0), 0x020fb928, size 0x44 */
/* No body here, and none is possible: D0 is the deleting destructor and the
 * compiler writes all of it. The inline `~dMeter_c() {}` in include/dMeter_c.h
 * is the whole source, and mwccarm emits D1 and D0 from it in that order, which
 * is cartridge order, and emits no D2 at all.
 *
 * What the cartridge's 0x44 bytes do: the same two vptr stores and the same
 * fBase_c chain call as D1 below, then `ldr r1, =GAME_HEAP_PTR; ldr r1, [r1]`
 * and `bl Memory::Deallocate(void*, mHeap::Heap_t*)` (arm9 0x0203c1e8) before returning
 * the object in r0. So the deallocation is fBase_c's operator delete inlined
 * here, and it does name a heap -- the global game heap, loaded from
 * arm9 0x020a0eac.
 */

/* ROM ordinal 0 -- dMeter_c::~dMeter_c() (complete, D1), 0x020fb8f8, size 0x30 */
/* No body here either, for the same reason. The cartridge's 0x30 bytes are two
 * vtable stores and one call, and every one of the three follows from
 * `struct dMeter_c : dBase_c`:
 *
 *     str r2, [r4]     ; _ZTV8dMeter_c   0x0210c2c8
 *     str r1, [r4]     ; _ZTV7dBase_c    0x0208e4b8  -- dBase_c's dtor, INLINED
 *     bl  0x02043d48   ; fBase_c::~fBase_c() (base, D2)
 *
 * The second store is there because include/dBase_c.h defines that class's
 * destructor in its class body, so it inlines into every derived destructor.
 * There is no third store and no second call: dMeter_c adds no member with a
 * destructor of its own.
 */
