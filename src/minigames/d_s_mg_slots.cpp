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
#include "G2.h"
#include "dScMgSlot3_c.h"
#include "Sound.h"
#include "OAM.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "types.h"
#include "PlayerInput.h"
#include "dWipe_c.h"
#include "G2x.h"
#include "CP15.h"
#include "GX.h"

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
void *func_02054ea8(void);
// Retained caller ABI: narrowing to the definition's u16 adds a mask in InitMainScreenBg.
unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char data_0209d45c;
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
void Mg_AddCredits(void *c, int delta);
void Mg_SubCredits(void *c, int val);
extern void Mg_SubmitScore(int self);
extern int data_ov006_0213e948[];
extern void Slot3_ReelDigits(char *c, int i);
extern int Sound_PlayIfNotActive(int, int, int, int);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
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
extern int LoadFile(int handle);

extern u8 data_0209d454;
extern int data_0208ee44;
extern void RenderOamMainScreen(void *, int, int, int, int);
extern void *sSlotsSlotBlinkSprites;
extern void Mg_LoseCredits(void *c);
extern void func_ov001_020ab3f0(void *c);
void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *m);
extern void *data_ov006_0213e528[];
int TouchArea_Update(void *c, int x);
void SlotBet_OnTouch(void *c);
void Mg_GainCredits(int x);
extern void func_ov001_020ab5b0(char *r0, int r1, short r2, short r3, short s4, short s5);
extern int Mg_GetSavedRecord0(void);
extern int sSlot1ReelPos[][2];
extern void *sSlot1Sprites[];
extern unsigned char sSlot1Payouts[];
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void Mg_RenderSubSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void SlotBlink_Draw(unsigned char *o);
extern void Mg_SaveRecordPair(int a, int b);
extern void Mg_PlayJingle(void *c);
extern void SlotLamp_Start(void *o);
extern void SlotLamp_Tick(void *o);
extern void SlotBlink_Start(void *o, s16 x, s16 y);
extern void SlotBlink_Tick(void *o);
extern void SlotReel_SetSpeed(void *o, int v);
extern int SlotReels_CheckStopped(void *self);
extern int sSlotReelScrollSpeeds[];
extern u8 sSlot1Payouts[];
extern s16 sSlotMatchBlinkX[][2];
extern s16 sSlotMatchBlinkY[][2];
extern s16 sSlotWildBlinkX[][2];
extern s16 sSlotWildBlinkY[][2];
extern dWipe_c data_0209f61c;

extern void Mg_RenderSubSpriteScaled(void *a0, int a1, int a2, int a3, int a4, int a5,
                                     unsigned short a6);
extern void MultiStore16(u16 val, char *dst, int nbytes);
}

// @symbol _ZN12dScMgSlot1_cD1Ev
/* ~dScMgSlot1_c() (D1, complete-object destructor). The empty body is the
   original ownership model: the compiler destroys the typed mBetIcon member,
   emitting betIcon_c's then dThIcon_c's vtable writes, before chaining to the
   dScMgBase_c base destructor. */
dScMgSlot1_c::~dScMgSlot1_c() {}

// @symbol _ZN12dScMgSlot1_cD0Ev
/* The compiler emits D0 from the complete-object destructor above. */

// @symbol _ZN12dScMgSlot3_cD1Ev
/* The compiler destroys mMascot, then the inlined dScMgSingle3DBase_c
   destructor runs. */
dScMgSlot3_c::~dScMgSlot3_c() {}

// @symbol _ZN12dScMgSlot3_cD0Ev
/* The compiler emits D0 from the complete-object destructor above. */

// @symbol _ZN12dScMgSlot3_c21RestoreScreenFromMenuEv
/* dScMgSlot3_c::RestoreScreenFromMenu - slot 30 override: puts BG1CNT
   back, then runs the base restore. This body is the arity witness cited
   in include/dScMgBase_c.h's slot-30 block.  The read-modify-write below clobbers
   r1 before the tail call, so the base cannot be taking a second argument. */
void dScMgSlot3_c::RestoreScreenFromMenu() {
    void *a = (void *)this;

    *(volatile unsigned short *)0x400000A = (*(volatile unsigned short *)0x400000A & 0x43) | 0x1118;
    ((dScMgBase_c *)a)->dScMgBase_c::RestoreScreenFromMenu();
}

