//cpp
/* Slot_machine scenes: Slot1 (three-reel slots) and Slot3 (the lamp/reel
 * variant), ov006 .text 0x0210a8c0..0x0210d740 -- the whole run, both
 * classes' every function, Slot1's factory included.
 *
 * _ZTS12dScMgSlot1_c 0x0213e5bc. _ZTI12dScMgSlot1_c 0x0213e5a0.
 * _ZTV12dScMgSlot1_c 0x0213eb40. _ZTS12dScMgSlot3_c 0x0213e5ac.
 * _ZTI12dScMgSlot3_c 0x0213e588. _ZTV12dScMgSlot3_c 0x0213eaa8.
 * Slot1: D1 at 0x0210a8c0, D0 at 0x0210a900. Slot3: D1 at 0x0210a954,
 * D0 at 0x0210a9a8.
 *
 * With defer_codegen off the optimizer brackets below bind to the functions
 * they enclose, and .text is emitted in source order, so this file is
 * written in ascending ROM order. Keep it.
 */

// leftovers:
// - Slot1 InitResources fills the reel strips through a char* cursor stepped
//   one 21-byte row per reel, cast back to the class for mReelStrip: the
//   member-indexed form mReelStrip[j][i] DIFFs (measured).
// - The ROM PMF constants the Slot3 state table copies (data_ov006_0213e5xx)
//   come from unlicensed .data, in retail copy order.
// - SaveScreenForMenu/RestoreScreenFromMenu keep slot-identity names from
//   the vtable; the OAM SECONDSE symbol read at Render is a link artifact of
//   the address, not a proven field.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgSlots_AssetId {
    ASSET_D_2D_BADWIZER_NSC_BIN = 0x0001,
    ASSET_D_2D_GRADE_NSC_BIN = 0x0004,
    ASSET_D_2D_MG_BG_PAUSE_NSC_BIN = 0x0067,
    ASSET_D_2D_MG_BG_SLOT_DOWN_NCG_BIN = 0x0072,
    ASSET_D_2D_MG_BG_SLOT_DOWN_NCL_BIN = 0x0073,
    ASSET_D_2D_MG_BG_SLOT_DOWN_NSC_BIN = 0x0074,
    ASSET_D_2D_MG_BG_SLOT_FACE02_NSC_BIN = 0x0075,
    ASSET_D_2D_MG_BG_SLOT_FACE_DOWN_NSC_BIN = 0x0076,
    ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN = 0x0077,
    ASSET_D_2D_MG_BG_SLOT_FACE_NCL_BIN = 0x0078,
    ASSET_D_2D_MG_BG_SLOT_FACE_NSC_BIN = 0x0079,
    ASSET_D_2D_MG_BG_SLOT_FACE_NUKI_NSC_BIN = 0x007a,
    ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN = 0x007b,
    ASSET_D_2D_MG_BG_SLOT_SCORE_UP_NSC_BIN = 0x007c,
    ASSET_D_2D_MG_BG_SLOT_UP_NCG_BIN = 0x007d,
    ASSET_D_2D_MG_BG_SLOT_UP_NCL_BIN = 0x007e,
    ASSET_D_2D_MG_BG_SLOT_UP_NSC_BIN = 0x007f,
    ASSET_D_2D_MG_SLOT_FACE_NCG_BIN = 0x00eb,
    ASSET_D_2D_MG_SLOT_FACE_NCL_BIN = 0x00ec,
    ASSET_D_2D_MG_SLOT_NCL_BIN = 0x00f1
};

#include "dScMgSlot1_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"
#include "dScMgSlot3_c.h"
#include "Sound.h"
#include "OAM.h"
#include "OamAttr.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "types.h"
#include "PlayerInput.h"
#include "dWipe_c.h"
#include "nitro/gx/g2.h"
#include "nitro/os/cache.h"
#include "nitro/gx/gx_load.h"
#include "LevelRng.h"
#include "nitro/mi/memory.h"
#include "math/CalcLib.h"

#pragma defer_codegen off
/* Codegen is deferred by default in mwccarm 2004/b56, which is why its
 * optimisation pragmas are otherwise file-global last-wins. With it off, the
 * positional brackets below bind to the functions they enclose -- and .text is
 * then emitted in SOURCE order, so this file is written in ASCENDING ROM order.
 */

typedef void (dScMgSlot3_c::*SlotState)();

/* A dScMgSlot3_c field's byte offset, for the sites that add it to an
   integer or stepped base the way the ROM does; the member spellings
   change those functions' code. */
#define SLOT3_OFF(member) ((int)&((dScMgSlot3_c *)0)->member)

/* One player's 4-byte touch record (PlayerInput.h): Slot1 Behavior reads
 * x/y through this view so the compiler, not the source, hoists them. */
struct TouchRec {
    u8 held;
    u8 edge;
    u8 x;
    u8 y;
};

extern "C" {
extern int sMgMainBannerY;
void SetBg1Offset(int a, int b);
int GetGameLanguage(void);
// Retained caller ABI: narrowing to the definition's u16 adds a mask in InitMainScreenBg.
unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char gMainScreenLayers;
void Mg_AddCredits(void *c, int delta);
void Mg_SubCredits(void *c, int val);
extern void Mg_SubmitScore(int self);
extern int data_ov006_0213e948[];
void OamRender10_d_s_mg_slots(int b, void *attr, int a2, int a3, int a4, int a5, int a6, int a7,
                              int a8, int a9);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void Mg_DrawCredits(char *a0, int a1, int a2, int a3);
void Mg_DrawScoreTally(char *a0, int a1, int a2, int a3);
extern void **sSlotLangSprites[];
extern SlotState sSlot3StateTable[];
/* The ROM PMF constants the Slot3 state table copies from (unlicensed
 * .data), in retail copy order. */
extern SlotState pmf_Slot3_StateIntro;
extern SlotState pmf_Slot3_StateSpin;
extern SlotState pmf_Slot3_StateJudge;
extern SlotState pmf_Slot3_StatePayout;
extern SlotState pmf_Slot3_StatePayout_2;
extern SlotState pmf_Slot3_StateWait;
extern SlotState pmf_Slot3_StateGameOver;
extern SlotState pmf_Slot3_StateDone;
extern void *LoadFile(int handle);

extern u8 gSubScreenLayers;
extern int data_0208ee44;
extern void RenderOamMainScreen(OamAttr *, int, int, int, int);
extern OamAttr sSlotsSlotBlinkSprites[];
extern void Mg_LoseCredits(int c);
void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *m);
extern void *data_ov006_0213e528[];
void Mg_GainCredits(int x);
extern int Mg_GetSavedRecord0(void);
extern int sSlot1ReelPos[][2];
extern void *sSlot1Sprites[];
extern unsigned char sSlot1Payouts[];
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void Mg_RenderSubSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void Mg_SaveRecordPair(int a, int b);
extern void Mg_PlayJingle(void *c);
extern int sSlotReelScrollSpeeds[];
extern u8 sSlot1Payouts[];
extern s16 sSlotMatchBlinkX[][2];
extern s16 sSlotMatchBlinkY[][2];
extern s16 sSlotWildBlinkX[][2];
extern s16 sSlotWildBlinkY[][2];
extern dWipe_c data_0209f61c;

