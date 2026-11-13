//cpp
/* ov004/dScMgBase_c -- the behavior every minigame scene shares, and the
 * sprite, OAM and graphics helpers that sit with it
 * (.text 0x020ad660..0x020b2c58, 125 functions).
 *
 * This is the whole ov004 linker unit, from dScMgBase_c::IsAltVariant at the
 * first byte of ov004's .text up to the constructor, which ends where
 * func_ov004_020b2c58 begins. It holds the eight scene overrides at
 * 0x020b04e8..0x020b0a38 plus 117 former one-function sources folded in
 * around them, among them the score-number drawer Mg_DrawNumberRotated, the
 * engine and background set-up slots 31 to 33, the destructor and the
 * constructor. Each former source keeps its own namespace block, except
 * Mg_DrawLine, now a dScMgBase_c member. Copies that repeated decl_common.h
 * or dScMgBase_c.h are gone; the local struct views and extern declarations
 * left disagree with a header or with each other, and are separate work.
 *
 * Definitions are in ROM order, lowest address first, under
 * defer_codegen off. The one exception is the constructor and destructor
 * at the end, which sit in a defer_codegen on bracket with the constructor
 * written first: see the note there.
 *
 * GraphCallback0, the key function of dScMgBase_c::graphCallback_c, is here,
 * and so is ~dScMgBase_c, the key function of dScMgBase_c itself, so this
 * file emits both classes' vtables and RTTI.
 *
 * Still unnamed: the header's unk_ fields, and the bytes it leaves as
 * padding that this file reaches by offset (+0xb0; the menu save buffers
 * go through the file-local MgMenuSaveArea); the func_ov004 and data_ov004
 * helpers are unnamed in symbols.txt.
 */

#include "dScMgBase_c.h"
#include "MgMenuRow.h"
#include "dWipe_c.h"
#include "dFdBrightness_c.h"
#include "decl_common.h"
#include "MgBanner.h"
#include "Sound.h"
#include "types.h"
#include "nitro/fx/fx_trig.h"
#include "dEnemyBase_c.h"
#include "PlayerInput.h"
#include "nitro/os/cache.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "Memory.h"
#include "math/Approach.h"
#include "nitro/hw/registers.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/mi/memory.h"
void Banner_Press(MgBanner *e);
int Banner_IsPressFinished(MgBanner *e);
extern "C" void Banner_Init(struct MgBanner *e, int c, int a1, int a2, int a3, int s1, short s2);
void Banner_Update(MgBanner *e);
void Banner_Free(MgBanner *e);
void Mg_ResetBannerLayout(void);

#pragma defer_codegen off

/* The 0x40 records at sMgFxPool are 0x20 bytes each. Mg_InitFx
   fills one: a Fix12 position, two optional vectors, a frame timer and a state.
   State 0 (UpdateDrift / DrawSparkle) adds mVecB into mVecA and
   mVecA into mPos each frame; state 1 (UpdateFlyTo /
   DrawFlyTo) interpolates mPos between the two vectors as the timer
   runs down. The class is not recovered, so the type stays a local view.
   The s32 members make the struct 4-aligned, so the pool's .bss section is
   4-aligned where the old byte-pad view left it 2-aligned. The ROM cannot
   tell the two apart: sMgFxPool sits at 0x020bebe8, right after the 0x40
   bytes of sMgSparkleSprites, and every function and the link are
   unchanged. */
struct MgFx {
    s32 mPos[2];  /* 0x00 */
    s32 mVecA[2]; /* 0x08 */
    s32 mVecB[2]; /* 0x10 */
    u8 pad_18[2];
    s16 mTimer;    /* 0x1a -- counts down to 0; 0 = free */
    s16 mDuration; /* 0x1c -- mTimer's starting value */
    s16 mState;    /* 0x1e -- index into both PMF tables */
    MgFx() {}
    ~MgFx() {}
    void DrawFlyTo();
    void DrawSparkle();
    void UpdateFlyTo();
    void UpdateDrift();
};

typedef void (MgFx::*EntPMF)();


/* Static minigame base state: the graphCallback_c registry instance, the
   MgFx record pool, and the two PMF state tables the dispatchers call
   through. Declaration order is the ROM's __sinit order. */
dScMgBase_c::graphCallback_c sMgGraphCallback;

MgFx sMgFxPool[0x40];

EntPMF sMgFxUpdateTable[2] = {&MgFx::UpdateDrift, &MgFx::UpdateFlyTo};

EntPMF sMgFxDrawTable[2] = {&MgFx::DrawSparkle, &MgFx::DrawFlyTo};

OamAttr *sMgSparkleSprites[16];



extern void *sMgWorkBuffer;

extern dGraph_c::callback_c *gGraphCallback;
namespace s20adf2c {
void Mg_UpdateFx(MgFx *c);
}

using s20adf2c::Mg_UpdateFx;

extern dFdBrightness_c *gSceneFader; /* the installed screen fader */
extern unsigned short data_020a0e5a[];

extern int data_0208ee44;
namespace s20ad90c {
void Mg_CommitRecords(void);
}

using s20ad90c::Mg_CommitRecords;
namespace s20b0aa0 {
void FreeGfxSlotsById(int arg);
}

using s20b0aa0::FreeGfxSlotsById;
void func_02012e1c(void);
namespace s20ae330 {
void Mg_InitMusic();
}

using s20ae330::Mg_InitMusic;
extern "C" {
extern void Enable3dEngines(void);
}

extern MgEntry gMgMenuRow;

extern dWipe_c data_0209f61c;

extern unsigned char data_0209d460;

extern unsigned char data_0209d458;
/* The scene's five saved records: func_02013580(table, i) reads record i of
   save table `table`, func_02013568(table, i, v) writes it, and
   Mg_GetSaveTable(scene) picks the table from byte 1 of param1. */
int func_02013580(int table, int index);
void func_02013568(int table, int index, int value);
namespace s20adc3c {
int Mg_GetSaveTable(void *scene);
}

using s20adc3c::Mg_GetSaveTable;


/* One table of OAM sprite lists per language, indexed by
   GetGameLanguage(); the helpers below hand its lists to the OAM
   wrappers. */
extern OamAttr **sMgSpriteTables[];


/* dScMgBase_c::IsAltVariant - slot 35, the LAST slot of dScMgBase_c's own
   eighteen.  Twenty bytes, and the end of the 18-35 keystone range: with this
   declared the class emits its full 36-slot vtable from source and no table in
   the minigame family is a prefix any more.

   It is a predicate on the scene's own spawn parameter: `param1 & 0xff`, the
   fBase_c word at +0x08 that every fBase_c is constructed with, tested against
   zero.  dScMgAmida_c is the only class in the family that overrides it, and
   it asks the narrower question (`== 1`); the other 31 tables carry this body.

   Thirteen call sites, all in ov006, all the same shape --
   `mov r0,<this>; ldr r1,[r0]; ldr r1,[r1,#0x8c]; blx r1` -- spread across
   FOUR leaf classes' code regions: dScMgCoin_c (2), dScMgPanel_c (4),
   dScMgSound_c (3), dScMgSnowball_c (4).  Each is a class asking the question
   of itself and branching on the answer, which is how a single minigame runs
   in two variants: two asset tables at ov006:0x02105488, a different field
   path at 0x0211b9e0, a whole block skipped at 0x02126f58.

   All thirteen follow the call with `cmp r0, #0`.  That makes this the one
   slot in the campaign whose return value is not merely permitted by the ROM
   but seen to be consumed by it.

   `IsAltVariant` is coined here (it was `Virtual8C`, after the +0x8c vtable
   offset): the low byte of param1 picks the second menu-row variant.

   The rename had to be scoped: 0x020ad660 is an overlay load base, so ov000,
   ov002, ov003, ov004 and ov007 all have unrelated symbols at that address
   (ov003's is dScTitle_c's D1).  Only ov004's is this function. */
int dScMgBase_c::IsAltVariant()
{
    return (param1 & 0xff) != 0;
}

namespace s20ad674 {

s32 GetGameLanguage(void)
{
    switch (GetOwnerLanguage())
    {
    case 5:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    default:
        return 0;
    }
}

}

namespace s20ad6f4 {

/* Returns mLastRecordRank: the index (0..4) at which Mg_RankSessionBest last
   inserted a score into the five saved records, or -1 if it placed nowhere
   (the constructor also seeds -1). */
int Mg_GetLastRecordRank(dScMgBase_c *scene, int)
{
    return scene->mLastRecordRank;
}

}

namespace s20ad6fc {
extern "C" {
extern MgEntry gMgMenuRow;
}

/* Returns the index (0..4) at which `value` would enter the scene's five
   saved records, or -1. Scoring mode 5 (gMgMenuRow.scoringMode) is
   lower-is-better and never places a value <= 0; every other mode is
   higher-is-better. The `(unsigned)` casts make the compares unsigned. */
int Mg_FindRecordRank(void *scene, int value)
{
    int i;
    if (gMgMenuRow.scoringMode == 5) {
        if (value <= 0) return -1;
        for (i = 0; i < 5; i++) {
            if (func_02013580(Mg_GetSaveTable(scene), i) > (unsigned)value) return i;
        }
    } else {
        for (i = 0; i < 5; i++) {
            if (func_02013580(Mg_GetSaveTable(scene), i) < (unsigned)value) return i;
        }
    }
    return -1;
}

}

namespace s20ad79c {
extern "C" {
extern MgEntry gMgMenuRow;
extern unsigned char gMgSaveDirty;
}


/* Saves a two-value result into saved records 0 and 1 of the current scene.
   A first value <= 0 replaces both with the menu row's record defaults 0
   and 1; a second value <= 0 alone falls back to default 1. A positive value
   is capped at 9999. Nothing is written when both already match;
   otherwise gMgSaveDirty is set to 1. */
void Mg_SaveRecordPair(int first, int second) {
    int v0 = first, v1 = second;
    if (gMgScene == 0) return;
    if (v0 <= 0) {
        v0 = gMgMenuRow.recordDefaults.v[0];
        v1 = gMgMenuRow.recordDefaults.v[1];
    } else {
        if (v0 > 9999) v0 = 9999;
    }
    if (v1 <= 0) {
        v1 = gMgMenuRow.recordDefaults.v[1];
    } else {
        if (v1 > 9999) v1 = 9999;
    }
    if (v0 == Mg_GetSavedRecord0()) {
        if (v1 == Mg_GetSavedRecord1()) return;
    }
    func_02013568(Mg_GetSaveTable(gMgScene), 0, v0);
    func_02013568(Mg_GetSaveTable(gMgScene), 1, v1);
    gMgSaveDirty = 1;
}

}

namespace s20ad878 {

/* Saved record 1 of the current scene, or 0 with no scene. */
int Mg_GetSavedRecord1(void) {
    void *scene = gMgScene;
    if (scene == 0) return 0;
    return func_02013580(Mg_GetSaveTable(scene), 1);
}

}

namespace s20ad8b8 {
extern "C" {
extern MgEntry gMgMenuRow;
}

/* Saved record 0 of the current scene; an empty record reads as the menu
   row's record default 0. 0 with no scene. */
int Mg_GetSavedRecord0(void) {
    if (gMgScene != 0) {
        int r = func_02013580(Mg_GetSaveTable(gMgScene), 0);
        if (r == 0)
            return gMgMenuRow.recordDefaults.v[0];
        return r;
    }
    return 0;
}

}

namespace s20ad940 {
void Mg_RankSessionBest(dScMgBase_c *scene);
}

namespace s20ad90c {
using s20ad940::Mg_RankSessionBest;


void Mg_CommitRecords(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0) return;
    Mg_RankSessionBest(scene);
}

}

namespace s20adafc {
int Mg_GetSessionBest(void);
}

namespace s20ada20 {
void Mg_ClearNewRecordFlag(void);
}

namespace s20ada40 {
void Mg_SetSessionBest(int score);
}

namespace s20adbc0 {
int Mg_GetSubmittedScore(void);
}

namespace s20ad940 {
using s20adafc::Mg_GetSessionBest;
using s20ad6fc::Mg_FindRecordRank;
using s20ada20::Mg_ClearNewRecordFlag;
using s20ada40::Mg_SetSessionBest;
using s20adbc0::Mg_GetSubmittedScore;
extern "C" {
extern unsigned char gMgSaveDirty;
}


/* Ranks the session best (mSessionBest, from Mg_GetSessionBest) among the
   five saved records with Mg_FindRecordRank and keeps that rank in
   mLastRecordRank (-1 when it placed nowhere). If it placed, the lower rows shift
   down and the submitted score mSubmittedScore (Mg_GetSubmittedScore) -- not the
   ranked value -- is written into that row, and gMgSaveDirty is set to 1.
   Either way the new-record flag is cleared, the session best moves to
   mRankedScore and is then reset to 0. */
void Mg_RankSessionBest(dScMgBase_c *scene)
{
    int n = Mg_FindRecordRank(scene, Mg_GetSessionBest());
    scene->mLastRecordRank = n;
    if (scene->mLastRecordRank < 0) {
        Mg_ClearNewRecordFlag();
        scene->mRankedScore = Mg_GetSessionBest();
        Mg_SetSessionBest(0);
        return;
    }
    int i;
    for (i = 4; i > scene->mLastRecordRank; i--) {
        int a = Mg_GetSaveTable(scene);
        int b = Mg_GetSaveTable(scene);
        int v = func_02013580(b, i-1);
        func_02013568(a, i, v);
    }
    int a = Mg_GetSaveTable(scene);
    int v = Mg_GetSubmittedScore();
    func_02013568(a, scene->mLastRecordRank, v);
    gMgSaveDirty = 1;
    Mg_ClearNewRecordFlag();
    scene->mRankedScore = Mg_GetSessionBest();
    Mg_SetSessionBest(0);
}

}

namespace s20ada20 {

/* Clears the new-record flag mNewRecord. */
void Mg_ClearNewRecordFlag(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        scene->mNewRecord = 0;
    }
}

}

namespace s20adc1c {
int Mg_GetTopRecord(void);
}

namespace s20ada40 {
extern "C" {
extern MgEntry gMgMenuRow;
}
using s20adc1c::Mg_GetTopRecord;


/* Stores `score` as the session best (mSessionBest). If it beats the saved
   top record (mTopRecord), it becomes the new record and mNewRecord is set.
   Beating it means, compared unsigned: in scoring mode 5, nonzero and either
   lower or no record yet (0); otherwise higher. */
void Mg_SetSessionBest(int score)
{
    dScMgBase_c *scene = gMgScene;
    if (scene == 0) return;
    scene->mSessionBest = score;
    if (gMgMenuRow.scoringMode == 5) {
        if (score == 0) return;
        if (Mg_GetTopRecord() != 0) {
            if ((unsigned)score >= (unsigned)Mg_GetTopRecord()) return;
        }
        Mg_SetTopRecord(score);
        gMgScene->mNewRecord = 1;
        return;
    }
    if ((unsigned)score <= (unsigned)Mg_GetTopRecord()) return;
    Mg_SetTopRecord(score);
    gMgScene->mNewRecord = 1;
}

}