// @symbol _ZN12dScMgSlot3_c17SaveScreenForMenuEv
/* dScMgSlot3_c::SaveScreenForMenu - slot 29 override.

   This body is the third and strongest witness that slot 29 takes no
   explicit parameter.  It CLOBBERS r1 -- the masked read-modify-write of the
   sub display-control register below compiles to `ldrh r1,[r2]; and; orr;
   strh` -- and only then tail-branches into the base with `bx ip`.  A second
   argument passed in r1 would reach dScMgBase_c::SaveScreenForMenu as a
   display-control word, so the base cannot be reading one. */
int dScMgSlot3_c::SaveScreenForMenu() {
    void *a = (void *)this;

    *(volatile unsigned short *)0x400000A = (*(volatile unsigned short *)0x400000A & 0x43) | 0x1000;
    return ((dScMgBase_c *)a)->dScMgBase_c::SaveScreenForMenu();
}

// @symbol _ZN12dScMgSlot3_c16InitMainScreenBgEv
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

    *(volatile unsigned short *)0x400000a &= ~3;
    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short *)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    data_0209d45c &= ~2;

    idx = GetGameLanguage();
    LoadCompressedFileAt(sSlotPauseBgNcgByLanguage[idx], func_02054ea8());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_PAUSE_NSC_BIN, G2::GetBG1ScrPtr());

    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1118;
}