extern int Mg_RenderSubSpriteScaled(void *a0, int a1, int a2, int a3, int a4, int a5,
                                    unsigned short a6);
}
extern int Sound_PlayIfNotActive(int, int, int, short);
extern void func_ov001_020ab3f0(void *c);
int TouchArea_Update(void *c, int x);
extern void func_ov001_020ab5b0(char *r0, int r1, short r2, short r3, short s4, short s5);

/* ~dScMgSlot1_c() (D1, complete-object destructor). The empty body is the
   original ownership model: the compiler destroys the typed mBetIcon member,
   emitting betIcon_c's then dThIcon_c's vtable writes, before chaining to the
   dScMgBase_c base destructor. */
dScMgSlot1_c::~dScMgSlot1_c() {}

/* The compiler emits D0 from the complete-object destructor above. */

/* The compiler destroys mMascot, then the inlined dScMgSingle3DBase_c
   destructor runs. */
dScMgSlot3_c::~dScMgSlot3_c() {}

/* The compiler emits D0 from the complete-object destructor above. */

/* dScMgSlot3_c::RestoreScreenFromMenu - slot 30 override: puts BG1CNT
   back, then runs the base restore. This body is the arity witness cited
   in include/dScMgBase_c.h's slot-30 block.  The read-modify-write below clobbers
   r1 before the tail call, so the base cannot be taking a second argument. */
void dScMgSlot3_c::RestoreScreenFromMenu() {
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                     GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
    dScMgBase_c::RestoreScreenFromMenu();
}

/* dScMgSlot3_c::SaveScreenForMenu - slot 29 override.

   This body is the third and strongest witness that slot 29 takes no
   explicit parameter.  It CLOBBERS r1 -- the masked read-modify-write of the
   sub display-control register below compiles to `ldrh r1,[r2]; and; orr;
   strh` -- and only then tail-branches into the base with `bx ip`.  A second
   argument passed in r1 would reach dScMgBase_c::SaveScreenForMenu as a
   display-control word, so the base cannot be reading one. */
int dScMgSlot3_c::SaveScreenForMenu() {
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                     GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    return dScMgBase_c::SaveScreenForMenu();
}

/* dScMgSlot3_c::InitMainScreenBg - slot 32, the only override of it in the family.

   Not an AfterClsn: the base slot has no ROM name at all, and the one the
   recovery pass borrowed belongs to dPathLiftActor_c, two forks away.  See the
   slot-32 block in include/dScMgBase_c.h.

   The base's body verbatim -- main BG1CNT reduced to 0x1000, scroll reset, BG1
   cleared from the main BG-enable shadow, this class's own language table and
   the shared screen map -- and then one more write, leaving BG1CNT at 0x1118
   instead of 0x1000: the same layer, pointed at this minigame's own character
   and screen base blocks. */
void dScMgSlot3_c::InitMainScreenBg() {
    int idx;

    G2_SetBG1Priority(0);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                     GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);

    SetBg1Offset(0, 0);

    gMainScreenLayers &= ~DISPLAY_LAYER_BG1;

    idx = GetGameLanguage();
    LoadCompressedFileAt(sSlotPauseBgNcgByLanguage[idx], G2_GetBG1CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_PAUSE_NSC_BIN, G2_GetBG1ScrPtr());

    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                     GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
}

// Keep the strided cursors cast back to the class. mStripLength is read by
// offset: a member read of it grows this body by 8 bytes (measured).
void dScMgSlot3_c::ReelDigits(int reelIndex) {
    char *c = (char *)this;
    int symbolIndex = (((dScMgSlot3_c *)(c + reelIndex * 4))->mReelPos[0] >> 12) / 80;
    int column;
    char *dst = c + reelIndex * 3;
    char *stripBytes = c + reelIndex * 5;
    for (column = 0; column < 3; column++) {
        ((dScMgSlot3_c *)dst)->mResultSymbols[0][0] =
            ((dScMgSlot3_c *)(stripBytes + symbolIndex))->mReelStrip[0][0];
        symbolIndex = (symbolIndex + 1) % *(unsigned char *)(c + SLOT3_OFF(mStripLength));
        dst += 1;
    }
}

void dScMgSlot3_c::StateDone() {}

void dScMgSlot3_c::StateGameOver() {
    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        mState = 7;
        Mg_SetSharedState(MG_STATE_GAME_OVER);
        mReelDifficulty = 0;
    }
    if (mStateTimer >= 0x3c)
        return;
    mReelWinPos[0] += 0x10000;
    mReelWinPos[1] -= 0x10000;
    mReelWinPos[2] += 0x10000;
}

void dScMgSlot3_c::StateWait() {}

void dScMgSlot3_c::StatePayout() {
    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        if (mState == 3) {
            if (mHudScore < 0x270f)
                mHudScore += 1;
            if (mHudScore > mBestHudScore)
                mBestHudScore = mHudScore;
            StartRound(4);
        } else {
            if (mCredits <= 0) {
                mStateTimer = 0x78;
                mState = 6;
            } else {
                StartRound(5);
            }
        }
    }

    if (mState == 3) {
        if (mStateTimer != 0x14)
            return;
        if (mPayoutRemaining <= 0)
            return;
        mPayoutRemaining -= 1;
        Mg_AddCredits(this, 1);
        Sound::PlayBank2_2D(0x149);
        mStateTimer += 5;
        return;
    }

    if (mState != 4)
        return;

    {
        unsigned char b = mStateTimer;
        if (b != 0x14 && b != 0x28 && b != 0x3c)
            return;
    }

    Mg_SubCredits(this, 1);
    Sound::PlayBank2_2D(0x14a);
}

