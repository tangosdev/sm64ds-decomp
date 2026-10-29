//cpp
/* dScMgAmida_c -- the ladder-lottery (Amida) minigame, ov006, 28 functions
 * (.text 0x020d1018..0x020d5a54).
 *
 * This file is the class's whole ov006 unit, in ROM order, lowest address
 * first, under `#pragma defer_codegen off`: from the destructor pair at
 * 0x020d1018 to dScMgAmida_c_Piece's constructor, the piece array's element
 * constructor.
 * The previous unit ends at 0x020d1018 with TrmpLine_Reset, and
 * dScMgBomroom_c's D1 starts the next at 0x020d5a54. tu_map stops this unit
 * at 0x020d5974; the relocation census in the manifest shows that the
 * factory and the piece constructor above it belong to it too. The destructor
 * is the key function, so this file also emits the vtable and RTTI.
 * The native profile, tables and metadata now own .data 0x0213b814..0x0213b9ac.
 * This production assignment does not establish the original source-file cut.
 *
 * The fifteen functions below Amida_RestoreBgChars came from one-function
 * files. Their bodies are kept as they matched there, with these changes:
 *   - ShuffleGoals, CheckHurryButton, CheckEdgeBoost and StepWalkers
 *     called slot 36 through stand-in vtable structs; they now call
 *     IsVariant2().
 *   - ProbeRung took the two rung ends as int pointers; it now takes them
 *     as by-value Points, the way the pen handler already declared it,
 *     and uses the member names.
 *   - gTouchX/gTouchY are declared as 4-byte rows, the form
 *     CheckHurryButton matched under. Indexing them as flat arrays costs
 *     it four words.
 *   - each shard's file-wide optimisation pragma is now a push/pop bracket
 *     around its own function.
 *   - StepWalkers was the last to match. It was an unmatched draft from
 *     2026-09-14 and was matched on 2026-10-02.
 *
 * Remaining matching constraints:
 * - StepWalkers, RenderBoard, Render and Behavior step a char* through
 *   the per-walker, per-lane and per-piece arrays and spell each field as
 *   element 0 of the stepped pointer (`((dScMgAmida_c *)lane)->
 *   mLaneAnimTimer[0]`). Indexing the member directly diffs: StepWalkers'
 *   pointer forms move bytes, and Behavior's piece walk keeps PIECE_OFF's
 *   cast arithmetic (see the macro). StepWalker takes plain members.
 * - Amida_RestoreBgChars and Amida_SaveBgChars stay free helpers: the
 *   object pointer they take is never read -- both only copy BG char VRAM --
 *   so membership cannot be proven from the bodies.
 * - gTouchX/gTouchY stay 4-byte rows (flat indexing costs CheckHurryButton
 *   four words, measured at promotion), the pattern table is five separate
 *   data symbols. mLineColor is named for its only use, the colour
 *   argument of the line-draw helper (always 2).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgAmida_c_AssetId {
    ASSET_D_2D_MG_AMIDA_FACE_F_NCG_BIN = 0x000a,
    ASSET_D_2D_MG_AMIDA_FACE_G_NCG_BIN = 0x000b,
    ASSET_D_2D_MG_AMIDA_FACE_I_NCG_BIN = 0x000c,
    ASSET_D_2D_MG_AMIDA_FACE_NCG_BIN = 0x000d,
    ASSET_D_2D_MG_AMIDA_FACE_NCL_BIN = 0x000e,
    ASSET_D_2D_MG_AMIDA_FACE_S_NCG_BIN = 0x000f,
    ASSET_D_2D_MG_AMIDA_NCG_BIN = 0x0010,
    ASSET_D_2D_MG_AMIDA_NCL_BIN = 0x0011,
    ASSET_D_2D_MG_BG_AMIDA1_NCG_BIN = 0x0015,
    ASSET_D_2D_MG_BG_AMIDA1_NCL_BIN = 0x0016,
    ASSET_D_2D_MG_BG_AMIDA1_NSC_BIN = 0x0017,
    ASSET_D_2D_MG_BG_AMIDA2_1_NSC_BIN = 0x0018,
    ASSET_D_2D_MG_BG_AMIDA2_2_NSC_BIN = 0x0019,
    ASSET_D_2D_MG_BG_AMIDA2_NCG_BIN = 0x001a,
    ASSET_D_2D_MG_BG_AMIDA2_NCL_BIN = 0x001b,
    ASSET_D_2D_MG_BG_AMIDA_FACE_DOWN_NCG_BIN = 0x001c,
    ASSET_D_2D_MG_BG_AMIDA_FACE_DOWN_NCL_BIN = 0x001d,
    ASSET_D_2D_MG_BG_AMIDA_FACE_DOWN_NSC_BIN = 0x001e,
    ASSET_D_2D_MG_BG_AMIDA_FACE_UP_NCG_BIN = 0x001f,
    ASSET_D_2D_MG_BG_AMIDA_FACE_UP_NCL_BIN = 0x0020,
    ASSET_D_2D_MG_BG_AMIDA_FACE_UP_NSC_BIN = 0x0021,
    ASSET_D_2D_MG_BG_OPTION_NSC_BIN = 0x005b,
    ASSET_D_2D_MG_OPTION_F_NCG_BIN = 0x00db,
    ASSET_D_2D_MG_OPTION_G_NCG_BIN = 0x00dc,
    ASSET_D_2D_MG_OPTION_I_NCG_BIN = 0x00dd,
    ASSET_D_2D_MG_OPTION_NCG_BIN = 0x00de,
    ASSET_D_2D_MG_OPTION_S_NCG_BIN = 0x00df
};

#include "types.h"
#include "G2.h"
#include "decl_common.h"
#include "dScMgAmida_c.h"
#include "SpawnInfo.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "CP15.h"
#include "GX.h"
#include "G2x.h"

extern "C" {
/* --- shared ov004 and main helpers --- */
extern void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern void Mg_DrawScoreTally(void *a0, int a1, int a2, int a3);
extern void MultiCopyHalf(void *dst, void *src, int nbytes);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern int RandomIntInternal(int *seed);
extern int Vec2_Len(int *v);
extern void FreeGfxSlotsById(int a);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern int LoadFile(int handle);
extern void SetSubBg2Offset(int a, int b);
extern void func_02012dbc(int a);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int a4);
extern int func_02012468(int handle, int a, int sfx, int b, int c, int d, int pan, short e);
extern int GetGameLanguage(void);

/* --- the class's own functions defined below --- */
extern void Amida_RestoreBgChars(void *);
extern void Amida_SaveBgChars(void *sb);
extern void *dScMgAmida_c_classInit(void);

/* --- data --- */
extern s16 data_02082214[];
extern u8 data_0209d45c;
extern u8 data_0209d454;
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern u8 gActivePlayerSlot;
// local extern: see above.
extern u8 gTouchHeld[];
// local extern: see above.
extern u8 gTouchEdge[];
// local extern: see above.
extern u8 gTouchX[][4];
// local extern: see above.
extern u8 gTouchY[][4];
extern int sAmidaPatternSpeedUp[];
extern s32 data_0208ee44;
extern char sAmidaPatterns[];
extern char sAmidaPatternGoalShuffle[];
extern char sAmidaPatternSpeeds[];
extern char sAmidaPatternRungSkip[];
extern char sAmidaPatternExtraRungs[];

/* File-local view of one 0x1c-byte pattern record, indexed by
   mPatternIndex. Each field but walkerStagger has its own symbol above
   (sAmidaPatterns is the record's first word, the walker count), and the
   readers keep those symbols. walkerStagger (+0x14) has none: the round
   setup multiplies it by 0x3c and the walker index into mWalkerDelay, so
   it is the seconds between walkers. */
struct AmidaPattern {
    s32 walkerCount;   /* 0x00 sAmidaPatterns */
    s32 goalShuffle;   /* 0x04 sAmidaPatternGoalShuffle */
    s32 speed;         /* 0x08 sAmidaPatternSpeeds */
    s32 rungSkip;      /* 0x0c sAmidaPatternRungSkip */
    s32 extraRungs;    /* 0x10 sAmidaPatternExtraRungs */
    s32 walkerStagger; /* 0x14 */
    s32 speedUp;       /* 0x18 sAmidaPatternSpeedUp */
};
typedef char AmidaPattern_size_must_be_0x1c[sizeof(AmidaPattern) == 0x1c ? 1 : -1];
extern void *data_ov006_0213a32c;
extern void *sAmidaSideSprite;
extern int data_ov006_0213a338[];
extern void *sAmidaBoardAnimSprites[];
extern void *sAmidaLaneAnimSprites[];
extern void *sAmidaWalkerSprites[];
extern int sAmidaLaneResultSprites[];
extern int sAmidaLaneWantSprites[];
extern int sAmidaBoardAnimationFrames[13];
extern s32 sAmidaLaneTargets[];
extern s32 sAmidaLaneSaved[];
}

namespace Memory {
void *Allocate(unsigned int size);
void Deallocate(void *ptr);
}

typedef struct {
    int x;
    int y;
} AmidaRungPosition;
extern AmidaRungPosition sAmidaInitialRungPositions[11];

typedef struct {
    int values[14];
} AmidaRenderPeriods;
extern AmidaRenderPeriods sAmidaRenderPeriods;

/* PIECE_AT(p) is piece 0 seen from a char* that has been stepped by
   whole pieces; PIECE_OFF names one field's offset from the scene. Both
   keep the cast arithmetic: the member form `&((dScMgAmida_c *)p)->mPieces[0]`
   grows Behavior by 0xc (measured 2026-10-20). */
#define PIECE_OFF(m) ((int)&((dScMgAmida_c *)0)->mPieces[0].m)
#define PIECE_AT(p) ((dScMgAmida_c_Piece *)((p) + PIECE_OFF(posX)))

/* Launder helpers for Behavior's piece walk: AT makes the compiler
   materialise each address on its own instead of folding it into a
   neighbouring add, which is what the ROM does there. */
#define AT(p, off) ((void*)(int)((char*)(p)+(off)))
#define IP(p, o) (*(int*)((char*)(p)+(o)))
#define IAP(p, o) (*(int*)AT(p,(o)))

#pragma defer_codegen off

// @symbol _ZN12dScMgAmida_cD1Ev
// @symbol _ZN12dScMgAmida_cD0Ev
/* Empty: the compiler tears the four member arrays down in reverse
   declaration order through __cxa_vec_cleanup -- the 0x80 pieces (with
   dScMgAmida_c_Piece's destructor), then the lane velocities, the lane
   positions and the cells (with Vector2's, NullDestructor_0203d47c). D1
   stores the vptr and ends in dScMgBase_c's D2; D0 repeats the same body and
   adds the delete. */