// @symbol Slot3_ReelDigits
extern "C" { /* .c-derived member: C linkage for the whole block */
void Slot3_ReelDigits(char *sceneBytes, int reelIndex);
// Keep the strided cursors cast back to the class. mStripLength is read by
// offset: a member read of it grows this body by 8 bytes (measured).
void Slot3_ReelDigits(char *c, int reelIndex) {
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
}

// @symbol Slot3_StateDone
extern "C" { /* .c-derived member: C linkage for the whole block */
void Slot3_StateDone(void) {}
}

// @symbol Slot3_StateGameOver
extern "C" { /* .c-derived member: C linkage for the whole block */

void Slot3_StateGameOver(char *c) {
    dScMgSlot3_c *self = (dScMgSlot3_c *)c;
    if (DecIfAbove0_Byte(&self->mStateTimer) == 0) {
        self->mState = 7;
        Mg_SetSharedState(MG_STATE_GAME_OVER);
        self->mReelDifficulty = 0;
    }
    if (self->mStateTimer >= 0x3c)
        return;
    {
        int *a = &self->mReelWinPos[0];
        int *b = &self->mReelWinPos[1];
        int *d = &self->mReelWinPos[2];
        *a = *a + 0x10000;
        *b = *b - 0x10000;
        *d = *d + 0x10000;
    }
}
}

// @symbol Slot3_StateWait
extern "C" { /* .c-derived member: C linkage for the whole block */
void Slot3_StateWait(void) {}
}

// @symbol Slot3_StatePayout
extern "C" void Slot3_StatePayout(char *c) {
    dScMgSlot3_c *self = (dScMgSlot3_c *)c;
    if (DecIfAbove0_Byte(&self->mStateTimer) == 0) {
        if (self->mState == 3) {
            if (self->mHudScore < 0x270f)
                self->mHudScore += 1;
            if (self->mHudScore > self->mBestHudScore)
                self->mBestHudScore = self->mHudScore;
            self->StartRound(4);
        } else {
            if (self->mCredits <= 0) {
                self->mStateTimer = 0x78;
                self->mState = 6;
            } else {
                self->StartRound(5);
            }
        }
    }

    if (self->mState == 3) {
        if (self->mStateTimer != 0x14)
            return;
        if (self->mPayoutRemaining <= 0)
            return;
        self->mPayoutRemaining -= 1;
        Mg_AddCredits(c, 1);
        Sound::PlayBank2_2D(0x149);
        self->mStateTimer += 5;
        return;
    }

    if (self->mState != 4)
        return;

    {
        unsigned char b = self->mStateTimer;
        if (b != 0x14 && b != 0x28 && b != 0x3c)
            return;
    }

    Mg_SubCredits(c, 1);
    Sound::PlayBank2_2D(0x14a);
}

// @symbol Slot3_StateJudge
extern "C" { /* .c-derived member: C linkage for the whole block */
void Slot3_StateJudge(char *c) {
    unsigned char columnSymbols[3];
    int column;
    int payout;
    int score;
    char *self;
    dScMgSlot3_c *scene = (dScMgSlot3_c *)c;

    if (DecIfAbove0_Byte(&scene->mStateTimer) != 0)
        return;

    self = c;
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

    scene->mWinColumn = -1;
    payout = 0;
    {
        int columnIndex;
        for (columnIndex = 0; columnIndex < 3; columnIndex++) {
            if (columnSymbols[columnIndex] == scene->mWinSymbol) {
                if (columnIndex == 1) {
                    scene->mWinColumn = columnIndex;
                    payout += 6;
                } else if (columnIndex == 0 || columnIndex == 2) {
                    scene->mWinColumn = columnIndex;
                    payout += 3;
                }
                break;
            }
        }
    }

    score = scene->mHudScore;
    if (payout > 0) {
        scene->mPayoutRemaining = payout;
        scene->mState = 3;
        func_02012790(0x26);
        scene->mStateTimer = 0x28;
        score += 1;
    } else {
        scene->mState = 4;
        func_02012790(0xe);
        scene->mStateTimer = 0x50;
    }
    Mg_SubmitScore(score);

    scene->mPromptEnabled = 0;
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off
// @symbol Slot3_StateSpin
extern "C" { /* .c-derived member: C linkage for the whole block */
/* Indexed address construction is byte-sensitive: direct member indexing
 * (scene->mReelStopped[reelIndex]) adds 4 bytes, so the per-reel arrays are
 * reached through the scene pointer stepped by the reel index, and the scroll
 * pointer is formed in two steps (measured: one step also adds 4 bytes).
 * Scalar fields are native. */
void Slot3_StateSpin(char *c) {
    dScMgSlot3_c *scene = (dScMgSlot3_c *)c;
    u8 playerSlot;
    int touchPressed;
    int touchOffset;
    int reelIndex;
    int one;
    int *speedTable;
    playerSlot = gActivePlayerSlot;
    touchPressed = 0;
    touchOffset = playerSlot * 4;
    one = 1;
    if (gTouchHeld[playerSlot * 4]) {
        if (gTouchEdge[touchOffset]) {
            touchPressed = 1;
        }
    }
    if (touchPressed != 0) {
        unsigned int touchY = gTouchY[playerSlot * 4];
        if (touchY >= 0x40 && touchY < 0x58) {
            if (scene->mReelSpinning[0] == one)
                scene->mReelSpinning[0] = 0;
        } else if (touchY >= 0x58 && touchY < 0x70) {
            if (scene->mReelSpinning[1] == 1)
                scene->mReelSpinning[1] = 0;
        } else if (touchY >= 0x70 && touchY < 0x88) {
            if (scene->mReelSpinning[2] == one)
                scene->mReelSpinning[2] = 0;
        }
    }
    reelIndex = 0;
    for (; reelIndex < 3; reelIndex++) {
        if (((dScMgSlot3_c *)(c + reelIndex))->mReelStopped[0] == 0) {
            int *scroll = (int *)(c + reelIndex * 4);
            int oldScroll;
            int oldSymbol;
            int newSymbol;
            speedTable = data_ov006_0213e948;
            scroll = &((dScMgSlot3_c *)scroll)->mReelPos[0];
            oldScroll = *scroll;
            oldSymbol = (oldScroll >> 12) / 0x50;
            if (((dScMgSlot3_c *)(c + reelIndex))->mReelReverse[0] == 1) {
                *scroll = oldScroll - speedTable[scene->mStoppedReelCount * 3 + reelIndex];
                if (*scroll < 0)
                    *scroll += scene->mReelPeriod;
            } else {
                *scroll = oldScroll + speedTable[scene->mStoppedReelCount * 3 + reelIndex];
                if (*scroll >= scene->mReelPeriod)
                    *scroll -= scene->mReelPeriod;
            }
            if (((dScMgSlot3_c *)(c + reelIndex))->mReelSpinning[0] == 0) {
                newSymbol = (*scroll >> 12) / 0x50;
                if (newSymbol != oldSymbol) {
                    if (((dScMgSlot3_c *)(c + reelIndex))->mReelReverse[0] == one)
                        *scroll = (oldSymbol * 0x50) << 12;
                    else
                        *scroll = (newSymbol * 0x50) << 12;
                    Slot3_ReelDigits(c, reelIndex);
                    ((dScMgSlot3_c *)(c + reelIndex))->mReelStopped[0] = 1;
                    scene->mStoppedReelCount += one;
                    scene->mFrameCounter = 0;
                    Sound::PlayBank2_2D(0x1a9);
                }
            }
        }
    }
    scene->mSpinSoundHandle = Sound_PlayIfNotActive(scene->mSpinSoundHandle, 2, 0x1a8, 0);
    if (scene->mStoppedReelCount >= 3u) {
        scene->mStateTimer = 0x1e;
        scene->mState = 2;
    }
}
}

#pragma opt_loop_invariants on
#pragma opt_strength_reduction on

// @symbol Slot3_StateIntro
extern "C" { /* .c-derived member: C linkage for the whole block */
void Mascot_LookAt(char *c, int *src);
void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6);
void Mascot_StopLooking(int *p);
void FreeGfxSlotsById(int arg);

void Slot3_StateIntro(char *p) {
    dScMgSlot3_c *scene = (dScMgSlot3_c *)p;
    if (scene->mReelDrawY == 0x50) {
        Mascot_LookAt((char *)&scene->mMascot, 0);
        Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    }

    if (scene->mReelDrawY > 0) {
        scene->mReelDrawY -= 1;
        if (scene->mReelDrawY != 0)
            return;
        scene->mWinSymbol = scene->mNextWinSymbol;
        Mascot_StopLooking((int *)&scene->mMascot);
        return;
    }

    scene->mStateTimer -= 1;
    if (scene->mStateTimer != 0)
        return;

    FreeGfxSlotsById(0x1d);

    {
        int *state = &scene->mState;
        if (scene->mPromptBlinkCount == 0) {
            scene->mPromptEnabled = 1;
            scene->mPromptBlinkCount = 1;
            scene->mPromptBlinkTimer = 0;
        }
        *state += 1;
    }
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot3_c10StartRoundEi
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
            *(int *)(int)(c + 0x5004) += 1;
        }
        *(int *)(int)(c + 0xbc) += 1;
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
                if ((u8)(((u32)RandomIntInternal(&data_0209e650) >> 16) & 1)) {
                    *(u8 *)(c + i + 0x5040) = 1;
                } else {
                    *(u8 *)(c + i + 0x5040) = 0;
                }
            }
        }

        mNextWinSymbol = ((u32)RandomIntInternal(&data_0209e650) >> 16) % mStripLength;
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
                        *(u8 *)(c + k + 0x5021) = mStripLength - k - 1;
                    } else {
                        *(u8 *)(w + k + 0x501c) = k;
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
            *(u8 *)(c + j2 + 0x502b) = 1;
            *(u8 *)(c + j2 + 0x502e) = 0;
            for (k2 = 0; k2 < 3; k2++) {
                *(u8 *)(w2 + k2 + 0x5031) = mStripLength;
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

#pragma opt_strength_reduction on

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot3_c6RenderEv
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
 * (measured). Only the receiver changed:
 * `self` is `this` now instead of a char* parameter cast back to the class. */
s32 dScMgSlot3_c::Render() {
    char *c = (char *)this;
    struct dScMgSlot3_c *self = this;
    int i, j;

    Camera_InitSlotsView((char *)self->pad_4660);
    Mascot_Draw((char *)&self->mMascot);

    if (self->mReelDrawY > 0) {
        for (i = 0; i < 3; i++) {
            OAM::Render(0, (OamAttr *)sSlot3FaceSprites[self->mWinSymbol * 3 + i], 0x80,
                        self->mReelDrawY + 0x10, -1, 2, 0x1000, 0x1000, 0, 1);
        }
        for (i = 0; i < 3; i++) {
            OAM::Render(0, (OamAttr *)sSlot3FaceSprites[self->mNextWinSymbol * 3 + i], 0x80,
                        self->mReelDrawY + 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    } else {
        for (i = 0; i < 3; i++) {
            OAM::Render(0, (OamAttr *)sSlot3FaceSprites[self->mWinSymbol * 3 + i], 0x80, 0x60, -1,
                        2, 0x1000, 0x1000, 0, 1);
        }
    }

    if (self->mState != 7) {
        if (self->mState == 6) {
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
                    row = (row + 1) % self->mStripLength;
                    y += 0x50;
                }
                p += 5;
            }
        }
    }

    if (self->mStoppedReelCount < 3 && (self->mFrameCounter & 0x20)) {
        int slot = 3;
        int ok = 1;
        int i3, j3;
        char *p = c;
        for (i3 = 0; i3 < 3; i3++) {
            if (((dScMgSlot3_c *)(c + i3))->mReelStopped[0] != 0) {
                if (slot < 3) {
                    if (self->mWinSymbol != ((dScMgSlot3_c *)(p + slot))->mResultSymbols[0][0])
                        ok = 0;
                } else {
                    u8 t = self->mWinSymbol;
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
                if (((dScMgSlot3_c *)(c + i4))->mReelStopped[0] != 0) {
                    Hud_RenderSprite(&data_ov006_0213e5dc, slot * 0x50 + 0x30, y, -1, -1);
                    count++;
                }
                y += 0x18;
            }
            if ((self->mFrameCounter & 0x3f) == 0x20) {
                Sound::PlayBank2_2D(count > 1 ? 0x1ac : 0x1ab);
            }
        }
    }

    Mg_DrawCredits(c, 0xc, 0xc, 0);
    Mg_DrawScoreTally(c, 0xe0, 0x14, 1);

    if (self->mState == 3 && self->mWinColumn >= 0) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2],
                              self->mWinColumn * 0x50 + 0x20, 0x28, 0);
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][13],
                              self->mWinColumn * 0x50 + 0x30, 0x28, 0);
        if (self->mWinColumn == 1) {
            Mg_DrawBigNumber(self->mWinColumn * 0x50 + 0x40, 0x28, 6, 0, 0, 0, 0x14);
        } else {
            Mg_DrawBigNumber(self->mWinColumn * 0x50 + 0x40, 0x28, 3, 0, 0, 0, 0x14);
        }
    } else if (self->mState == 4 && self->mWinColumn < 0) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2], 0x70, 0x28, 0);
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][14], 0x80, 0x28, 0);
        Mg_DrawBigNumber(0x90, 0x28, 3, 0, 0, 0, 0x28);
    }

    Mg_RenderMainSpriteScaled(sSlotLamp1Sprite, 0x18, 0x30, -1, 0, 0x1000, self->mLamp1Angle);
    Mg_RenderMainSpriteScaled(sSlotLamp2Sprite, 0x40, 0x10, -1, 0, 0x1000, self->mLamp2Angle);

    return 1;
}