void dScMgSlot3_c::StateJudge() {
    unsigned char columnSymbols[3];
    int column;
    int payout;
    int score;
    char *self;

    if (DecIfAbove0_Byte(&mStateTimer) != 0)
        return;

    self = (char *)this;
    for (column = 0; column < 3; column++) {
        int reelIndex;
        columnSymbols[column] = ((dScMgSlot3_c *)self)->mResultSymbols[0][column];
        reelIndex = 1;
        do {
            int symbolIndex = (column + (((dScMgSlot3_c *)self)->mReelPos[reelIndex] >> 12) / 80) %
                              ((dScMgSlot3_c *)self)->mStripLength;
            if (columnSymbols[column] !=
                ((dScMgSlot3_c *)self)->mReelStrip[reelIndex][symbolIndex]) {
                columnSymbols[column] = 5;
                break;
            }
        } while (++reelIndex < 3);
        // Retain the compiler address barrier; deleting it grows this body by 32 bytes.
        self = (char *)((unsigned)self | (unsigned)(column - column));
    }

    mWinColumn = -1;
    payout = 0;
    {
        int columnIndex;
        for (columnIndex = 0; columnIndex < 3; columnIndex++) {
            if (columnSymbols[columnIndex] == mWinSymbol) {
                if (columnIndex == 1) {
                    mWinColumn = columnIndex;
                    payout += 6;
                } else if (columnIndex == 0 || columnIndex == 2) {
                    mWinColumn = columnIndex;
                    payout += 3;
                }
                break;
            }
        }
    }

    score = mHudScore;
    if (payout > 0) {
        mPayoutRemaining = payout;
        mState = 3;
        func_02012790(0x26);
        mStateTimer = 0x28;
        score += 1;
    } else {
        mState = 4;
        func_02012790(0xe);
        mStateTimer = 0x50;
    }
    Mg_SubmitScore(score);

    mPromptEnabled = 0;
}

void dScMgSlot3_c::StateSpin() {
    u8 playerSlot;
    int touchPressed;
    int touchOffset;
    int reelIndex;
    playerSlot = gActivePlayerSlot;
    touchPressed = 0;
    touchOffset = playerSlot * 4;
    if (gTouchHeld[playerSlot * 4]) {
        if (gTouchEdge[touchOffset]) {
            touchPressed = 1;
        }
    }
    if (touchPressed != 0) {
        unsigned int touchY = gTouchY[playerSlot * 4];
        if (touchY >= 0x40 && touchY < 0x58) {
            if (mReelSpinning[0] == 1)
                mReelSpinning[0] = 0;
        } else if (touchY >= 0x58 && touchY < 0x70) {
            if (mReelSpinning[1] == 1)
                mReelSpinning[1] = 0;
        } else if (touchY >= 0x70 && touchY < 0x88) {
            if (mReelSpinning[2] == 1)
                mReelSpinning[2] = 0;
        }
    }
    for (reelIndex = 0; reelIndex < 3; reelIndex++) {
        if (mReelStopped[reelIndex] == 0) {
            int oldSymbol;
            int newSymbol;
            oldSymbol = (mReelPos[reelIndex] >> 12) / 0x50;
            if (mReelReverse[reelIndex] == 1) {
                mReelPos[reelIndex] -= data_ov006_0213e948[mStoppedReelCount * 3 + reelIndex];
                if (mReelPos[reelIndex] < 0)
                    mReelPos[reelIndex] += mReelPeriod;
            } else {
                mReelPos[reelIndex] += data_ov006_0213e948[mStoppedReelCount * 3 + reelIndex];
                if (mReelPos[reelIndex] >= mReelPeriod)
                    mReelPos[reelIndex] -= mReelPeriod;
            }
            if (mReelSpinning[reelIndex] == 0) {
                newSymbol = (mReelPos[reelIndex] >> 12) / 0x50;
                if (newSymbol != oldSymbol) {
                    if (mReelReverse[reelIndex] == 1)
                        mReelPos[reelIndex] = (oldSymbol * 0x50) << 12;
                    else
                        mReelPos[reelIndex] = (newSymbol * 0x50) << 12;
                    ReelDigits(reelIndex);
                    mReelStopped[reelIndex] = 1;
                    mStoppedReelCount++;
                    mFrameCounter = 0;
                    Sound::PlayBank2_2D(0x1a9);
                }
            }
        }
    }
    mSpinSoundHandle = Sound_PlayIfNotActive(mSpinSoundHandle, 2, 0x1a8, 0);
    if (mStoppedReelCount >= 3u) {
        mStateTimer = 0x1e;
        mState = 2;
    }
}

extern "C" {
void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6);
void FreeGfxSlotsById(int arg);
}

void dScMgSlot3_c::StateIntro() {
    if (mReelDrawY == 0x50) {
        mMascot.LookAt(0);
        Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    }

    if (mReelDrawY > 0) {
        mReelDrawY -= 1;
        if (mReelDrawY != 0)
            return;
        mWinSymbol = mNextWinSymbol;
        mMascot.StopLooking();
        return;
    }

    mStateTimer -= 1;
    if (mStateTimer != 0)
        return;

    FreeGfxSlotsById(0x1d);

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    mState++;
}