dScMgAmida_c::~dScMgAmida_c() {}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgAmida_c() above,
 * so this spells out what the deleting destructor does in terms of it: the
 * D1 body, called qualified so it is a direct call, then the class-specific
 * operator delete. mwccarm never defines _MSC_VER, so nothing here reaches
 * the cartridge object. */
extern "C" dScMgAmida_c *_ZN12dScMgAmida_cD0Ev(dScMgAmida_c *thiz) {
    thiz->dScMgAmida_c::~dScMgAmida_c();
    dScMgAmida_c::operator delete(thiz);
    return thiz;
}
#endif

// @symbol _ZN18dScMgAmida_c_PieceD1Ev
/* dScMgAmida_c_Piece's empty inline destructor (dScMgAmida_c.h), emitted
   here, after the class destructor that first uses it. */

// @symbol _ZN12dScMgAmida_c12IsAltVariantEv
/* Slot 35, the family's only override of it. The base asks whether the low
   byte of fBase_c::param1 is nonzero; this asks whether it is exactly 1.
   Nothing in this class calls it; the callers are in dScMgCoin_c,
   dScMgPanel_c, dScMgSound_c and dScMgSnowball_c. */
int dScMgAmida_c::IsAltVariant() {
    return (param1 & 0xff) == 1;
}

// @symbol _ZN12dScMgAmida_c10IsVariant2Ev
/* Slot 36, this class's own: whether the low byte of param1 (the menu
   entry's sceneParam) is 2. The Rec Room lists Amida twice, and its second
   entry (row 24) passes 2 (notes/minigames/runtime-checks.md, IsAltVariant),
   so this picks that variant. IsAltVariant above is the == 1 test (the
   base's own asks only for nonzero). */
int dScMgAmida_c::IsVariant2() {
    return (((int)param1) & 0xff) == 2;
}

// @symbol _ZN12dScMgAmida_c15InitSubScreenBgEv
/* Slot 31: the sub screen's BG1 set up for this game. Same shape as the
   base's, with 4 for the BG1CNT base bits and this class's own language
   table. Not a Kill; see the slot-31 block in include/dScMgBase_c.h. */
int dScMgAmida_c::InitSubScreenBg() {
    volatile unsigned short *reg = (volatile unsigned short *)0x400100a;
    int id;
    void *t;
    *reg = (unsigned short)((*reg & 0x43) | 4);
    *reg = (unsigned short)(*reg & ~0x40);
    *reg = (unsigned short)(*reg & ~3);
    SetSubBg1Offset(0, 0);
    data_0209d454 = (unsigned char)(data_0209d454 & ~2);
    id = GetGameLanguage();
    t = (void *)G2S::GetBG1CharPtr();
    LoadCompressedFileAt(sAmidaOptionNcgByLanguage[id], t);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_OPTION_NSC_BIN, G2S::GetBG1ScrPtr());
}

// @symbol _ZN12dScMgAmida_c12ShuffleGoalsEi
/* Refills and shuffles mLaneResult. When IsVariant2() holds, the entries
   come from the pattern index, are copied to mLaneWants, and the table is
   reshuffled until it differs somewhere from that copy as seen through
   mLanePerm; otherwise the first arg1 entries (at most 4) are 1 and
   the rest 0, shuffled once. */