namespace s20adafc {

/* The session best, mSessionBest; 0 with no scene. */
int Mg_GetSessionBest(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mSessionBest;
    }
    return 0;
}

}

namespace s20adb1c {
using s20adafc::Mg_GetSessionBest;
using s20ada40::Mg_SetSessionBest;
extern "C" {
extern MgEntry gMgMenuRow;
}

/* Submits a score: stores it in mSubmittedScore (outside scoring mode 5, capped
   at the menu row's scoreCap when that is nonzero) and passes it to
   Mg_SetSessionBest when it beats the session best. */
void Mg_SubmitScore(int score) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0) return;
    if (gMgMenuRow.scoringMode == 5) {
        scene->mSubmittedScore = score;
        if (Mg_GetSessionBest() != 0) {
            if ((unsigned)score >= (unsigned)Mg_GetSessionBest()) return;
        }
        Mg_SetSessionBest(score);
    } else {
        int cap = gMgMenuRow.scoreCap;
        if (cap != 0) {
            if ((unsigned)score > (unsigned)cap) score = cap;
        }
        scene->mSubmittedScore = score;
        if ((unsigned)score <= (unsigned)Mg_GetSessionBest()) return;
        Mg_SetSessionBest(score);
    }
}

}

namespace s20adbc0 {

/* The submitted score, mSubmittedScore; 0 with no scene. */
int Mg_GetSubmittedScore(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mSubmittedScore;
    }
    return 0;
}

}

namespace s20adbe0 {

/* The new-record flag, mNewRecord; 0 with no scene. */
int Mg_IsNewRecord(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mNewRecord;
    }
    return 0;
}

}

namespace s20adc00 {

/* Sets the saved top record (mTopRecord); no-op with no scene. */
void Mg_SetTopRecord(int v) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        scene->mTopRecord = v;
    }
}

}

namespace s20adc1c {

/* The saved top record (mTopRecord); 0 with no scene. */
int Mg_GetTopRecord(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mTopRecord;
    }
    return 0;
}

}

namespace s20adc3c {

/* The scene's save-table id: byte 1 of param1. */
int Mg_GetSaveTable(void *scene) {
    return (((dScMgBase_c *)scene)->param1 & 0xff00) >> 8;
}

}

namespace s20adc4c {
extern "C" {
extern int sMgWorkBuffer[];
}

int Mg_GetWorkBuffer(void) { return sMgWorkBuffer[0]; }

}

namespace s20adc5c {

void Ov004_Deallocate(void *ptr)
{
    Deallocate(ptr);
}

}

namespace s20adc68 {
extern "C" {
extern void *LoadFile(int handle);
}

void *Ov004_LoadFile(int id) {
    return LoadFile(id);
}

}

namespace s20adc74 {
extern "C" {
extern void *func_020182bc(const char *path);
}


void *Ov004_LoadFileByPath(const char *path)
{
    return func_020182bc(path);
}

}

namespace s20b1d60 {
void Mg_RenderSpriteOnNoMtx(void *, int, int, int, int);
}

namespace s20adc80 {
using s20ad674::GetGameLanguage;
using s20b1d60::Mg_RenderSpriteOnNoMtx;

}
/* State 1's draw: word 2 of the current language's entry in
   sMgSpriteTables, at mPos >> 12. */
void MgFx::DrawFlyTo() {
    int idx=s20adc80::GetGameLanguage();
    OamAttr **t = sMgSpriteTables[idx];
    s20adc80::Mg_RenderSpriteOnNoMtx(t[2], mPos[0] >> 0xc, mPos[1] >> 0xc, -1, -1);
}

namespace s20adcc8 {
using s20b1d60::Mg_RenderSpriteOnNoMtx;
extern "C" {
extern int sMgSparkleSpritesGuard;
extern OamAttr *data_ov004_020bea14[];
extern OamAttr *sMgSparkleSprites[];
}

}
/* State 0's draw. The first call builds the 16-entry frame table
   sMgSparkleSprites from five words at data_ov004_020bea14 (bit 0 of
   sMgSparkleSpritesGuard marks it built); the frame is picked by the low four
   bits of mTimer. */
void MgFx::DrawSparkle() {
    if ((s20adcc8::sMgSparkleSpritesGuard & 1) == 0) {
        OamAttr **s = s20adcc8::data_ov004_020bea14;
        OamAttr **d = s20adcc8::sMgSparkleSprites;
        d[0] = s[0];
        d[1] = s[0];
        d[2] = s[0];
        d[3] = s[0];
        d[4] = s[0];
        d[5] = s[4];
        d[6] = s[4];
        d[7] = s[3];
        d[8] = s[2];
        d[9] = s[1];
        d[10] = s[2];
        d[11] = s[2];
        d[12] = s[3];
        d[13] = s[4];
        d[14] = s[4];
        d[15] = s[0];
        s20adcc8::sMgSparkleSpritesGuard |= 1;
    }
    {
        int idx = ((int)mTimer) & 0xf;
        int x = mPos[0] >> 0xc;
        int y = mPos[1] >> 0xc;
        s20adcc8::Mg_RenderSpriteOnNoMtx(s20adcc8::sMgSparkleSprites[idx], x, y, -1, -1);
    }
}

namespace s20add88 {
extern "C" {
extern EntPMF sMgFxDrawTable[];
}

/* Draws one record through its state's handler. */
void Mg_DrawFx(MgFx *c) {
    (c->*sMgFxDrawTable[c->mState])();
}

}

namespace s20b1b40 {
void Mg_GainCredits(int c);
}

namespace s20addcc {
using s20b1b40::Mg_GainCredits;

inline long long inline_fn(int arg0)
{
  return (long long) arg0;
}

}
/* State 1's update. Counts mTimer down, calling Mg_GainCredits(1) on
   the frame it reaches 0, then places mPos between the two vectors by the
   remaining fraction s = mTimer / mDuration: x = mVecA.x * s + mVecB.x * (1 - s),
   y = mVecB.y + s * s * (mVecA.y - mVecB.y), all Fix12. */
void MgFx::UpdateFlyTo() {
    using namespace s20addcc;
    {
        if (ApproachLinear2(mTimer, 0, 1)) {
            Mg_GainCredits(1);
        }
        int a = mTimer;
        int b = mDuration;
        int s = FX_Div(a << 0xc, b << 0xc);
        int c3 = mDuration;
        int c2 = mTimer;
        int t = FX_Div((c3 - c2) << 0xc, c3 << 0xc);
        int v8 = mVecA[0];
        int v10 = mVecB[0];
        long long m1 = inline_fn(v8) * inline_fn(s);
        long long m2 = inline_fn(v10) * inline_fn(t);
        long long m3 = inline_fn(s) * inline_fn(s);
        long long m4 = inline_fn((int)((m3 + 0x800) >> 0xc)) * inline_fn(mVecA[1] - mVecB[1]);
        mPos[0] = ((int)((m1 + 0x800) >> 0xc)) + ((int)((m2 + 0x800) >> 0xc));
        mPos[1] = mVecB[1] + ((int)((m4 + 0x800) >> 0xc));
    }
}

/* State 0's update: counts mTimer down, passes mVecA to func_0203d630 with
   0xff8, then adds mVecB into mVecA and mVecA into mPos. */
void MgFx::UpdateDrift() {
    ApproachLinear2(mTimer, 0, 1);
    func_0203d630(mVecA, 0xff8);
    mVecA[0] += mVecB[0];
    mVecA[1] += mVecB[1];
    mPos[0] += mVecA[0];
    mPos[1] += mVecA[1];
}

namespace s20adf2c {
extern "C" {
extern EntPMF sMgFxUpdateTable[];
}

/* Updates one record through its state's handler. */
void Mg_UpdateFx(MgFx *c) {
    (c->*sMgFxUpdateTable[c->mState])();
}

}

namespace s20adf70 {

/* Fills record `c`. mDuration starts equal to mTimer; either vector may
   be omitted with a null pointer. */
extern "C" void Mg_InitFx(MgFx *e, short state, short timer, int *pos, int *vecA, int *vecB) {
    e->mState = state;
    e->mTimer = timer;
    e->mDuration = e->mTimer;
    e->mPos[0] = pos[0];
    e->mPos[1] = pos[1];
    if (vecA) {
        e->mVecA[0] = vecA[0];
        e->mVecA[1] = vecA[1];
    }
    if (vecB) {
        e->mVecB[0] = vecB[0];
        e->mVecB[1] = vecB[1];
    }
}

}

namespace s20adfc4 {
extern "C" void Mg_InitFx(void *rec, char *state, short timer, short pos, int *vecA, int *vecB);

/* Fills the first free record of the 0x40 at sMgFxPool (see
   Mg_InitFx); does nothing without a position or a free record.
   The parameter types are not the callers': `state` arrives as an int and
   `pos` is the position pointer, tested whole (movs r4, r2). They are left
   as they are because retyping them adds new disagreements with the
   callers' differing declarations in the declaration-agreement gate. */
extern "C" void Mg_SpawnFx(char *state, short timer, short pos, int *vecA, int *vecB, int *unused){
  if(pos==0) return;
  int i;
  for(i=0;i<0x40;i++){
    if(sMgFxPool[i].mTimer == 0){
      Mg_InitFx(&sMgFxPool[i], state, timer, pos, vecA, vecB);
      return;
    }
  }
}

}

/* dScMgBase_c::graphCallback_c::GraphCallback3 -- slot 3: calls the
   registered scene's OnGraphCallback3 (slot 25, +0x64) and reports handled. */
int dScMgBase_c::graphCallback_c::GraphCallback3()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        p->OnGraphCallback3();
    }

    return 1;
}

/* dScMgBase_c::graphCallback_c::GraphCallback2 -- slot 2.
 *
 * Unlike slots 0, 1 and 3, this one RETURNS the scene's OnGraphCallback2 (slot 24,
 * +0x60) result rather than discarding it and reporting handled; it only
 * reports handled when no scene is registered.
 */
int dScMgBase_c::graphCallback_c::GraphCallback2()
{
    dScMgBase_c *p = mScene;

    if (p == 0) {
        return 1;
    }

    return p->OnGraphCallback2();
}

/* dScMgBase_c::graphCallback_c::GraphCallback1 -- slot 1: calls the
   registered scene's OnGraphCallback1 (slot 22, +0x58) and reports handled. */
int dScMgBase_c::graphCallback_c::GraphCallback1()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        p->OnGraphCallback1();
    }

    return 1;
}

/* dScMgBase_c::graphCallback_c::GraphCallback0 -- slot 0: calls the
   registered scene's OnGraphCallback0 (slot 23, +0x5c) and reports handled. */
int dScMgBase_c::graphCallback_c::GraphCallback0()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        p->OnGraphCallback0();
    }

    return 1;
}

/* graphCallback_c ctor -- the base callback_c vptr store, the derived vptr
   store, and mScene = 0 are the three writes the ROM body makes. */
dScMgBase_c::graphCallback_c::graphCallback_c() : mScene(0) {}

/* dScMgBase_c::OnGraphCallback3 - slot 25: nonzero while the menu is closed. */
int dScMgBase_c::OnGraphCallback3()
{
    return mMenuOpen == 0;
}

/* dScMgBase_c::OnGraphCallback2 - slot 24. When mMenuOpen differs from its latch
   mMenuScreenSaved, calls SaveScreenForMenu (slot 29) if the menu has opened or
   RestoreScreenFromMenu (slot 30) if it has closed, then latches it. */
int dScMgBase_c::OnGraphCallback2()
{
    int v2 = mMenuOpen;
    int v1 = mMenuScreenSaved;
    if (v1 == v2) goto done;
    if (v2 == 0) {
        RestoreScreenFromMenu();
    } else {
        SaveScreenForMenu();
    }
    mMenuScreenSaved = mMenuOpen;
done:
    return 1;
}

/* dScMgBase_c::OnGraphCallback1 - slot 22 */
int dScMgBase_c::OnGraphCallback1()
{
    return 1;
}

/* dScMgBase_c::OnGraphCallback0 - slot 23 */
int dScMgBase_c::OnGraphCallback0()
{
    return 1;
}

namespace s20ae1a8 {
extern "C" {
extern MgEntry gMgMenuRow;
}

/* The menu row's scoring mode, or -1 with no scene. */
int Mg_GetScoringMode(void) {
    if (gMgScene != 0) {
        return gMgMenuRow.scoringMode;
    }
    return -1;
}

}

namespace s20ae1cc {
extern "C" {
extern MgEntry gMgMenuRow;
}


/* The menu row's music mode, or 0 with no scene. The
   music helpers below treat 0 as "stop the music" and 1 as "play
   Mg_GetMusicTrack's track". */
int Mg_GetMusicMode(void) {
    if (gMgScene != 0) {
        return gMgMenuRow.musicMode;
    }
    return 0;
}

}

namespace s20ae1f0 {

/* The scene's music track, mMusicTrack; 5 with no scene. */
int Mg_GetMusicTrack(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) return scene->mMusicTrack;
    return 5;
}

}

namespace s20ae20c {
extern "C" {
extern void d_s_mg_basePlaySubHold(unsigned int a, unsigned int b, unsigned int c, int d, int e);
}
using s20ae1cc::Mg_GetMusicMode;
using s20ae1f0::Mg_GetMusicTrack;

/* Sound::PlaySub with third argument 0 and its flag set starts nothing:
   it only sets the music volume target (sMusicVolumeTarget, which
   ChangeMusicVolume and PauseMusic also write) back to 0x7f, with step
   0x7f000, and zeroes sSubVolumeTarget. Then, while a scene exists and in
   music mode 1, this loads the scene's track. */
void Mg_RestoreMusic(void)
{
    Sound::PlaySub(0, 0x7f, 0, 0x7f000, 1);
    if (gMgScene == 0) return;
    if (Mg_GetMusicMode() != 1) return;
    Sound::LoadAndSetMusic_Layer1(Mg_GetMusicTrack());
}

}

namespace s20ae274 {
using s20ae1cc::Mg_GetMusicMode;


/* While a scene exists, starts sequence `id` on Sound::PlaySub's own
   player (sSubHandle) unless it is already the current one, and sets
   the music volume target to 0x40 [GUESS: ducks the music; PauseMusic uses
   0x40 too]. The Mg_GetMusicMode result is not used. */
void Mg_PlayJingle(void* id) {
    if (gMgScene == 0) return;
    Mg_GetMusicMode();
    Sound::PlaySub((unsigned int)id, 0x40, 0x7f, 0x7f000, 1);
}

}