void dScMgSlot3_c::StartRound(int mode) {
    char *c = (char *)this;

    if (mode == 3 || mode == 0x12) {
        mCredits = 0xc;
        mCreditsShown = mCredits;
        mLevel = mReelDifficulty;
        if (mLevel > 0x270e) {
            mLevel = 0x270e;
        }
        mHudScore = 0;
        Mg_SubmitScore(mHudScore);
        Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    } else if (mode == 4) {
        if (mReelDifficulty < 5) {
            mReelDifficulty += 1;
        }
        mLevel += 1;
        if (mLevel > 0x270e) {
            mLevel = 0x270e;
        }
    }

    if (mode == 3 || mode == 0x12 || mode == 4) {
        int v;
        int v2;
        int i;
        int j;
        u8 k;
        int m;

        v = mReelDifficulty;
        if (v <= 0) {
            mStripLength = 3;
        } else if (v <= 2) {
            mStripLength = 4;
        } else {
            mStripLength = 5;
        }

        v2 = mReelDifficulty;
        if (v2 <= 0 || v2 == 1 || v2 == 3) {
            mReelReverse[0] = 0;
            mReelReverse[1] = 1;
            mReelReverse[2] = 0;
        } else if (v2 == 2 || v2 == 4) {
            mReelReverse[0] = 1;
            mReelReverse[1] = 0;
            mReelReverse[2] = 1;
        } else {
            for (i = 0; i < 3; i++) {
                if ((u8)(((u32)RandomIntInternal(&data_0209e650.value) >> 16) & 1)) {
                    mReelReverse[i] = 1;
                } else {
                    mReelReverse[i] = 0;
                }
            }
        }

        mNextWinSymbol = ((u32)RandomIntInternal(&data_0209e650.value) >> 16) % mStripLength;
        if (mode != 4) {
            mReelDrawY = 0;
            mWinSymbol = mNextWinSymbol;
        } else {
            mReelDrawY = 0x50;
            func_02012790(0x1aa);
        }

        {
            char *w = c;
            for (j = 0; j < 3; j++, w += 5) {
                for (k = 0; k < mStripLength; k++) {
                    if (j == 1) {
                        mReelStrip[1][k] = mStripLength - k - 1;
                    } else {
                        ((dScMgSlot3_c *)w)->mReelStrip[0][k] = k;
                    }
                }
            }
        }
        mReelPeriod = (mStripLength * 0x50) << 12;
    }

    {
        int j2;
        int k2;
        int m2;
        char *w2 = c;
        for (j2 = 0; j2 < 3; j2++, w2 += 3) {
            mReelSpinning[j2] = 1;
            mReelStopped[j2] = 0;
            for (k2 = 0; k2 < 3; k2++) {
                /* mResultSymbols[j2][k2]; a typed spelling grows the function. */
                *(u8 *)(w2 + k2 + SLOT3_OFF(mResultSymbols)) = mStripLength;
            }
        }
        for (m2 = 0; m2 < 3; m2++) {
            mReelWinPos[m2] = 0;
        }
        mStoppedReelCount = 0;
        mStateTimer = 0x3c;
        mState = 0;
        mWinColumn = -1;
        mPayoutRemaining = 0;
    }
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
// Kept: pragma-free member spellings match except the mReelPos[i] load, which needs it.
#pragma opt_strength_reduction off
/* dScMgSlot3_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the second of the two slots where this class's
 * table differs from dScMgSingle3DBase_c's. The old file's `recovered name:`
 * comment agreed, and here it is right.
 *
 * Draws the three reels -- the win-line pass at state 6 and the scrolling pass
 * otherwise -- then the payout markers, the dMeter_c, and the two swinging lamps whose
 * angles are mLamp1Angle/mLamp2Angle.
 *
 * The pragma is load-bearing, not tidying; the `(int)` launder on the two indexed
 * reads of mReelPos (0x4fe4) and mReelWinPos (0x4ff4) is there for the same
 * reason, and those two stay raw: every member spelling of them DIFFs
 * (measured). So do the row walks through `p` and the y counters in the two
 * reel passes and the mResultSymbols reads in the marker scan; mReelStopped[i]
 * and the plain members take their real spellings. */
s32 dScMgSlot3_c::Render() {
    char *c = (char *)this;
    int i;

    Camera_InitSlotsView(&mCamera);
    mMascot.Draw();

    if (mReelDrawY > 0) {
        for (i = 0; i < 3; i++) {
            OAM::Render(0, (OamAttr *)sSlot3FaceSprites[mWinSymbol * 3 + i], 0x80,
                        mReelDrawY + 0x10, -1, 2, 0x1000, 0x1000, 0, 1);
        }
        for (i = 0; i < 3; i++) {
            OAM::Render(0, (OamAttr *)sSlot3FaceSprites[mNextWinSymbol * 3 + i], 0x80,
                        mReelDrawY + 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    } else {
        for (i = 0; i < 3; i++) {
            OAM::Render(0, (OamAttr *)sSlot3FaceSprites[mWinSymbol * 3 + i], 0x80, 0x60, -1, 2,
                        0x1000, 0x1000, 0, 1);
        }
    }

    if (mState != 7) {
        if (mState == 6) {
            int i2, j2;
            char *p = c;
            for (i2 = 0; i2 < 3; i2++) {
                int y;
                for (j2 = 0, y = 0x30; j2 < 3; j2++) {
                    Hud_RenderSprite(
                        sSlot3FaceSprites[((dScMgSlot3_c *)(p + j2))->mResultSymbols[0][0] * 3 +
                                          i2],
                        y - (*(int *)((int)c + i2 * 4 + SLOT3_OFF(mReelWinPos)) >> 12), 0x60, -1,
                        -1);
                    y += 0x50;
                }
                p += 3;
            }
        } else {
            char *p = c;
            int row, rem;
            int i2, j2;
            for (i2 = 0; i2 < 3; i2++) {
                int a = *(int *)(((int)c + i2 * 4 + SLOT3_OFF(mReelPos))) >> 12;
                int y;
                row = a / 0x50;
                rem = a % 0x50;
                for (j2 = 0, y = 0x30; j2 < 4; j2++) {
                    Hud_RenderSprite(
                        sSlot3FaceSprites[((dScMgSlot3_c *)(p + row))->mReelStrip[0][0] * 3 + i2],
                        y - rem, 0x60, -1, -1);
                    row = (row + 1) % mStripLength;
                    y += 0x50;
                }
                p += 5;
            }
        }
    }

    if (mStoppedReelCount < 3 && (mFrameCounter & 0x20)) {
        int slot = 3;
        int ok = 1;
        int i3, j3;
        char *p = c;
        for (i3 = 0; i3 < 3; i3++) {
            if (mReelStopped[i3] != 0) {
                if (slot < 3) {
                    if (mWinSymbol != ((dScMgSlot3_c *)(p + slot))->mResultSymbols[0][0])
                        ok = 0;
                } else {
                    u8 t = mWinSymbol;
                    for (j3 = 0; j3 < 3; j3++) {
                        if (t == ((dScMgSlot3_c *)(p + j3))->mResultSymbols[0][0]) {
                            slot = j3;
                            break;
                        }
                        if (j3 == 2)
                            ok = 0;
                    }
                }
            }
            if (ok == 0)
                break;
            p += 3;
        }
        if (ok != 0 && slot < 3) {
            int count = 0;
            int i4;
            int y = 0x4c;
            for (i4 = 0; i4 < 3; i4++) {
                if (mReelStopped[i4] != 0) {
                    Hud_RenderSprite(&data_ov006_0213e5dc, slot * 0x50 + 0x30, y, -1, -1);
                    count++;
                }
                y += 0x18;
            }
            if ((mFrameCounter & 0x3f) == 0x20) {
                Sound::PlayBank2_2D(count > 1 ? 0x1ac : 0x1ab);
            }
        }
    }

    Mg_DrawCredits(c, 0xc, 0xc, 0);
    Mg_DrawScoreTally(c, 0xe0, 0x14, 1);

    if (mState == 3 && mWinColumn >= 0) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2], mWinColumn * 0x50 + 0x20,
                              0x28, 0);
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][13], mWinColumn * 0x50 + 0x30,
                              0x28, 0);
        if (mWinColumn == 1) {
            Mg_DrawBigNumber(mWinColumn * 0x50 + 0x40, 0x28, 6, 0, 0, 0, 0x14);
        } else {
            Mg_DrawBigNumber(mWinColumn * 0x50 + 0x40, 0x28, 3, 0, 0, 0, 0x14);
        }
    } else if (mState == 4 && mWinColumn < 0) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2], 0x70, 0x28, 0);
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][14], 0x80, 0x28, 0);
        Mg_DrawBigNumber(0x90, 0x28, 3, 0, 0, 0, 0x28);
    }

    Mg_RenderMainSpriteScaled(sSlotLamp1Sprite, 0x18, 0x30, -1, 0, 0x1000, mLamp1Angle);
    Mg_RenderMainSpriteScaled(sSlotLamp2Sprite, 0x40, 0x10, -1, 0, 0x1000, mLamp2Angle);

    return 1;
}