#pragma opt_strength_reduction on

// @symbol _ZN12dScMgSlot3_c8BehaviorEv
/* dScMgSlot3_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the two slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Steps the reel state machine through the pointer-to-member table at
 * sSlot3StateTable, ticks the frame counter at 0x503f, spins the two reel
 * offsets while state 1 is running, and flips the blend bit in data_0209d45c off a
 * coin toss so the machine's lights flicker.
 *
 * The state table uses this class's pointer-to-member type.
 */
s32 dScMgSlot3_c::Behavior() {
    dScMgSlot3_c *self = this;
    int i;
    unsigned char t;

    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;

    (self->*sSlot3StateTable[self->mState])();

    pc = &self->mFrameCounter;
    *pc = *pc + 1;
    Mascot_Update(&self->mMascot);

    for (i = 0; i < 3; i++) {
        if (self->mState == 1) {
            ph1 = &self->mLamp1Angle;
            ph2 = &self->mLamp2Angle;
            *ph1 = *ph1 - 0x200;
            *ph2 = *ph2 - 0x400;
            break;
        }
    }

    t = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1;
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= ~2;

    return 1;
}

// @symbol _ZN12dScMgSlot3_c13InitResourcesEv
/* Load the reel assets and initialise the scene state. */
s32 dScMgSlot3_c::InitResources() {
    char *c = (char *)this;

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1118;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1218;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1418;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~3;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~3;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN, (void *)func_02054d88());

    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_FACE_NCL_BIN);
        GX::LoadBGPltt((const void *)h, 0x60, 0x1a0);
        GXS::LoadBGPltt((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE02_NSC_BIN, G2::GetBG1ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NUKI_NSC_BIN, G2::GetBG2ScrPtr());

    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_FACE_NSC_BIN);
        func_02056314((void *)h, 0, 0x800);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(ASSET_D_2D_MG_SLOT_FACE_NCG_BIN, (void *)0x6400000);

    data_0209d45c = 0x1f;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN, G2S::GetBG2CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_DOWN_NSC_BIN, G2S::GetBG2ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_SLOT_FACE_NCG_BIN, (void *)0x6600000);

    {
        int h = LoadFile(ASSET_D_2D_MG_SLOT_FACE_NCL_BIN);
        CP15::FlushAndInvalidateDataCache((u32)h, 0xe0);
        GX::LoadOBJPltt((const void *)h, 0, 0xe0);
        GXS::LoadOBJPltt((const void *)h, 0, 0xe0);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x14;
    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 0, 0xc, 0xc, 4);

    Mascot_Load((char *)&mMascot);
    Camera_InitSlotsView((char *)pad_4660);

    mReelDifficulty = (int)mLevel;

    ((dScMgSlot3_c *)c)->StartRound(3);

    mWinSymbol = mNextWinSymbol;
    mReelDrawY = 0;

    for (int i = 0; i < 3; i++) {
        int rnd = RandomIntInternal(&data_0209e650);
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
   constructors, mSysTracker and mMascot are the compiler's. */
inline dScMgSlot3_c::dScMgSlot3_c() {}

// @symbol dScMgSlot3_c_classInit
/* Reconstructed source-style name: SM64DS proves dScMgSlot3_c through RTTI,
 * allocation size, vtable identity, and the MG_SLOT3 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0210c120. */
extern "C" void *dScMgSlot3_c_classInit(void) {
    return new dScMgSlot3_c;
}

// @symbol SlotLamp_Start
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotLamp_Start(void *raw) {
    SlotLamp *lamp = (SlotLamp *)raw;
    lamp->mTimer = 0x48;
    lamp->mOn = 1;
    data_0209d454 |= 2;
}
}