namespace s20ae2c8 {
using s20ae1cc::Mg_GetMusicMode;
using s20ae1f0::Mg_GetMusicTrack;

/* Music mode 0 stops the music; mode 1 plays the scene's track. */
void Mg_StartMusic(void) {
    if (gMgScene == 0) return;
    int r = Mg_GetMusicMode();
    if (r == 0) {
        Sound::StopLoadedMusic_Layer1(1);
        return;
    }
    if (r != 1) return;
    Sound::LoadAndSetMusic_Layer1(Mg_GetMusicTrack());
}

}

namespace s20ae330 {
using s20ae1cc::Mg_GetMusicMode;
using s20ae1f0::Mg_GetMusicTrack;
extern "C" {
extern MgEntry gMgMenuRow;
}


/* Music mode 0 stops the music, and replays the scene's track only when
   the menu row's scene ID is 0x16f; any nonzero mode plays the
   track. */
void Mg_InitMusic(void) {
    if (gMgScene == 0)
        return;
    if (Mg_GetMusicMode() == 0) {
        Sound::StopLoadedMusic_Layer1(1);
        if (gMgMenuRow.id != 0x16f)
            return;
        Sound::LoadAndSetMusic_Layer1(Mg_GetMusicTrack());
        return;
    }
    Sound::LoadAndSetMusic_Layer1(Mg_GetMusicTrack());
}

}

namespace s20ae3b4 {
extern "C" {
/* dScMgBase_c::PaintSquare - slot 34.  THE BRUSH.

   Walks a `size` x `size` square centred on (cx, cy) and paints every cell
   inside it with palette index `colour`.  The address arithmetic is the DS
   4bpp background-character layout: `(x/8 + (y/8)*32)*32 + (y&7)*4` picks the
   word holding eight pixels of one row of one 8x8 tile, `(x&7)*4` is the
   nibble inside it, and MIi_CpuCopy32 is used for the read and the write-back
   rather than a plain load/store.  Sixteen colours, one nibble per pixel.  All
   four sides clip.

   It reads exactly the two object fields slot 33 (InitEngines) initialises.
   mBrushLayer is the layer index: 0..3 select G2S_GetBG0CharPtr through
   GetBG3CharPtr, and anything else returns without drawing -- which is what
   slot 33's -1 buys.  mBrushOnMain gates the wrapped region above the touch
   screen, where the MAIN engine's G2_GetBG*CharPtr are used instead and y is
   folded by `+ sMgTopScreenYOffset + 0xc0`.  So slot 33 leaves the brush
   disabled and a minigame arms it by choosing a layer.

   Its only in-family caller is ov004:0x020ae5c4, the line rasteriser sitting
   immediately after it in the image, which dispatches through +0x88 at seven
   separate sites as it walks a segment.  See the slot-34 block in
   include/dScMgBase_c.h for the arity and return-type measurements.

   `PaintSquare` is coined here from what the body does; it was `Virtual88`,
   after the +0x88 vtable offset. The cartridge names no method of this
   class. */

extern int sMgPaintWord;
extern int sMgTopScreenYOffset;
}

}

void dScMgBase_c::PaintSquare(int cx, int cy, int colour, int size) {
    using s20ae3b4::sMgPaintWord;
    using s20ae3b4::sMgTopScreenYOffset;

    int oi;
    int x0;
    int ci0;
    int m;
    int xx;
    int yy;
    int ci;
    int yrow;
    int four;
    int half;

    half = size / 2;
    oi = 0;
    if (size <= 0)
        return;
    x0 = cx - half;
    ci0 = oi;
    m = 0xf;
    yrow = cy - half;
    four = 4;
    do {
        ci = ci0;
        if (size > 0) {
            do {
                yy = yrow;
                xx = ci + x0;
                int* base;
                int* cell;
                int sh;

                if (xx < 0)
                    goto inc;
                if (xx >= 0x100)
                    goto inc;
                if (yrow < -0xc0 - sMgTopScreenYOffset)
                    goto inc;
                if (yrow < -sMgTopScreenYOffset) {
                    if (mBrushOnMain == 0)
                        goto inc;
                    switch (mBrushLayer) {
                    case 0:
                        base = (int *)G2_GetBG0CharPtr();
                        break;
                    case 1:
                        base = (int *)G2_GetBG1CharPtr();
                        break;
                    case 2:
                        base = (int *)G2_GetBG2CharPtr();
                        break;
                    case 3:
                        base = (int *)G2_GetBG3CharPtr();
                        break;
                    default: return;
                    }
                    yy += sMgTopScreenYOffset + 0xc0;
                } else {
                    if (yrow < 0)
                        goto inc;
                    if (yrow >= 0xc0)
                        goto inc;
                    switch (mBrushLayer) {
                    case 0:
                        base = (int *)G2S_GetBG0CharPtr();
                        break;
                    case 1:
                        base = (int *)G2S_GetBG1CharPtr();
                        break;
                    case 2:
                        base = (int *)G2S_GetBG2CharPtr();
                        break;
                    case 3:
                        base = (int *)G2S_GetBG3CharPtr();
                        break;
                    default: return;
                    }
                }
                cell = (int*)((char*)base + ((xx / 8 + (yy / 8) * 32) * 32) + ((yy & 7) * 4));
                MI_CpuCopy32(cell, &sMgPaintWord, four);
                sh = (xx & 7) << 2;
                sMgPaintWord = (sMgPaintWord & (-1 ^ (m << sh))) | (colour << sh);
                MI_CpuCopy32(&sMgPaintWord, cell, four);
            inc:;
            } while (++ci < size);
        }
        yrow += 1;
        oi += 1;
    } while (oi < size);
}

/* dScMgBase_c::Mg_DrawLine (ov004 0x020ae5c4): a Bresenham line from (x0, y0)
 * to (x1, y1), plotted point by point through the virtual PaintSquare (slot 34).
 * A zero-length line plots once. Load-bearing codegen, measured:
 * - The plot calls are the header's virtual PaintSquare, spelled directly. A
 *   function-pointer temp evaluated before the call puts the vtable load ahead
 *   of the stack argument, in a separate register.
 * - yy1 = y1 is a copy the vertical loop compares against (the ROM's [sp, #0xc]
 *   slot); the horizontal loop compares x1 itself.
 * - adx and ady are copied before their sign tests, in that order. Testing in
 *   the other order swaps the register pair and the flag reuse. */
void dScMgBase_c::Mg_DrawLine(int x0, int y0, int x1, int y1, int colour, int size) {
    int dx, dy, adx, ady, err;
    int yy1;
    if (x0 == x1 && y0 == y1) {
        PaintSquare(x0, y0, colour, size);
        return;
    }
    yy1 = y1;
    PaintSquare(x0, y0, colour, size);
    dy = y1 - y0;
    dx = x1 - x0;
    adx = dx;
    ady = dy;
    if (adx < 0) adx = -adx;
    if (ady < 0) ady = -ady;
    if (adx >= ady) {
        err = adx / 2;
        for (;;) {
            if (dx == 0) {
                PaintSquare(x0, y0, colour, size);
                return;
            }
            if (dx > 0) {
                err += ady;
                x0++;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                PaintSquare(x0, y0, colour, size);
                if (x0 == x1) return;
            } else {
                err += ady;
                x0--;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                PaintSquare(x0, y0, colour, size);
                if (x0 == x1) return;
            }
        }
    } else {
        err = ady / 2;
        for (;;) {
            if (dy == 0) {
                PaintSquare(x0, y0, colour, size);
                return;
            }
            if (dy > 0) {
                err += adx;
                y0++;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                PaintSquare(x0, y0, colour, size);
                if (y0 == yy1) return;
            } else {
                err += adx;
                y0--;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                PaintSquare(x0, y0, colour, size);
                if (y0 == yy1) return;
            }
        }
    }
}

namespace s20aea30 {
void Mg_DrawSpriteList(void *self, void *actor, void *c2, void *c3);
}

namespace s20ae858 {
using s20ad674::GetGameLanguage;
using s20aea30::Mg_DrawSpriteList;
extern "C" {
extern int *sMgMenuItem0SpritesByLanguage[];
extern int *sMgMenuItem1SpritesByLanguage[];
extern int *sMgMenuItem2SpritesByLanguage[];
extern OamAttr *data_ov006_021346bc;
}


/* Draws the three-item overlay menu: each item's per-language sprite list
   at its mMenuItemNX/Y, followed by data_ov006_021346bc at the same spot.
   The highlighted item (mMenuCursor) goes through a matrix whose diagonal
   is FX_Inv of a pulse that mMenuCursorPhase drives through
   FX_SinIdx. Nothing is drawn while mTouchOptions.Draw() returns
   nonzero. */
void Mg_DrawMenu(dScMgBase_c *scene)
{
    int extra[3];
    s16 *tbl[3];
    int buf[4];
    int i;

    if (scene->mTouchOptions.Draw() != 0)
        return;

    extra[0] = *sMgMenuItem0SpritesByLanguage[GetGameLanguage()];
    extra[1] = *sMgMenuItem1SpritesByLanguage[GetGameLanguage()];
    extra[2] = *sMgMenuItem2SpritesByLanguage[GetGameLanguage()];

    tbl[0] = &scene->mMenuItem0X;
    tbl[1] = &scene->mMenuItem1X;
    tbl[2] = &scene->mMenuItem2X;

    for (i = 0; i < 3; i++) {
        if (i == scene->mMenuCursor) {
            int idx = (int)(((u32)scene->mMenuCursorPhase << 0xf) >> 0x10) >> 4;
            char *cursor = (char *)extra[i];
            s16 *pos;
            u16 v;
            int *pbuf = buf;
            int scale = FX_Inv(FX_MUL(FX_SinIdx(idx), 0x800) + 0x1000);

            {
                volatile int *vp = pbuf;
                vp[0] = 0;
                vp[1] = 0;
                vp[2] = 0;
                vp[3] = 0;
            }
            buf[0] = scale;
            buf[3] = scale;

            pos = tbl[i];
            do {
                Mg_DrawSpriteListEntry((s20aea78::S *)cursor, pos[0], pos[1],
                                       (s20aea78::M *)buf);
                v = *(u16 *)(cursor + 6);
                cursor += 8;
            } while (v != 0xffff);

            Mg_DrawSpriteList(data_ov006_021346bc, (void *)(int)tbl[i][0], (void *)(int)tbl[i][1], 0);
        } else {
            s16 *pos = tbl[i];
            Mg_DrawSpriteList((void *)extra[i], (void *)(int)pos[0], (void *)(int)pos[1], 0);
            Mg_DrawSpriteList(data_ov006_021346bc, (void *)(int)pos[0], (void *)(int)pos[1], 0);
        }
    }
}

}

namespace s20aea30 {

/* Draws an OAM sprite list: calls Mg_DrawSpriteListEntry on each 8-byte
   entry from `self`, up to and including the one whose attr3 is 0xffff. */
void Mg_DrawSpriteList(void *self, void *actor, void *c2, void *c3)
{
    void *r8 = self;
    void *r7 = actor;
    void *r6 = c2;
    void *r5 = c3;
    u16 sentinel = 0xffff;
    do {
        Mg_DrawSpriteListEntry((s20aea78::S *)r8, (int)r7, (int)r6, (s20aea78::M *)r5);
        u16 v = *(u16*)((char*)r8 + 6);
        r8 = (char*)r8 + 8;
        if (v == sentinel) break;
    } while (1);
}

}

namespace s20aea78 {

struct M { int _00, _01, _10, _11; };
extern "C" void Mg_RenderSpriteOnMtx(void* a0, int a1, int a2, int a3, int a4, struct M* a5);


struct BF { unsigned int lo:10; unsigned int hi:22; };

struct S { int x; BF y; };


/* Draws one sprite-list entry at (a1, a2) through Mg_RenderSpriteOnMtx.
   When the scene's GetDisplayMode (slot 26) returns 2, the copy
   drawn has 0x200 subtracted from its tile number (attr2 bits 0..9). */
void Mg_DrawSpriteListEntry(S* self, int a1, int a2, struct M* a3)
{
    S v = *self;
    if (gMgScene->GetDisplayMode() == 2) {
        v.y.lo = (self->y.lo - 0x200) & 0x3ff;
    }
    Mg_RenderSpriteOnMtx(&v, a1, a2, 0, -1, a3);
}

}

extern "C" {
extern void func_0203b958(short* o, short* a, short* b);
}
void func_02012dd0(void *a);


/* The overlay menu's frame while it is open: slides the items in and, on
   a new touch with nothing highlighted, highlights the touched item in
   mMenuCursor (within 0x60 x 0x18 of item 0, 0x40 x 0x13 of the others, by
   func_0203b958's offsets) and sets the pick delay (0x10 for
   item 0, else 0). Once mMenuCursorPhase reaches 0x10000 and that timer
   reaches 0, item 0 (and any other value) calls CloseMenu
   (slot 28), item 1 starts a scene fade, and item 2 calls
   mTouchOptions.Open() and clears the highlight. */
void dScMgBase_c::UpdateMenu() {
    unsigned char idx = gActivePlayerSlot;
    unsigned short flags = data_020a0e5a[idx * 2];
    int unlocked;
    short tmp[2];
    short d0[2];
    short d1[2];
    short d2[2];
    short a, b;

    if ((flags & 0x40) || (flags & 0x80) || (flags & 0x20) || (flags & 0x10)) {
        func_02012790(0xe);
    }
    if (mTouchOptions.Update() != 0)
        return;

    ApproachLinear2(mMenuItem0X, 0x80, 0x10);
    ApproachLinear2(mMenuItem1X, 0x80, 0x10);
    ApproachLinear2(mMenuItem2Y, 0x90, 5);

    idx = gActivePlayerSlot;
    unlocked = 0;
    if (gTouchHeld[idx * 4] != 0) {
        unlocked = (gTouchEdge[idx * 4] != 0);
    }
    if (unlocked == 0) goto after;
    if (mMenuCursor >= 0)
        goto after;
    tmp[0] = gTouchX[idx * 4];
    tmp[1] = gTouchY[idx * 4];
    func_0203b958(d0, tmp, &mMenuItem0X);
    func_0203b958(d1, tmp, &mMenuItem1X);
    func_0203b958(d2, tmp, &mMenuItem2X);
    a = d0[0]; if (a < 0) a = -a;
    if (a < 0x60) {
        b = d0[1]; if (b < 0) b = -b;
        if (b < 0x18) {
            func_02012790(0x62);
            mMenuCursor = 0;
            /* 0x10 for item 0 and 0 for the others */
            mMenuPickDelay = 0x10;
            goto after;
        }
    }
    a = d1[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d1[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x63);
            mMenuCursor = 1;
            mMenuPickDelay = 0;
            goto after;
        }
    }
    a = d2[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d2[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x62);
            mMenuCursor = 2;
            mMenuPickDelay = 0;
        }
    }