#pragma opt_strength_reduction on

/* dScMgSlot3_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the two slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Steps the reel state machine through the pointer-to-member table at
 * sSlot3StateTable, ticks the frame counter at 0x503f, spins the two reel
 * offsets while state 1 is running, and flips the blend bit in gMainScreenLayers off a
 * coin toss so the machine's lights flicker.
 *
 * The state table uses this class's pointer-to-member type.
 */
s32 dScMgSlot3_c::Behavior() {
    int i;
    u8 lightsOn;

    (this->*sSlot3StateTable[mState])();

    mFrameCounter++;
    mMascot.Update();

    for (i = 0; i < 3; i++) {
        if (mState == 1) {
            mLamp1Angle -= 0x200;
            mLamp2Angle -= 0x400;
            break;
        }
    }

    lightsOn = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 1;
    if (lightsOn)
        gMainScreenLayers |= DISPLAY_LAYER_BG1;
    else
        gMainScreenLayers &= ~DISPLAY_LAYER_BG1;

    return 1;
}

/* Load the reel assets and initialise the scene state. */
s32 dScMgSlot3_c::InitResources() {
    char *c = (char *)this;

    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                     GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x18000);
    G2_BG2Mosaic(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                         GX_BG_CHARBASE_0x18000);
    G2_BG3Mosaic(0);

    G2_SetBG0Priority(0);
    G2_SetBG1Priority(0);
    G2_SetBG2Priority(1);
    G2_SetBG3Priority(3);

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN, (void *)G2_GetBG3CharPtr());

    {
        void *h = LoadFile(ASSET_D_2D_MG_BG_SLOT_FACE_NCL_BIN);
        GX_LoadBGPltt(h, 0x60, 0x1a0);
        GXS_LoadBGPltt(h, 0x60, 0x1a0);
        Deallocate(h);
    }

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE02_NSC_BIN, G2_GetBG1ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NUKI_NSC_BIN, G2_GetBG2ScrPtr());

    {
        void *h = LoadFile(ASSET_D_2D_MG_BG_SLOT_FACE_NSC_BIN);
        GX_LoadBG3Scr(h, 0, 0x800);
        Deallocate(h);
    }

    LoadCompressedFileAt(ASSET_D_2D_MG_SLOT_FACE_NCG_BIN, G2_GetOBJCharPtr());

    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 |
                        DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ;
    G2S_SetBG2Priority(3);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                          GX_BG_CHARBASE_0x18000);
    G2S_BG2Mosaic(0);

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN, G2S_GetBG2CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_DOWN_NSC_BIN, G2S_GetBG2ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_SLOT_FACE_NCG_BIN, G2S_GetOBJCharPtr());

    {
        void *h = LoadFile(ASSET_D_2D_MG_SLOT_FACE_NCL_BIN);
        DC_FlushRange(h, 0xe0);
        GX_LoadOBJPltt(h, 0, 0xe0);
        GXS_LoadOBJPltt(h, 0, 0xe0);
        Deallocate(h);
    }

    gSubScreenLayers = 0x14;
    G2_SetBlendAlpha(0, 0xc, 0xc, 4);

    mMascot.Load();
    Camera_InitSlotsView(&mCamera);

    mReelDifficulty = (int)mLevel;

    ((dScMgSlot3_c *)c)->StartRound(3);

    mWinSymbol = mNextWinSymbol;
    mReelDrawY = 0;

    for (int i = 0; i < 3; i++) {
        int rnd = RandomIntInternal(&data_0209e650.value);
        u32 divisor = mStripLength;
        u32 val = ((u32)rnd >> 16) % divisor;
        mReelPos[i] = val * 0x50000;
    }

    sMgWinBannerX = 0x80;
    sMgWinBannerY = 0xa0;
    sMgLoseBannerX = 0x80;
    sMgLoseBannerY = 0xa0;
    sMgMainBannerX = 0x80;
    sMgMainBannerY = 0x60;
    sMgResultsBannerX = 0x80;
    sMgResultsBannerY = ~0x1b;

    Mg_SetTopScreenYOffset(0x20);

    data_0208ee44 = 1;

    mLamp1Angle = 0;
    mLamp2Angle = 0;
    return 1;
}

/* The constructor is inline, so the factory below holds it: the base
   constructors, mParticle and mMascot are the compiler's. */
inline dScMgSlot3_c::dScMgSlot3_c() {}

/* Reconstructed source-style name: SM64DS proves dScMgSlot3_c through RTTI,
 * allocation size, vtable identity, and the MG_SLOT3 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0210c120. */
void *dScMgSlot3_c_classInit(void) {
    return new dScMgSlot3_c;
}

void SlotLamp::Start() {
    mTimer = 0x48;
    mOn = 1;
    gSubScreenLayers |= 2;
}

void SlotLamp::Tick() {
    if (mTimer <= 0)
        return;
    mTimer -= 1;
    if ((mTimer & 7) != 0)
        return;
    mOn ^= 1;
    if (mOn != 0)
        gSubScreenLayers |= 2;
    else
        gSubScreenLayers &= ~2;
}

/* mPayoutLamp (0x469c, a SlotLamp) blinks bit 1 of gSubScreenLayers the same
 * way: a timer and an on flag. Clear clears it, Start starts it and
 * Tick ticks it. */
void SlotLamp::Clear() {
    mTimer = 0;
    mOn = 0;
}

void SlotBlink::Start(s16 x, s16 y) {
    mX = x;
    mY = y;
    mTimer = 0x48;
    mVisible = 1;
}

void SlotBlink::Draw() {
    if (mVisible == 0)
        return;
    RenderOamMainScreen(sSlotsSlotBlinkSprites, mX, mY, -1, -1);
}

void SlotBlink::Tick() {
    if (mTimer <= 0)
        return;
    mTimer -= 1;
    if ((mTimer & 7) == 0) {
        mVisible ^= 1;
    }
}

/* mMatchBlink and mWildBlink (0x4684, 0x4690) are SlotBlinks: s16 x and y,
 * a timer and a visible flag. Clear clears one, Start starts one, Tick
 * ticks it and Draw draws it. */
void SlotBlink::Clear() {
    mTimer = 0;
    mVisible = 0;
}

/* dThIcon_c.h keeps its words at 0x1c and 0x20 as unk_01c/unk_020 because
 * each derived icon gives them its own role. This file-local view names
 * dScMgSlot1_c::betIcon_c's:
 * - mBetCount: OnTouch adds one per touch, up to 3; betIcon_c::Render
 *   draws that many bet coins; Behavior state 3 records mCredits plus
 *   mPayoutMultiplier per bet.
 * - mPayoutTicks: ArmPayout loads mBetCount * multiplier * 4;
 *   betIcon_c::Behavior counts it down, paying a credit every 4 ticks, and
 *   Behavior state 6 waits for it to reach 0.
 * Reset clears both. */