// @symbol SlotLamp_Tick
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotLamp_Tick(void *o_) {
    SlotLamp *lamp = (SlotLamp *)o_;
    if (lamp->mTimer <= 0)
        return;
    lamp->mTimer -= 1;
    if ((lamp->mTimer & 7) != 0)
        return;
    lamp->mOn ^= 1;
    if (lamp->mOn != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}
}

// @symbol SlotLamp_Clear
/* mPayoutLamp (0x469c, a SlotLamp) blinks bit 1 of data_0209d454 the same
 * way: a timer and an on flag. This clears it, 0210c180 starts it and
 * 0210c1a8 ticks it. */
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotLamp_Clear(char *blink) {
    ((SlotLamp *)blink)->mTimer = 0;
    ((SlotLamp *)blink)->mOn = 0;
}
}

// @symbol SlotBlink_Start
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBlink_Start(void *raw, s16 x, s16 y) {
    SlotBlink *sprite = (SlotBlink *)raw;
    sprite->mX = x;
    sprite->mY = y;
    sprite->mTimer = 0x48;
    sprite->mVisible = 1;
}
}

// @symbol SlotBlink_Draw
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBlink_Draw(unsigned char *raw) {
    SlotBlink *sprite = (SlotBlink *)raw;
    if (sprite->mVisible == 0)
        return;
    RenderOamMainScreen(&sSlotsSlotBlinkSprites, sprite->mX, sprite->mY, -1, -1);
}
}