after:
    if (mMenuCursor < 0)
        return;
    if (ApproachLinear(mMenuCursorPhase, 0x10000, 0x1000) == 0)
        return;
    if (ApproachLinear2(mMenuPickDelay, 0, 1) == 0)
        return;

    switch (mMenuCursor) {
    case 0:
        CloseMenu();
        return;
    case 1:
        dScene_c::SetFaders(&data_0209f61c);
        data_0209f61c.SetType(1);
        dScene_c::StartSceneFade(5, 0, 0);
        if (mQuitFadeStarted != 0)
            return;
        func_02012dd0((void *)0x3c);
        mQuitFadeStarted = 1;
        return;
    case 2:
        mTouchOptions.Open();
        mMenuCursor = -1;
        mMenuCursorPhase = 0;
        return;
    default:
        CloseMenu();
        return;
    }
}

/* What SaveScreenForMenu and RestoreScreenFromMenu keep in the header's
   pad_228 while the menu is up. A file-local view: dScMgBase_c.h is shared
   by 36 sources, and only these two bodies touch the buffers. It overlays
   the whole object from `this`; a view cast from pad_228 moves the literal
   pool. */
struct MgMenuSaveArea {
    u8 pad_000[0x2228];     /* 0x000 -- the dScMgBase_c members before them */
    u8 subObjChars[0x2000]; /* 0x2228 -- sub-screen OBJ VRAM at 0x06606000 */
    u8 mainBgPltt[0x200];   /* 0x4228 -- main BG palette from colour 16 on */
    u8 subBgPltt[0x200];    /* 0x4428 -- first 0x80 bytes of the sub BG palette */
};

namespace s20aeed8 {
extern "C" {
/* dScMgBase_c::RestoreScreenFromMenu - slot 30.

   WHAT IT DOES, because the name says the opposite of both halves.  This is
   the exact mirror of slot 29: 29 saves the screen and draws the three-item
   overlay menu over it, and this restores everything 29 saved.  POWCNT1's
   screen-swap bit comes back out of mSavedScreenSwap; both DISPCNTs get their
   layer-enable fields back from data_0209d460 / data_0209d458 and then their
   BG-enable fields back from mSavedMainBgBits / mSavedSubBgBits; 0x400 bytes
   of BG palette come back out of the buffer at +0x4228, the second 0x200 of it
   to the sub screen; and 0x2000 bytes of sub-screen OBJ VRAM go back to
   0x06606000 from +0x2228.  dScMgBase_c::OnGraphCallback2 calls this one when
   mMenuOpen falls to zero and slot 29 when it rises.  It is the menu coming
   down.  It is not a Yoshi egg, and it does not return a vector -- see the
   slot-30 block in include/dScMgBase_c.h for the AAPCS measurement that
   refutes the `ReturnVec` in the name. */
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern u8 data_0209d460;
extern u8 data_0209d458;

/* RETURN TYPE, MEASURED -- and it corrects the header's earlier `int`, which
   that slot's own evidence block called "a HINT".  Declaring this member `int`
   and letting control fall off the end does not cost an instruction, but it
   does reserve r0 as the result register, and the closing block wants four
   scratch registers.  mwcc then allocates r1-r4 where the ROM allocates
   r0-r3, and fourteen of the function's ninety-three words differ with no
   other change:

       ROM   ldr r0,[pc,#0x38] / mov r3,#0x4000000 / ldr r1,[r3] / ...
       int   ldr r1,[pc,#0x38] / mov r4,#0x4000000 / ldr r2,[r4] / ...

   Spelled `void` it is byte-exact.  Both overrides and decl_common.h:2321
   already said void; the retail source did too. */
}

}

void dScMgBase_c::RestoreScreenFromMenu() {
    using s20aeed8::gSubScreenLayers;
    using s20aeed8::data_0209d458;
    using s20aeed8::gMainScreenLayers;
    using s20aeed8::data_0209d460;
    MgMenuSaveArea *save = (MgMenuSaveArea *)this;

    gMainScreenLayers = 0;
    gSubScreenLayers = 0;

    *(u32*)0x4000000 &= ~0x1f00;
    *(u32*)0x4001000 &= ~0x1f00;
    REG_POWCNT1 = (mSavedScreenSwap << 15) | (REG_POWCNT1 & ~0x8000);
    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0xe000) | ((u32)data_0209d460 << 13);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0xe000) | ((u32)data_0209d458 << 13);

    GX_LoadBGPltt(save->mainBgPltt, 0x20, 0x1e0);
    {
        /* stepped from mainBgPltt: save->subBgPltt moves the literal pool */
        char *p = (char *)save->mainBgPltt;
        p += 0x200;
        GXS_LoadBGPltt(p, 0, 0x80);
    }

    if (GetDisplayMode() != 2) {
        MI_CpuCopy32(save->subObjChars, (u8 *)G2S_GetOBJCharPtr() + 0x6000, 0x2000);
    }

    gMainScreenLayers = (u8)mSavedMainBgBits;
    gSubScreenLayers = (u8)mSavedSubBgBits;

    if (GetDisplayMode() == 2)
        return;

    *(u32 *)0x4000000 = (*(u32 *)0x4000000 & ~0x1f00) | ((u32)gMainScreenLayers << 8);
    *(u32 *)0x4001000 = (*(u32 *)0x4001000 & ~0x1f00) | ((u32)gSubScreenLayers << 8);
}

namespace s20af04c {
extern "C" {
/* dScMgBase_c::CloseMenu - slot 28.

   A hit from underneath closes the three-item overlay menu
   that OpenMenu opens, resets the polymorphic touch-icon set, and
   re-enables the 3D engines only if the class's own GetDisplayMode
   says so. */

extern void Enable3dEngines();

/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
}

}

int dScMgBase_c::CloseMenu()
{ using s20af04c::Enable3dEngines;
    func_02012e1c();
    mMenuOpen = 0;
    mTouchOptions.Close();
    int r = GetDisplayMode();
    if (r != 0) Enable3dEngines();
}

namespace s20af094 {
extern "C" {
/* dScMgBase_c::SaveScreenForMenu - slot 29.

   The two buffers at +0x2228 and +0x4228 sit inside the header's pad_228;
   this body and RestoreScreenFromMenu reach them through MgMenuSaveArea.

   WHAT IT DOES.  The name is coined from this body; until 2026-10-14 the
   slot carried `OnAimedAtWithEgg`, transplanted from include/dActor_c.h at
   the same slot index.  Nothing in the cartridge carries a method name for
   this class.  The body saves the screen and then draws over it.  It stashes POWCNT1's
   screen-swap bit and the two BG-enable bytes into the object, clears every
   layer- and window-enable bit in both DISPCNTs, saves 0x400 bytes of BG
   palette out of palette RAM into +0x4228 and loads the menu's palettes over
   the top, then saves 0x2000 bytes of sub-screen OBJ VRAM at 0x06606000 into
   +0x2228 and decompresses a per-language image over that.  Its one caller in
   this module is dScMgBase_c::OnGraphCallback2, which reaches it only on a rising edge
   of mMenuOpen.  That is the three-item overlay menu going up, not a Yoshi egg.
   See the slot 29 block in include/dScMgBase_c.h for why the name is kept. */
}
using s20ad674::GetGameLanguage;
extern "C" {
extern void DecompressLZ16(void *src, void *dst);

extern unsigned char gMainScreenLayers;
extern unsigned char gSubScreenLayers;

/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
}

}

void func_0201f32c(int arg0);

int dScMgBase_c::SaveScreenForMenu() {
    using s20af094::DecompressLZ16;
    using s20af094::GetGameLanguage;
    using s20af094::gSubScreenLayers;
    using s20af094::gMainScreenLayers;

    MgMenuSaveArea *save = (MgMenuSaveArea *)this;

    if (GetDisplayMode() != 0)
        Disable3dEngines();

    mSavedScreenSwap = GX_GetDispSelect();

    mSavedMainBgBits = gMainScreenLayers;
    mSavedSubBgBits = gSubScreenLayers;

    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    gMainScreenLayers = 0;
    gSubScreenLayers = 0;

    *(volatile int *)0x4000000 &= ~0x1f00;
    *(volatile int *)0x4001000 &= ~0x1f00;
    *(volatile int *)0x4000000 &= ~0xe000;
    *(volatile int *)0x4001000 &= ~0xe000;

    MI_CpuCopy32((int *)0x5000020, save->mainBgPltt, 0x1e0);
    {
        /* stepped from mainBgPltt: save->subBgPltt moves the literal pool */
        int *pltt = (int *)save->mainBgPltt;
        MI_CpuCopy32((int *)0x5000400, pltt + 0x80, 0x80);
    }
    DC_FlushRange(save->mainBgPltt, 0x400);

    GX_LoadBGPltt((const void *)sMgMenuBgPltt, 0x20, 0xa0);
    GXS_LoadBGPltt((const void *)sMgMenuSubBgPltt, 0, 0x80);

    {
        volatile unsigned short tmp = 0;
        MI_CpuFill16((char *)0x5000000, tmp, 2);
    }
    {
        volatile unsigned short tmp = 0;
        MI_CpuFill16((char *)0x5000400, tmp, 2);
    }

    func_0201f32c(mSceneKind);

    gMainScreenLayers = DISPLAY_LAYER_BG1 | DISPLAY_LAYER_OBJ;
    gSubScreenLayers = 0x10;

    if (GetDisplayMode() != 2) {
        int *vram = (int *)((u8 *)G2S_GetOBJCharPtr() + 0x6000);
        MI_CpuCopy32(vram, save->subObjChars, 0x2000);
        int idx = GetGameLanguage();
        DecompressLZ16(sMgMenuScreenByLanguage[idx], vram);
        DC_FlushRange(save->subObjChars, 0x2000);
    }
}

/* dScMgBase_c::OpenMenu - slot 27. Unless mMenuDisabled is set, opens
   the three-item overlay menu: items start at their slide-in positions,
   the prompt is disabled and nothing is highlighted. */
void dScMgBase_c::OpenMenu()
{
    if (mMenuDisabled != 0) return;
    func_02012e78();
    mMenuItem0X = -0x80;
    mMenuItem0Y = 0x30;
    mMenuItem1X = 0x180;
    mMenuItem1Y = 0x60;
    mMenuItem2X = 0x80;
    mMenuItem2Y = 0xe0;
    mPromptEnabled = 0;
    mMenuOpen = 1;
    mMenuCursor = -1;
    mMenuCursorPhase = 0;
}

namespace s20af2f8 {
extern "C" {
extern char sMgBrushPalette;
}


/* Sets up the brush that PaintSquare paints with. `sb` is the scene: stores
   `en` in mBrushOnMain and the layer `mode` (0..3) in mBrushLayer, fills
   the first 0x300 screen entries of that sub-screen BG layer (and of the
   main-screen one when `en` is set) with tile numbers 0..0x2ff under
   palette `base`, zeroes 0x6000 bytes of the layer's character data, and
   loads the 0x20-byte palette sMgBrushPalette into palette slot `base` on
   both screens. Any other mode returns at once. */
void Mg_InitBrush(char* sb, char en, int mode, int base)
{
    dScMgBase_c *scene = (dScMgBase_c *)sb;
    volatile unsigned short v0, v1, v2, v3, v4, v5, v6, v7;
    volatile unsigned short w0, w1, w2, w3, w4, w5, w6, w7;
    int val;
    int i;
    int off;
    void *t;

    scene->mBrushOnMain = en;
    i = 0;
    off = i;
    scene->mBrushLayer = mode;
    val = i;
    val += base << 12;

    for (; i < 0x300; i++, val++, off += 2) {
        switch (scene->mBrushLayer) {
        case 0:
            if (scene->mBrushOnMain) {
                t = (char *)G2_GetBG0ScrPtr() + off;
                MI_CpuFill16(t, v0 = val, 2);
            }
            t = (char *)G2S_GetBG0ScrPtr() + off;
            MI_CpuFill16(t, v1 = val, 2);
            break;
        case 1:
            if (scene->mBrushOnMain) {
                t = (char *)G2_GetBG1ScrPtr() + off;
                MI_CpuFill16(t, v2 = val, 2);
            }
            t = (char *)G2S_GetBG1ScrPtr() + off;
            MI_CpuFill16(t, v3 = val, 2);
            break;
        case 2:
            if (scene->mBrushOnMain) {
                t = (char *)G2_GetBG2ScrPtr() + off;
                MI_CpuFill16(t, v4 = val, 2);
            }
            t = (char *)G2S_GetBG2ScrPtr() + off;
            MI_CpuFill16(t, v5 = val, 2);
            break;
        case 3:
            if (scene->mBrushOnMain) {
                t = (char *)G2_GetBG3ScrPtr() + off;
                MI_CpuFill16(t, v6 = val, 2);
            }
            t = (char *)G2S_GetBG3ScrPtr() + off;
            MI_CpuFill16(t, v7 = val, 2);
            break;
        default:
            return;
        }
    }

    switch (scene->mBrushLayer) {
    case 0:
        if (scene->mBrushOnMain) {
            t = G2_GetBG0CharPtr();
            w0 = 0;
            MI_CpuFill16(t, w0, 0x6000);
        }
        t = G2S_GetBG0CharPtr();
        w1 = 0;
        MI_CpuFill16(t, w1, 0x6000);
        break;
    case 1:
        if (scene->mBrushOnMain) {
            t = G2_GetBG1CharPtr();
            w2 = 0;
            MI_CpuFill16(t, w2, 0x6000);
        }
        t = G2S_GetBG1CharPtr();
        w3 = 0;
        MI_CpuFill16(t, w3, 0x6000);
        break;
    case 2:
        if (scene->mBrushOnMain) {
            t = G2_GetBG2CharPtr();
            w4 = 0;
            MI_CpuFill16(t, w4, 0x6000);
        }
        t = G2S_GetBG2CharPtr();
        w5 = 0;
        MI_CpuFill16(t, w5, 0x6000);
        break;
    case 3:
        if (scene->mBrushOnMain) {
            t = (void *)G2_GetBG3CharPtr();
            w6 = 0;
            MI_CpuFill16(t, w6, 0x6000);
        }
        t = G2S_GetBG3CharPtr();
        w7 = 0;
        MI_CpuFill16(t, w7, 0x6000);
        break;
    default:
        return;
    }

    GX_LoadBGPltt(&sMgBrushPalette, base << 5, 0x20);
    GXS_LoadBGPltt(&sMgBrushPalette, base << 5, 0x20);
}

}