struct SlotBetIcon {
    u8 mIcon[0x1c];   /* 0x00 -- dThIcon_c's vptr and named fields */
    s32 mBetCount;    /* 0x1c */
    s32 mPayoutTicks; /* 0x20 */
};
typedef char SlotBetIcon_size_must_be_0x24[sizeof(SlotBetIcon) == 0x24 ? 1 : -1];
#define BET_ICON(p) ((SlotBetIcon *)(p))

/* The symbol name is a misreading: Behavior state 5 passes &mBetIcon and
 * the payout multiplier, and this arms the bet icon's payout countdown. */
void dScMgSlot1_c::betIcon_c::ArmPayout(int value) {
    BET_ICON(this)->mPayoutTicks = (BET_ICON(this)->mBetCount * value) << 2;
}

void dScMgSlot1_c::betIcon_c::OnTouch() {
    void *scene;
    int count;
    unsigned int sid;

    if (BET_ICON(this)->mBetCount >= 3) {
        return;
    }
    if (mOn != 0) {
        return;
    }
    scene = gMgScene;
    count = (scene != 0) ? ((dScMgBase_c *)scene)->mCredits : 0;
    if (count <= 0) {
        return;
    }
    BET_ICON(this)->mBetCount += 1;
    Mg_LoseCredits(1);
    func_ov001_020ab3f0(this);
    sid = 0x163;
    Sound::PlayBank2_2D(sid);
}

void dScMgSlot1_c::betIcon_c::Reset() {
    BET_ICON(this)->mBetCount = 0;
    BET_ICON(this)->mPayoutTicks = 0;
    if (mTouchKind >= 1)
        mReady = 1;
}

void dScMgSlot1_c::betIcon_c::Render() {
    OAM::RenderSub((OamAttr *)data_ov006_0213e528[mOn], mX, mY, -1, 1);
    int i;
    int y = 0xb0;
    int zero = 0;
    for (i = 0; i < BET_ICON(this)->mBetCount; i++) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2], 0xb0 + i * 0x10, y,
                              (void *)zero);
    }
}

void dScMgSlot1_c::betIcon_c::Behavior() {
    int val;
    if (TouchArea_Update(this, -1))
        OnTouch();
    val = BET_ICON(this)->mPayoutTicks;
    if (val != 0) {
        if ((val & 3) == 0) {
            Mg_GainCredits(1);
            Sound::PlayBank2_2D(0x149);
        }
        BET_ICON(this)->mPayoutTicks -= 1;
    }
    dThIcon_c::Behavior();
}

void dScMgSlot1_c::betIcon_c::Place() {
    func_ov001_020ab5b0((char *)this, 1, 0x90, 0xb0, 0x10, 8);
    mPressTime = 6;
}

int dScMgSlot1_c::CloseMenu() {
    dScMgBase_c::CloseMenu();
    SetSubBg1Offset(0x100, 0);
}

/* Slot 27. Takes no argument: the ROM body never reads r1. The qualified
 * base call is the ROM's direct branch. */
void dScMgSlot1_c::OpenMenu() {
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OpenMenu();
}

int dScMgSlot1_c::SlotReels_CheckStopped() {
    int matches;
    int matchedSymbol;
    int reelIndex;

    mWildCount = 0;
    matches = 1;
    for (reelIndex = 0; reelIndex < 3; reelIndex++) {
        if (mReelStrip[reelIndex][mSymbolIndex[reelIndex]] == 5) {
            mWildCount++;
        } else {
            matchedSymbol = mReelStrip[reelIndex][mSymbolIndex[reelIndex]];
            break;
        }
    }
    if (mWildCount >= 3) {
        matchedSymbol = 5;
    } else if (reelIndex < 2) {
        for (reelIndex = reelIndex + 1; reelIndex < 3; reelIndex++) {
            u8 symbol = mReelStrip[reelIndex][mSymbolIndex[reelIndex]];
            if (symbol == 5) {
                mWildCount++;
            } else if (matchedSymbol != symbol) {
                matches = 0;
            }
        }
    }
    if (matches) {
        if (mSpeedTableOffset < 2) {
            mSpeedTableOffset += 1;
        }
        mMatchedSymbol = matchedSymbol;
    }
    return matches;
}

void dScMgSlot1_c::InitHud() {
    mPrevMatchedSymbol = -1;
    unk_4707 = 1;
    mSpeedTableOffset = 0;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
}

/* Slot 18. */
void dScMgSlot1_c::StartRound(int i) {
    char *raw = (char *)this;

    if (i == 4) {
        mPrevMatchedSymbol = mMatchedSymbol;
    } else if (i == 3) {
        InitHud();
    }
    mBetIcon.Reset();
    mState = 0;
}

/* dScMgSlot1_c::Render -- the recovered symbol at this address
   (_ZN3OAM7SECONDSE, i.e. "OAM::SECONDS") was a name-recovery-heuristic
   artifact, not a real name; this function genuinely is Render (see
   include/dScMgSlot1_c.h's file banner). mBetIcon.Render() is the ROM's
   virtual call through the typed nested betIcon_c member at 0x4660. */
s32 dScMgSlot1_c::Render() {
    unsigned char *sceneBytes = (unsigned char *)this;
    int i, pos, drawY, j;
    unsigned char *reelBytes = sceneBytes;
    unsigned char wildCount;
    for (i = 0; i < 3; i++) {
        pos = mSymbolIndex[i];
        drawY = sSlot1ReelPos[i][1] - ((s32)mReelScroll[i] >> 12);
        for (j = 0; j < 2; j++) {
            Hud_RenderSprite(sSlot1Sprites[((dScMgSlot1_c *)(reelBytes + pos))->mReelStrip[0][0]],
                             sSlot1ReelPos[i][0], drawY, -1, 3);
            pos--;
            drawY += 0x40;
            if (pos < 0)
                pos += 0x15;
        }
        reelBytes += 0x15;
    }
    mBetIcon.Render();
    Mg_DrawBigNumber(0x70, 0xb0, mCredits, 0, 1, 1, 0x14);
    if (mState >= 5 && (mHasMatch != 0 || (mWildCount != 0 && mWildCount < 3))) {
        wildCount = mWildCount;
        if (wildCount != 0 && wildCount < 3) {
            i = 0;
            if ((int)wildCount > 0) {
                pos = 0x18;
                do {
                    Mg_RenderSubSpriteScaled(sSlot1Sprites[0xb], pos, 0x30, -1, 2, 0x1000, 0);
                    pos += 0x10;
                    i++;
                } while (i < mWildCount);
            }
            Mg_RenderSubSpritePalMtx(sSlotLangSprites[GetGameLanguage()][1], 0x50, 0x30, -1, 2, 0);
            Mg_DrawBigNumber(0x60, 0x30, mWildCount * 2, 0, 2, 2, 0x14);
        }
        if (mHasMatch != 0) {
            i = 0;
            pos = 0x18;
            for (; i < 3; i++) {
                Mg_RenderSubSpriteScaled(sSlot1Sprites[mMatchedSymbol + 6], pos, 0x40, -1, 2,
                                         0x1000, 0);
                pos += 0x10;
            }
            Mg_RenderSubSpritePalMtx(sSlotLangSprites[GetGameLanguage()][1], 0x50, 0x40, -1, 2, 0);
            Mg_DrawBigNumber(0x60, 0x40, sSlot1Payouts[mMatchedSymbol], 0, 2, 2, 0x14);
        }
    }
    mMatchBlink.Draw();
    mWildBlink.Draw();
    return 1;
}