// @symbol SlotBlink_Tick
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBlink_Tick(void *o_) {
    SlotBlink *sprite = (SlotBlink *)o_;
    if (sprite->mTimer <= 0)
        return;
    sprite->mTimer -= 1;
    if ((sprite->mTimer & 7) == 0) {
        sprite->mVisible ^= 1;
    }
}
}

// @symbol SlotBlink_Clear
/* mMatchBlink and mWildBlink (0x4684, 0x4690) are SlotBlinks: s16 x and y,
 * a timer and a visible flag. This clears one, 0210c218 starts one, 0210c278
 * ticks it and 0210c234 draws it. */
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBlink_Clear(char *sprite) {
    ((SlotBlink *)sprite)->mTimer = 0;
    ((SlotBlink *)sprite)->mVisible = 0;
}
}

/* dThIcon_c.h keeps its words at 0x1c and 0x20 as unk_01c/unk_020 because
 * each derived icon gives them its own role. This file-local view names
 * dScMgSlot1_c::betIcon_c's:
 * - mBetCount: SlotBet_OnTouch adds one per touch, up to 3; betIcon_c::Render
 *   draws that many bet coins; Behavior state 3 records mCredits plus
 *   mPayoutMultiplier per bet.
 * - mPayoutTicks: SlotReel_SetSpeed loads mBetCount * multiplier * 4;
 *   betIcon_c::Behavior counts it down, paying a credit every 4 ticks, and
 *   Behavior state 6 waits for it to reach 0.
 * SlotBet_Reset clears both. */
struct SlotBetIcon {
    u8 mIcon[0x1c];   /* 0x00 -- dThIcon_c's vptr and named fields */
    s32 mBetCount;    /* 0x1c */
    s32 mPayoutTicks; /* 0x20 */
};
typedef char SlotBetIcon_size_must_be_0x24[sizeof(SlotBetIcon) == 0x24 ? 1 : -1];
#define BET_ICON(p) ((SlotBetIcon *)(p))

// @symbol SlotReel_SetSpeed
extern "C" { /* .c-derived member: C linkage for the whole block */
/* The symbol name is a misreading: Behavior state 5 passes &mBetIcon and
 * the payout multiplier, and this arms the bet icon's payout countdown. */
void SlotReel_SetSpeed(void *raw, int value) {
    BET_ICON(raw)->mPayoutTicks = (BET_ICON(raw)->mBetCount * value) << 2;
}
}