namespace s20af5e0 {

/* One OAM record: GXOamAttr's attr0/attr1 word, then attr2's fields and
   attr3. Spelled locally because including nitro/gx/gxcommon.h renumbers
   this TU's @NNN symbols. */
typedef struct Item {
    u32 attr01;
    u32 charNo : 10;
    u32 priority : 2;
    u32 cParam : 4;
    u32 attr3 : 16;
} Item;


/* Copies an OAM sprite list (8-byte entries, ending at the one whose
   attr3 is 0xffff) from `src` to `dst`, adding `add` to each tile number
   (the low ten bits of attr2). At most 0x20 entries: the 32nd is given the
   0xffff terminator. Returns the number of entries written. */
extern "C" int Mg_CopySpriteList(Item* src, Item* dst, int add)
{
    int n = 1;
    int i;
    for (i = 0; i < 0x20; i++) {
        dst[i] = *src;
        dst[i].charNo = (add + src->charNo) & 0x3ff;
        if (src->attr3 == 0xffff)
            break;
        if (i == 0x1f) {
            dst[i].attr3 = 0xffff;
            break;
        }
        src++;
        n++;
    }
    return n;
}

}

/* OAM::Render is a C++-linkage member of the global OAM namespace, so its
 * declaration sits outside the address namespace and its extern "C" block. */
struct OamAttr; struct Matrix2x2;
namespace OAM {
void Render(bool draw, OamAttr *obj, int px, int py, int pal, int prio, Matrix2x2 *mtx);
void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
int Render(bool, OamAttr *, int, int, int, int, int, int);
void RenderSub(OamAttr *data, s32 x, s32 y, s32 palette, s32 priority);
}

/* A view of include/dScMgD3DBase_c.h down to mDrawSubScreen; the member
 * names and offsets are the header's. Including the header itself
 * renumbers this file's compiler-generated @ labels (@329 becomes @529),
 * which the TU manifest pins; this view leaves them alone. */
struct D3DBaseView : dScMgBase_c {
    s32 mSavedSubObjBank; /* 0x4660 */
    u16 mDrawSubScreen;   /* 0x4664 */
};

/* The sprite wrappers from here to Mg_RenderSpriteBothMtx share one gate.
 * With no scene they draw nothing. With the menu closed and the scene's
 * GetDisplayMode (slot 26) returning 2 they read
 * dScMgD3DBase_c::mDrawSubScreen and draw once, or not at all. Otherwise they
 * take their normal path. The cast is proven by that gate: 2 is returned only
 * by dScMgD3DBase_c's body (ov006:0x020e6e4c), which its four children
 * inherit without overriding, so the scene is a dScMgD3DBase_c whenever the
 * field is read. The OAM::Render overloads taking a Fix12<int> by value stay
 * mangled (see include/OAM.h). */

namespace s20af68c {
extern "C" {
extern void OamRender10_d_s_mg_base_a(int, OamAttr *, s32, s32, int, int, int, int, int, int);
}

void Hud_RenderSprite(void *attr, int x, int y, int palette, int priority) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 0)
                return;
            OAM::Render(0, (OamAttr *)attr, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
            return;
        }
    }
    OAM::RenderSub((OamAttr *)attr, x, y, palette, priority);
}

}

namespace s20af770 {

struct OamAttr;

typedef int Fix12i;
extern "C" {
extern int OamRender8_d_s_mg_base(int show, struct OamAttr *attr, int a, int b, int c, int d,
                                  Fix12i e, int f);
}


int Mg_RenderSubSpriteScaled(void *a0, int a1, int a2, int a3, int a4, int a5,
                                        unsigned short a6) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return 0;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 0)
                return 0;
            return OAM::Render(0, (::OamAttr *)a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return OAM::Render(1, (::OamAttr *)a0, a1, a2, a3, a4, a5, a6);
}

}

namespace s20af868 {

void Mg_RenderSubSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 0)
                return;
            OAM::Render(0, (OamAttr *)a0, a1, a2, a3, a4, (Matrix2x2 *)a5);
            return;
        }
    }
    OAM::Render(1, (OamAttr *)a0, a1, a2, a3, a4, (Matrix2x2 *)a5);
}

}

namespace s20af948 {

void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *m) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 0)
                return;
            OAM::Render(false, (OamAttr *)a, b, c, -1, -1, (Matrix2x2 *)m);
            return;
        }
    }
    OAM::Render(true, (OamAttr *)a, b, c, -1, -1, (Matrix2x2 *)m);
}

}

namespace s20afa20 {
extern "C" {
extern void OamRender10_d_s_mg_base_a(int, OamAttr *, s32, s32, int, int, int, int, int, int);
}


void RenderOamMainScreen(OamAttr *oam, int x, int y, int palette, int priority) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 1)
                return;
            OAM::Render(0, oam, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
            return;
        }
    }
    OAM::Render(0, oam, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
}

}

namespace s20afb20 {

struct OamAttr;

typedef int Fix12i;
extern "C" {
extern int OamRender8_d_s_mg_base(int show, struct OamAttr *attr, int a, int b, int c, int d,
                                  Fix12i e, int f);
}


int Mg_RenderMainSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5,
                                         unsigned short a6) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return 0;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 1)
                return 0;
            return OAM::Render(0, (::OamAttr *)a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return OAM::Render(0, (::OamAttr *)a0, a1, a2, a3, a4, a5, a6);
}

}

namespace s20afc18 {

void Mg_RenderMainSpritePalMtx(void *a0, int a1, int a2, int a3, int a4, void *a5) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 1)
                return;
            OAM::Render(0, (OamAttr *)a0, a1, a2, a3, a4, (Matrix2x2 *)a5);
            return;
        }
    }
    OAM::Render(0, (OamAttr *)a0, a1, a2, a3, a4, (Matrix2x2 *)a5);
}

}

namespace s20afcf8 {

void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0 && scene->GetDisplayMode() == 2) {
        if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 1)
            return;
        OAM::Render(false, attr, x, y, -1, -1, mtx);
        return;
    }
    OAM::Render(false, attr, x, y, -1, -1, mtx);
}

}

namespace s20afdd0 {
extern "C" {
extern int sMgTopScreenYOffset;
extern int OamRender10_d_s_mg_base_b(int, void *, int, int, int, int, int, int, int, int);
}

void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen == 1) {
                OAM::Render(0, (OamAttr *)a0, a1, a2 + 0xc0 + sMgTopScreenYOffset, a3, a4, 0x1000,
                            0x1000, 0, -1);
                return;
            }
            OAM::Render(0, (OamAttr *)a0, a1, a2, a3, a4, 0x1000, 0x1000, 0, -1);
            return;
        }
    }
    OAM::Render(0, (OamAttr *)a0, a1, a2 + 0xc0 + sMgTopScreenYOffset, a3, a4, 0x1000, 0x1000, 0,
                -1);
    OAM::RenderSub((OamAttr *)a0, a1, a2, a3, a4);
}

}

namespace s20aff38 {

struct OamAttr;
extern "C" {
extern int sMgTopScreenYOffset;
}

typedef int Fix12i;
extern "C" {
extern int OamRender8_d_s_mg_base(int show, struct OamAttr *attr, int a, int b, int c, int d,
                                  Fix12i e, int f);
}


int Mg_RenderSpriteBothScaled(struct OamAttr *a0, int a1, int a2, int a3, int a4,
                                         Fix12i a5, int a6) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return 0;
    if (scene->mMenuOpen == 0 && scene->GetDisplayMode() == 2) {
        if (((D3DBaseView *)gMgScene)->mDrawSubScreen == 1) {
            int hp = sMgTopScreenYOffset;
            if (a2 >= -0x100 - hp && a2 < -hp) {
                return OAM::Render(0, (::OamAttr *)a0, a1, a2 + 0xc0 + hp, a3, a4, a5, a6);
            }
            if (a2 >= -0x40 && a2 < 0xc0) {
                return OAM::Render(0, (::OamAttr *)a0, a1, a2, a3, a4, a5, a6);
            }
        }
    } else {
        int hp = sMgTopScreenYOffset;
        if (a2 >= -0x100 - hp && a2 < -hp) {
            return OAM::Render(0, (::OamAttr *)a0, a1, a2 + 0xc0 + hp, a3, a4, a5, a6);
        }
        if (a2 >= -0x40 && a2 < 0xc0) {
            return OAM::Render(1, (::OamAttr *)a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return 0;
}

}

namespace s20b0104 {
extern "C" {
extern int sMgTopScreenYOffset;
}

void RenderOamBothScreens(void *attr, int x, int y, int palette, int priority, void *mtx) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen == 1) {
                OAM::Render(false, (OamAttr *)attr, x, y + 0xc0 + sMgTopScreenYOffset, palette,
                            priority, (Matrix2x2 *)mtx);
                return;
            }
            OAM::Render(false, (OamAttr *)attr, x, y, palette, priority, (Matrix2x2 *)mtx);
            return;
        }
    }
    OAM::Render(false, (OamAttr *)attr, x, y + 0xc0 + sMgTopScreenYOffset, palette, priority,
                (Matrix2x2 *)mtx);
    OAM::Render(true, (OamAttr *)attr, x, y, palette, priority, (Matrix2x2 *)mtx);
}

}

namespace s20b023c {
extern "C" {
extern int sMgTopScreenYOffset;
}

void Mg_RenderSpriteBothPalMtx(void *a0, int a1, int a2, int a3, void *a4) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen == 1) {
                OAM::Render(0, (OamAttr *)a0, a1, a2 + 0xc0 + sMgTopScreenYOffset, a3, -1,
                            (Matrix2x2 *)a4);
                return;
            }
            OAM::Render(0, (OamAttr *)a0, a1, a2, a3, -1, (Matrix2x2 *)a4);
            return;
        }
    }
    OAM::Render(0, (OamAttr *)a0, a1, a2 + 0xc0 + sMgTopScreenYOffset, a3, -1, (Matrix2x2 *)a4);
    OAM::Render(1, (OamAttr *)a0, a1, a2, a3, -1, (Matrix2x2 *)a4);
}

}

namespace s20b0380 {
extern "C" {
extern int sMgTopScreenYOffset;
}

void Mg_RenderSpriteBothMtx(void *a0, int a1, int a2, void *a3) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen == 1) {
                OAM::Render(0, (OamAttr *)a0, a1, a2 + 0xc0 + sMgTopScreenYOffset, -1, -1,
                            (Matrix2x2 *)a3);
                return;
            }
            OAM::Render(0, (OamAttr *)a0, a1, a2, -1, -1, (Matrix2x2 *)a3);
            return;
        }
    }
    OAM::Render(0, (OamAttr *)a0, a1, a2 + 0xc0 + sMgTopScreenYOffset, -1, -1, (Matrix2x2 *)a3);
    OAM::Render(1, (OamAttr *)a0, a1, a2, -1, -1, (Matrix2x2 *)a3);
}

}

namespace s20b04c0 {
extern "C" {
extern int sMgTopScreenYOffset;
}

/* Get and set sMgTopScreenYOffset: the offset PaintSquare and the sprite
   wrappers above add, with 0xc0, to y on their upper-screen path. */
int Mg_GetTopScreenYOffset(void) {
    return sMgTopScreenYOffset;
}

}

namespace s20b04d0 {
extern "C" {
extern int sMgTopScreenYOffset;
}

void Mg_SetTopScreenYOffset(int v) {
    sMgTopScreenYOffset = v;
}

}

/* dScMgBase_c::GetDisplayMode - slot 26: which display layout the scene
 * uses. BeforeInitResources turns the 3D engines on for any nonzero answer.
 *
 * `return 0;` here, `return 1;` in dScMgSingle3DBase_c's override and
 * `return 2;` in dScMgD3DBase_c's.  Three distinct constants out of three
 * bodies is what makes the return type int rather than void, and it is why the
 * caller has to be able to tell the three apart. */
int dScMgBase_c::GetDisplayMode()
{
    return 0;
}

void dScMgBase_c::OnPendingDestroy()
{
}

s32 dScMgBase_c::Render()
{
    return 1;
}

/* Draws the menu instead of the scene while it is open. The two loops draw
 * the three banner records in two passes, those with mGfxSlot clear first. */
int dScMgBase_c::BeforeRender()
{
    int i;
    MgBanner *p;
    int i2;
    MgBanner *p2;
    int j;
    MgFx *fx;

    if (dScene_c::BeforeRender() == 0)
        return 0;

    if (mMenuOpen != 0) {
        Mg_DrawMenu(this);
        return 0;
    }

    if (mStateController.mSuspended == 0) {
        p = (MgBanner *)gMgBanners;
        for (i = 0; i < 3; i++, p++) {
            if (p->mFileIndex == 0x1d)
                continue;
            if (p->mGfxSlot != 0)
                continue;
            Banner_Draw(p);
        }
        p2 = (MgBanner *)gMgBanners;
        for (i2 = 0; i2 < 3; i2++, p2++) {
            if (p2->mFileIndex == 0x1d)
                continue;
            if (p2->mGfxSlot == 0)
                continue;
            Banner_Draw(p2);
        }
        mStateController.Render();
    }

    fx = sMgFxPool;
    for (j = 0; j < 0x40; j++, fx++) {
        if (sMgFxPool[j].mTimer != 0)
            Mg_DrawFx(fx);
    }

    DrawPrompt();
    return 1;
}

s32 dScMgBase_c::Behavior()
{
    return 1;
}

/* Handles input and the menu, runs the UI state controller and the shared
 * records, approaches mCreditsShown toward mCredits, and advances the 40-frame
 * counter. Returns 0 to stop the derived scene's behavior this frame. */
int dScMgBase_c::BeforeBehavior()
{
    int mode;
    unsigned short flags;

    if (dScene_c::BeforeBehavior() == 0)
        return 0;

    if (gSceneFader->IsAtStart()) {
        mode = gActivePlayerSlot;
        flags = data_020a0e5a[mode * 2];
        if ((flags & 8) || (flags & 4) || (flags & 1) || (flags & 2)) {
            if (mMenuOpen != 0) {
                if (mMenuCursor < 0) {
                    if (mTouchOptions.IsIdle())
                        CloseMenu();
                }
            } else {
                if (gTouchHeld[mode * 4] == 0)
                    OpenMenu();
            }
        }
    }

    if (mMenuOpen != 0) {
        UpdateMenu();
        return 0;
    }
    if (mMenuScreenSaved != 0)
        return 0;

    if (mStateController.mSuspended == 0) {
        int i;
        MgBanner *g;
        mStateController.Behavior();
        g = (MgBanner *)gMgBanners;
        for (i = 0; i < 3; i++) {
            if (g->mFileIndex != 0x1d)
                Banner_Update(g);
            g++;
        }
    }

    {
        int j;
        MgFx *p = sMgFxPool;
        MgFx *base = sMgFxPool;
        for (j = 0; j < 0x40; j++) {
            if (base[j].mTimer != 0)
                Mg_UpdateFx(p);
            p++;
        }
    }

    if (mRunDuringFade == 0) {
        if (gSceneFader->IsAtStart() == 0)
            return 0;
    }

    ApproachLinear(mCreditsShown, mCredits, data_0208ee44);

    mFrameCounter++;
    if (mFrameCounter >= 0x28)
        mFrameCounter = 0;

    return 1;
}

void dScMgBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        if (gMgMenuRow.ownRecords == 0)
            Mg_CommitRecords();
        gGraphCallback = 0;
        sMgGraphCallback.mScene = 0;
        FreeGfxSlotsById(0x1d);
        if (sMgWorkBuffer != 0) {
            ::operator delete[](sMgWorkBuffer);
            sMgWorkBuffer = 0;
        }
        if (mMenuOpen != 0) {
            func_02012e1c();
            Sound::StopLoadedMusic_Layer1(1);
        }
        Mg_FreeBannerFiles();
    }
    dScene_c::AfterCleanupResources(vfSuccess);
}

/* Finishes the display and font setup, then reports to dScene_c. */
void dScMgBase_c::AfterInitResources(u32 vfSuccess)
{
    InitMainScreenBg();
    LoadFont(2);
    Mg_InitMusic();
    dScene_c::AfterInitResources(vfSuccess);
}

/* Mg_ResetBannerLayout takes no parameter. The `add r0, r4, #0x4000` just
   before the call is the base of the mNewRecord store, left in r0. */

/* Sets up the scene and both displays. Slots 26, 31 and 33 must stay
 * virtual calls so derived scenes can override them. */
bool dScMgBase_c::BeforeInitResources()
{
    if (dScene_c::BeforeInitResources() == 0) return 0;
    if (GetDisplayMode() == 0)
        Disable3dEngines();
    else
        Enable3dEngines();
    mMusicTrack = gMgMenuRow.musicTrack;
    if (sMgWorkBuffer == 0)
        sMgWorkBuffer = ::operator new[](0x4000);
    /* +0xb0 is the header's pad_0b0. Scanning ov004 and ov006 turns up no
       other word access at +0xb0 in a scene method or through gMgScene, so
       no role is established and the store stays raw. */
    if (gMgScene != 0)
        *(int*)((char*)gMgScene + 0xb0) = 0;
    mHudScore = 0;
    mBestHudScore = 0;
    mNewRecord = 0;
    Mg_ResetBannerLayout();
    InitEngines();
    Mg_LoadBannerFiles();
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.SetType(0);
    data_0209d460 = 0;
    data_0209d458 = 0;
    InitSubScreenBg();
    return 1;
}

namespace s20b0a38 {

/* Clears mStateController.mSuspended, so BeforeBehavior and BeforeRender
   step and draw the UI state controller again. */
void Mg_ResumeSharedState(void)
{
    dScMgBase_c *scene = gMgScene;
    if (scene == 0) return;
    scene->mStateController.mSuspended = 0;
}

}

namespace s20b0a54 {

/* Sets the UI state controller's state and clears mRecordRevealTimer. The singleton
   is read again after SetState, as the ROM does. */
void Mg_SetSharedState(s32 state)
{
    dScMgBase_c *scene = gMgScene;
    if (!scene)
        return;
    scene->mStateController.SetState(state);
    gMgScene->mRecordRevealTimer = 0;
}

}

namespace s20b0aa0 {
extern "C" {
extern MgBanner gMgBanners[];
}

/* Calls Banner_Free on each of the three 0x134-byte records at
   gMgBanners whose word at +0x20 equals `arg` -- or, for 0x1d,
   on each whose word is not 0x1d. */
void FreeGfxSlotsById(int arg)
{
  int i;
  MgBanner *p;
  i = arg;
  if (i == 0x1d)
  {
    p = gMgBanners;
    for (i = 0; i < 3; i++)
    {
        if (p->mFileIndex != 0x1d) {
            Banner_Free(p);
        }
      p++;
    }

  }
  else
  {
    p = gMgBanners;
    for (i = 0; i < 3; i++)
    {
        if (arg == p->mFileIndex) {
            Banner_Free(p);
        }
      p++;
    }

  }
}

}