/* Mario Slot's per-frame state machine (vtable slot 6). Byte-exact since
 * 2026-10-12; the 19-word residue banked as a compiler floor was a source
 * artefact. The object is spelled ONE way here, through members: a draft
 * that read some fields as members and others through `char *c =
 * (char *)this` with literal offsets made 2004/b56 form the hoisted touch
 * record address as one folded `add r0, r0, r2, lsl #2` (0x818); with a
 * single name it materializes `lsl r1, r2, #2` / `add r0, r0, r1` as the
 * cartridge does (+0x268). Case 7 is the one deliberate exception: the ROM
 * walks the reel-symbol rows with a strided copy of `this` (mov r5, r4).
 * The touch x/y are read INSIDE the hit loop through TouchRec, not hoisted
 * by hand; the compiler hoists them and that colours the loop. */
s32 dScMgSlot1_c::Behavior() {
    int i;

    switch (mState) {
    case 0:
        for (i = 0; i < 3; i++) {
            mReelSpinning[i] = 1;
        }
        mStoppedReelCount = 0;
        mHasMatch = 0;
        mWildCount = 0;
        mState = 1;
        mBetIcon.OnTouch();
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        mStateTimer = 0x3c;
        break;
    case 1:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            Sound::PlayBank2_2D(0x160);
            mStateTimer = 0x1e;
            mState = 2;
        }
        break;
    case 2:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            mState = 3;
        }
        break;
    case 3: {
        int moving = 0;
        for (i = 0; i < 3; i++) {
            if (mReelSpinning[i] == 1 || mReelScroll[i] != 0) {
                moving++;
                mReelScroll[i] -= sSlotReelScrollSpeeds[mSpeedTableOffset + i];
                if ((int)mReelScroll[i] < 0) {
                    if (mReelSpinning[i] == 1) {
                        mSymbolIndex[i]++;
                        mSymbolIndex[i] %= 21;
                        mReelScroll[i] += 0x40000;
                    } else {
                        mReelScroll[i] = 0;
                    }
                }
            }
        }
        if (moving != 0) {
            mSpinSoundHandle = Sound_PlayIfNotActive(mSpinSoundHandle, 2, 0x161, 0);
        }
        if (mStoppedReelCount < 3) {
            int hit = 0;
            u8 idx = gActivePlayerSlot;
            if (gTouchHeld[idx * 4] != 0) {
                if (gTouchEdge[idx * 4] != 0) {
                    hit = 1;
                }
            }
            if (hit != 0) {
                for (i = 0; i < 3; i++) {
                    if (mReelSpinning[i] == 1) {
                        int dx = ((TouchRec *)gTouchHeld)[idx].x - sSlot1ReelPos[i][0];
                        int dy = ((TouchRec *)gTouchHeld)[idx].y - sSlot1ReelPos[i][1];
                        if (dx <= 0x20 && dx >= -0x20 && dy <= 0x20 && dy >= -0x20) {
                            mReelSpinning[i] = 0;
                            mStoppedReelCount++;
                            if (mBetIcon.mReady != 0) {
                                mBetIcon.mReady = 0;
                                Mg_SaveRecordPair(mCredits, mHudScore);
                            }
                            func_02012718(0x162, sSlot1ReelPos[i][0] << 12);
                            break;
                        }
                    }
                }
            }

        } else {
            int stopped = 1;
            for (i = 0; i < 3; i++) {
                if (mReelScroll[i] != 0) {
                    stopped = 0;
                }
            }
            if (stopped == 1) {
                mHasMatch = SlotReels_CheckStopped();
                mPayoutMultiplier = 0;
                if (mHasMatch != 0) {
                    mPayoutMultiplier += sSlot1Payouts[mMatchedSymbol];
                }
                if (mWildCount != 0 && mWildCount < 3) {
                    mPayoutMultiplier += mWildCount << 1;
                }
                Mg_SaveRecordPair(mCredits + mPayoutMultiplier * BET_ICON(&mBetIcon)->mBetCount,
                                  mHudScore);
                mState = 4;
                mStateTimer = 0x3c;
            }
        }
        break;
    }
    case 4:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            if (mHasMatch != 0) {
                mMatchBlink.Start(sSlotMatchBlinkX[mMatchedSymbol][0],
                                  sSlotMatchBlinkY[mMatchedSymbol][0]);
            }
            if (mWildCount != 0 && mWildCount < 3) {
                mWildBlink.Start(sSlotWildBlinkX[mWildCount - 1][0],
                                 sSlotWildBlinkY[mWildCount - 1][0]);
            }
            if (mPayoutMultiplier != 0) {
                mState = 5;
                mStateTimer = 0x64;
                mPayoutLamp.Start();
                func_02012790(0x26);
            } else {
                mState = 7;
                mStateTimer = 0x3c;
                func_02012790(0xe);
            }
            mPromptEnabled = 0;
        }
        break;
    case 5:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            mBetIcon.ArmPayout(mPayoutMultiplier);
            mStateTimer = 0x3c;
            mState = 6;
        }
        break;
    case 6: {
        int done = (BET_ICON(&mBetIcon)->mPayoutTicks == 0) ? 1 : 0;
        if (done != 0) {
            Mg_SubmitScore(mCredits);
            if (DecIfAbove0_Byte(&mStateTimer) == 0) {
                StartRound(4);
            }
        }
        break;
    }
    case 7:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            if (mCredits > 0) {
                StartRound(5);
            } else {
                int j;
                char *p = (char *)this;
                for (j = 0; j < 3; j++) {
                    mReelScroll[j] += 0x40000;
                    mSymbolIndex[j]++;
                    mSymbolIndex[j] %= 21;
                    ((dScMgSlot1_c *)(p + mSymbolIndex[j]))->mReelStrip[0][0] = 0xd;
                    p += 0x15;
                }
                mState = 9;
            }
        }
        break;
    case 9:
    case 11: {
        int idx = (mState - 9) >> 1;
        mReelScroll[idx] -= 0x2000;
        if ((int)mReelScroll[idx] < 0) {
            mReelScroll[idx] = 0;
            mStateTimer = 0xa;
            mState++;
            func_02012718(0x164, sSlot1ReelPos[idx][0] << 12);
            if (idx == 0) {
                Mg_PlayJingle((void *)2);
            }
        }
        break;
    }
    case 13:
        mReelScroll[2] -= 0x2000;
        if ((int)mReelScroll[2] < 0) {
            mReelScroll[2] = 0;
            mStateTimer = 0x3c;
            mState = 8;
            func_02012718(0x164, sSlot1ReelPos[2][0] << 12);
        }
        break;
    case 8:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            dScene_c::SetFaders(&data_0209f61c);
            data_0209f61c.SetType(2);
            dScene_c::StartSceneFade(5, 0, 0);
        }
        break;
    case 10:
    case 12:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            mState++;
        }
        break;
    }
    mBetIcon.Behavior();
    mMatchBlink.Tick();
    mWildBlink.Tick();
    mPayoutLamp.Tick();
    return 1;
}