// @symbol SlotBet_OnTouch
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBet_OnTouch(void *c_) {
    dThIcon_c *icon = (dThIcon_c *)c_;
    void *scene;
    int count;
    unsigned int sid;

    if (BET_ICON(icon)->mBetCount >= 3) {
        return;
    }
    if (icon->mOn != 0) {
        return;
    }
    scene = gMgScene;
    count = (scene != 0) ? ((dScMgBase_c *)scene)->mCredits : 0;
    if (count <= 0) {
        return;
    }
    BET_ICON(icon)->mBetCount += 1;
    Mg_LoseCredits((void *)1);
    func_ov001_020ab3f0(icon);
    sid = 0x163;
    Sound::PlayBank2_2D(sid);
}
}

// @symbol SlotBet_Reset
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBet_Reset(void *raw) {
    dThIcon_c *icon = (dThIcon_c *)raw;
    BET_ICON(icon)->mBetCount = 0;
    BET_ICON(icon)->mPayoutTicks = 0;
    if (icon->mTouchKind >= 1)
        icon->mReady = 1;
}
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
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

// @symbol _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
void dScMgSlot1_c::betIcon_c::Behavior() {
    int val;
    if (TouchArea_Update(this, -1))
        SlotBet_OnTouch(this);
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

// @symbol SlotBet_Place
extern "C" { /* .c-derived member: C linkage for the whole block */
void SlotBet_Place(char *icon) {
    func_ov001_020ab5b0(icon, 1, 0x90, 0xb0, 0x10, 8);
    ((dThIcon_c *)icon)->mPressTime = 6;
}
}

// @symbol _ZN12dScMgSlot1_c9CloseMenuEv

int dScMgSlot1_c::CloseMenu() {
    dScMgBase_c::CloseMenu();
    SetSubBg1Offset(0x100, 0);
}

// @symbol _ZN12dScMgSlot1_c8OpenMenuEv
/* Slot 27. Takes no argument: the ROM body never reads r1. The qualified
 * base call is the ROM's direct branch. */
void dScMgSlot1_c::OpenMenu() {
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OpenMenu();
}

// @symbol SlotReels_CheckStopped
extern "C" { /* .c-derived member: C linkage for the whole block */
int SlotReels_CheckStopped(void *self) {
    dScMgSlot1_c *scene = (dScMgSlot1_c *)self;
    int matches;
    int matchedSymbol;
    int reelIndex;

    scene->mWildCount = 0;
    matches = 1;
    for (reelIndex = 0; reelIndex < 3; reelIndex++) {
        if (scene->mReelStrip[reelIndex][scene->mSymbolIndex[reelIndex]] == 5) {
            scene->mWildCount++;
        } else {
            matchedSymbol = scene->mReelStrip[reelIndex][scene->mSymbolIndex[reelIndex]];
            break;
        }
    }
    if (scene->mWildCount >= 3) {
        matchedSymbol = 5;
    } else if (reelIndex < 2) {
        for (reelIndex = reelIndex + 1; reelIndex < 3; reelIndex++) {
            u8 symbol = scene->mReelStrip[reelIndex][scene->mSymbolIndex[reelIndex]];
            if (symbol == 5) {
                scene->mWildCount++;
            } else if (matchedSymbol != symbol) {
                matches = 0;
            }
        }
    }
    if (matches) {
        if (scene->mSpeedTableOffset < 2) {
            scene->mSpeedTableOffset += 1;
        }
        scene->mMatchedSymbol = matchedSymbol;
    }
    return matches;
}
}

// @symbol Slot1_InitHud
extern "C" { /* .c-derived member: C linkage for the whole block */
void Slot1_InitHud(void *self) {
    char *raw = (char *)self;
    ((dScMgSlot1_c *)raw)->mPrevMatchedSymbol = -1;
    *(u8 *)(raw + 0x4707) = 1;
    dScMgSlot1_c *scene = (dScMgSlot1_c *)self;
    scene->mSpeedTableOffset = 0;
    scene->mCredits = Mg_GetSavedRecord0();
    scene->mCreditsShown = scene->mCredits;
}
}

// @symbol _ZN12dScMgSlot1_c10StartRoundEi
/* Slot 18. */
void dScMgSlot1_c::StartRound(int i) {
    char *raw = (char *)this;

    if (i == 4) {
        mPrevMatchedSymbol = mMatchedSymbol;
    } else if (i == 3) {
        Slot1_InitHud(raw);
    }
    SlotBet_Reset(&mBetIcon);
    mState = 0;
}

#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot1_c6RenderEv
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
    SlotBlink_Draw((unsigned char *)&mMatchBlink);
    SlotBlink_Draw((unsigned char *)&mWildBlink);
    return 1;
}

#pragma opt_strength_reduction on