#pragma push
#pragma opt_strength_reduction off
void dScMgAmida_c::ShuffleGoals(int arg1) {
    int i;
    int k;
    int j;
    int t;
    int u;
    int *pi;

    if (IsVariant2()) {
        switch (mPatternIndex) {
        case 0:
            mLaneResult[0] = 0;
            mLaneResult[1] = 0;
            mLaneResult[2] = 3;
            mLaneResult[3] = 3;
            break;
        case 1:
        case 3:
        case 5:
            mLaneResult[0] = 0;
            mLaneResult[1] = 0;
            mLaneResult[2] = 1;
            mLaneResult[3] = 3;
            break;
        default:
            for (i = 0; i < 4; i++)
                mLaneResult[i] = i;
            break;
        }
        for (i = 0; i < 4; i++)
            mLaneWants[i] = mLaneResult[i];
        {
            /* The swap addresses are built as this + i*4 + offset: the
               plain &mLaneResult[i] form shrinks ShuffleGoals by 0x18
               (measured 2026-10-20). */
            int off = (int)&((dScMgAmida_c *)0)->mLaneResult;
            arg1 = 1;
            do {
                for (i = 0; i < 4; i++) {
                    t = RandomIntInternal(&data_0209d4b8);
                    pi = (int *)((char *)this + (i << 2) + off);
                    j = ((u32)(((u32)t >> 16) & 0x7fff) * 4) >> 15;
                    t = *pi;
                    {
                        int *pjx = (int *)((char *)this + (j << 2) + off);
                        u = *pjx;
                        *pi = u;
                        *pjx = t;
                    }
                }
                for (k = 0; k < 4; k++) {
                    if (mLaneWants[k] != mLaneResult[mLanePerm[k]]) {
                        arg1 = 0;
                        break;
                    }
                }
            } while (arg1 == 1);
        }
    } else {
        if (arg1 > 4)
            arg1 = 4;
        for (k = 0; k < arg1; k++)
            mLaneResult[k] = 1;
        for (; arg1 < 4; arg1++)
            mLaneResult[arg1] = 0;
        {
            int off = (int)&((dScMgAmida_c *)0)->mLaneResult;
            for (i = 0; i < 4; i++) {
                t = RandomIntInternal(&data_0209d4b8);
                pi = (int *)((char *)this + (i << 2) + off);
                j = ((u32)(((u32)t >> 16) & 0x7fff) * 4) >> 15;
                t = *pi;
                {
                    int *pjx = (int *)((char *)this + (j << 2) + off);
                    u = *pjx;
                    *pi = u;
                    *pjx = t;
                }
            }
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c11ClearStrokeEv
void dScMgAmida_c::ClearStroke() {
    int row = 0;
    int col;
    int rowoff = 0;
    int one = 0;
    int val = 0;
    for (; row < 0x100; row++) {
        for (col = 0; col < 0x158; col++) {
            unsigned char *base = mStrokeGrid;
            unsigned char *p = (unsigned char *)(rowoff + (int)base) + col;
            if (*p != 0)
                *p = val;
        }
        rowoff += 0x158;
    }
    Amida_RestoreBgChars(this);
    mLineStartSet = 0;
}

// @symbol _ZN12dScMgAmida_c11PaintSquareEiiii
/* Slot 34 probes or paints the ladder grid. Its first coordinate is the
   horizontal lane position; the grid is stored by x, then by y + 0xc0.
   In probe mode, a diagonal also tests the two adjacent cells so it cannot
   slip between occupied cells. Marks of 3 or above are drawn rungs.

   The fourth argument is unused: this override chooses a brush size of 2
   or 4 before calling the base implementation. */
void dScMgAmida_c::PaintSquare(int brushX, int brushY, int colour, int /* size */) {

    u8 lineEndSet;
    u8 setupInk;
    int gridY;

    if (mProbeMode == 1) {
        int prevX;
        int prevXMinusOne;
        int prevXPlusOne;
        if ((mInkGrid + brushX * 0x158)[brushY + 0xc0] >= 3u) {
            mProbeHit = 1;
        }
        prevX = mPrev.x;
        if (brushX != prevX || brushY != mPrev.y) {
            prevXMinusOne = prevX - 1;
            if (brushX == prevXMinusOne && brushY == mPrev.y - 1) {
                u8 *ink = mInkGrid;
                if ((ink + (brushX + 1) * 0x158)[brushY + 0xc0] >= 3u) {
                    mProbeHit = 1;
                } else if ((ink + brushX * 0x158)[brushY + 0xc1] >= 3u) {
                    mProbeHit = 1;
                }
            } else {
                prevXPlusOne = prevX + 1;
                if (brushX == prevXPlusOne && brushY == mPrev.y - 1) {
                    u8 *ink = mInkGrid;
                    if ((ink + (brushX - 1) * 0x158)[brushY + 0xc0] >= 3u) {
                        mProbeHit = 1;
                    } else if ((ink + brushX * 0x158)[brushY + 0xc1] >= 3u) {
                        mProbeHit = 1;
                    }
                } else if (brushX == prevXMinusOne && brushY == mPrev.y + 1) {
                    u8 *ink = mInkGrid;
                    if ((ink + (brushX + 1) * 0x158)[brushY + 0xc0] >= 3u) {
                        mProbeHit = 1;
                    } else if ((ink + brushX * 0x158)[brushY + 0xbf] >= 3u) {
                        mProbeHit = 1;
                    }
                } else if (brushX == prevXPlusOne && brushY == mPrev.y + 1) {
                    u8 *ink = mInkGrid;
                    if ((ink + (brushX - 1) * 0x158)[brushY + 0xc0] >= 3u) {
                        mProbeHit = 1;
                    } else if ((ink + brushX * 0x158)[brushY + 0xbf] >= 3u) {
                        mProbeHit = 1;
                    }
                }
            }
        }
        mPrev.x = brushX;
        mPrev.y = brushY;
        return;
    }

    if (mLineCommit == 1) {
        dScMgBase_c::PaintSquare(brushX, brushY, colour, 4);
        if (brushX < 0)
            return;
        if (brushX >= 0x100)
            return;
        if (brushY < -0xc0)
            return;
        if (brushY >= 0x98)
            return;
        (mInkGrid + brushX * 0x158)[brushY + 0xc0] = mLineCount;
        return;
    }

    lineEndSet = mLineEndSet;
    if (lineEndSet == 1) {
        dScMgBase_c::PaintSquare(brushX, brushY, colour, 2);
        return;
    }

    setupInk = mSetupInk;
    gridY = brushY + 0xc0;
    if (setupInk != 0) {
        dScMgBase_c::PaintSquare(brushX, brushY, colour, 4);
        if (brushX >= 0 && brushX < 0x100 && gridY >= 0 && gridY < 0x158) {
            (mInkGrid + brushX * 0x158)[gridY] = mLineCount;
        }
    } else {
        u8 mark;
        if (brushX < 0 || brushX >= 0x100 || gridY < 0 || gridY >= 0x158) {
            dScMgBase_c::PaintSquare(brushX, brushY, colour, 4);
            return;
        }
        if (brushX == 0x20 || brushX == 0x60 || brushX == 0xa0 || brushX == 0xe0) {
            if (mLineStartSet == 0) {
                mLineStartSet = 1;
                mLineStart.x = brushX;
                mLineStart.y = brushY;
                mLineEnd.x = brushX;
                mLineEnd.y = brushY;
                return;
            }
            if (lineEndSet != 0)
                return;
            if (brushX == mLineStart.x) {
                mLineEnd.y = brushY;
            } else {
                mLineEndSet = 1;
                mLineEnd.x = brushX;
                mLineEnd.y = brushY;
            }
            return;
        }
        if (mLineStartSet == 0) {
            dScMgBase_c::PaintSquare(brushX, brushY, colour, 2);
            return;
        }
        mark = (mStrokeGrid + brushX * 0x158)[gridY];
        if (mark != 0 && mLineCount != mark)
            goto fail;
        mark = (mInkGrid + brushX * 0x158)[gridY];
        if (mark != 0 && mLineCount != mark) {
        fail:
            mLineStartSet = 0;
            mLineEndSet = 1;
            return;
        }
    }

    dScMgBase_c::PaintSquare(brushX, brushY, colour, 2);
    (mStrokeGrid + brushX * 0x158)[gridY] = mLineCount;
}

// @symbol _ZN12dScMgAmida_c16CheckHurryButtonEv
void dScMgAmida_c::CheckHurryButton() {
    if (IsVariant2() == 0)
        return;
    if (mHurry == 1)
        return;
    unsigned int pressed = 0;
    int playerSlot = gActivePlayerSlot;
    if (gTouchHeld[playerSlot << 2] != 0) {
        unsigned int touchOffset = playerSlot << 2;
        if (gTouchEdge[touchOffset] != 0)
            pressed = 1;
    }
    if (pressed == 0)
        return;
    int touchX = gTouchX[playerSlot][0];
    int touchY = gTouchY[playerSlot][0];
    if (touchX < 0x60)
        return;
    if (touchX >= 0xa0)
        return;
    if (touchY < 0xa0)
        return;
    if (touchY >= 0xc0)
        return;
    mHurry = 1;
    Sound::PlayBank2_2D(0x151);
}

// @symbol _ZN12dScMgAmida_c14CheckEdgeBoostEv
void dScMgAmida_c::CheckEdgeBoost() {
    int playerSlot;
    int pressed;
    int touchX;
    int touchY;

    if (IsVariant2())
        return;

    playerSlot = gActivePlayerSlot;
    pressed = 0;
    if (gTouchHeld[playerSlot * 4]) {
        if (gTouchEdge[playerSlot * 4] != 0)
            pressed = 1;
    }
    if (pressed != 0) {
        mPenLocked = 0;
        mEdgeBoostCount = 0;
    }
    playerSlot = gActivePlayerSlot;
    if (gTouchHeld[playerSlot * 4] != 0) {
        touchX = gTouchX[playerSlot][0];
        touchY = gTouchY[playerSlot][0];
        if ((touchX >= 0 && touchX < 0x10 && touchY >= 0x40 && touchY < 0x80) ||
            (touchX >= 0xf0 && touchX < 0x100 && touchY >= 0x40 && touchY < 0x80)) {
            if (mEdgeBoostCount >= 5) {
                mEdgeBoost = 1;
            } else {
                mEdgeBoostCount += 1;
                mEdgeBoost = 0;
            }
            mPenLocked = 1;
        } else {
            mEdgeBoost = 0;
            mPenLocked = 0;
            mEdgeBoostCount = 0;
        }
    } else {
        mEdgeBoost = 0;
        mEdgeBoostCount = 0;
    }
}

// @symbol _ZN12dScMgAmida_c9HandlePenEv
/* The pen handler: follows the stylus while it is down, and on release
   turns the stroke into a rung between two neighbouring lanes, probed with
   ProbeRung before it is painted. Point's special members are
   what shape the calls; see include/dScMgAmida_c.h. */
void dScMgAmida_c::HandlePen() {
    int oldX;
    int oldY;

    if (this->mLineCount >= 0xff)
        return;
    if (this->mHurry == 1)
        return;

    oldX = this->mPen.x;
    oldY = this->mPen.y;

    if (gTouchHeld[gActivePlayerSlot * 4] != 0) {
        int newX = gTouchX[gActivePlayerSlot][0];
        int newY = gTouchY[gActivePlayerSlot][0];
        if (newX < 0)
            return;
        if (newX >= 0x100)
            return;
        if (newY < 0)
            return;
        if (newY >= this->mLineEndY)
            return;
        if (this->mPenLocked != 0)
            return;
        this->mPen.x = newX;
        this->mPen.y = newY;
        if (this->mPenStart.x == -1 || this->mPenStart.y == -1) {
            this->mPenStart.x = newX;
            this->mPenStart.y = newY;
            func_02012718(0xdc, this->mPenStart.x << 12);
        }
        if (oldX == -1 && oldY == -1)
            return;
        if (oldX == this->mPen.x && oldY == this->mPen.y) {
            this->mPenTapped = 1;
            return;
        }
        Mg_DrawLine(oldX, oldY, this->mPen.x, this->mPen.y, this->mLineColor, 1);
        if (this->mPenTapped == 1) {
            this->mPenSoundHandle = func_02012468(this->mPenSoundHandle, 2, 0xde, 4, 0, 0,
                                                  func_020126e8(this->mPen.x << 12), 0);
            return;
        }
        this->mPenSoundHandle = func_02012468(this->mPenSoundHandle, 2, 0xdd, 4, 0, 0,
                                              func_020126e8(this->mPen.x << 12), 0);
        return;
    } else {
        int released;
        if (gTouchHeld[gActivePlayerSlot * 4] == 0 && gTouchEdge[gActivePlayerSlot * 4] != 0) {
            released = 1;
        } else {
            released = 0;
        }
        if (released != 0) {
            if (this->mPenStart.x != -1 && this->mPenStart.y != -1 && oldX != -1 && oldY != -1) {
                if (this->mLineStartSet == 0) {
                    int d1 = oldX - this->mPenStart.x;
                    if (d1 < 0)
                        d1 = -d1;
                    if (d1 < 0x10) {
                        int d2 = oldY - this->mPenStart.y;
                        if (d2 < 0)
                            d2 = -d2;
                        if (d2 >= 0x10)
                            goto classify;
                    } else {
                    classify:
                        if (oldX >= 0x21 && oldX <= 0x3f) {
                            if (this->mPenStart.x >= 0x41 && this->mPenStart.x <= 0x5f) {
                                this->mLineStart.x = 0x60;
                                this->mLineStart.y = this->mPenStart.y;
                                this->mLineEnd.x = 0x20;
                                this->mLineEnd.y = this->mPen.y;
                            }
                        } else if (oldX >= 0x41 && oldX <= 0x5f) {
                            if (this->mPenStart.x >= 0x21 && this->mPenStart.x <= 0x3f) {
                                this->mLineStart.x = 0x20;
                                this->mLineStart.y = this->mPenStart.y;
                                this->mLineEnd.x = 0x60;
                                this->mLineEnd.y = this->mPen.y;
                            }
                        } else if (oldX >= 0x61 && oldX <= 0x7f) {
                            if (this->mPenStart.x >= 0x81 && this->mPenStart.x <= 0x9f) {
                                this->mLineStart.x = 0xa0;
                                this->mLineStart.y = this->mPenStart.y;
                                this->mLineEnd.x = 0x60;
                                this->mLineEnd.y = this->mPen.y;
                            }
                        } else if (oldX >= 0x81 && oldX <= 0x9f) {
                            if (this->mPenStart.x >= 0x61 && this->mPenStart.x <= 0x7f) {
                                this->mLineStart.x = 0x60;
                                this->mLineStart.y = this->mPenStart.y;
                                this->mLineEnd.x = 0xa0;
                                this->mLineEnd.y = this->mPen.y;
                            }
                        } else if (oldX >= 0xa1 && oldX <= 0xbf) {
                            if (this->mPenStart.x >= 0xc1 && this->mPenStart.x <= 0xdf) {
                                this->mLineStart.x = 0xe0;
                                this->mLineStart.y = this->mPenStart.y;
                                this->mLineEnd.x = 0xa0;
                                this->mLineEnd.y = this->mPen.y;
                            }
                        } else if (oldX >= 0xc1 && oldX <= 0xdf) {
                            if (this->mPenStart.x >= 0xa1 && this->mPenStart.x <= 0xbf) {
                                this->mLineStart.x = 0xa0;
                                this->mLineStart.y = this->mPenStart.y;
                                this->mLineEnd.x = 0xe0;
                                this->mLineEnd.y = this->mPen.y;
                            }
                        }
                        if (this->mLineStart.x >= 0 && this->mLineEnd.x >= 0) {
                            if (ProbeRung(this->mLineStart, this->mLineEnd) == 0) {
                                this->mLineStartSet = 1;
                                this->mLineEndSet = 1;
                            }
                        }
                    }
                } else if (this->mLineEndSet == 0) {
                    int dx, dsx, adx, adsx;
                    dx = oldX - this->mLineStart.x;
                    adx = dx < 0 ? -dx : dx;
                    dsx = this->mPenStart.x - this->mLineStart.x;
                    adsx = dsx < 0 ? -dsx : dsx;
                    if (adx >= adsx) {
                        dx = dx < 0 ? -dx : dx;
                        if (dx >= 0x20) {
                            if (oldX < this->mLineStart.x) {
                                this->mLineEnd.x = this->mLineStart.x - 0x40;
                            } else {
                                this->mLineEnd.x = this->mLineStart.x + 0x40;
                            }
                            this->mLineStart.y = this->mLineEnd.y;
                            this->mLineEnd.y = this->mPen.y;
                        }
                    } else {
                        dsx = dsx < 0 ? -dsx : dsx;
                        if (dsx >= 0x20) {
                            if (this->mPenStart.x < this->mLineStart.x) {
                                this->mLineEnd.x = this->mLineStart.x - 0x40;
                            } else {
                                this->mLineEnd.x = this->mLineStart.x + 0x40;
                            }
                            this->mLineEnd.y = this->mPenStart.y;
                        }
                    }
                    if (this->mLineStart.x >= 0 && this->mLineEnd.x >= 0) {
                        if (ProbeRung(this->mLineStart, this->mLineEnd) == 0) {
                            this->mLineEndSet = 1;
                        }
                    }
                }
            }

            if (this->mLineStartSet == 0 || this->mLineEndSet == 0) {
                if (this->mPenLocked == 0 && this->mHurry == 0)
                    func_02012790(0xe);
                ClearStroke();
                this->mLineStart.x = -1;
                this->mLineStart.y = -1;
                this->mLineEnd.x = -1;
                this->mLineEnd.y = -1;
                return;
            }
            if (this->mLineEnd.x == this->mLineStart.x) {
                Amida_RestoreBgChars(this);
                int row = 0;
                int col;
                int rowoff = 0;
                int one = 0;
                int val = 0;
                for (; row < 0x100; row++) {
                    for (col = 0; col < 0x158; col++) {
                        (this->mStrokeGrid + rowoff)[col] = val;
                    }
                    rowoff += 0x158;
                }
            } else {
                Point a(mLineStart);
                Point b(mLineEnd);
                int hit;
                if (a.x < b.x) {
                    a.x += 2;
                    b.x -= 2;
                } else {
                    a.x -= 2;
                    b.x += 2;
                }
                {
                    if (ProbeRung(a, b) == 1) {
                        if (this->mPenLocked == 0 && this->mHurry == 0)
                            func_02012790(0xe);
                        ClearStroke();
                        this->mLineStart.x = -1;
                        this->mLineStart.y = -1;
                        this->mLineEnd.x = -1;
                        this->mLineEnd.y = -1;
                        return;
                    }
                }
                if (this->mLineStart.x < this->mLineEnd.x) {
                    u8 *row = this->mInkGrid + this->mLineEnd.x * 0x158;
                    hit = *(row + this->mLineEnd.y + 0x218);
                    if (hit == 0) {
                        hit = *(this->mInkGrid + this->mLineStart.x * 0x158 + this->mLineStart.y -
                                0x98);
                    }
                } else {
                    u8 *row = this->mInkGrid + this->mLineEnd.x * 0x158;
                    hit = *(row + this->mLineEnd.y - 0x98);
                    if (hit == 0) {
                        hit = *(this->mInkGrid + this->mLineStart.x * 0x158 + this->mLineStart.y +
                                0x218);
                    }
                }
                if (hit == 0) {
                    ClearStroke();
                    this->mLineCommit = 1;
                    Mg_DrawLine(a.x, a.y, b.x, b.y, this->mLineColor, 1);
                    if (a.x < b.x) {
                        Mg_DrawLine(a.x - 1, a.y, a.x - 1, a.y, this->mLineColor, 1);
                        Mg_DrawLine(b.x + 1, b.y, b.x + 1, b.y, this->mLineColor, 1);
                    } else {
                        Mg_DrawLine(a.x + 1, a.y, a.x + 1, a.y, this->mLineColor, 1);
                        Mg_DrawLine(b.x - 1, b.y, b.x - 1, b.y, this->mLineColor, 1);
                    }
                    this->mLineCommit = 0;
                    Amida_SaveBgChars(this);
                    this->mLineCount++;
                    Sound::PlayBank2_2D(0xdf);
                } else {
                    if (this->mPenLocked == 0 && this->mHurry == 0)
                        func_02012790(0xe);
                    ClearStroke();
                    this->mLineStart.x = -1;
                    this->mLineStart.y = -1;
                    this->mLineEnd.x = -1;
                    this->mLineEnd.y = -1;
                    return;
                }
            }
        }
        this->mPen.x = -1;
        this->mPen.y = -1;
        this->mPenStart.x = -1;
        this->mPenStart.y = -1;
        this->mLineStartSet = 0;
        this->mLineEndSet = 0;
        this->mPenTapped = 0;
    }
}

// @symbol _ZN12dScMgAmida_c9ProbeRungENS_5PointES0_
/* Probes the rung from p1 to p2 without painting it: slot 34 runs in probe
   mode over the line and reports whether any cell it crosses is taken. */
int dScMgAmida_c::ProbeRung(Point p1, Point p2) {
    int startY, startX;
    this->mProbeHit = 0;
    this->mProbeMode = 1;
    startY = p1.y;
    startX = p1.x;
    this->mPrev.x = startX;
    this->mPrev.y = startY;
    Mg_DrawLine(p1.x, p1.y, p2.x, p2.y, this->mLineColor, 1);
    this->mProbeMode = 0;
    return this->mProbeHit;
}

// @symbol _ZN12dScMgAmida_c10StepWalkerEii
#pragma push
#pragma opt_common_subs off
int dScMgAmida_c::StepWalker(int walker, int dir) {
    int gridX, gridY;
    u8 lineMark;
    u8 *walkerMark;

    gridX = mWalkerCell[walker].x;
    gridY = mWalkerCell[walker].y + 0xc0;

    // Directions run across a 3x3 neighbourhood, skipping the centre.
    switch (dir) {
    case 0:
        gridX--;
        gridY--;
        break;
    case 1:
        gridY--;
        break;
    case 2:
        gridX++;
        gridY--;
        break;
    case 3:
        gridX--;
        break;
    case 4:
        gridX++;
        break;
    case 5:
        gridX--;
        gridY++;
        break;
    case 6:
        gridY++;
        break;
    case 7:
        gridX++;
        gridY++;
        break;
    default:
        return 0;
    }

    {
        u8 *marks = (u8 *)mWalkerMark;
        lineMark = marks[walker];
        walkerMark = marks + walker;
    }

    if (lineMark >= 2) {
        u8 nextMark = *(u8 *)(mInkGrid + gridX * 0x158 + gridY);
        if (nextMark != lineMark)
            return 0;

        mWalkerDir[walker] = dir;
        mWalkerCell[walker].x = gridX;
        mWalkerCell[walker].y = gridY - 0xc0;
        return 1;
    } else {
        int progress500, soundPosition;

        if (*(u8 *)(mInkGrid + gridX * 0x158 + gridY) < 2)
            return 0;

        mWalkerDir[walker] = dir;
        *walkerMark = *(u8 *)(mInkGrid + gridX * 0x158 + gridY);
        mWalkerCell[walker].x = gridX;
        mWalkerCell[walker].y = gridY - 0xc0;

        progress500 = (mWalkerCell[walker].y + 0xd4) * 0x1f4 / (mLineEndY + 0xd4);
        soundPosition = func_020126e8(mWalkerCell[walker].x << 0xc);
        func_020126ac(0x1bf, 6, 0, progress500, soundPosition);
        return 1;
    }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c11StepWalkersEv
/* The per-step routine Behavior runs mScrollAccum / 16 times a frame:
   counts down mWalkerDelay, then steps each walker along the drawn lines
   with StepWalker; once every walker is in (mWalkersDone reaches
   mWalkerCount) it closes the round.
   The `if (mRoundCount >= 5)` arm returns on its own while the
   0x1e path and the next-round store share the one `return` after the
   `reachedGoal == 1` if/else. That keeps the two arms in different blocks, so
   mwccarm keeps the ROM's branch there instead of predicating it
   (notes/mwccarm-codegen.md 6cn). */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgAmida_c::StepWalkers() {
    char *sceneBytes = (char *)this;
    s32 delayIndex;
    s32 walker;
    s32 walkerCount;

    for (delayIndex = 0; delayIndex < (walkerCount = mWalkerCount);) {
        s32 *delaySlot = &((dScMgAmida_c *)((int)sceneBytes + delayIndex * 4))->mWalkerDelay[0];
        s32 delay = *delaySlot;
        delayIndex++;
        if (delay > 0) {
            *delaySlot = delay - 1;
        }
    }

    walker = 0;
    if (walkerCount <= 0) {
        return;
    }

    {
        s32 v20 = 0xd7;
        s32 v1c = 0xcf;
        s32 v18 = 0xc7;
        s32 v14 = 0xbf;
        s32 v10 = 0xe0;
        s32 v34 = 2;
        s32 v30 = 5;
        s32 v38 = 7;
        s32 vC = 0;
        s32 v60 = 0;
        s32 v5c = 0;
        s32 v58 = 0;
        s32 v54 = 0;
        s32 v50 = 0;
        s32 v4c = 0;
        s32 v48 = 0;
        s32 v44 = 0;
        s32 v40 = 0;
        s32 c1 = 1;
        s32 c4 = 4;
        s32 c6 = 6;
        s32 v24 = 0x1c0;
        s32 c3 = 3;
        s32 v3c = 0;
        s32 v2c = 0;
        s32 v28 = 0;

        do {
            if (mWalkerDelay[walker] > 0) {
                continue;
            }

            {
                int ent = (int)sceneBytes + walker * 8;
                s32 progress500;
                s32 *walkerY = &((dScMgAmida_c *)ent)->mWalkerCell[0].y;
                s32 y = *walkerY;
                s32 goalY = mLineEndY;

                if (y > goalY) {
                    u8 *done = (u8 *)&((dScMgAmida_c *)(sceneBytes + walker))->mWalkerDone[0];
                    if (*done != 1) {
                        s32 reachedGoal = vC;
                        s32 *walkerX;
                        s32 x;

                        if (IsVariant2() != 0) {
                            u32 wantedGoal = (u32)mLaneWants[walker];
                            walkerX =
                                &((dScMgAmida_c *)(sceneBytes + walker * 8))->mWalkerCell[0].x;
                            x = *walkerX;
                            switch (x) {
                            case 0x20:
                                if (mLaneResult[0] == (s32)wantedGoal) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[0] = c1;
                                }
                                break;
                            case 0x60:
                                if (mLaneResult[1] == (s32)wantedGoal) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[1] = c1;
                                }
                                break;
                            case 0xa0:
                                if (mLaneResult[2] == (s32)wantedGoal) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[2] = c1;
                                }
                                break;
                            case 0xe0:
                                if (mLaneResult[3] == (s32)wantedGoal) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[3] = c1;
                                }
                                break;
                            }
                        } else {
                            walkerX = &((dScMgAmida_c *)ent)->mWalkerCell[0].x;
                            x = *walkerX;
                            switch (x) {
                            case 0x20:
                                if (mLaneResult[0] == 1) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[0] = c1;
                                }
                                break;
                            case 0x60:
                                if (mLaneResult[1] == 1) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[1] = c1;
                                }
                                break;
                            case 0xa0:
                                if (mLaneResult[2] == 1) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[2] = c1;
                                }
                                break;
                            case 0xe0:
                                if (mLaneResult[3] == 1) {
                                    reachedGoal = c1;
                                } else {
                                    mLaneFlashFlag[3] = c1;
                                }
                                break;
                            }
                        }

                        if (reachedGoal == 1) {
                            if (IsVariant2() != 0) {
                                if (mRoundFailed == 0) {
                                    func_02012718(v10, *walkerX << 0xc);
                                    u32 wantedGoal = (u32)mLaneWants[walker];
                                    switch (wantedGoal) {
                                    default:
                                        break;
                                    case 0:
                                        func_02012718(v14, *walkerX << 0xc);
                                        break;
                                    case 1:
                                        func_02012718(v18, *walkerX << 0xc);
                                        break;
                                    case 2:
                                        func_02012718(v1c, *walkerX << 0xc);
                                        break;
                                    case 3:
                                        func_02012718(v20, *walkerX << 0xc);
                                        break;
                                    }
                                } else {
                                    func_02012718(0x1c3, *walkerX << 0xc);
                                }
                                *done = c1;
                                mWalkersDone += 1;
                                if (mWalkersDone < mWalkerCount) {
                                    continue;
                                }
                                if (mRoundFailed == 0) {
                                    mRoundCount += 1;
                                    mScore += 1;
                                    if (mScore > 0x270f) {
                                        mScore = 0x270f;
                                    }
                                    mRoundTimer = 0x78;
                                    return;
                                }
                                mRoundTimer = 0x1e;
                            } else {
                                s32 *walkerX =
                                    &((dScMgAmida_c *)(sceneBytes + walker * 8))->mWalkerCell[0].x;
                                s32 speedMode;
                                func_02012718(v10, *walkerX << 0xc);
                                func_02012718(0x1c1, *walkerX << 0xc);
                                *done = c1;
                                mWalkersDone += 1;
                                if (mWalkersDone < mWalkerCount) {
                                    continue;
                                }
                                mRoundCount += 1;
                                mScore += 1;
                                if (mScore > 0x270f) {
                                    mScore = 0x270f;
                                }
                                speedMode =
                                    *(s32 *)((char *)sAmidaPatternSpeedUp + mPatternIndex * 0x1c);
                                switch (speedMode) {
                                case 0:
                                    break;
                                case 1:
                                    if ((mRoundCount % 2) == 0) {
                                        mScrollSpeed += 5;
                                    }
                                    break;
                                case 2:
                                    mScrollSpeed += 5;
                                    break;
                                }
                                if (mScrollSpeed > 0x64) {
                                    mScrollSpeed = 0x64;
                                }
                                if (mRoundCount >= 5) {
                                    mResultWaitTimer = 0x3c;
                                    mState = 2;
                                    mFinished = 1;
                                    return;
                                }
                                mState = 0;
                            }
                            return;
                        }

                        if (IsVariant2() != 0) {
                            func_02012dbc(5);
                            if (mRoundFailed == 0) {
                                func_02012718(0x1c2, mWalkerCell[walker].x << 0xc);
                            } else {
                                func_02012718(0x1c3, mWalkerCell[walker].x << 0xc);
                            }
                            *done = 1;
                            mRoundFailed = 1;
                            mWalkersDone += 1;
                            if (mWalkersDone >= mWalkerCount) {
                                mRoundTimer = 0x3c;
                            }
                            return;
                        }
                        func_02012718(0xe1, mWalkerCell[walker].x << 0xc);
                        mResultWaitTimer = 0x3c;
                        mState = 2;
                        mRoundFailed = 1;
                        return;
                    }
                    continue;
                }

                if (y < -0xc0 || y >= 0x98) {
                    *walkerY += 1;
                } else {
                    u8 *walkerMark = (u8 *)&((dScMgAmida_c *)(sceneBytes + walker))->mWalkerMark[0];
                    if ((u32)*walkerMark <= 1) {
                        *walkerY = y + 1;
                        if (StepWalker(walker, c3) == 0) {
                            StepWalker(walker, c4);
                        }
                    } else {
                        progress500 = ((y + 0xd4) * 0x1f4) / (goalY + 0xd4);
                        s32 *walkerDir =
                            &((dScMgAmida_c *)(sceneBytes + walker * 4))->mWalkerDir[0];
                        u32 direction = *walkerDir;
                        switch (direction) {
                        default:
                            break;
                        case 0:
                            if (mWalkerCell[walker].x == 0x21 || mWalkerCell[walker].x == 0x61 ||
                                mWalkerCell[walker].x == 0xa1 || mWalkerCell[walker].x == 0xe1) {
                                {
                                    s32 *walkerX = &((dScMgAmida_c *)(sceneBytes + walker * 8))
                                                        ->mWalkerCell[0]
                                                        .x;
                                    *walkerX -= 1;
                                    *walkerDir = c6;
                                    *walkerMark = c1;
                                    func_020126ac(v24, c6, v28, progress500,
                                                  func_020126e8(*walkerX << 0xc));
                                }
                            } else if (StepWalker(walker, v2c) == 0 &&
                                       StepWalker(walker, c1) == 0 && StepWalker(walker, c3) == 0 &&
                                       StepWalker(walker, v30) == 0) {
                                if (StepWalker(walker, v34) == 0) {
                                    *walkerDir = v38;
                                }
                            }
                            break;
                        case 1:
                            if (StepWalker(walker, c1) == 0 && StepWalker(walker, v3c) == 0 &&
                                StepWalker(walker, v34) == 0 && StepWalker(walker, c3) == 0) {
                                if (StepWalker(walker, c4) == 0) {
                                    *walkerDir = c6;
                                }
                            }
                            break;
                        case 2:
                            if (mWalkerCell[walker].x == 0x1f || mWalkerCell[walker].x == 0x5f ||
                                mWalkerCell[walker].x == 0x9f || mWalkerCell[walker].x == 0xdf) {
                                {
                                    s32 *walkerX = &((dScMgAmida_c *)(sceneBytes + walker * 8))
                                                        ->mWalkerCell[0]
                                                        .x;
                                    *walkerX += 1;
                                    *walkerDir = c6;
                                    *walkerMark = c1;
                                    func_020126ac(v24, c6, v40, progress500,
                                                  func_020126e8(*walkerX << 0xc));
                                }
                            } else if (StepWalker(walker, v34) == 0 &&
                                       StepWalker(walker, c1) == 0 && StepWalker(walker, c4) == 0 &&
                                       StepWalker(walker, v44) == 0) {
                                if (StepWalker(walker, v38) == 0) {
                                    *walkerDir = v30;
                                }
                            }
                            break;
                        case 3:
                            if (mWalkerCell[walker].x == 0x21 || mWalkerCell[walker].x == 0x61 ||
                                mWalkerCell[walker].x == 0xa1 || mWalkerCell[walker].x == 0xe1) {
                                {
                                    s32 *walkerX = &((dScMgAmida_c *)(sceneBytes + walker * 8))
                                                        ->mWalkerCell[0]
                                                        .x;
                                    *walkerX -= 1;
                                    *walkerDir = c6;
                                    *walkerMark = c1;
                                    func_020126ac(v24, c6, v48, progress500,
                                                  func_020126e8(*walkerX << 0xc));
                                }
                            } else if (StepWalker(walker, c3) == 0 &&
                                       StepWalker(walker, v4c) == 0 &&
                                       StepWalker(walker, v30) == 0 &&
                                       StepWalker(walker, c1) == 0) {
                                if (StepWalker(walker, c6) == 0) {
                                    *walkerDir = c4;
                                }
                            }
                            break;
                        case 4:
                            if (mWalkerCell[walker].x == 0x1f || mWalkerCell[walker].x == 0x5f ||
                                mWalkerCell[walker].x == 0x9f || mWalkerCell[walker].x == 0xdf) {
                                {
                                    s32 *walkerX = &((dScMgAmida_c *)(sceneBytes + walker * 8))
                                                        ->mWalkerCell[0]
                                                        .x;
                                    *walkerX += 1;
                                    *walkerDir = c6;
                                    *walkerMark = c1;
                                    func_020126ac(v24, c6, v50, progress500,
                                                  func_020126e8(*walkerX << 0xc));
                                }
                            } else if (StepWalker(walker, c4) == 0 &&
                                       StepWalker(walker, v34) == 0 &&
                                       StepWalker(walker, v38) == 0 &&
                                       StepWalker(walker, c1) == 0) {
                                if (StepWalker(walker, c6) == 0) {
                                    *walkerDir = c3;
                                }
                            }
                            break;
                        case 5:
                            if (mWalkerCell[walker].x == 0x21 || mWalkerCell[walker].x == 0x61 ||
                                mWalkerCell[walker].x == 0xa1 || mWalkerCell[walker].x == 0xe1) {
                                {
                                    s32 *walkerX = &((dScMgAmida_c *)(sceneBytes + walker * 8))
                                                        ->mWalkerCell[0]
                                                        .x;
                                    *walkerX -= 1;
                                    *walkerDir = c6;
                                    *walkerMark = c1;
                                    func_020126ac(v24, c6, v54, progress500,
                                                  func_020126e8(*walkerX << 0xc));
                                }
                            } else if (StepWalker(walker, v30) == 0 &&
                                       StepWalker(walker, c6) == 0 && StepWalker(walker, c3) == 0 &&
                                       StepWalker(walker, v58) == 0) {
                                if (StepWalker(walker, v38) == 0) {
                                    *walkerDir = v34;
                                }
                            }
                            break;
                        case 6:
                            if (StepWalker(walker, c6) == 0 && StepWalker(walker, v30) == 0 &&
                                StepWalker(walker, v38) == 0 && StepWalker(walker, c3) == 0) {
                                if (StepWalker(walker, c4) == 0) {
                                    *walkerDir = c1;
                                }
                            }
                            break;
                        case 7:
                            if (mWalkerCell[walker].x == 0x1f || mWalkerCell[walker].x == 0x5f ||
                                mWalkerCell[walker].x == 0x9f || mWalkerCell[walker].x == 0xdf) {
                                {
                                    s32 *walkerX = &((dScMgAmida_c *)(sceneBytes + walker * 8))
                                                        ->mWalkerCell[0]
                                                        .x;
                                    *walkerX += 1;
                                    *walkerDir = c6;
                                    *walkerMark = c1;
                                    func_020126ac(v24, c6, v5c, progress500,
                                                  func_020126e8(*walkerX << 0xc));
                                }
                            } else if (StepWalker(walker, v38) == 0 &&
                                       StepWalker(walker, c6) == 0 && StepWalker(walker, c4) == 0 &&
                                       StepWalker(walker, v30) == 0 &&
                                       StepWalker(walker, v34) == 0) {
                                *walkerDir = v60;
                            }
                            break;
                        }
                    }
                }
            }
        } while (++walker < mWalkerCount);
    }
}
#pragma pop