namespace s20b0b1c {


struct Vec2 {
    short x;
    short y;
};
extern "C" {
extern MgBanner gMgBanners[3];
extern int sMgBannerButtonsArmed;

}


// The ROM scales idx into the addressing mode here, which only the 4-byte-record view of the lane reproduces.
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))
int Mg_CheckBannerButton(int arg) {
    int idx = gActivePlayerSlot;
    int flag = 0;
    int i;

    if (TOUCH_REC(gTouchHeld)[idx][0] != 0) {
        if (TOUCH_REC(gTouchEdge)[idx][0] != 0) flag = 1;
    }

    if (flag) {
        MgBanner *o = gMgBanners;
        int cx = TOUCH_REC(gTouchX)[idx][0];
        int cy = TOUCH_REC(gTouchY)[idx][0];
        for (i = 0; i < 3; i++, o++) {
            if (arg == o->mFileIndex && o->mArrived != 0) {
                volatile struct Vec2 pos;
                int dx, dy;
                short ty;
                pos.x = o->mX;
                ty = o->mY;
                dx = pos.x - cx;
                pos.y = ty;
                dy = pos.y - cy;
                if (dx <= 0x40 && dx >= -0x40 && dy <= 0x13 && dy >= -0x13) {
                    if (sMgBannerButtonsArmed != 0) {
                        Banner_Press(o);
                        if ((unsigned int)(arg - 1) <= 1) {
                            gMgScene->mMenuDisabled = 1;
                        }
                    }
                }
            }
        }
    } else {
        int j;
        MgBanner *o = gMgBanners;
        for (j = 0; j < 3; j++, o++) {
            if (arg == o->mFileIndex) {
                if (Banner_IsPressFinished(o) != 0) {
                    if (arg == 1) {
                    } else if (arg == 2) {
                    } else {
                        sMgBannerButtonsArmed = 1;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

}

namespace s20b0cac {
extern "C" {
extern struct MgBanner gMgBanners[];
}

/* Hands its arguments to Banner_Init for the first of the three
   records at gMgBanners whose mFileIndex is 0x1d (free); does nothing
   when none is. */
void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6) {
    int i;
    for (i = 0; i < 3; i++) {
        if (gMgBanners[i].mFileIndex == 0x1d) {
            Banner_Init(&gMgBanners[i], c, a1, a2, a3, arg5, arg6);
            return;
        }
    }
}

}

namespace s20b0d30 {
extern "C" {
extern char sMgBannerGfxSlotUsed[];
}


void Mg_ResetOverlays(void) {
    FreeGfxSlotsById(0x1d);
    int i = 0;
    do {
        sMgFxPool[i].mTimer = 0;
        i++;
    } while (i < 0x40);
    int j = 0;
    char* p = sMgBannerGfxSlotUsed;
    do {
        j++;
        *p++ = 0;
    } while (j < 3);
}

}

namespace s20b0d8c {
using s20ad674::GetGameLanguage;
using s20af68c::Hud_RenderSprite;

/* Draws per-language sprite word 0x11 or 0x12 at (arg1, arg2): word 0x11
   for the first 20 frames of the scene's 40-frame mFrameCounter cycle,
   word 0x12 for the rest. */
void Mg_DrawBlinkSprite(void *c, s32 arg1, s32 arg2)
{
    s32 idx1 = ((dScMgBase_c *)c)->mFrameCounter;
    s32 r4 = (idx1 < 0x14) ? 0x11 : 0x12;
    s32 idx2 = GetGameLanguage();
    OamAttr **table = sMgSpriteTables[idx2];
    Hud_RenderSprite(table[r4], arg1, arg2, -1, -1);
}

}

namespace s20b0de0 {
using s20ad674::GetGameLanguage;
using s20afa20::RenderOamMainScreen;

}

/* Draws the per-language prompt sprite at (0xc0, 0xb0) while mPromptEnabled
   is set: blinking (shown for the first 0x18 frames of each 0x30-frame
   mPromptBlinkTimer cycle) for four cycles, then every frame. */
void dScMgBase_c::DrawPrompt() {
    using namespace s20b0de0;
    if (mPromptEnabled == 0)
        return;
    if ((unsigned int)mPromptBlinkCount < 4U) {
        mPromptBlinkTimer++;
        if ((unsigned int)mPromptBlinkTimer >= 0x30U) {
            mPromptBlinkTimer = 0U;
            mPromptBlinkCount++;
        }
        if ((unsigned int)mPromptBlinkTimer >= 0x18U)
            return;
    }
    {
        int i = GetGameLanguage();
        RenderOamMainScreen(sMgSpriteTables[i][10], 0xc0, 0xb0, -1, -1);
    }
}

namespace s20b0e84 {
using s20ad674::GetGameLanguage;
using s20afa20::RenderOamMainScreen;


static inline int sdiv4(int d) {
    int t = d + (int)((unsigned)(d >> 1) >> 0x1e);
    return t >> 2;
}


static inline int sdiv2(int e) {
    return (e + (int)((unsigned)e >> 31)) >> 1;
}


/* Draws a tally of `sb` in a column at x 0xf0, upward from y 0xa8: one
   icon (per-language sprite word 9) per unit, and from five upward the
   whole fives as a number (Mg_DrawNumber) beside sprite word 11 with
   only the remainder as icons. mTallyShown holds the last tally. A rise plays
   sound 0x1bb (func_02012790) and starts the mTallyAnimTimer counter -- one step a
   frame, two when slot 26 returns 2 -- while the newest icon blinks with a
   period of 60 mTallyAnimTimer steps (30 frames when slot 26 returns 2); a fall
   stops it. When the tally reaches a multiple of
   five, the icons close up as mTallyAnimTimer runs from mTimeLimit to
   mTimeLimit + 0x20, with sound 0x1ba as they start. */
void Mg_DrawTally(dScMgBase_c *sl, int sb) {
    OamAttr **r8;
    int r7;
    int i;
    int pin;

    {
        int t = sl->mTallyShown;
        if (t < sb) {
            sl->mTallyAnimTimer = 1;
            sl->mTallyShown = sb;
            func_02012790(0x1bb);
        } else if (t > sb) {
            sl->mTallyAnimTimer = 0;
            sl->mTallyShown = sb;
        } else if (sl->mTallyAnimTimer > 0) {
            if (sl->GetDisplayMode() == 2) {
                sl->mTallyAnimTimer += 2;
            } else {
                sl->mTallyAnimTimer += 1;
            }
        }
    }

    r8 = sMgSpriteTables[GetGameLanguage()];
    r7 = 0xa8;

    if (sb < 5) {
        r7 -= 8;
        i = 0;
        if (sb > 0) {
            do {
                if (i == sb - 1 && sl->mTallyAnimTimer > 0 && sl->mTallyAnimTimer % 0x3c >= 0x1e) {
                    break;
                }
                RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
                i += 1;
                r7 -= 0x10;
            } while (i < sb);
        }
        if (sl->mTallyAnimTimer >= sl->mTimeLimit) {
            sl->mTallyAnimTimer = 0;
        }
        return;
    }

    if (sb == 5) {
        int u = sl->mTallyAnimTimer;
        if (u == 0) {
            Mg_DrawNumber(0xf0, r7, 5, 1, 0, 0, 0x14);
            RenderOamMainScreen(r8[11], 0xf0, r7, -1, -1);
        } else if (u < sl->mTimeLimit) {
            r7 -= 8;
            i = 0;
            do {
                RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
                i += 1;
                r7 -= 0x10;
            } while (i < 4);
            if (sl->mTallyAnimTimer % 0x3c < 0x1e) {
                RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
            }
        } else {
            int d = (sl->mTimeLimit + 0x20) - sl->mTallyAnimTimer;
            r7 -= sdiv4(d);
            i = 0;
            do {
                RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
                {
                    unsigned e = (sl->mTimeLimit + 0x20) - sl->mTallyAnimTimer;
                    r7 -= sdiv2((int)e);
                }
                i += 1;
            } while (i < 5);
        }
        if (sl->GetDisplayMode() == 2) {
            int b = sl->mTimeLimit;
            int c = sl->mTallyAnimTimer;
            if (c == b || c == b + 1) {
                func_02012790(0x1ba);
            }
        } else if (sl->mTallyAnimTimer == sl->mTimeLimit) {
            func_02012790(0x1ba);
        }
        if (sl->mTallyAnimTimer >= sl->mTimeLimit + 0x20) {
            sl->mTallyAnimTimer = 0;
        }
        return;
    }

    {
        int rem = sb % 5;
        if (rem == 0) {
            int u = sl->mTallyAnimTimer;
            if (u == 0) {
                Mg_DrawNumber(0xf0, r7, sb, 1, 0, 0, 0x14);
                RenderOamMainScreen(r8[11], 0xf0, r7, -1, -1);
            } else if (u < sl->mTimeLimit) {
                Mg_DrawNumber(0xf0, r7, sb - 5, 1, 0, 0, 0x14);
                RenderOamMainScreen(r8[11], 0xf0, r7, -1, -1);
                r7 -= 0x18;
                i = 0;
                do {
                    if (i == 4 && sl->mTallyAnimTimer > 0 && sl->mTallyAnimTimer % 0x3c >= 0x1e) {
                        break;
                    }
                    RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
                    i += 1;
                    r7 -= 0x10;
                } while (i < 5);
            } else {
                int d;
                Mg_DrawNumber(0xf0, r7, sb - 5, 1, 0, 0, 0x14);
                RenderOamMainScreen(r8[11], 0xf0, r7, -1, -1);
                d = (sl->mTimeLimit + 0x20) - sl->mTallyAnimTimer;
                r7 -= sdiv4(d);
                i = 0;
                do {
                    unsigned e = (sl->mTimeLimit + 0x20) - sl->mTallyAnimTimer;
                    r7 -= sdiv2((int)e);
                    RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
                    i += 1;
                } while (i < 5);
            }
            if (sl->GetDisplayMode() == 2) {
                int b = sl->mTimeLimit;
                int c = sl->mTallyAnimTimer;
                if (c == b || c == b + 1) {
                    func_02012790(0x1ba);
                }
            } else if (sl->mTallyAnimTimer == sl->mTimeLimit) {
                func_02012790(0x1ba);
            }
            if (sl->mTallyAnimTimer >= sl->mTimeLimit + 0x20) {
                sl->mTallyAnimTimer = 0;
            }
            return;
        }

        {
            int base5 = (sb / 5) * 5;
            Mg_DrawNumber(0xf0, r7, base5, 1, 0, 0, 0x14);
        }
        RenderOamMainScreen(r8[11], 0xf0, r7, -1, -1);
        r7 -= 0x18;
        sb = 0;
        if (rem > 0) {
            pin = rem - 1;
            do {
                if (sb == pin && sl->mTallyAnimTimer > 0 && sl->mTallyAnimTimer % 0x3c >= 0x1e) {
                    break;
                }
                RenderOamMainScreen(r8[9], 0xf0, r7, -1, -1);
                sb += 1;
                r7 -= 0x10;
            } while (sb < rem);
        }
        if (sl->mTallyAnimTimer >= sl->mTimeLimit) {
            sl->mTallyAnimTimer = 0;
        }
    }
}

}

namespace s20b1710 {
void Mg_DrawRecordRow(dScMgBase_c *scene, int x, int y, int val);
}

namespace s20b14f0 {
using s20ad6f4::Mg_GetLastRecordRank;

using s20b1710::Mg_DrawRecordRow;


/* Draws the five saved records in rows 0x18 apart from (0xa8, 0x28),
   revealing one more row every 0x14 frames as mRecordRevealTimer counts up. mRecordRevealTimer
   then loops over 0x64..0x8b, and the session best (mRankedScore) blinks on
   that 40-frame loop: in the row of the rank it took (mLastRecordRank, read
   through Mg_GetLastRecordRank), or in a row below the five when it placed
   nowhere. Mg_GetLastRecordRank takes only the scene; the second argument
   passed here is unused. */
extern "C" void Mg_DrawRecordTable(dScMgBase_c *scene) {
    int a2 = 0x28;
    int n;
    int i;
    int j;
    int k;

    scene->mRecordRevealTimer += 1;
    if (scene->mRecordRevealTimer >= 0x8c)
        scene->mRecordRevealTimer = 0x64;

    n = Mg_GetLastRecordRank(scene, scene->mRankedScore);
    if (n < 0)
        goto neg;

    i = 0;
    if (n > 0)
    {
        do
        {
            if (scene->mRecordRevealTimer >= i * 0x14)
                Mg_DrawRecordRow(scene, 0xa8, a2, func_02013580(Mg_GetSaveTable(scene), i));
            a2 += 0x18;
            i++;
        } while (i < n);
    }

    if (scene->mRecordRevealTimer >= n * 0x14)
    {
        if (scene->mRecordRevealTimer < 0x64 || scene->mRecordRevealTimer - 0x64 < 0x14)
            Mg_DrawRecordRow(scene, 0xa8, a2, scene->mRankedScore);
        else
            Mg_DrawRecordRow(scene, 0xa8, a2, -1);
    }

    a2 += 0x18;
    j = n + 1;
    if (j >= 5)
        return;
    do
    {
        if (scene->mRecordRevealTimer >= j * 0x14)
            Mg_DrawRecordRow(scene, 0xa8, a2, func_02013580(Mg_GetSaveTable(scene), j));
        a2 += 0x18;
        j++;
    } while (j < 5);
    return;

neg:
    k = 0;
    do
    {
        if (scene->mRecordRevealTimer >= k * 0x14)
            Mg_DrawRecordRow(scene, 0xa8, a2, func_02013580(Mg_GetSaveTable(scene), k));
        a2 += 0x18;
        k++;
    } while (k < 5);

    if (scene->mRecordRevealTimer < 0x64)
        return;

    if (scene->mRecordRevealTimer - 0x64 < 0x14)
        Mg_DrawRecordRow(scene, 0xa8, a2 + 0x10, scene->mRankedScore);
    else
        Mg_DrawRecordRow(scene, 0xa8, a2 + 0x10, -1);
}

}

namespace s20b1710 {
using s20ad674::GetGameLanguage;
using s20af68c::Hud_RenderSprite;
extern "C" {
extern MgEntry gMgMenuRow;
}


/* Draws one record row at y. In scoring mode 5 (gMgMenuRow.scoringMode)
   `val` is a time in frames, drawn as seconds, sprite word 7, tenths and
   hundredths ending at x, and a negative val draws nothing. Otherwise
   modes 1, 2 and 4 draw two label sprites at x 0x58 and 0x68 first; mode
   3 then draws val 4 px left through Mg_DrawNumber, every other mode
   through Mg_DrawBigNumber. A negative val draws no number. */
void Mg_DrawRecordRow(dScMgBase_c *scene, int x, int y, int val) {
    int idx = GetGameLanguage();
    int mode = gMgMenuRow.scoringMode;
    OamAttr **tbl = sMgSpriteTables[idx];
    int flag = 0;

    if (mode == 5) {
        int major;
        int rem;
        int pct;
        if (val < 0)
            return;
        major = val / 60;
        Mg_DrawNumber(x - 0x18, y, major, flag, -1, 1, 0xa);
        Hud_RenderSprite(tbl[7], x - 0x10, y, -1, -1);

        rem = val % 60;
        pct = rem * 100;
        Mg_DrawNumber(x - 8, y, pct / 600, 0, -1, 1, 0xa);

        rem = val % 60;
        pct = rem * 100 / 60;
        Mg_DrawNumber(x, y, pct % 10, 0, -1, 1, 0xa);
        return;
    }

    if (mode == 3)
        flag = 1;
    switch (mode) {
    case 1:
        Hud_RenderSprite(tbl[9], 0x58, y, -1, -1);
        Hud_RenderSprite(tbl[1], 0x68, y, -1, -1);
        break;
    case 2:
        Hud_RenderSprite(tbl[2], 0x58, y, -1, -1);
        Hud_RenderSprite(tbl[1], 0x68, y, -1, -1);
        break;
    case 3:
        flag = 1;
        break;
    case 4:
        Hud_RenderSprite(tbl[0x13], 0x58, y, -1, -1);
        Hud_RenderSprite(tbl[1], 0x68, y, -1, -1);
        break;
    default:
        break;
    }

    if (flag == 1)
        x -= 4;
    if (val < 0)
        return;
    if (flag == 1)
        Mg_DrawNumber(x, y, val, 0, -1, 1, 0xa);
    else
        Mg_DrawBigNumber(x, y, val, 0, -1, 1, 0);
}

}

namespace s20b19f0 {
using s20ad674::GetGameLanguage;
using s20afa20::RenderOamMainScreen;

/* Draws sprite word 8 at (0x20, 0xc) and `value`, starting at x
   0x48, through Mg_DrawNumber. */
void Mg_DrawTopLeftCounter(int value) {
    int i = GetGameLanguage();
    RenderOamMainScreen(sMgSpriteTables[i][8], 0x20, 0xc, -1, -1);
    Mg_DrawNumber(0x48, 0xc, value, 1, -1, 2, 0xa);
}

}

namespace s20b1a5c {
using s20ad674::GetGameLanguage;
using s20afa20::RenderOamMainScreen;
extern "C" {
extern int sMgCounterMaxByDigits[];
}

/* Draws a0, capped at sMgCounterMaxByDigits[a1], right-aligned at (0xf4,
   0xc), and sprite word 4 to its left at an x that a1 sets. */
void Mg_DrawTopRightCounter(int a0, int a1){
    int lim = sMgCounterMaxByDigits[a1];
    int c = a0;
    int r4 = 0xf4;
    int i;
    if (c >= lim)
        c = lim;
    Mg_DrawNumber(0xf4, 0xc, c, 1, -1, 1, 0xa);
    r4 = r4 - (0x44 - (6 - a1) * 8);
    i = GetGameLanguage();
    RenderOamMainScreen(sMgSpriteTables[i][4], r4, 0xc, -1, -1);
}

}

namespace s20b1aec {

/* The scene's mLevel; 0 with no scene. */
int Mg_GetLevel(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) return scene->mLevel;
    return 0;
}

}

namespace s20b1b78 {
void Mg_SubCredits(void *c, int val);
}

namespace s20b1b08 {
using s20b1b78::Mg_SubCredits;

/* Subtracts `amount` from the scene's mCredits (Mg_SubCredits). */
void Mg_LoseCredits(int amount) {
    void *v = gMgScene;
    if (v == 0) return;
    Mg_SubCredits(v, amount);
}

}

namespace s20b1ba0 {
void Mg_AddCredits(void *c, s32 delta);
}

namespace s20b1b40 {
using s20b1ba0::Mg_AddCredits;

/* Adds `amount` to the scene's mCredits (Mg_AddCredits). */
void Mg_GainCredits(int amount) {
    void *v = gMgScene;
    if (v == 0) return;
    Mg_AddCredits(v, amount);
}

}

namespace s20b1b78 {

/* mCredits -= val, clamped to 0..9999. */
void Mg_SubCredits(void* c, int val) {
    dScMgBase_c *scene = (dScMgBase_c *)c;
    int r2 = scene->mCredits;
    r2 -= val;
    if (r2 < 0) r2 = 0;
    else if (r2 > 0x270f) r2 = 0x270f;
    scene->mCredits = r2;
}

}

namespace s20b1ba0 {

/* mCredits += delta, clamped to 0..9999. */
void Mg_AddCredits(void *c, s32 delta)
{
    dScMgBase_c *scene = (dScMgBase_c *)c;
    s32 v = scene->mCredits + delta;
    if (v < 0) v = 0;
    else if (v > 0x270f) v = 0x270f;
    scene->mCredits = v;
}

}

namespace s20b1de8 {
void Mg_DrawCreditsIcon(int r0, int r1, int r2, int r3);
}

namespace s20b1bc8 {
using s20b1de8::Mg_DrawCreditsIcon;

/* Draws sprite word 2 at (a1, a2) on wrapper a3 (Mg_DrawCreditsIcon),
   then the scene's mCreditsShown beside it: below 100 through
   Mg_DrawBigNumber, otherwise through Mg_DrawNumber. */
void Mg_DrawCredits(char *a0, int a1, int a2, int a3)
{
    Mg_DrawCreditsIcon(a1, a2, a3, -1);
    int v = ((dScMgBase_c *)a0)->mCreditsShown;
    if (v < 0x64) {
        Mg_DrawBigNumber(a1+0x12, a2, v, a3, -1, 2, 0);
    } else {
        Mg_DrawNumber(a1+0xc, a2, v, a3, -1, 2, 0x32);
    }
}

}

namespace s20b1c68 {

struct M { int _00, _01, _10, _11; };
using s20b0104::RenderOamBothScreens;
using s20afc18::Mg_RenderMainSpritePalMtx;

/* a3 picks the wrapper: -1 RenderOamBothScreens, 0 Mg_RenderSubSpritePalMtx,
   anything else Mg_RenderMainSpritePalMtx; each is passed -1 in its place. */
extern "C" void Mg_RenderSpriteOnMtx(void* a0, int a1, int a2, int a3, int a4, struct M* a5){
  if(a3==-1){
    RenderOamBothScreens(a0, a1, a2, -1, a4, a5);
    return;
  }
  if(a3==0){
    Mg_RenderSubSpritePalMtx(a0, a1, a2, -1, a4, a5);
    return;
  }
  Mg_RenderMainSpritePalMtx(a0, a1, a2, -1, a4, a5);
}

}

namespace s20b1cf0 {
using s20afdd0::Mg_RenderSpriteBoth;
using s20af68c::Hud_RenderSprite;
using s20afa20::RenderOamMainScreen;

/* `sel` picks the wrapper: -1 Mg_RenderSpriteBoth, 0 Hud_RenderSprite,
   anything else RenderOamMainScreen; each is passed -1 in its place. */
void Mg_RenderSpriteOn(OamAttr *a, int b, int c, int sel, int e) {
    if (sel == -1) {
        Mg_RenderSpriteBoth(a, b, c, -1, e);
        return;
    }
    if (sel == 0) {
        Hud_RenderSprite(a, b, c, -1, e);
        return;
    }
    RenderOamMainScreen(a, b, c, -1, e);
}

}

namespace s20b1d60 {
using s20b0104::RenderOamBothScreens;
using s20afc18::Mg_RenderMainSpritePalMtx;

/* Mg_RenderSpriteOnMtx without a matrix: the wrappers get 0. */
void Mg_RenderSpriteOnNoMtx(void* a0, int a1, int a2, int a3, int a4){
  if(a3==-1){
    RenderOamBothScreens(a0, a1, a2, -1, a4, 0);
    return;
  }
  if(a3==0){
    Mg_RenderSubSpritePalMtx(a0, a1, a2, -1, a4, 0);
    return;
  }
  Mg_RenderMainSpritePalMtx(a0, a1, a2, -1, a4, 0);
}

}

namespace s20b1de8 {
using s20ad674::GetGameLanguage;
using s20b1d60::Mg_RenderSpriteOnNoMtx;


/* Draws sprite word 2 at (r0, r1) through Mg_RenderSpriteOnNoMtx. */
void Mg_DrawCreditsIcon(int r0, int r1, int r2, int r3) {
    int idx = GetGameLanguage();
    OamAttr **t = sMgSpriteTables[idx];
    Mg_RenderSpriteOnNoMtx(t[2], r0, r1, r2, r3);
}

}

namespace s20b1e34 {
using s20b0e84::Mg_DrawTally;


/* Draws the mHudScore tally through Mg_DrawTally (a tail call). */
void Mg_DrawScoreTally(void* a, int unused1, int unused2, int unused3) {
    Mg_DrawTally((dScMgBase_c *)a, ((dScMgBase_c *)a)->mHudScore);
}

}

namespace s20b1e44 {
using s20ad674::GetGameLanguage;
using s20afcf8::DrawOamSprite;

/* Draws `arg` centred at (0x80, 0xc) and sprite word 3 at (0x60, 0xc). */
void Mg_DrawTopCenterCounter(int arg) {
    int r;
    Mg_DrawBigNumber(0x80, 0xc, arg, 1, -1, 0, 0);
    r = GetGameLanguage();
    DrawOamSprite(sMgSpriteTables[r][3], 0x60, 0xc, 0);
}

}

namespace s20b1ea4 {
extern "C" {
extern OamAttr *sMgDigitSprites[];
}
using s20b1d60::Mg_RenderSpriteOnNoMtx;


/* Draws `val`, capped at 999999, in digits 8 px apart: glyph
   sMgDigitSprites[fb + digit], where fb 0 means 0xa and 0x14 means
   0x1e. Mode 0 centres the number on x, mode 1 ends it at x, any other
   mode starts it at x. */
void Mg_DrawNumber(int x, int a1, int val, int a3, int a4, int mode, int fb) {
    int d[6];
    int i;
    if (val >= 0xf423f) val = 0xf423f;
    for (i = 0; i < 6; i++) d[i] = 0;
    d[0] = val;
    while (d[0] >= 0x186a0) { d[5]++; d[0] -= 0x186a0; }
    while (d[0] >= 0x2710) { d[4]++; d[0] -= 0x2710; }
    while (d[0] >= 0x3e8) { d[3]++; d[0] -= 0x3e8; }
    while (d[0] >= 0x64) { d[2]++; d[0] -= 0x64; }
    while (d[0] >= 0xa) { d[1]++; d[0] -= 0xa; }
    if (fb == 0) fb = 0xa;
    else if (fb == 0) fb = 0xa;
    else if (fb == 0x14) fb = 0x1e;
    else if (fb == 0xa) fb = 0xa;

    if (d[5] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0x14;
        else if (mode == 1) x -= 0x28;
        r5 = 5;
        for (k = 0; k < 5; k++) {
            Mg_RenderSpriteOnNoMtx(sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[4] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0x10;
        else if (mode == 1) x -= 0x20;
        r5 = 4;
        for (k = 0; k < 4; k++) {
            Mg_RenderSpriteOnNoMtx(sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[3] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0xc;
        else if (mode == 1) x -= 0x18;
        r5 = 3;
        for (k = 0; k < 3; k++) {
            Mg_RenderSpriteOnNoMtx(sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[2] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 8;
        else if (mode == 1) x -= 0x10;
        r5 = 2;
        for (k = 0; k < 2; k++) {
            Mg_RenderSpriteOnNoMtx(sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[1] != 0) {
        if (mode == 0) x -= 4;
        else if (mode == 1) x -= 8;
        Mg_RenderSpriteOnNoMtx(sMgDigitSprites[fb + d[1]], x, a1, a3, a4);
        x += 8;
    }
    Mg_RenderSpriteOnNoMtx(sMgDigitSprites[d[0] + fb], x, a1, a3, a4);
}

}

// Kept: without it the thousands counter moves from r0 to r3 (register colour only).
#pragma push
#pragma opt_propagation off
namespace s20b2220 {

/* Draws a score number rotated by an angle: splits the value (clamped to
   9999) into thousands, hundreds, tens and units, builds the rotation-and-
   scale matrix from the sine table, and draws the digits centred on x: four
   digits from x-0x30, three from x-0x20, two from x-0x10, a lone unit at x.
   The fixed-point multiply and the 2x2 rotation are the NitroSDK FX_Mul and
   MTX_Rot22 shapes, written as local inlines. Spelled that way the
   allocator keeps the thousands counter in r0 as the cartridge does; with
   the products written out in the body it lands in ip or r3. */
struct M { int _00, _01, _10, _11; };
extern "C" void Mg_RenderSpriteOnMtx(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
extern "C" {
extern OamAttr *sMgDigitSprites[];
}


static inline int FX_Mul(int v1, int v2)
{
    s64 t = (s64)v1 * v2;
    return (int)((t + 0x800) >> 12);
}


static inline void MTX_Rot22(struct M *p, int sinVal, int cosVal)
{
    p->_00 = cosVal;
    p->_01 = sinVal;
    p->_10 = -sinVal;
    p->_11 = cosVal;
}


void Mg_DrawNumberRotated(int x, int y, int value, int a3, int a4, int scale, u16 angle)
{
    struct M m;
    int idx;
    int te, hu, th;

    if (value >= 9999) value = 9999;
    te = hu = th = 0;
    while (value >= 1000) { value -= 1000; th++; }
    while (value >= 100) { value -= 100; hu++; }
    while (value >= 10) { value -= 10; te++; }

    idx = angle >> 4;
    MTX_Rot22(&m, FX_Mul(FX_SinIdx(idx), scale), FX_Mul(FX_CosIdx(idx), scale));

    if (th != 0) {
        Mg_RenderSpriteOnMtx(sMgDigitSprites[th], x - 0x30, y, a3, a4, &m);
        Mg_RenderSpriteOnMtx(sMgDigitSprites[hu], x - 0x10, y, a3, a4, &m);
        Mg_RenderSpriteOnMtx(sMgDigitSprites[te], x + 0x10, y, a3, a4, &m);
        x += 0x30;
    } else if (hu != 0) {
        Mg_RenderSpriteOnMtx(sMgDigitSprites[hu], x - 0x20, y, a3, a4, &m);
        Mg_RenderSpriteOnMtx(sMgDigitSprites[te], x, y, a3, a4, &m);
        x += 0x20;
    } else if (te != 0) {
        Mg_RenderSpriteOnMtx(sMgDigitSprites[te], x - 0x10, y, a3, a4, &m);
        x += 0x10;
    }
    Mg_RenderSpriteOnMtx(sMgDigitSprites[value], x, y, a3, a4, &m);
}

}
#pragma pop

namespace s20b2444 {
extern "C" {
extern OamAttr *sMgDigitSprites[];
}
using s20b1d60::Mg_RenderSpriteOnNoMtx;


/* Draws `num` in digits 16 px apart, glyph sMgDigitSprites[idx +
   digit]. The units digit sits at a1 for sel 1 (or any value but 0 and
   2), at a1 + 8 per extra digit for sel 0, and at a1 + 16 per extra digit
   for sel 2, so the number ends at, centres on, or starts at a1. 0 draws
   one glyph; a negative num draws nothing. */
void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx)
{
    unsigned t = num;
    int digits = 0;
    int off;
    if (num != 0) {
        do {
            t /= 10;
            digits++;
        } while (t != 0);
    }
    off = 0;
    switch (sel) {
    case 0:
        off = (digits << 3) - 8;
        break;
    case 1:
        break;
    case 2:
        off = (digits << 4) - 0x10;
        break;
    }
    if (num == 0) {
        Mg_RenderSpriteOnNoMtx(sMgDigitSprites[idx], a1, a2, a4, a5);
        return;
    }
    if (num <= 0)
        return;
    do {
        Mg_RenderSpriteOnNoMtx(sMgDigitSprites[num % 10 + idx], a1 + off, a2, a4, a5);
        num /= 10;
        off -= 0x10;
    } while (num > 0);
}

}

namespace s20b2574 {
using s20ad674::GetGameLanguage;
using s20afcf8::DrawOamSprite;


/* Unless arg1 is 1, draws sprite words 0 and 1 at (12, 12) and (30, 12)
   and arg0 from x 0x30. With arg1 1, draws sprite word 0 arg0 times,
   16 px apart from (12, 12). */
void Mg_DrawStockCounter(int arg0, int arg1)
{
    OamAttr **table = sMgSpriteTables[GetGameLanguage()];
    OamAttr *sb = table[0];
    int b = 12;
    if (arg1 != 1) {
        DrawOamSprite(sb, b, b, 0);
        DrawOamSprite(sMgSpriteTables[GetGameLanguage()][1], 30, b, 0);
        Mg_DrawBigNumber(0x30, b, arg0, 1, -1, 2, 0);
    } else {
        int i;
        for (i = 0; i < arg0; i++) {
            Mg_RenderSubSpriteMtx(sb, b, 12, 0);
            b += 16;
        }
    }
}

}

namespace s20b2980 {
void Mg_DisableAllBanks(void);
}

namespace s20b290c {
void Mg_ResetBgOffsets(void);
}

namespace s20b265c {
using s20b2980::Mg_DisableAllBanks;
using s20b290c::Mg_ResetBgOffsets;
using s20ad674::GetGameLanguage;
using s20adc68::Ov004_LoadFile;
extern "C" {
extern void DecompressLZ16(void *src, void *dst);
}
using s20adc5c::Ov004_Deallocate;
using s20b0d30::Mg_ResetOverlays;
using s20b0aa0::FreeGfxSlotsById;
extern "C" {
extern int sMgTopScreenYOffset;
extern u8 gMainScreenLayers;
extern int sMgStockCounterFiles[];
extern u8 gSubScreenLayers;
extern dGraph_c::callback_c *gGraphCallback;
extern int data_0208ee44;
}

}

void dScMgBase_c::InitEngines() {
    using s20b265c::DecompressLZ16;
    using s20b265c::FreeGfxSlotsById;
    using s20b265c::GetGameLanguage;
    using s20b265c::Ov004_Deallocate;
    using s20b265c::data_0208ee44;
    using s20b265c::gSubScreenLayers;
    using s20b265c::gMainScreenLayers;
    using s20b265c::gGraphCallback;
    using s20b265c::sMgStockCounterFiles;
    using s20b265c::sMgTopScreenYOffset;
    using s20b265c::Ov004_LoadFile;
    using s20b265c::Mg_ResetOverlays;
    using s20b265c::Mg_ResetBgOffsets;
    using s20b265c::Mg_DisableAllBanks;
    void *p;

    sMgTopScreenYOffset = 0;
    mBrushOnMain = 0;
    mBrushLayer = -1;
    GXS_DispOn();
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    Mg_DisableAllBanks();
    Mg_ResetBgOffsets();
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    GX_SetBankForBG(GX_VRAM_BG_256_AB);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    gMainScreenLayers = DISPLAY_LAYER_OBJ;
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    p = Ov004_LoadFile(sMgStockCounterFiles[GetGameLanguage()]);
    DecompressLZ16(p, (u8 *)G2_GetOBJCharPtr() + 0x4000);
    DecompressLZ16(p, (u8 *)G2S_GetOBJCharPtr() + 0x4000);
    Ov004_Deallocate(p);
    p = Ov004_LoadFile(0xc3);
    DC_FlushRange(p, 0x100u);
    GX_LoadOBJPltt(p, 0x100u, 0x100u);
    GXS_LoadOBJPltt(p, 0x100u, 0x100u);
    Ov004_Deallocate(p);
    gSubScreenLayers = 0x10;
    sMgGraphCallback.mScene = this;
    gGraphCallback = &sMgGraphCallback;
    Mg_ResetOverlays();
    FreeGfxSlotsById(0x1d);
    data_0208ee44 = 1;
}

namespace s20b27f4 {
using s20ad674::GetGameLanguage;
extern "C" {
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char gMainScreenLayers;
}

}

void dScMgBase_c::InitMainScreenBg() {
    using s20b27f4::GetGameLanguage;
    using s20b27f4::LoadCompressedFileAt;
    using s20b27f4::gMainScreenLayers;
    int f;
    G2_SetBG1Priority(0);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                     GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(false);
    SetBg1Offset(0, 0);
    gMainScreenLayers &= ~DISPLAY_LAYER_BG1;
    f = GetGameLanguage();
    LoadCompressedFileAt(sMgPauseBgNcgByLanguage[f], (void *)G2_GetBG1CharPtr());
    LoadCompressedFileAt(0x67, G2_GetBG1ScrPtr());
}

namespace s20b2880 {
using s20ad674::GetGameLanguage;
extern "C" {
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char gSubScreenLayers;
}

}

int dScMgBase_c::InitSubScreenBg() {
    using s20b2880::GetGameLanguage;
    using s20b2880::LoadCompressedFileAt;
    using s20b2880::gSubScreenLayers;
    int f;
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    G2S_BG1Mosaic(false);
    G2S_SetBG1Priority(0);
    SetSubBg1Offset(0, 0);
    gSubScreenLayers &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(sMgOptionNcgByLanguage[f], (void *)G2S_GetBG1CharPtr());
    LoadCompressedFileAt(0x5b, G2S_GetBG1ScrPtr());
}

namespace s20b290c {

/* Resets the scroll of all eight BG layers, main and sub, to (0, 0). */
void Mg_ResetBgOffsets(void) {
    SetBg0Offset(0, 0);
    SetBg1Offset(0, 0);
    SetBg2Offset(0, 0);
    SetBg3Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
}

}

namespace s20b2980 {

void Mg_DisableAllBanks(void) {
    GX::DisableAllBanks();
}

}

/* dScMgBase_c::OnDealDone - slot 21. */
void dScMgBase_c::OnDealDone()
{
}

/* Slot 20: the shared results state calls it once while it shows a new
   record (dMgState_c, state 17). See dScMgBase_c.h. */
/* The base does nothing: the ROM body is a single `bx lr`. */
void dScMgBase_c::OnNewRecord()
{
}

/* dScMgBase_c::CanEndState - vtable slot 19. Both the name and the signature are evidenced here: this
   body carries its own `recovered name:` above, and dScMgJump_c and
   dScMgBSC_c carry theirs. The parameter is an int, not dActor_c.h:132's
   `daPly_c &` -- see include/dScMgBase_c.h. The base ignores the mode. */
int dScMgBase_c::CanEndState(int /* mode */)
{
    return 1;
}

/* vtable slot 18. The name is NOT a `recovered name:` -- this body carried
   none. It comes from include/dActor_c.h:131 and from ov006/symbols.txt,
   which already named dScMgCoin_c's override of this slot. The signature,
   unlike the name, is measured; see include/dScMgBase_c.h.
   The base ignores both arguments -- every descendant that cares reads
   them. */
void dScMgBase_c::StartRound(int arg)
{
}

namespace s20b29a0 {
/* Calls the scene's StartRound (slot 18). Its callers, all in dMgState_c,
   pass the scene singleton and its mState, declaring the state as a
   void *, so the parameter stays one here. */
extern "C" void Mg_CallStartRound(dScMgBase_c *scene, void *state)
{
    scene->StartRound((int)state);
}

}

/* The constructor and destructor are the end of the unit, and the one place
   the file is not in ROM order. Under defer_codegen off the out-of-line
   destructor comes out D1, D0, D2; the cartridge has D2, D0, D1 and then C2.
   With deferred codegen mwccarm emits a run of definitions in reverse, so
   writing the constructor first and the destructor after it, inside this
   bracket, produces D2, D0, D1, C2 as on the cartridge. */
#pragma push
#pragma defer_codegen on

namespace s20b2adc {
extern "C" {
extern short sMgSceneKindByGame[];
}

}

dScMgBase_c::dScMgBase_c() : mRunDuringFade(0), unk_0c2(1) {
    using s20b2adc::sMgSceneKindByGame;
    mMenuOpen = 0;
    mMenuScreenSaved = 0;
    mMenuDisabled = 0;
    mQuitFadeStarted = 0;

    gMgScene = this;
    mLevel = gMgMenuRow.level; /* dScMiniGm_c::mStartLevel */

    mSceneKind = sMgSceneKindByGame[(param1 >> 16) & 0xff];
    /* param1 &= 0xffff, stored through an s32 lvalue. Spelled as a plain
       u32 `param1 &= 0xffff` (or `param1 = param1 & 0xffff`), mwccarm forms
       the address this + 8 in ip first and the constructor grows by 4
       bytes; `param1 = (u16)param1` shrinks it by 4. */
    *(s32 *)&param1 = param1 & 0xffff;
    mTallyShown = 0;
    mTallyAnimTimer = 0;

    if (actorID == 0x16e || actorID == 0x185 ||
        actorID == 0x16d || actorID == 0x182)
        mTimeLimit = 0x3c;
    else
        mTimeLimit = 0x78;

    mRecordRevealTimer = 0;
    mFrameCounter = 0;
    mSubmittedScore = 0;
    mSessionBest = 0;
    mRankedScore = 0;
    mLastRecordRank = -1;

    Mg_SetTopRecord(func_02013580(Mg_GetSaveTable(this), 0));
}

/* The destructor clears the shared minigame pointer; CodeWarrior supplies the
   vptr restores and the dScene_c teardown. This one out-of-line definition
   emits all three variants, D2 at 0x020b29c0, D0 at 0x020b2a18 and D1 at
   0x020b2a84, in that order inside the deferred bracket. As the first
   declared virtual it is the key function, so this file also emits the
   dScMgBase_c vtable and RTTI. */
dScMgBase_c::~dScMgBase_c()
{
    gMgScene = 0;
}

#pragma pop