// @symbol _ZN12dScMgSlot1_c8BehaviorEv
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
        SlotBet_OnTouch(&mBetIcon);
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
                mHasMatch = SlotReels_CheckStopped(this);
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
                SlotBlink_Start(&mMatchBlink, sSlotMatchBlinkX[mMatchedSymbol][0],
                                sSlotMatchBlinkY[mMatchedSymbol][0]);
            }
            if (mWildCount != 0 && mWildCount < 3) {
                SlotBlink_Start(&mWildBlink, sSlotWildBlinkX[mWildCount - 1][0],
                                sSlotWildBlinkY[mWildCount - 1][0]);
            }
            if (mPayoutMultiplier != 0) {
                mState = 5;
                mStateTimer = 0x64;
                SlotLamp_Start(&mPayoutLamp);
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
            SlotReel_SetSpeed(&mBetIcon, mPayoutMultiplier);
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
            data_0209f61c.func_0202ec9c(2);
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
    SlotBlink_Tick(&mMatchBlink);
    SlotBlink_Tick(&mWildBlink);
    SlotLamp_Tick(&mPayoutLamp);
    return 1;
}

// @symbol _ZN12dScMgSlot1_c13InitResourcesEv
/* The ov004 scroll words (sMgScoreDigitsX..sMgMainBannerX) name
 * no struct; they are stored raw. The reel-table fill keeps a stepped char*
 * cursor over mReelStrip (see leftovers). */
s32 dScMgSlot1_c::InitResources() {
    char *c = (char *)this;
    volatile unsigned short fill;
    int i, j;

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1210;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1310;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_UP_NCG_BIN, (void *)func_02054d88());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN, (char *)G2::GetBG2CharPtr() + 0x7800);
    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_UP_NCL_BIN);
        GX::LoadBGPltt((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_SCORE_UP_NSC_BIN, G2::GetBG2ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_UP_NSC_BIN, G2::GetBG3ScrPtr());

    data_0209d45c = 0x1c;

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x1410;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & ~3) | 1;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x4010;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~0x40;
    SetSubBg1Offset(0x100, 0);

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1310;
    *(volatile u16 *)0x400100e = *(volatile u16 *)0x400100e & ~0x40;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_DOWN_NCG_BIN, G2S::GetBG2CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN,
                         (char *)G2S::GetBG0CharPtr() + 0x7800);
    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_DOWN_NCL_BIN);
        GXS::LoadBGPltt((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }
    LoadCompressedFileAt(ASSET_D_2D_GRADE_NSC_BIN, G2S::GetBG0ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_BADWIZER_NSC_BIN, (char *)G2S::GetBG1ScrPtr() + 0x800);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_DOWN_NSC_BIN, G2S::GetBG2ScrPtr());

    {
        char *d3 = (char *)G2S::GetBG3ScrPtr();
        fill = 0x43e4;
        MultiStore16(fill, d3, 0x800);
    }

    LoadCompressedFileAt(sSlotNcgByLanguage[GetGameLanguage()], (void *)0x6400000);
    LoadCompressedFileAt(sSlotNcgByLanguage[GetGameLanguage()], (void *)0x6600000);

    {
        int h = LoadFile(ASSET_D_2D_MG_SLOT_NCL_BIN);
        GX::LoadOBJPltt((const void *)h, 0, 0x40);
        GXS::LoadOBJPltt((const void *)h, 0, 0x40);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x1d;
    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 1, 0x1c, 7, 9);

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
    SlotBet_Place((char *)&mBetIcon);
    StartRound(3);
    Mg_SetTopScreenYOffset(0x20);
    data_0208ee44 = 1;
    return 1;
}

/* The constructor is inline, so the factory below holds it: the base
   constructor, mBetIcon, and the three marker clears. */
inline dScMgSlot1_c::dScMgSlot1_c() {
    SlotBlink_Clear((char *)&mMatchBlink);
    SlotBlink_Clear((char *)&mWildBlink);
    SlotLamp_Clear((char *)&mPayoutLamp);
}

// @symbol dScMgSlot1_c_classInit
/* Reconstructed source-style name. Historical alias: func_ov006_0210d6b8. */
extern "C" void *dScMgSlot1_c_classInit(void) {
    return new dScMgSlot1_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_02132894 shard).
 * The Slot3 state table plain-copied from the ROM PMF constants, in
 * retail copy order. */
SlotState sSlot3StateTable[8] = {
    pmf_Slot3_StateIntro,    pmf_Slot3_StateSpin, pmf_Slot3_StateJudge,    pmf_Slot3_StatePayout,
    pmf_Slot3_StatePayout_2, pmf_Slot3_StateWait, pmf_Slot3_StateGameOver, pmf_Slot3_StateDone,
};