// @symbol Amida_RestoreBgChars
extern "C" void Amida_RestoreBgChars(void *) {
    char *mainChars = (char *)func_02054efc();
    MultiCopyHalf((char *)func_02054efc() + 0xc000, mainChars, 0x6000);
    char *subChars = (char *)G2S::GetBG0CharPtr();
    MultiCopyHalf((char *)G2S::GetBG0CharPtr() + 0x6000, subChars, 0x6000);
}

// @symbol Amida_SaveBgChars
/* The scene pointer its caller passes is never read. */
extern "C" void Amida_SaveBgChars(void *) {
    char *mainChars = (char *)func_02054efc() + 0xc000;
    MultiCopyHalf((char *)func_02054efc(), mainChars, 0x6000);
    char *subChars = (char *)G2S::GetBG0CharPtr() + 0x6000;
    MultiCopyHalf(G2S::GetBG0CharPtr(), subChars, 0x6000);
}

// @symbol _ZN12dScMgAmida_c11InitWalkersEv
void dScMgAmida_c::InitWalkers() {
    int i;
    int j;
    int k;
    int pick;
    int tmp;
    u32 rand;
    int again;
    u8 seenNow[4];
    u8 seenBefore[4];

    this->mHurry = 0;
    if (this->IsVariant2() != 0) {
        again = 1;
        for (i = 0; i < 4; i++) {
            this->mLanePerm[i] = i;
        }
        do {
            for (k = 0; k < 4; k++) {
                rand = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                pick = ((rand & 0x7fff) * 4) >> 15;
                tmp = this->mLanePerm[k];
                this->mLanePerm[k] = this->mLanePerm[pick];
                this->mLanePerm[pick] = tmp;
            }
            if (this->mRoundCount == 0) {
                again = 0;
                switch (this->mPatternIndex) {
                case 0:
                    this->mLaneWants[0] = 0;
                    this->mLaneWants[1] = 0;
                    this->mLaneWants[2] = 3;
                    this->mLaneWants[3] = 3;
                    break;
                case 1:
                case 3:
                case 5:
                    this->mLaneWants[0] = 0;
                    this->mLaneWants[1] = 0;
                    this->mLaneWants[2] = 1;
                    this->mLaneWants[3] = 3;
                    break;
                default:
                    for (i = 0; i < 4; i++) {
                        this->mLaneWants[i] = i;
                    }
                    break;
                }
                for (j = 0; j < 4; j++) {
                    sAmidaLaneTargets[this->mLanePerm[j]] = this->mLaneWants[j];
                }
            } else {
                for (i = 0; i < 4; i++) {
                    sAmidaLaneSaved[i] = sAmidaLaneTargets[i];
                }
                for (j = 0; j < 4; j++) {
                    sAmidaLaneTargets[this->mLanePerm[j]] = this->mLaneWants[j];
                }
                for (j = 0; j < 4; j++) {
                    if (sAmidaLaneSaved[j] != sAmidaLaneTargets[j]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    } else if (this->mWalkerCount == 1) {
        if (this->mRoundCount == 0) {
            pick = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4) >> 15;
            tmp = this->mLanePerm[0];
            this->mLanePerm[0] = this->mLanePerm[pick];
            this->mLanePerm[pick] = tmp;
        } else {
            pick = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            tmp = this->mLanePerm[0];
            this->mLanePerm[0] = this->mLanePerm[pick];
            this->mLanePerm[pick] = tmp;
        }
    } else {
        again = 1;
        for (int n = 0; n < 4; n++) {
            seenBefore[n] = 0;
        }
        for (j = 0; j < this->mWalkerCount; j++) {
            seenBefore[this->mLanePerm[j]] = 1;
        }
        do {
            for (int n = 0; n < 4; n++) {
                seenNow[n] = 0;
            }
            for (i = 0; i < 4; i++) {
                this->mLanePerm[i] = i;
            }
            for (k = 0; k < 4; k++) {
                rand = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                pick = ((rand & 0x7fff) * 4) >> 15;
                tmp = this->mLanePerm[k];
                this->mLanePerm[k] = this->mLanePerm[pick];
                this->mLanePerm[pick] = tmp;
            }
            for (j = 0; j < this->mWalkerCount; j++) {
                seenNow[this->mLanePerm[j]] = 1;
            }
            if (this->mRoundCount == 0) {
                again = 0;
            } else {
                for (int n = 0; n < 4; n++) {
                    if (seenNow[n] != seenBefore[n]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    }

    for (i = 0; i < 4; i++) {
        if (this->IsVariant2() != 0) {
            this->mWalkerDelay[i] = 0;
        } else {
            this->mWalkerDelay[i] =
                i * ((AmidaPattern *)sAmidaPatterns)[this->mPatternIndex].walkerStagger * 0x3c;
        }
        this->mWalkerCell[i].x = (this->mLanePerm[i] << 6) + 0x20;
        if (this->IsVariant2() != 0) {
            this->mWalkerCell[i].y = -0xcc;
        } else {
            this->mWalkerCell[i].y = -0xd4;
        }
        this->mWalkerDir[i] = -1;
        this->mWalkerMark[i] = 0;
        this->mWalkerDone[i] = 0;
    }
    this->mWalkersDone = 0;
    this->mRoundTimer = 0;
}

// @symbol _ZN12dScMgAmida_c10SetupRoundEv
#pragma push
#pragma opt_strength_reduction off
void dScMgAmida_c::SetupRound() {
    char *raw = (char *)this;
    int level;
    int shuffle;
    AmidaRungPosition rungs[11];
    int skip[11];

    mFinished = 0;
    mRoundFailed = 0;

    if (mScore > 0x270f) {
        mScore = 0x270f;
    }
    shuffle = 0;

    level = mLevel;
    if (IsVariant2() != 0) {
        if ((u32)mLevel >= 7) {
            level = 7;
            shuffle = 1;
        }
    } else if (level >= 0xa) {
        shuffle = 1;
        level = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15) + 4;
        if (level == mPatternIndex) {
            level = 9;
        }
    }
    mPatternIndex = level;

    if (IsVariant2() != 0) {
        mWalkerCount = 4;
    } else {
        mWalkerCount = ((AmidaPattern *)sAmidaPatterns)[mPatternIndex].walkerCount;
    }

    mScrollSpeed = ((*(int *)(sAmidaPatternSpeeds + mPatternIndex * 0x1c) - 1) * 5) + 0xb;

    {
        u32 stage = mLevel;
        if (stage >= 0x13) {
            mScrollSpeed = (int)((stage / 10 - 1) * 5) + mScrollSpeed;
            if (mScrollSpeed > 0x64) {
                mScrollSpeed = 0x64;
            }
        }
    }

    {
        char *d;
        volatile u16 v1;
        volatile u16 v2;
        d = (char *)func_02054efc();
        v1 = 0;
        MultiStore16(v1, d, 0x6000);
        d = (char *)G2S::GetBG0CharPtr();
        v2 = 0;
        MultiStore16(v2, d, 0x6000);
    }

    mLineStart.x = -1;
    mLineStart.y = -1;
    mLineEnd.x = -1;
    mLineEnd.y = -1;
    mPen.x = -1;
    mPen.y = -1;
    mPenStart.x = -1;
    mPenStart.y = -1;

    {
        int z = 0;
        mLineStartSet = z;
        mLineEndSet = z;
        mProbeMode = z;
        mProbeHit = z;
        mPenTapped = z;
        mPenSoundHandle = z;
        do {
            mLanePerm[z] = z;
            z += 1;
        } while (z < 4);
    }

    mRoundCount = 0;
    InitWalkers();
    ShuffleGoals(*(int *)(sAmidaPatternGoalShuffle + mPatternIndex * 0x1c));
    Mg_SetTopScreenYOffset(0x20);

    mSetupInk = 1;
    mLineCount = 1;
    /* Both buffers are cleared through a raw read of the pointer: indexing
       mStrokeGrid and mInkGrid as members changes the code. */
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(mStrokeGrid + off + j) = 0;
                *(mInkGrid + off + j) = 0;
            }
        }
    }

    if (IsVariant2() != 0) {
        Mg_DrawLine(0x20, -0xb4, 0x20, mLineEndY, mLineColor, 1);
        Mg_DrawLine(0x60, -0xb4, 0x60, mLineEndY, mLineColor, 1);
        Mg_DrawLine(0xa0, -0xb4, 0xa0, mLineEndY, mLineColor, 1);
        Mg_DrawLine(0xe0, -0xb4, 0xe0, mLineEndY, mLineColor, 1);
    } else {
        Mg_DrawLine(0x20, -0xd4, 0x20, mLineEndY, mLineColor, 1);
        Mg_DrawLine(0x60, -0xd4, 0x60, mLineEndY, mLineColor, 1);
        Mg_DrawLine(0xa0, -0xd4, 0xa0, mLineEndY, mLineColor, 1);
        Mg_DrawLine(0xe0, -0xd4, 0xe0, mLineEndY, mLineColor, 1);
    }

    mLineCount++;
    {
        typedef struct {
            AmidaRungPosition e[11];
        } Blk88;
        *(Blk88 *)rungs = *(Blk88 *)sAmidaInitialRungPositions;
    }

    {
        int i;
        for (i = 0; i < 0xb; i++) {
            skip[i] = 0;
        }
    }

    if (shuffle == 1) {
        if (IsVariant2() != 0) {
            int msk = 0x7fff;
            int cnt = 3;
            do {
                int idx = ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & msk) * 0xb) >> 15;
                if (skip[idx] == 0) {
                    cnt--;
                    skip[idx] = 1;
                }
            } while (cnt > 0);
        } else {
            int sel = *(int *)(sAmidaPatternRungSkip + mPatternIndex * 0x1c);
            switch (sel) {
            case 0:
                break;
            case 1:
                skip[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15] =
                    1;
                break;
            case 2: {
                int cnt =
                    (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) +
                    1;
                int j = 0;
                if (cnt > 0) {
                    int *seedp2 = &data_0209d4b8;
                    do {
                        skip[((u32)(((u32)RandomIntInternal(seedp2) >> 16) & 0x7fff) * 0xb) >> 15] =
                            1;
                        j++;
                    } while (j < cnt);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    {
        int sel2;
        if (IsVariant2() != 0) {
            switch (mPatternIndex) {
            case 0:
            case 1:
            case 2:
                sel2 = 0;
                break;
            case 3:
            case 4:
                sel2 = 1;
                break;
            default:
                sel2 = 2;
                break;
            }
        } else {
            sel2 = *(int *)(sAmidaPatternRungSkip + mPatternIndex * 0x1c);
        }

        switch (sel2) {
        case 0:
            break;
        case 1: {
            int i;
            for (i = 0; i < 5; i++) {
                if (skip[i] == 0) {
                    int x = rungs[i].x;
                    int y = rungs[i].y - 0x20;
                    Mg_DrawLine(x, y, x + 0x40, y, mLineColor, 1);
                }
            }
            break;
        }
        case 2:
        case 3: {
            int i;
            for (i = 0; i < 0xb; i++) {
                if (skip[i] == 0) {
                    int x = rungs[i].x;
                    int y = rungs[i].y - 0x20;
                    Mg_DrawLine(x, y, x + 0x40, y, mLineColor, 1);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    mLineCount++;
    if (IsVariant2() == 0) {
        int v = *(int *)(sAmidaPatternExtraRungs + mPatternIndex * 0x1c);
        if (v != 0 && v == 1) {
            Mg_DrawLine(0x60, 0x2d, 0xa0, 0x2d, mLineColor, 1);
            Mg_DrawLine(0x20, 0x5a, 0x60, 0x5a, mLineColor, 1);
            Mg_DrawLine(0xa0, 0x5a, 0xe0, 0x5a, mLineColor, 1);
        }
    }

    mSetupInk = 0;
    mLineCount++;
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(mStrokeGrid + off + j) = 0;
            }
        }
    }

    Amida_SaveBgChars(this);
    /* pad_5370: this store is the only access in the class, so there is no
       evidence for a name yet. */
    *(s32 *)(raw + 0x5370) = 1;
    *(volatile u16 *)0x04000050 = 0;
    G2x::SetBlendAlpha((volatile unsigned short *)0x04001050, 4, 8, 6, 0x10);
    Mg_ShowBanner(0xd, 0x80, 0x60, 1, -1, 0xd);

    mStartBannerTimer = 0x3c;
    mState = 1;
    {
        int i = 0;
        for (; i < 4; i++) {
            mLaneFlashTimer[i] = 0;
            mLaneFlashFrame[i] = 0;
            mLaneFlashFlag[i] = 0;
            mLaneAnimTimer[i] = 0;
            mLaneAnimFrame[i] = 0;
        }
        mBgScrollPhase = 0;
        mEndDelayTimer = 0;
        mEdgeBoostCount = 0;
        mEdgeBoost = 0;
        mHurry = 0;
        mPenLocked = 0;
        {
            int z = 0;
            int xw[2];
            xw[0] = 0x20;
            xw[1] = 0;
            for (; z < 4; z++) {
                mLanePos[z].x = xw[0] << 12;
                mLanePos[z].y = 0xb0000;
                mLaneVel[z].x = xw[1];
                mLaneVel[z].y = xw[1];
                xw[0] += 0x40;
            }
            mFinaleTimer = xw[1];
            {
                int v = 0;
                for (; v < 0x80; v++) {
                    PIECE_AT(raw)->active = 0;
                    PIECE_AT(raw)->timer = 0;
                    raw += 0x18;
                }
            }
        }
    }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c11RenderBoardEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgAmida_c::RenderBoard() {
    void *scene = (void *)this;
    typedef struct {
        int a[13];
    } T13;
    unsigned char *raw = (unsigned char *)scene;
    int noPalette = -1;
    int i;
    int y;

    i = 0;
    y = 0x20;
    for (; i < 4; y += 0x40, i++) {
        int *lane = (int *)(raw + i * 4);
        if (mLaneResult[i] != 0) {
            int *animTimer = &((dScMgAmida_c *)lane)->mLaneAnimTimer[0];
            (*animTimer)++;
            if (*animTimer >= 6) {
                int *animFrame = &((dScMgAmida_c *)lane)->mLaneAnimFrame[0];
                *animTimer = 0;
                (*animFrame)++;
                if (*animFrame >= 14)
                    *animFrame = 0;
            }
            Mg_RenderSpriteBoth(sAmidaLaneAnimSprites[mLaneAnimFrame[i]], mLanePos[i].x >> 12,
                                (mLanePos[i].y >> 12) - 4, noPalette, 0);
        } else {
            int *timer;
            int *frame;
            int frames[13];
            if (mLaneFlashFlag[i] == 0) {
                timer = &((dScMgAmida_c *)lane)->mLaneFlashTimer[0];
                frame = &((dScMgAmida_c *)lane)->mLaneFlashFrame[0];
                (*timer)++;
                if (*frame >= 12) {
                    if (*timer >= 6) {
                        *timer = 0;
                        *frame = 0;
                    }
                } else if (*timer >= 4) {
                    *timer = 0;
                    (*frame)++;
                }
            } else {
                timer = &((dScMgAmida_c *)lane)->mLaneFlashTimer[0];
                frame = &((dScMgAmida_c *)lane)->mLaneFlashFrame[0];
                (*timer)++;
                if (*frame >= 12) {
                    if (*timer >= 5) {
                        *timer = 0;
                        *frame = 0;
                    }
                } else if (*timer >= 2) {
                    *timer = 0;
                    (*frame)++;
                }
            }
            *(T13 *)frames = *(T13 *)sAmidaBoardAnimationFrames;
            Mg_RenderSpriteBoth(sAmidaBoardAnimSprites[frames[*frame]], y, 0xb8, noPalette, 0);
        }
    }
    {
        int j;
        for (j = 0; j < mWalkerCount; j++) {
            Mg_RenderSpriteBoth(data_ov006_0213a32c, mWalkerCell[j].x, mWalkerCell[j].y, -1, 0);
        }
    }
    Mg_RenderSpriteBoth(sAmidaSideSprite, 8, 0x60, -1, 0);
    Mg_RenderSpriteBoth(sAmidaSideSprite, 0xf8, 0x60, -1, 0);
}
#pragma pop

// @symbol _ZN12dScMgAmida_c14RenderBoardAltEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgAmida_c::RenderBoardAlt() {
    int noPalette = -1;
    int i;
    int j;
    int x;
    if (mHurry == 0) {
        Mg_RenderSpriteBoth((void *)data_ov006_0213a338[0], 0x80, 0xb0, noPalette, 0);
    }
    for (i = 0; i < 4; i++) {
        Mg_RenderSpriteBoth((void *)sAmidaLaneWantSprites[mLaneWants[i]], mWalkerCell[i].x,
                            mWalkerCell[i].y, noPalette, 0);
    }
    j = 0;
    x = 0x20;
    for (; j < 4; x += 0x40, j++) {
        Mg_RenderSpriteBoth((void *)sAmidaLaneResultSprites[mLaneResult[j]], x, 0x78, noPalette, 0);
    }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c6RenderEv
/* These two pragmas are load-bearing: without them Render compiles 0xc
 * bytes larger than the ROM (0x2ac vs 0x2a0) and rombuild drops to 102/106. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
s32 dScMgAmida_c::Render() {
    char *raw = (char *)this;

    mBgScrollPhase += 0xc0;
    {
        u16 idx = mBgScrollPhase;
        int sine = data_02082214[(idx >> 4) << 1];
        int off = (sine + (int)((unsigned)(sine >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, 0);
    }
    Mg_DrawScoreTally(raw, 0xe0, 0x14, 1);

    if (mState == 3 && mEndDelayTimer == 0) {
        data_0209d45c &= ~1;
        data_0209d454 &= ~1;
        if (IsVariant2() == 0 && mFinished == 1) {
            int i;
            for (i = 0; i < 4; i++) {
                if (mLaneResult[i] != 0) {
                    int *counterA = &((dScMgAmida_c *)(raw + i * 4))->mLaneAnimTimer[0];
                    (*counterA)++;
                    AmidaRenderPeriods periods = sAmidaRenderPeriods;
                    if (*counterA >= periods.values[i]) {
                        int *counterB;
                        *counterA = 0;
                        counterB = &((dScMgAmida_c *)(raw + i * 4))->mLaneAnimFrame[0];
                        (*counterB)++;
                        if (*counterB >= 0xe)
                            *counterB = 0;
                    }
                    Mg_RenderSpriteBoth(sAmidaLaneAnimSprites[mLaneAnimFrame[i]],
                                        mLanePos[i].x >> 12, (mLanePos[i].y >> 12) - 4, -1, 0);
                }
            }

            int piece = 0;
            int zero = 0;
            int noPalette = -1;
            for (; piece < 0x80; piece++, raw += 0x18) {
                if (PIECE_AT(raw)->active != 0) {
                    int age = PIECE_AT(raw)->timer;
                    int x = PIECE_AT(raw)->posX;
                    int idx = age / 4;
                    int y = PIECE_AT(raw)->posY;
                    Mg_RenderSpriteBoth(sAmidaWalkerSprites[idx], x >> 12, y >> 12, noPalette,
                                        zero);
                }
            }
        }
        return 1;
    } else {
        data_0209d45c |= 1;
        data_0209d454 |= 1;
        if (IsVariant2() != 0) {
            RenderBoardAlt();
        } else {
            RenderBoard();
        }
        return 1;
    }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c8BehaviorEv
#pragma push
#pragma opt_strength_reduction off
s32 dScMgAmida_c::Behavior() {
    char *raw = (char *)this;
    int piece;
    char *piecePtr;
    int *vel;
    int speed;
    int steps;
    int k;
    int accum;
    int touch, j4, touched;
    int lane;
    int j;
    int slot;
    char *scan;
    int randA, randB;
    u32 bv, av;
    s16 cA;
    int idxA;
    int cosB, sinA;

    switch (mState) {
    case 0:
        if (mFinished == 1) {
            SetupRound();
        } else if (mRoundFailed == 1) {
            SetupRound();
        } else {
            InitWalkers();
        }
        mState = 1;
        // fall through
    case 1:
        CheckEdgeBoost();
        CheckHurryButton();
        HandlePen();
        if (IsVariant2() != 0) {
            if (mHurry == 1) {
                mScrollAccum += mRoundCount * 5 + 0x20;
            }
        } else if (mEdgeBoost == 1) {
            mScrollAccum += 0x64;
        } else {
            mScrollAccum += mScrollSpeed;
        }
        accum = mScrollAccum;
        mScrollAccum &= 0xf;
        steps = accum / 16;
        if (IsVariant2() == 0 || mHurry != 0) {
            if (mRoundTimer > 0) {
                mRoundTimer -= 1;
                if (mRoundTimer == 0) {
                    if (mRoundFailed == 1) {
                        mResultWaitTimer = 0x3c;
                        mState = 2;
                    } else if (mRoundCount < 5) {
                        mState = 0;
                    } else {
                        mResultWaitTimer = 0x3c;
                        mState = 2;
                        mFinished = 1;
                    }
                }
            }
            k = 0;
            if (steps > 0) {
                do {
                    if (mState != 1)
                        break;
                    StepWalkers();
                    k++;
                } while (k < steps);
            }
        }
        if (mStartBannerTimer > 0) {
            mStartBannerTimer -= 1;
            if (mStartBannerTimer <= 0) {
                FreeGfxSlotsById(0xd);
                if (mPromptBlinkCount == 0) {
                    mPromptEnabled = 1;
                    mPromptBlinkCount = 1;
                    mPromptBlinkTimer = 0;
                }
            }
        }
        Mg_SubmitScore(mScore);
        mHudScore = mScore;
        break;
    case 2:
        mEdgeBoost = 0;
        if (IsVariant2() != 0) {
            if (mResultWaitTimer > 0)
                mResultWaitTimer -= 1;
        } else {
            if (mResultWaitTimer > 0)
                mResultWaitTimer -= 1;
        }
        if (mResultWaitTimer != 0)
            break;
        mPromptEnabled = 0;
        if (mFinished == 1) {
            mState = 3;
            Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
        } else {
            mState = 3;
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            mEndDelayTimer = 0xb4;
            mFinaleTimer = 0;
        }
        break;
    case 3:
        if (mEndDelayTimer > 0)
            mEndDelayTimer -= 1;
        touched = 0;
        touch = gActivePlayerSlot;
        j4 = touch * 4;
        if (gTouchHeld[touch * 4] != 0) {
            if (gTouchEdge[j4] != 0)
                touched = 1;
        }
        if (touched != 0)
            mEndDelayTimer = 0;
        if (IsVariant2() != 0)
            break;
        if (mFinished != 1)
            break;
        mFinaleTimer += 1;
        lane = 0;
        do {
            mLaneVel[lane].y -= 0x100;
            mLanePos[lane].x += mLaneVel[lane].x;
            mLanePos[lane].y += mLaneVel[lane].y;
            lane++;
        } while (lane < 4);

        piecePtr = raw;
        vel = &mPieces[0].velX;
        for (piece = 0; piece < 0x80; piece++) {
            if (PIECE_AT(piecePtr)->active != 0) {
                IAP(piecePtr, PIECE_OFF(posX)) += IP(piecePtr, PIECE_OFF(velX));
                IAP(piecePtr, PIECE_OFF(posY)) += IP(piecePtr, PIECE_OFF(velY));
                speed = Vec2_Len(vel) * 7 / 8;
                if (func_0203d434(vel) != 0)
                    func_0203d630(vel, speed);
                IAP(piecePtr, PIECE_OFF(timer)) = IAP(piecePtr, PIECE_OFF(timer)) + 1;
                {
                    if (PIECE_AT(piecePtr)->timer >= 0x10)
                        PIECE_AT(piecePtr)->active = 0;
                }
            }
            piecePtr += 0x18;
            vel += 6;
        }

        for (j = 0; j < 4; j++) {
            if (mLaneResult[j] != 0) {
                slot = 0;
                scan = raw;
                do {
                    if (PIECE_AT(scan)->active != 1) {
                        randA = RandomIntInternal(&data_0209d4b8);
                        randB = RandomIntInternal(&data_0209d4b8);
                        bv = ((u32)randB >> 16) & 0x7fff;
                        av = ((u32)randA >> 16) & 0x7fff;
                        cosB = data_02082214[((int)((bv << 15) >> 16) >> 4) * 2 + 1];
                        idxA = ((int)((av << 17) >> 16) >> 4) * 2;
                        cA = data_02082214[idxA + 1];
                        {
                            mPieces[slot].posX = FX_MUL(cosB, (int)((0x8000LL * cA + 0x800) >> 12));
                            sinA = data_02082214[idxA];
                            mPieces[slot].posY =
                                FX_MUL(cosB, (int)((0x8000LL * sinA + 0x800) >> 12));
                            mPieces[slot].posX += mLanePos[j].x;
                            mPieces[slot].posY += mLanePos[j].y;
                            mPieces[slot].velX =
                                FX_MUL(cosB, (int)((0x1000LL * (int)(cA) + 0x800) >> 12));
                            mPieces[slot].velY =
                                FX_MUL(cosB, (int)((0x1000LL * sinA + 0x800) >> 12));
                        }
                        mPieces[slot].velY -= 0x400;
                        mPieces[slot].active = 1;
                        mPieces[slot].timer = 0;
                        break;
                    }
                    slot++;
                    scan += 0x18;
                } while (slot < 0x80);
            }
        }
        break;
    }
    return 1;
}
#pragma pop

// @symbol _ZN12dScMgAmida_c10StartRoundEi
/* Slot 18: counts a win (arg 0) or resets the score, then sets up the next board. */
void dScMgAmida_c::StartRound(int arg) {
    if (arg == 0) {
        if (mLevel >= 0x7cf)
            goto final;
        mLevel = mLevel + 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        mScore = 0;
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
        Mg_SubmitScore(mScore);
        mHudScore = mScore;
    }
final:
    SetupRound();
}

// @symbol _ZN12dScMgAmida_c13InitResourcesEv
s32 dScMgAmida_c::InitResources() {
    volatile u16 zeroMain;
    volatile u16 zeroSub;
    void *file;

    mStrokeGrid = (u8 *)Memory::Allocate(0x15800);
    mInkGrid = (u8 *)Memory::Allocate(0x15800);

    if (IsVariant2() != 0) {
        mLineEndY = 0x78;
        mLineColor = 2;
    } else {
        mLineEndY = 0x98;
        mLineColor = 2;
    }

    data_0208ee44 = 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x3218;
    *(volatile u16 *)0x4000008 &= ~0x40;
    *(volatile s32 *)0x4000010 = 0;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;

    file = func_02054efc();
    zeroMain = 0;
    MultiStore16((u16)zeroMain, file, 0x6000);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x2214;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile s32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;

    file = (char *)G2S::GetBG0CharPtr();
    zeroSub = 0;
    MultiStore16((u16)zeroSub, file, 0x6000);

    Mg_InitBrush((char *)this, 1, 0, 2);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x140c;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile s32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    if (IsVariant2() != 0) {
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA_FACE_UP_NCG_BIN, (void *)func_02054d88());
        file = (void *)LoadFile(ASSET_D_2D_MG_BG_AMIDA_FACE_UP_NCL_BIN);
        CP15::FlushAndInvalidateDataCache((u32)file, 0x1a0);
        GX::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA_FACE_UP_NSC_BIN, G2::GetBG3ScrPtr());
    } else {
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA1_NCG_BIN, (void *)func_02054d88());
        file = (void *)LoadFile(ASSET_D_2D_MG_BG_AMIDA1_NCL_BIN);
        CP15::FlushAndInvalidateDataCache((u32)file, 0x1a0);
        GX::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA1_NSC_BIN, G2::GetBG3ScrPtr());
    }

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x40c;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile s32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x60c;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile s32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    if (IsVariant2() != 0) {
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA_FACE_DOWN_NCG_BIN, G2S::GetBG3CharPtr());
        file = (void *)LoadFile(ASSET_D_2D_MG_BG_AMIDA_FACE_DOWN_NCL_BIN);
        GXS::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA_FACE_DOWN_NSC_BIN, G2S::GetBG3ScrPtr());
        data_0209d45c |= 9;
        data_0209d454 |= 9;
    } else {
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA2_NCG_BIN, G2S::GetBG3CharPtr());
        file = (void *)LoadFile(ASSET_D_2D_MG_BG_AMIDA2_NCL_BIN);
        GXS::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA2_2_NSC_BIN, G2S::GetBG2ScrPtr());
        LoadCompressedFileAt(ASSET_D_2D_MG_BG_AMIDA2_1_NSC_BIN, G2S::GetBG3ScrPtr());
        *(volatile s16 *)0x4000050 = 0;
        G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 4, 8, 6, 0x10);
        data_0209d45c |= 9;
        data_0209d454 |= 0xd;
    }

    if (IsVariant2() != 0) {
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_S_NCG_BIN, (void *)0x6400000);
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_S_NCG_BIN, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_I_NCG_BIN, (void *)0x6400000);
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_I_NCG_BIN, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_G_NCG_BIN, (void *)0x6400000);
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_G_NCG_BIN, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_F_NCG_BIN, (void *)0x6400000);
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_F_NCG_BIN, (void *)0x6600000);
        } else {
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_NCG_BIN, (void *)0x6400000);
            LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_FACE_NCG_BIN, (void *)0x6600000);
        }
        file = (void *)LoadFile(ASSET_D_2D_MG_AMIDA_FACE_NCL_BIN);
        GX::LoadOBJPltt(file, 0, 0x100);
        GXS::LoadOBJPltt(file, 0, 0x100);
        Deallocate(file);
    } else {
        LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_NCG_BIN, (void *)0x6400000);
        LoadCompressedFileAt(ASSET_D_2D_MG_AMIDA_NCG_BIN, (void *)0x6600000);
        file = (void *)LoadFile(ASSET_D_2D_MG_AMIDA_NCL_BIN);
        GX::LoadOBJPltt(file, 0, 0x100);
        GXS::LoadOBJPltt(file, 0, 0x100);
        Deallocate(file);
    }

    mPatternIndex = 0;
    mScore = mLevel * 5;
    SetupRound();
    return 1;
}