/* The ov004 scroll words (sMgScoreDigitsX..sMgMainBannerX) name
 * no struct; they are stored raw. The reel-table fill keeps a stepped char*
 * cursor over mReelStrip (see leftovers). */
s32 dScMgSlot1_c::InitResources() {
    char *c = (char *)this;
    volatile unsigned short fill;
    int i, j;

    G2_SetBG2Priority(2);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);
    G2_BG2Mosaic(0);
    G2_SetBG3Priority(3);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9800,
                         GX_BG_CHARBASE_0x10000);
    G2_BG3Mosaic(0);

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_UP_NCG_BIN, (void *)G2_GetBG3CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN, (char *)G2_GetBG2CharPtr() + 0x7800);
    {
        void *h = LoadFile(ASSET_D_2D_MG_BG_SLOT_UP_NCL_BIN);
        GX_LoadBGPltt(h, 0x60, 0x1a0);
        Deallocate(h);
    }
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_SCORE_UP_NSC_BIN, G2_GetBG2ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_UP_NSC_BIN, G2_GetBG3ScrPtr());

    gMainScreenLayers = DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ;

    G2S_SetBG0Priority(1);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                      GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(0);
    G2S_SetBG1Priority(1);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(0);
    SetSubBg1Offset(0x100, 0);

    G2S_SetBG2Priority(2);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                          GX_BG_CHARBASE_0x18000);
    G2S_BG2Mosaic(0);
    G2S_SetBG3Priority(3);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9800,
                          GX_BG_CHARBASE_0x10000);
    G2S_BG3Mosaic(0);

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_DOWN_NCG_BIN, G2S_GetBG2CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN, (char *)G2S_GetBG0CharPtr() + 0x7800);
    {
        void *h = LoadFile(ASSET_D_2D_MG_BG_SLOT_DOWN_NCL_BIN);
        GXS_LoadBGPltt(h, 0x60, 0x1a0);
        Deallocate(h);
    }
    LoadCompressedFileAt(ASSET_D_2D_GRADE_NSC_BIN, G2S_GetBG0ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_BADWIZER_NSC_BIN, (char *)G2S_GetBG1ScrPtr() + 0x800);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_DOWN_NSC_BIN, G2S_GetBG2ScrPtr());

    {
        char *d3 = (char *)G2S_GetBG3ScrPtr();
        fill = 0x43e4;
        MI_CpuFill16(d3, fill, 0x800);
    }

    LoadCompressedFileAt(sSlotNcgByLanguage[GetGameLanguage()], G2_GetOBJCharPtr());
    LoadCompressedFileAt(sSlotNcgByLanguage[GetGameLanguage()], G2S_GetOBJCharPtr());

    {
        void *h = LoadFile(ASSET_D_2D_MG_SLOT_NCL_BIN);
        GX_LoadOBJPltt(h, 0, 0x40);
        GXS_LoadOBJPltt(h, 0, 0x40);
        Deallocate(h);
    }

    gSubScreenLayers = 0x1d;
    G2_BlendNone();
    G2S_SetBlendAlpha(1, 0x1c, 7, 9);

    sMgWinBannerX = 0x80;
    sMgWinBannerY = 0x40;
    sMgLoseBannerX = 0x80;
    sMgLoseBannerY = 0x40;
    sMgMainBannerX = 0x80;
    sMgMainBannerY = 0x40;

    if ((int)param1 & 0xff) {
        unsigned char *tbl = sSlotReelSymbolsAlt;
        char *p = c;
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 0x15; i++) {
                ((dScMgSlot1_c *)(p + i))->mReelStrip[0][0] = tbl[i];
            }
            mSymbolIndex[j] = 0;
            mReelScroll[j] = 0x40000;
            mReelSpinning[j] = 1;
            tbl += 0x15;
            p += 0x15;
        }
    } else {
        int k;
        unsigned char *tbl = sSlotReelSymbols;
        char *p = c;
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 0x15; i++) {
                ((dScMgSlot1_c *)(p + i))->mReelStrip[0][0] = tbl[i];
            }
            mSymbolIndex[k] = 0;
            mReelScroll[k] = 0x40000;
            mReelSpinning[k] = 1;
            tbl += 0x15;
            p += 0x15;
        }
    }

    mMatchedSymbol = 5;
    mStartCredits = 0xa;
    Mg_SubmitScore(mStartCredits);
    mBetIcon.Place();
    StartRound(3);
    Mg_SetTopScreenYOffset(0x20);
    data_0208ee44 = 1;
    return 1;
}

/* The constructor is inline, so the factory below holds it: the base
   constructor, mBetIcon, and the three marker clears. */
inline dScMgSlot1_c::dScMgSlot1_c() {
    mMatchBlink.Clear();
    mWildBlink.Clear();
    mPayoutLamp.Clear();
}

/* Reconstructed source-style name. Historical alias: func_ov006_0210d6b8. */
void *dScMgSlot1_c_classInit(void) {
    return new dScMgSlot1_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_02132894 shard).
 * The Slot3 state table plain-copied from the ROM PMF constants, in
 * retail copy order. */
SlotState sSlot3StateTable[8] = {
    pmf_Slot3_StateIntro,    pmf_Slot3_StateSpin, pmf_Slot3_StateJudge,    pmf_Slot3_StatePayout,
    pmf_Slot3_StatePayout_2, pmf_Slot3_StateWait, pmf_Slot3_StateGameOver, pmf_Slot3_StateDone,
};