// @symbol _ZN12dScMgAmida_c21AfterCleanupResourcesEj
/* Returns before the base call too when vfSuccess != 2, as the ROM does. */
void dScMgAmida_c::AfterCleanupResources(u32 vfSuccess) {
    if (vfSuccess != 2)
        return;
    Memory::Deallocate(mStrokeGrid);
    Memory::Deallocate(mInkGrid);
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol dScMgAmida_c_classInit
/* `new` inlines the implicit constructor: dScMgBase_c's C2, this class's
   vptr, then the four member arrays in declaration order through
   __cxa_vec_ctor -- the cells, lane positions and lane velocities with
   Vector2's structors, the pieces with dScMgAmida_c_Piece's. */
extern "C" void *dScMgAmida_c_classInit(void) {
    return new dScMgAmida_c;
}

// @symbol _ZN18dScMgAmida_c_PieceC1Ev
/* dScMgAmida_c_Piece's empty inline constructor (dScMgAmida_c.h), emitted
   here, after the factory that first uses it. */

/* Runtime asset handles in GetGameLanguage order. These are IDs in the
 * overlay 0 file table, not NitroFS file numbers. */
int sAmidaOptionNcgByLanguage[5] = {
    ASSET_D_2D_MG_OPTION_NCG_BIN,   ASSET_D_2D_MG_OPTION_S_NCG_BIN, ASSET_D_2D_MG_OPTION_F_NCG_BIN,
    ASSET_D_2D_MG_OPTION_G_NCG_BIN, ASSET_D_2D_MG_OPTION_I_NCG_BIN,
};

/* RenderBoard copies all thirteen frame selectors before drawing. */
int sAmidaBoardAnimationFrames[13] = {0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0};

/* Render copies this entire block, then indexes it by lane. That access
 * and the fourteen-word extent are preserved; the original grouping of
 * these values is not established by the observed lane accesses alone. */
AmidaRenderPeriods sAmidaRenderPeriods = {{2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1}};

/* Initial rung endpoints used by the five- and eleven-rung line loops. */
AmidaRungPosition sAmidaInitialRungPositions[11] = {
    {32, -135}, {160, -135}, {96, -90}, {32, -45},  {160, -45}, {96, -120},
    {32, -105}, {160, -105}, {32, -75}, {160, -75}, {96, -60},
};

/* Match the existing void* factory declaration; its original return type
 * remains a separate source-reconstruction question. */
extern "C" {
BaseSpawnInfo<void> g_profile_MG_AMIDA = {dScMgAmida_c_classInit, 371, 371};
}
