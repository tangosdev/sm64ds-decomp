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
 * padding that this file reaches by offset (+0xb0; the menu pick delay
 * and save buffers go through the file-local MgMenuPickView and
 * MgMenuSaveArea); the func_ov004 and data_ov004 helpers are unnamed in
 * symbols.txt.
 */

#include "dScMgBase_c.h"
#include "MgMenuRow.h"
#include "dWipe_c.h"
#include "decl_common.h"
#include "Sound.h"
#include "types.h"
#include "dEnemyBase_c.h"
#include "PlayerInput.h"
#include "CP15.h"
#include "G2.h"
#include "GX.h"
#include "Memory.h"
#include "cstd.h"
#include "math/Approach.h"

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

extern "C" {
/* Static minigame base state: the graphCallback_c registry instance, the
   MgFx record pool, and the two PMF state tables the dispatchers call
   through. Declaration order is the ROM's __sinit order. */
dScMgBase_c::graphCallback_c sMgGraphCallback;
MgFx sMgFxPool[0x40];
EntPMF sMgFxUpdateTable[2] = {&MgFx::UpdateDrift, &MgFx::UpdateFlyTo};
EntPMF sMgFxDrawTable[2] = {&MgFx::DrawSparkle, &MgFx::DrawFlyTo};
int sMgSparkleSprites[16];
}

/* Local view of the installed screen fader at data_0209f5bc (a dFader_c,
 * see dMgState_c's view): slot 5 is IsAtStart, true while no fade runs.
 * This file calls it IsActive and uses it to gate input. */
struct SceneVCall6 {
    virtual int v0(); virtual int v1(); virtual int v2();
    virtual int v3(); virtual int v4(); virtual int IsActive();
};


extern "C" {
extern void *sMgWorkBuffer;
extern void **data_0209d4a8;
void Mg_UpdateMenu(char *c);
void Banner_Update(char *c);
void Mg_UpdateFx(char *c);
extern SceneVCall6 *data_0209f5bc;
extern unsigned short data_020a0e5a[];
extern int data_0208ee44;
extern void Mg_CommitRecords(void);
extern void FreeGfxSlotsById(int arg);
extern void func_0203cbc0(void *ptr);
extern void func_02012e1c(void);
void Mg_InitMusic();
extern void Enable3dEngines(void);
extern MgEntry gMgMenuRow;
extern dWipe_c data_0209f61c;
extern unsigned char data_0209d460[];
extern unsigned char data_0209d458[];

/* The scene's five saved records: func_02013580(table, i) reads record i of
   save table `table`, func_02013568(table, i, v) writes it, and
   Mg_GetSaveTable(scene) picks the table from byte 1 of param1. */
int func_02013580(int table, int index);
void func_02013568(int table, int index, int value);
int Mg_GetSaveTable(void *scene);

/* One table of sprite-list words per language, indexed by
   GetGameLanguage(); the helpers below hand its words to the OAM
   wrappers. */
extern int *sMgSpriteTables[];
}

namespace s20ad660 {
extern "C" {
// @symbol _ZN11dScMgBase_c12IsAltVariantEv
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
}
}

int dScMgBase_c::IsAltVariant()
{
    return (param1 & 0xff) != 0;
}

// @symbol GetGameLanguage
namespace s20ad674 {
extern "C" {
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
}

// @symbol Mg_GetLastRecordRank
namespace s20ad6f4 {
extern "C" {
/* Returns mLastRecordRank: the index (0..4) at which Mg_RankSessionBest last
   inserted a score into the five saved records, or -1 if it placed nowhere
   (the constructor also seeds -1). */
int Mg_GetLastRecordRank(dScMgBase_c *scene)
{
    return scene->mLastRecordRank;
}
}
}

// @symbol Mg_FindRecordRank
namespace s20ad6fc {
extern "C" {
extern MgEntry gMgMenuRow;
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
}

// @symbol Mg_SaveRecordPair
namespace s20ad79c {
extern "C" {
extern MgEntry gMgMenuRow;
extern int gMgSaveDirty[];

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
    *(unsigned char*)gMgSaveDirty = 1;
}
}
}

// @symbol Mg_GetSavedRecord1
namespace s20ad878 {
extern "C" {
/* Saved record 1 of the current scene, or 0 with no scene. */
int Mg_GetSavedRecord1(void) {
    void *scene = gMgScene;
    if (scene == 0) return 0;
    return func_02013580(Mg_GetSaveTable(scene), 1);
}
}
}

// @symbol Mg_GetSavedRecord0
namespace s20ad8b8 {
extern "C" {
extern MgEntry gMgMenuRow;
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
}

// @symbol Mg_CommitRecords
namespace s20ad90c {
extern "C" {
extern void Mg_RankSessionBest(dScMgBase_c *scene);

void Mg_CommitRecords(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0) return;
    Mg_RankSessionBest(scene);
}
}
}

// @symbol Mg_RankSessionBest
namespace s20ad940 {
extern "C" {
int Mg_GetSessionBest(void);
int Mg_FindRecordRank(void *scene, int value);
void Mg_ClearNewRecordFlag(void);
void Mg_SetSessionBest(int score);
int Mg_GetSubmittedScore(void);
extern unsigned char gMgSaveDirty[];

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
    gMgSaveDirty[0] = 1;
    Mg_ClearNewRecordFlag();
    scene->mRankedScore = Mg_GetSessionBest();
    Mg_SetSessionBest(0);
}
}
}

// @symbol Mg_ClearNewRecordFlag
namespace s20ada20 {
extern "C" {
/* Clears the new-record flag mNewRecord. */
void Mg_ClearNewRecordFlag(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        scene->mNewRecord = 0;
    }
}
}
}

// @symbol Mg_SetSessionBest
namespace s20ada40 {
extern "C" {
extern MgEntry gMgMenuRow;
extern int Mg_GetTopRecord(void);

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
}

// @symbol Mg_GetSessionBest
namespace s20adafc {
extern "C" {
/* The session best, mSessionBest; 0 with no scene. */
int Mg_GetSessionBest(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mSessionBest;
    }
    return 0;
}
}
}

// @symbol Mg_SubmitScore
namespace s20adb1c {
extern "C" {
extern int Mg_GetSessionBest(void);
extern int Mg_SetSessionBest(int);
extern MgEntry gMgMenuRow;
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
}

// @symbol Mg_GetSubmittedScore
namespace s20adbc0 {
extern "C" {
/* The submitted score, mSubmittedScore; 0 with no scene. */
int Mg_GetSubmittedScore(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mSubmittedScore;
    }
    return 0;
}
}
}

// @symbol Mg_IsNewRecord
namespace s20adbe0 {
extern "C" {
/* The new-record flag, mNewRecord; 0 with no scene. */
int Mg_IsNewRecord(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mNewRecord;
    }
    return 0;
}
}
}

// @symbol Mg_SetTopRecord
namespace s20adc00 {
extern "C" {
/* Sets the saved top record (mTopRecord); no-op with no scene. */
void Mg_SetTopRecord(int v) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        scene->mTopRecord = v;
    }
}
}
}

// @symbol Mg_GetTopRecord
namespace s20adc1c {
extern "C" {
/* The saved top record (mTopRecord); 0 with no scene. */
int Mg_GetTopRecord(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) {
        return scene->mTopRecord;
    }
    return 0;
}
}
}

// @symbol Mg_GetSaveTable
namespace s20adc3c {
extern "C" {
/* The scene's save-table id: byte 1 of param1. */
int Mg_GetSaveTable(void *scene) {
    return (((dScMgBase_c *)scene)->param1 & 0xff00) >> 8;
}
}
}

// @symbol Mg_GetWorkBuffer
namespace s20adc4c {
extern "C" {
extern int sMgWorkBuffer[];
int Mg_GetWorkBuffer(void) { return sMgWorkBuffer[0]; }
}
}

// @symbol Ov004_Deallocate
namespace s20adc5c {
extern "C" {
void Ov004_Deallocate(void *ptr)
{
    Deallocate(ptr);
}
}
}

// @symbol Ov004_LoadFile
namespace s20adc68 {
extern "C" {
extern void LoadFile(void);
void Ov004_LoadFile(void) { LoadFile(); }
}
}

// @symbol Ov004_LoadFileByPath
namespace s20adc74 {
extern "C" {
extern void *func_020182bc(const char *path);

void *Ov004_LoadFileByPath(const char *path)
{
    return func_020182bc(path);
}
}
}

// @symbol _ZN4MgFx9DrawFlyToEv
namespace s20adc80 {
extern "C" {
extern int GetGameLanguage(void);
extern int Mg_RenderSpriteOnNoMtx(int,int,int,long long);
}
}
/* State 1's draw: word 2 of the current language's entry in
   sMgSpriteTables, at mPos >> 12. */
void MgFx::DrawFlyTo() {
    int idx=s20adc80::GetGameLanguage();
    int *t=sMgSpriteTables[idx];
    s20adc80::Mg_RenderSpriteOnNoMtx(t[2], mPos[0] >> 0xc, mPos[1] >> 0xc, -1);
}

// @symbol _ZN4MgFx11DrawSparkleEv
namespace s20adcc8 {
extern "C" {
extern void Mg_RenderSpriteOnNoMtx(int a0,int a1,int a2,int a3,int a4);
extern int sMgSparkleSpritesGuard;
extern int data_ov004_020bea14[];
extern int sMgSparkleSprites[];
}
}
/* State 0's draw. The first call builds the 16-entry frame table
   sMgSparkleSprites from five words at data_ov004_020bea14 (bit 0 of
   sMgSparkleSpritesGuard marks it built); the frame is picked by the low four
   bits of mTimer. */
void MgFx::DrawSparkle() {
    if ((s20adcc8::sMgSparkleSpritesGuard & 1) == 0) {
        int *s = s20adcc8::data_ov004_020bea14;
        int *d = s20adcc8::sMgSparkleSprites;
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

// @symbol Mg_DrawFx
namespace s20add88 {
extern "C" {
extern EntPMF sMgFxDrawTable[];
/* Draws one record through its state's handler. */
extern "C" void Mg_DrawFx(MgFx *c) {
    (c->*sMgFxDrawTable[c->mState])();
}
}
}

// @symbol _ZN4MgFx11UpdateFlyToEv
namespace s20addcc {
extern "C" {
void Mg_GainCredits(void *c);
inline long long inline_fn(int arg0)
{
  return (long long) arg0;
}
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
            Mg_GainCredits((void *)1);
        }
        int a = mTimer;
        int b = mDuration;
        int s = cstd::fdiv(a << 0xc, b << 0xc);
        int c3 = mDuration;
        int c2 = mTimer;
        int t = cstd::fdiv((c3 - c2) << 0xc, c3 << 0xc);
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

// @symbol _ZN4MgFx11UpdateDriftEv
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

// @symbol Mg_UpdateFx
namespace s20adf2c {
extern "C" {
extern EntPMF sMgFxUpdateTable[];
/* Updates one record through its state's handler. */
extern "C" void Mg_UpdateFx(MgFx *c) {
    (c->*sMgFxUpdateTable[c->mState])();
}
}
}

// @symbol Mg_InitFx
namespace s20adf70 {
extern "C" {
/* Fills record `c`. mDuration starts equal to mTimer; either vector may
   be omitted with a null pointer. */
void Mg_InitFx(char *c, short state, short timer, int *pos, int *vecA, int *vecB){
    MgFx *e = (MgFx *)c;
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
}

// @symbol Mg_SpawnFx
namespace s20adfc4 {
extern "C" {
extern void Mg_InitFx(void *rec, char *state, short timer, short pos, int *vecA, int *vecB);
/* Fills the first free record of the 0x40 at sMgFxPool (see
   Mg_InitFx); does nothing without a position or a free record.
   The parameter types are not the callers': `state` arrives as an int and
   `pos` is the position pointer, tested whole (movs r4, r2). They are left
   as they are because retyping them adds new disagreements with the
   callers' differing declarations in the declaration-agreement gate. */
void Mg_SpawnFx(char *state, short timer, short pos, int *vecA, int *vecB, int *unused){
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
}

namespace s20ae03c {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback3Ev
/* dScMgBase_c::graphCallback_c::GraphCallback3 -- slot 3: calls the
   registered scene's OnGraphCallback3 (slot 25, +0x64) and reports handled. */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback3()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        p->OnGraphCallback3();
    }

    return 1;
}

namespace s20ae06c {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback2Ev
/* dScMgBase_c::graphCallback_c::GraphCallback2 -- slot 2.
 *
 * Unlike slots 0, 1 and 3, this one RETURNS the scene's OnGraphCallback2 (slot 24,
 * +0x60) result rather than discarding it and reporting handled; it only
 * reports handled when no scene is registered.
 */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback2()
{
    dScMgBase_c *p = mScene;

    if (p == 0) {
        return 1;
    }

    return p->OnGraphCallback2();
}

namespace s20ae0a4 {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback1Ev
/* dScMgBase_c::graphCallback_c::GraphCallback1 -- slot 1: calls the
   registered scene's OnGraphCallback1 (slot 22, +0x58) and reports handled. */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback1()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        p->OnGraphCallback1();
    }

    return 1;
}

namespace s20ae0d4 {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback0Ev
/* dScMgBase_c::graphCallback_c::GraphCallback0 -- slot 0: calls the
   registered scene's OnGraphCallback0 (slot 23, +0x5c) and reports handled. */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback0()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        p->OnGraphCallback0();
    }

    return 1;
}

// @symbol _ZN11dScMgBase_c15graphCallback_cC1Ev
/* graphCallback_c ctor -- the base callback_c vptr store, the derived vptr
   store, and mScene = 0 are the three writes the ROM body makes. */
dScMgBase_c::graphCallback_c::graphCallback_c() : mScene(0) {}

namespace s20ae128 {
extern "C" {
// @symbol _ZN11dScMgBase_c16OnGraphCallback3Ev
/* dScMgBase_c::OnGraphCallback3 - slot 25: nonzero while the menu is closed. */
}
}

int dScMgBase_c::OnGraphCallback3()
{
    return mMenuOpen == 0;
}

namespace s20ae140 {
extern "C" {
// @symbol _ZN11dScMgBase_c16OnGraphCallback2Ev
/* dScMgBase_c::OnGraphCallback2 - slot 24. When mMenuOpen differs from its latch
   mMenuScreenSaved, calls SaveScreenForMenu (slot 29) if the menu has opened or
   RestoreScreenFromMenu (slot 30) if it has closed, then latches it. */
}
}

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

namespace s20ae198 {
extern "C" {
// @symbol _ZN11dScMgBase_c16OnGraphCallback1Ev
/* dScMgBase_c::OnGraphCallback1 - slot 22 */
}
}

int dScMgBase_c::OnGraphCallback1()
{
    return 1;
}

namespace s20ae1a0 {
extern "C" {
// @symbol _ZN11dScMgBase_c16OnGraphCallback0Ev
/* dScMgBase_c::OnGraphCallback0 - slot 23 */
}
}

int dScMgBase_c::OnGraphCallback0()
{
    return 1;
}

// @symbol Mg_GetScoringMode
namespace s20ae1a8 {
extern "C" {
extern MgEntry gMgMenuRow;
/* The menu row's scoring mode, or -1 with no scene. */
int Mg_GetScoringMode(void) {
    if (gMgScene != 0) {
        return gMgMenuRow.scoringMode;
    }
    return -1;
}
}
}

// @symbol Mg_GetMusicMode
namespace s20ae1cc {
extern "C" {
extern MgEntry gMgMenuRow;

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
}

// @symbol Mg_GetMusicTrack
namespace s20ae1f0 {
extern "C" {
/* The scene's music track, mMusicTrack; 5 with no scene. */
int Mg_GetMusicTrack(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) return scene->mMusicTrack;
    return 5;
}
}
}

// @symbol Mg_RestoreMusic
namespace s20ae20c {
extern "C" {
extern void d_s_mg_basePlaySubHold(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern int Mg_GetMusicMode(void);
extern int Mg_GetMusicTrack(void);
/* Sound::PlaySub with third argument 0 and its flag set starts nothing:
   it only sets the music volume target (data_0208e42c, which
   ChangeMusicVolume and PauseMusic also write) back to 0x7f, with step
   0x7f000, and zeroes data_0209b470. Then, while a scene exists and in
   music mode 1, this loads the scene's track. */
void Mg_RestoreMusic(void)
{
    Sound::PlaySub(0, 0x7f, 0, 0x7f000, 1);
    if (gMgScene == 0) return;
    if (Mg_GetMusicMode() != 1) return;
    Sound::LoadAndSetMusic_Layer1(Mg_GetMusicTrack());
}
}
}

// @symbol Mg_PlayJingle
namespace s20ae274 {
extern "C" {
extern int Mg_GetMusicMode(void);

/* While a scene exists, starts sequence `id` on Sound::PlaySub's own
   player (data_0209b4b0) unless it is already the current one, and sets
   the music volume target to 0x40 [GUESS: ducks the music; PauseMusic uses
   0x40 too]. The Mg_GetMusicMode result is not used. */
void Mg_PlayJingle(void* id) {
    if (gMgScene == 0) return;
    Mg_GetMusicMode();
    Sound::PlaySub((unsigned int)id, 0x40, 0x7f, 0x7f000, 1);
}
}
}

// @symbol Mg_StartMusic
namespace s20ae2c8 {
extern "C" {
int Mg_GetMusicMode(void);
int Mg_GetMusicTrack(void);
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
}

// @symbol Mg_InitMusic
namespace s20ae330 {
extern "C" {
extern int Mg_GetMusicMode(void);
extern int Mg_GetMusicTrack(void);
extern MgEntry gMgMenuRow;

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
}

namespace s20ae3b4 {
extern "C" {
// @symbol _ZN11dScMgBase_c11PaintSquareEiiii
/* dScMgBase_c::PaintSquare - slot 34.  THE BRUSH.

   Walks a `size` x `size` square centred on (cx, cy) and paints every cell
   inside it with palette index `colour`.  The address arithmetic is the DS
   4bpp background-character layout: `(x/8 + (y/8)*32)*32 + (y&7)*4` picks the
   word holding eight pixels of one row of one 8x8 tile, `(x&7)*4` is the
   nibble inside it, and MultiCopy_Int is used for the read and the write-back
   rather than a plain load/store.  Sixteen colours, one nibble per pixel.  All
   four sides clip.

   It reads exactly the two object fields slot 33 (InitEngines) initialises.
   mBrushLayer is the layer index: 0..3 select G2S::GetBG0CharPtr through
   GetBG3CharPtr, and anything else returns without drawing -- which is what
   slot 33's -1 buys.  mBrushOnMain gates the wrapped region above the touch
   screen, where the MAIN engine's G2::GetBG*CharPtr are used instead and y is
   folded by `+ sMgTopScreenYOffset + 0xc0`.  So slot 33 leaves the brush
   disabled and a minigame arms it by choosing a layer.

   Its only in-family caller is ov004:0x020ae5c4, the line rasteriser sitting
   immediately after it in the image, which dispatches through +0x88 at seven
   separate sites as it walks a segment.  See the slot-34 block in
   include/dScMgBase_c.h for the arity and return-type measurements.

   `PaintSquare` is coined here from what the body does; it was `Virtual88`,
   after the +0x88 vtable offset. The cartridge names no method of this
   class. */
extern "C" {
extern int* func_02054efc(void);
extern int* func_02054ea8(void);
extern int* func_02054d88(void);
extern void MultiCopy_Int(void* a, void* b, int n);
}

extern "C" {
extern int sMgPaintWord;
extern int sMgTopScreenYOffset;
}
}
}

void dScMgBase_c::PaintSquare(int cx, int cy, int colour, int size) {
    using s20ae3b4::MultiCopy_Int;
    using s20ae3b4::sMgPaintWord;
    using s20ae3b4::sMgTopScreenYOffset;
    using s20ae3b4::func_02054d88;
    using s20ae3b4::func_02054ea8;
    using s20ae3b4::func_02054efc;

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
                    case 0: base = func_02054efc(); break;
                    case 1: base = func_02054ea8(); break;
                    case 2:
                        base = (int *)G2::GetBG2CharPtr();
                        break;
                    case 3: base = func_02054d88(); break;
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
                        base = (int *)G2S::GetBG0CharPtr();
                        break;
                    case 1:
                        base = (int *)G2S::GetBG1CharPtr();
                        break;
                    case 2:
                        base = (int *)G2S::GetBG2CharPtr();
                        break;
                    case 3:
                        base = (int *)G2S::GetBG3CharPtr();
                        break;
                    default: return;
                    }
                }
                cell = (int*)((char*)base + ((xx / 8 + (yy / 8) * 32) * 32) + ((yy & 7) * 4));
                MultiCopy_Int(cell, &sMgPaintWord, four);
                sh = (xx & 7) << 2;
                sMgPaintWord = (sMgPaintWord & (-1 ^ (m << sh))) | (colour << sh);
                MultiCopy_Int(&sMgPaintWord, cell, four);
            inc:;
            } while (++ci < size);
        }
        yrow += 1;
        oi += 1;
    } while (oi < size);
}

// @symbol _ZN11dScMgBase_c11Mg_DrawLineEiiiiii
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

#pragma push
#pragma opt_common_subs off
// @symbol Mg_DrawMenu
namespace s20ae858 {
extern "C" {
extern "C" {
int GetGameLanguage(void);
void Mg_DrawSpriteList(void *self, int a1, int a2, int a3);
}

extern int *sMgMenuItem0SpritesByLanguage[];
extern int *sMgMenuItem1SpritesByLanguage[];
extern int *sMgMenuItem2SpritesByLanguage[];
extern s16 data_02082214[];
extern int data_ov006_021346bc;

/* Draws the three-item overlay menu: each item's per-language sprite list
   at its mMenuItemNX/Y, followed by data_ov006_021346bc at the same spot.
   The highlighted item (mMenuCursor) goes through a matrix whose diagonal
   is func_02053200 of a pulse that mMenuCursorPhase drives through the
   sin/cos table data_02082214. Nothing is drawn while
   mTouchOptions.Draw() returns nonzero. */
extern "C" void Mg_DrawMenu(dScMgBase_c *scene)
{
    int extra[3];
    s16 *tbl[3];
    int buf[4];
    int zero1;
    int zero2;
    int zero0;
    int i;

    if (scene->mTouchOptions.Draw() != 0)
        return;

    extra[0] = *sMgMenuItem0SpritesByLanguage[GetGameLanguage()];
    extra[1] = *sMgMenuItem1SpritesByLanguage[GetGameLanguage()];
    extra[2] = *sMgMenuItem2SpritesByLanguage[GetGameLanguage()];

    tbl[0] = &scene->mMenuItem0X;
    tbl[1] = &scene->mMenuItem1X;
    tbl[2] = &scene->mMenuItem2X;

    i = 0;
    zero1 = 0;
    zero2 = 0;
    zero0 = 0;

    for (; i < 3; i++) {
        if (i == scene->mMenuCursor) {
            unsigned int field = (unsigned int)scene->mMenuCursorPhase;
            unsigned int shifted = (field << 0xf) >> 0x10;
            int idx0 = (int)shifted >> 4;
            int s0 = data_02082214[idx0 * 2];
            char *cursor = (char *)extra[i];
            int arg0;
            int result;
            s16 *pos;
            u16 v;
            int *pbuf = buf;

            arg0 = (int)((((s64)s0 << 11) + 0x800) >> 12) + 0x1000;
            result = func_02053200(arg0);

            {
                volatile int *vp = pbuf;
                vp[0] = 0;
                vp[1] = 0;
                vp[2] = 0;
                vp[3] = 0;
            }
            buf[0] = result;
            buf[3] = result;

            pos = tbl[i];
            do {
                Mg_DrawSpriteListEntry((void *)cursor, (void *)(int)pos[0], (void *)(int)pos[1], (void *)pbuf);
                v = *(u16 *)(cursor + 6);
                cursor += 8;
            } while (v != 0xffff);

            pos = tbl[i];
            Mg_DrawSpriteList((void *)data_ov006_021346bc, pos[0], pos[1], zero0);
        } else {
            s16 *pos = tbl[i];
            char *ep = (char *)extra[i];
            Mg_DrawSpriteList((void *)ep, pos[0], pos[1], zero1);
            Mg_DrawSpriteList((void *)data_ov006_021346bc, pos[0], pos[1], zero2);
        }
    }
}
}
}
#pragma pop

// @symbol Mg_DrawSpriteList
namespace s20aea30 {
extern "C" {
/* Draws an OAM sprite list: calls Mg_DrawSpriteListEntry on each 8-byte
   entry from `self`, up to and including the one whose attr3 is 0xffff. */
extern "C" void Mg_DrawSpriteList(void *self, void *actor, void *c2, void *c3)
{
    void *r8 = self;
    void *r7 = actor;
    void *r6 = c2;
    void *r5 = c3;
    u16 sentinel = 0xffff;
    do {
        Mg_DrawSpriteListEntry(r8, r7, r6, r5);
        u16 v = *(u16*)((char*)r8 + 6);
        r8 = (char*)r8 + 8;
        if (v == sentinel) break;
    } while (1);
}
}
}

// @symbol Mg_DrawSpriteListEntry
namespace s20aea78 {
extern "C" {
struct M { int _00, _01, _10, _11; };
extern "C" {
extern void Mg_RenderSpriteOnMtx(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
}

struct BF { unsigned int lo:10; unsigned int hi:22; };
struct S { int x; BF y; };

/* Draws one sprite-list entry at (a1, a2) through Mg_RenderSpriteOnMtx.
   When the scene's GetDisplayMode (slot 26) returns 2, the copy
   drawn has 0x200 subtracted from its tile number (attr2 bits 0..9). */
extern "C" void Mg_DrawSpriteListEntry(S* self, int a1, int a2, struct M* a3)
{
    S v = *self;
    if (gMgScene->GetDisplayMode() == 2) {
        v.y.lo = (self->y.lo - 0x200) & 0x3ff;
    }
    Mg_RenderSpriteOnMtx(&v, a1, a2, 0, -1, a3);
}
}
}

/* The header's pad_4644 as Mg_UpdateMenu uses it: the pick delay a touched
   item counts down (ApproachLinear2) before the pick acts. A file-local
   view, like MgMenuSaveArea below; this function is its only user. */
struct MgMenuPickView {
    u8 pad_000[0x4644]; /* 0x000 -- the dScMgBase_c members before it */
    s16 pickDelay;      /* 0x4644 */
};

// @symbol Mg_UpdateMenu
namespace s20aeb24 {
extern "C" {
extern "C" {
extern void func_0203b958(short* o, short* a, short* b);
extern void func_02012dd0(int a);

extern unsigned short data_020a0e5a[];
extern dWipe_c data_0209f61c;

/* The overlay menu's frame while it is open: slides the items in and, on
   a new touch with nothing highlighted, highlights the touched item in
   mMenuCursor (within 0x60 x 0x18 of item 0, 0x40 x 0x13 of the others, by
   func_0203b958's offsets) and sets the pick delay (0x10 for
   item 0, else 0). Once mMenuCursorPhase reaches 0x10000 and that timer
   reaches 0, item 0 (and any other value) calls CloseMenu
   (slot 28), item 1 starts a scene fade, and item 2 calls
   mTouchOptions.Open() and clears the highlight. */
void Mg_UpdateMenu(char* c)
{
    dScMgBase_c *self = (dScMgBase_c *)c;
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
    if (self->mTouchOptions.Update() != 0) return;

    ApproachLinear2(self->mMenuItem0X, 0x80, 0x10);
    ApproachLinear2(self->mMenuItem1X, 0x80, 0x10);
    ApproachLinear2(self->mMenuItem2Y, 0x90, 5);

    idx = gActivePlayerSlot;
    unlocked = 0;
    if (gTouchHeld[idx * 4] != 0) {
        unlocked = (gTouchEdge[idx * 4] != 0);
    }
    if (unlocked == 0) goto after;
    if (self->mMenuCursor >= 0) goto after;
    tmp[0] = gTouchX[idx * 4];
    tmp[1] = gTouchY[idx * 4];
    func_0203b958(d0, tmp, &self->mMenuItem0X);
    func_0203b958(d1, tmp, &self->mMenuItem1X);
    func_0203b958(d2, tmp, &self->mMenuItem2X);
    a = d0[0]; if (a < 0) a = -a;
    if (a < 0x60) {
        b = d0[1]; if (b < 0) b = -b;
        if (b < 0x18) {
            func_02012790(0x62);
            self->mMenuCursor = 0;
            /* 0x10 for item 0 and 0 for the others */
            ((MgMenuPickView *)c)->pickDelay = 0x10;
            goto after;
        }
    }
    a = d1[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d1[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x63);
            self->mMenuCursor = 1;
            ((MgMenuPickView *)c)->pickDelay = 0;
            goto after;
        }
    }
    a = d2[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d2[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x62);
            self->mMenuCursor = 2;
            ((MgMenuPickView *)c)->pickDelay = 0;
        }
    }

after:
    if (self->mMenuCursor < 0) return;
    if (ApproachLinear(self->mMenuCursorPhase, 0x10000, 0x1000) == 0) return;
    if (ApproachLinear2(((MgMenuPickView *)c)->pickDelay, 0, 1) == 0)
        return;

    switch (self->mMenuCursor) {
    case 0:
        self->CloseMenu();
        return;
    case 1:
        dScene_c::SetFaders(&data_0209f61c);
        data_0209f61c.func_0202ec9c(1);
        dScene_c::StartSceneFade(5, 0, 0);
        if (self->mQuitFadeStarted != 0) return;
        func_02012dd0(0x3c);
        self->mQuitFadeStarted = 1;
        return;
    case 2:
        self->mTouchOptions.Open();
        self->mMenuCursor = -1;
        self->mMenuCursorPhase = 0;
        return;
    default:
        self->CloseMenu();
        return;
    }
}
}
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
// @symbol _ZN11dScMgBase_c21RestoreScreenFromMenuEv
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
extern "C" u8 data_0209d45c;
extern "C" u8 data_0209d454;
extern "C" u8 data_0209d460;
extern "C" u8 data_0209d458;

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
    using s20aeed8::data_0209d454;
    using s20aeed8::data_0209d458;
    using s20aeed8::data_0209d45c;
    using s20aeed8::data_0209d460;
    MgMenuSaveArea *save = (MgMenuSaveArea *)this;

    data_0209d45c = 0;
    data_0209d454 = 0;

    *(u32*)0x4000000 &= ~0x1f00;
    *(u32*)0x4001000 &= ~0x1f00;
    *(u16*)0x4000304 = (mSavedScreenSwap << 15) | (*(u16*)0x4000304 & ~0x8000);
    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0xe000) | ((u32)data_0209d460 << 13);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0xe000) | ((u32)data_0209d458 << 13);

    GX::LoadBGPltt(save->mainBgPltt, 0x20, 0x1e0);
    {
        /* stepped from mainBgPltt: save->subBgPltt moves the literal pool */
        char *p = (char *)save->mainBgPltt;
        p += 0x200;
        GXS::LoadBGPltt(p, 0, 0x80);
    }

    if (GetDisplayMode() != 2)
    {
        char *src = (char*)0x6600000; src += 0x6000;
        MultiCopy_Int((int *)save->subObjChars, (int *)src, 0x2000);
    }

    data_0209d45c = (u8)mSavedMainBgBits;
    data_0209d454 = (u8)mSavedSubBgBits;

    if (GetDisplayMode() == 2)
        return;

    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0x1f00) | ((u32)data_0209d45c << 8);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0x1f00) | ((u32)data_0209d454 << 8);
}

namespace s20af04c {
extern "C" {
// @symbol _ZN11dScMgBase_c9CloseMenuEv
/* dScMgBase_c::CloseMenu - slot 28.

   A hit from underneath closes the three-item overlay menu
   that OpenMenu opens, resets the polymorphic touch-icon set, and
   re-enables the 3D engines only if the class's own GetDisplayMode
   says so. */

extern "C" void func_02012e1c(char *c);
extern "C" void Enable3dEngines();

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
{ using s20af04c::Enable3dEngines; using s20af04c::func_02012e1c;
    func_02012e1c((char *)this);
    mMenuOpen = 0;
    mTouchOptions.Close();
    int r = GetDisplayMode();
    if (r != 0) Enable3dEngines();
}

namespace s20af094 {
extern "C" {
// @symbol _ZN11dScMgBase_c17SaveScreenForMenuEv
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
extern "C" void MultiStore16(unsigned short val, char *dst, int nbytes);
extern "C" void func_0201f32c(int arg0);
extern "C" int GetGameLanguage(void);
extern "C" void DecompressLZ16(void *src, void *dst);

extern "C" unsigned char data_0209d45c;
extern "C" unsigned char data_0209d454;

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

int dScMgBase_c::SaveScreenForMenu() {
    using s20af094::DecompressLZ16;
    using s20af094::GetGameLanguage;
    using s20af094::MultiStore16;
    using s20af094::data_0209d454;
    using s20af094::data_0209d45c;
    using s20af094::func_0201f32c;
    MgMenuSaveArea *save = (MgMenuSaveArea *)this;

    if (GetDisplayMode() != 0)
        func_02019028();

    mSavedScreenSwap = (*(volatile u16 *)0x4000304 & 0x8000) >> 15;

    mSavedMainBgBits = data_0209d45c;
    mSavedSubBgBits = data_0209d454;

    *(volatile u16 *)0x4000304 |= 0x8000;
    data_0209d45c = 0;
    data_0209d454 = 0;

    *(volatile int *)0x4000000 &= ~0x1f00;
    *(volatile int *)0x4001000 &= ~0x1f00;
    *(volatile int *)0x4000000 &= ~0xe000;
    *(volatile int *)0x4001000 &= ~0xe000;

    MultiCopy_Int((int *)0x5000020, (int *)save->mainBgPltt, 0x1e0);
    {
        /* stepped from mainBgPltt: save->subBgPltt moves the literal pool */
        int *pltt = (int *)save->mainBgPltt;
        MultiCopy_Int((int *)0x5000400, pltt + 0x80, 0x80);
    }
    CP15::FlushAndInvalidateDataCache((u32)save->mainBgPltt, 0x400);

    GX::LoadBGPltt((const void *)sMgMenuBgPltt, 0x20, 0xa0);
    GXS::LoadBGPltt((const void *)sMgMenuSubBgPltt, 0, 0x80);

    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000000, 2);
    }
    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000400, 2);
    }

    func_0201f32c(mSceneKind);

    data_0209d45c = 0x12;
    data_0209d454 = 0x10;

    if (GetDisplayMode() != 2) {
        int *vbase = (int *)0x6600000;
        int *vram = vbase + 0x1800;
        MultiCopy_Int(vram, (int *)save->subObjChars, 0x2000);
        int idx = GetGameLanguage();
        DecompressLZ16(sMgMenuScreenByLanguage[idx], vram);
        CP15::FlushAndInvalidateDataCache((u32)save->subObjChars, 0x2000);
    }
}

namespace s20af27c {
extern "C" {
// @symbol _ZN11dScMgBase_c8OpenMenuEv
/* dScMgBase_c::OpenMenu - slot 27. Unless mMenuDisabled is set, opens
   the three-item overlay menu: items start at their slide-in positions,
   the prompt is disabled and nothing is highlighted. */
}
}

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

// @symbol Mg_InitBrush
namespace s20af2f8 {
extern "C" {
extern "C" {
extern void MultiStore16(int val, int ptr, int n);
extern int func_02054efc(void);
extern int func_02054ea8(void);

extern char sMgBrushPalette;

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
    int t;

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
                t = (int)G2::GetBG0ScrPtr();
                t = t + off;
                MultiStore16(v0 = val, t, 2);
            }
            t = (int)G2S::GetBG0ScrPtr();
            t = t + off;
            MultiStore16(v1 = val, t, 2);
            break;
        case 1:
            if (scene->mBrushOnMain) {
                t = (int)G2::GetBG1ScrPtr();
                t = t + off;
                MultiStore16(v2 = val, t, 2);
            }
            t = (int)G2S::GetBG1ScrPtr();
            t = t + off;
            MultiStore16(v3 = val, t, 2);
            break;
        case 2:
            if (scene->mBrushOnMain) {
                t = (int)G2::GetBG2ScrPtr();
                t = t + off;
                MultiStore16(v4 = val, t, 2);
            }
            t = (int)G2S::GetBG2ScrPtr();
            t = t + off;
            MultiStore16(v5 = val, t, 2);
            break;
        case 3:
            if (scene->mBrushOnMain) {
                t = (int)G2::GetBG3ScrPtr();
                t = t + off;
                MultiStore16(v6 = val, t, 2);
            }
            t = (int)G2S::GetBG3ScrPtr();
            t = t + off;
            MultiStore16(v7 = val, t, 2);
            break;
        default:
            return;
        }
    }

    switch (scene->mBrushLayer) {
    case 0:
        if (scene->mBrushOnMain) {
            t = func_02054efc();
            w0 = 0;
            MultiStore16(w0, t, 0x6000);
        }
        t = (int)G2S::GetBG0CharPtr();
        w1 = 0;
        MultiStore16(w1, t, 0x6000);
        break;
    case 1:
        if (scene->mBrushOnMain) {
            t = func_02054ea8();
            w2 = 0;
            MultiStore16(w2, t, 0x6000);
        }
        t = (int)G2S::GetBG1CharPtr();
        w3 = 0;
        MultiStore16(w3, t, 0x6000);
        break;
    case 2:
        if (scene->mBrushOnMain) {
            t = (int)G2::GetBG2CharPtr();
            w4 = 0;
            MultiStore16(w4, t, 0x6000);
        }
        t = (int)G2S::GetBG2CharPtr();
        w5 = 0;
        MultiStore16(w5, t, 0x6000);
        break;
    case 3:
        if (scene->mBrushOnMain) {
            t = func_02054d88();
            w6 = 0;
            MultiStore16(w6, t, 0x6000);
        }
        t = (int)G2S::GetBG3CharPtr();
        w7 = 0;
        MultiStore16(w7, t, 0x6000);
        break;
    default:
        return;
    }

    GX::LoadBGPltt(&sMgBrushPalette, base << 5, 0x20);
    GXS::LoadBGPltt(&sMgBrushPalette, base << 5, 0x20);
}
}
}
}

#pragma push
#pragma opt_strength_reduction off
// @symbol Mg_CopySpriteList
namespace s20af5e0 {
extern "C" {
typedef struct Item {
    u32 w0;
    u32 val : 10;
    u32 rest : 22;
} Item;

/* Copies an OAM sprite list (8-byte entries, ending at the one whose
   attr3 is 0xffff) from `src` to `dst`, adding `add` to each tile number
   (the low ten bits of attr2). At most 0x20 entries: the 32nd is given the
   0xffff terminator. Returns the number of entries written. */
int Mg_CopySpriteList(Item* src, Item* dst, int add)
{
    int n = 1;
    int i = 0;
    Item* q = dst;
    do {
        u32 w;
        u32* p;
        dst[i].w0 = src->w0;
        *(u32*)((char*)&dst[i] + 4) = *(u32*)((char*)src + 4);
        w = (add + src->val) & 0x3ff;
        p = (u32*)((char*)q + 4);
        *p = (*p & ~0x3ff) | ((u32)(w) & 0x3ff);
        if (*(u16*)((char*)src + 6) == 0xffff)
            break;
        if (i == 0x1f) {
            *(u16*)((char*)&dst[i] + 6) = 0xffff;
            break;
        }
        i++;
        src++;
        n++;
        q++;
    } while (i < 0x20);
    return n;
}
}
}
#pragma pop

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

// @symbol Hud_RenderSprite
namespace s20af68c {
extern "C" {
extern "C" {
void OamRender10_d_s_mg_base_a(int, OamAttr *, s32, s32, int, int, int, int, int, int);
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
}
}

// @symbol Mg_RenderSubSpriteScaled
namespace s20af770 {
extern "C" {
extern "C" {
struct OamAttr;
}
typedef int Fix12i;
extern "C" int OamRender8_d_s_mg_base(int show, struct OamAttr *attr, int a, int b, int c, int d,
                                      Fix12i e, int f);

extern "C" int Mg_RenderSubSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5,
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
}

// @symbol Mg_RenderSubSpritePalMtx
namespace s20af868 {
extern "C" {
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
}

// @symbol Mg_RenderSubSpriteMtx
namespace s20af948 {
extern "C" {
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
}

// @symbol RenderOamMainScreen
namespace s20afa20 {
extern "C" {
extern "C" {
void OamRender10_d_s_mg_base_a(int, OamAttr *, s32, s32, int, int, int, int, int, int);

extern "C" void RenderOamMainScreen(int oam, int x, int y, int palette, int priority) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0) {
        if (scene->GetDisplayMode() == 2) {
            if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 1)
                return;
            OAM::Render(0, (OamAttr *)oam, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
            return;
        }
    }
    OAM::Render(0, (OamAttr *)oam, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
}
}
}
}

// @symbol Mg_RenderMainSpriteScaled
namespace s20afb20 {
extern "C" {
extern "C" {
struct OamAttr;
}
typedef int Fix12i;
extern "C" int OamRender8_d_s_mg_base(int show, struct OamAttr *attr, int a, int b, int c, int d,
                                      Fix12i e, int f);

extern "C" int Mg_RenderMainSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5,
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
}

// @symbol Mg_RenderMainSpritePalMtx
namespace s20afc18 {
extern "C" {
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
}

// @symbol DrawOamSprite
namespace s20afcf8 {
extern "C" {
extern "C" void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3) {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->mMenuOpen == 0 && scene->GetDisplayMode() == 2) {
        if (((D3DBaseView *)gMgScene)->mDrawSubScreen != 1)
            return;
        OAM::Render(false, (OamAttr *)arg0, (int)arg1, arg2, -1, -1, (Matrix2x2 *)arg3);
        return;
    }
    OAM::Render(false, (OamAttr *)arg0, (int)arg1, arg2, -1, -1, (Matrix2x2 *)arg3);
}
}
}

// @symbol Mg_RenderSpriteBoth
namespace s20afdd0 {
extern "C" {
extern "C" {
extern int sMgTopScreenYOffset;
int OamRender10_d_s_mg_base_b(int, void *, int, int, int, int, int, int, int, int);
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
}
}

// @symbol Mg_RenderSpriteBothScaled
namespace s20aff38 {
extern "C" {
extern "C" {
struct OamAttr;
extern int sMgTopScreenYOffset;
}
typedef int Fix12i;
extern "C" int OamRender8_d_s_mg_base(int show, struct OamAttr *attr, int a, int b, int c, int d,
                                      Fix12i e, int f);

extern "C" int Mg_RenderSpriteBothScaled(struct OamAttr *a0, int a1, int a2, int a3, int a4,
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
}

// @symbol RenderOamBothScreens
namespace s20b0104 {
extern "C" {
extern "C" {
extern int sMgTopScreenYOffset;
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
}
}

// @symbol Mg_RenderSpriteBothPalMtx
namespace s20b023c {
extern "C" {
extern "C" {
extern int sMgTopScreenYOffset;
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
}
}

// @symbol Mg_RenderSpriteBothMtx
namespace s20b0380 {
extern "C" {
extern "C" {
extern int sMgTopScreenYOffset;
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
}
}

// @symbol Mg_GetTopScreenYOffset
namespace s20b04c0 {
extern "C" {
extern int sMgTopScreenYOffset[];
/* Get and set sMgTopScreenYOffset: the offset PaintSquare and the sprite
   wrappers above add, with 0xc0, to y on their upper-screen path. */
int Mg_GetTopScreenYOffset(void) { return sMgTopScreenYOffset[0]; }
}
}

// @symbol Mg_SetTopScreenYOffset
namespace s20b04d0 {
extern "C" {
extern int sMgTopScreenYOffset[];
void Mg_SetTopScreenYOffset(int v) { sMgTopScreenYOffset[0] = v; }
}
}

namespace s20b04e0 {
extern "C" {
// @symbol _ZN11dScMgBase_c14GetDisplayModeEv
/* dScMgBase_c::GetDisplayMode - slot 26: which display layout the scene
 * uses. BeforeInitResources turns the 3D engines on for any nonzero answer.
 *
 * `return 0;` here, `return 1;` in dScMgSingle3DBase_c's override and
 * `return 2;` in dScMgD3DBase_c's.  Three distinct constants out of three
 * bodies is what makes the return type int rather than void, and it is why the
 * caller has to be able to tell the three apart. */
}
}

int dScMgBase_c::GetDisplayMode()
{
    return 0;
}

// @symbol _ZN11dScMgBase_c16OnPendingDestroyEv
void dScMgBase_c::OnPendingDestroy()
{
}

// @symbol _ZN11dScMgBase_c6RenderEv
s32 dScMgBase_c::Render()
{
    return 1;
}

/* A view of one of the three 0x134-byte banner records at gMgBanners, with
 * the member names src/minigames/unit020b2c84.cpp gives MgBanner there. */
struct MgBanner {
    char pad_00[0x20];
    int mFileIndex; /* 0x20 index into the file table; 29 when free */
    char pad_24[0xc];
    s16 mGfxSlot; /* 0x30 0..2, see sMgBannerGfxSlotUsed */
    char pad_32[0x102];
};

// @symbol _ZN11dScMgBase_c12BeforeRenderEv
/* Draws the menu instead of the scene while it is open. The two loops draw
 * the three banner records in two passes, those with mGfxSlot clear first. */
int dScMgBase_c::BeforeRender()
{
    int i; char *p; int i2; char *p2; int j;

    if (dScene_c::BeforeRender() == 0)
        return 0;

    if (mMenuOpen != 0) {
        Mg_DrawMenu(this);
        return 0;
    }

    if (mStateController.mSuspended == 0) {
        p = gMgBanners;
        for (i = 0; i < 3; i++, p += 0x134) {
            if (((MgBanner *)p)->mFileIndex == 0x1d)
                continue;
            if (((MgBanner *)p)->mGfxSlot != 0)
                continue;
            Banner_Draw(p);
        }
        p2 = gMgBanners;
        for (i2 = 0; i2 < 3; i2++, p2 += 0x134) {
            if (((MgBanner *)p2)->mFileIndex == 0x1d)
                continue;
            if (((MgBanner *)p2)->mGfxSlot == 0)
                continue;
            Banner_Draw(p2);
        }
        mStateController.Render();
    }

    p = (char *)sMgFxPool;
    for (j = 0; j < 0x40; j++, p += 0x20) {
        if (sMgFxPool[j].mTimer != 0)
            Mg_DrawFx(p);
    }

    Mg_DrawPrompt(this);
    return 1;
}

// @symbol _ZN11dScMgBase_c8BehaviorEv
s32 dScMgBase_c::Behavior()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c14BeforeBehaviorEv
/* Handles input and the menu, runs the UI state controller and the shared
 * records, approaches mCreditsShown toward mCredits, and advances the 40-frame
 * counter. Returns 0 to stop the derived scene's behavior this frame. */
int dScMgBase_c::BeforeBehavior()
{
    int mode;
    unsigned short flags;

    if (dScene_c::BeforeBehavior() == 0)
        return 0;

    if (data_0209f5bc->IsActive()) {
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
        Mg_UpdateMenu((char *)this);
        return 0;
    }
    if (mMenuScreenSaved != 0)
        return 0;

    if (mStateController.mSuspended == 0) {
        int i;
        char *g;
        mStateController.Behavior();
        g = gMgBanners;
        for (i = 0; i < 3; i++) {
            if (((MgBanner *)g)->mFileIndex != 0x1d)
                Banner_Update(g);
            g += 0x134;
        }
    }

    {
        int j;
        char *p = (char *)sMgFxPool;
        MgFx *base = sMgFxPool;
        for (j = 0; j < 0x40; j++) {
            if (base[j].mTimer != 0)
                Mg_UpdateFx(p);
            p += 0x20;
        }
    }

    if (mRunDuringFade == 0) {
        if (data_0209f5bc->IsActive() == 0)
            return 0;
    }

    ApproachLinear(mCreditsShown, mCredits, data_0208ee44);

    mFrameCounter++;
    if (mFrameCounter >= 0x28)
        mFrameCounter = 0;

    return 1;
}

// @symbol _ZN11dScMgBase_c21AfterCleanupResourcesEj
void dScMgBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        if (gMgMenuRow.ownRecords == 0)
            Mg_CommitRecords();
        data_0209d4a8 = 0;
        sMgGraphCallback.mScene = 0;
        FreeGfxSlotsById(0x1d);
        if (sMgWorkBuffer != 0) {
            func_0203cbc0(sMgWorkBuffer);
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

// @symbol _ZN11dScMgBase_c18AfterInitResourcesEj
/* Finishes the display and font setup, then reports to dScene_c. */
void dScMgBase_c::AfterInitResources(u32 vfSuccess)
{
    InitMainScreenBg();
    LoadFont(2);
    Mg_InitMusic();
    dScene_c::AfterInitResources(vfSuccess);
}

// @symbol _ZN11dScMgBase_c19BeforeInitResourcesEv
namespace s20b0930 {
extern "C" {
/* local extern: decl_common.h declares this `(char *)`, but the definition
   in src/minigames/d_mg_state.cpp takes no parameter and only resets the
   sMg*Banner position globals. The `add r0, r4, #0x4000` the cartridge has
   just before the call is the base of the mNewRecord store
   (`strb r1, [r0, #0x65c]`), left in r0, not an argument. */
void Mg_ResetBannerLayout(void);
}
}

/* Sets up the scene and both displays. Slots 26, 31 and 33 must stay
 * virtual calls so derived scenes can override them. */
bool dScMgBase_c::BeforeInitResources()
{
    if (dScene_c::BeforeInitResources() == 0) return 0;
    if (GetDisplayMode() == 0)
        func_02019028();
    else
        Enable3dEngines();
    mMusicTrack = gMgMenuRow.musicTrack;
    if (sMgWorkBuffer == 0)
        sMgWorkBuffer = Memory::operator_new2(0x4000);
    /* +0xb0 is the header's pad_0b0. Scanning ov004 and ov006 turns up no
       other word access at +0xb0 in a scene method or through gMgScene, so
       no role is established and the store stays raw. */
    if (gMgScene != 0)
        *(int*)((char*)gMgScene + 0xb0) = 0;
    mHudScore = 0;
    mBestHudScore = 0;
    mNewRecord = 0;
    s20b0930::Mg_ResetBannerLayout();
    InitEngines();
    Mg_LoadBannerFiles();
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.func_0202ec9c(0);
    data_0209d460[0] = 0;
    data_0209d458[0] = 0;
    InitSubScreenBg();
    return 1;
}

// @symbol Mg_ResumeSharedState
namespace s20b0a38 {
extern "C" {
/* Clears mStateController.mSuspended, so BeforeBehavior and BeforeRender
   step and draw the UI state controller again. */
void Mg_ResumeSharedState(void)
{
    dScMgBase_c *scene = gMgScene;
    if (scene == 0) return;
    scene->mStateController.mSuspended = 0;
}
}
}

// @symbol Mg_SetSharedState
namespace s20b0a54 {
extern "C" {
/* Sets the UI state controller's state and clears mRecordRevealTimer. The singleton
   is read again after SetState, as the ROM does. */
extern "C" void Mg_SetSharedState(s32 state)
{
    dScMgBase_c *scene = gMgScene;
    if (!scene)
        return;
    scene->mStateController.SetState(state);
    gMgScene->mRecordRevealTimer = 0;
}
}
}

// @symbol FreeGfxSlotsById
namespace s20b0aa0 {
extern "C" {
extern void Banner_Free(void *c);
struct E
{
  char pad[0x20];
  int f20;
  char pad2[0x134 - 0x24];
};
extern struct E gMgBanners[];
/* Calls Banner_Free on each of the three 0x134-byte records at
   gMgBanners whose word at +0x20 equals `arg` -- or, for 0x1d,
   on each whose word is not 0x1d. */
void FreeGfxSlotsById(int arg)
{
  int i;
  struct E *p;
  i = arg;
  if (i == 0x1d)
  {
    p = gMgBanners;
    for (i = 0; i < 3; i++)
    {
      if (p->f20 != 0x1d)
      {
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
      if (arg == p->f20)
      {
        Banner_Free(p);
      }
      p++;
    }

  }
}
}
}

// @symbol Mg_CheckBannerButton
namespace s20b0b1c {
extern "C" {

struct Vec2 {
    short x;
    short y;
};

struct Obj004 {
    char pad0[0x10];
    struct Vec2 pos;  /* 0x10 */
    char pad14[0xc];
    int f20;          /* 0x20 */
    char pad24[0xe];
    unsigned short f32;/* 0x32 */
    char pad34[0x100];
};

extern struct Obj004 gMgBanners[3];
extern int sMgBannerButtonsArmed;

extern void Banner_Press(struct Obj004 *o);
extern int Banner_IsPressFinished(struct Obj004 *o);

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
        struct Obj004 *o = gMgBanners;
        int cx = TOUCH_REC(gTouchX)[idx][0];
        int cy = TOUCH_REC(gTouchY)[idx][0];
        for (i = 0; i < 3; i++, o++) {
            if (arg == o->f20 && o->f32 != 0) {
                volatile struct Vec2 pos;
                int dx, dy;
                short ty;
                pos.x = o->pos.x;
                ty = o->pos.y;
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
        struct Obj004 *o = gMgBanners;
        for (j = 0; j < 3; j++, o++) {
            if (arg == o->f20) {
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
}

// @symbol Mg_ShowBanner
namespace s20b0cac {
extern "C" {
extern struct MgBanner gMgBanners[];
extern void Banner_Init(struct MgBanner *e, int c, int a1, int a2, int a3, int s1, short s2);
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
}

// @symbol Mg_ResetOverlays
namespace s20b0d30 {
extern "C" {
extern char sMgBannerGfxSlotUsed[];

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
}

// @symbol Mg_DrawBlinkSprite
namespace s20b0d8c {
extern "C" {
extern s32 GetGameLanguage(void);
extern void Hud_RenderSprite(void *fn, s32 a, s32 b, s32 r3arg, s32 stack_arg);
/* Draws per-language sprite word 0x11 or 0x12 at (arg1, arg2): word 0x11
   for the first 20 frames of the scene's 40-frame mFrameCounter cycle,
   word 0x12 for the rest. */
void Mg_DrawBlinkSprite(void *c, s32 arg1, s32 arg2)
{
    s32 idx1 = ((dScMgBase_c *)c)->mFrameCounter;
    s32 r4 = (idx1 < 0x14) ? 0x11 : 0x12;
    s32 idx2 = GetGameLanguage();
    int *table = sMgSpriteTables[idx2];
    Hud_RenderSprite((void *)table[r4], arg1, arg2, -1, -1);
}
}
}

// @symbol Mg_DrawPrompt
namespace s20b0de0 {
extern "C" {
s32 GetGameLanguage(void);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

/* Draws the per-language prompt sprite at (0xc0, 0xb0) while mPromptEnabled
   is set: blinking (shown for the first 0x18 frames of each 0x30-frame
   mPromptBlinkTimer cycle) for four cycles, then every frame. */
void Mg_DrawPrompt(char* c)
{
    dScMgBase_c *self = (dScMgBase_c *)c;
    if (self->mPromptEnabled == 0)
        return;
    if ((unsigned int)self->mPromptBlinkCount < 4U) {
        self->mPromptBlinkTimer++;
        if ((unsigned int)self->mPromptBlinkTimer >= 0x30U) {
            self->mPromptBlinkTimer = 0U;
            self->mPromptBlinkCount++;
        }
        if ((unsigned int)self->mPromptBlinkTimer >= 0x18U)
            return;
    }
    {
        int i = GetGameLanguage();
        RenderOamMainScreen(sMgSpriteTables[i][10], 0xc0, 0xb0, -1, -1);
    }
}
}
}

// @symbol Mg_DrawTally
namespace s20b0e84 {
extern "C" {
extern "C" int GetGameLanguage(void);
extern "C" void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

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
extern "C" void Mg_DrawTally(dScMgBase_c *sl, int sb) {
    int *r8;
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
}

// @symbol Mg_DrawRecordTable
namespace s20b14f0 {
extern "C" {
extern int Mg_GetLastRecordRank(void* self, int a);
extern int Mg_GetSaveTable(void* self);
extern int func_02013580(int a, int b);
extern void Mg_DrawRecordRow(void* self, int a1, int a2, int a3);

/* Draws the five saved records in rows 0x18 apart from (0xa8, 0x28),
   revealing one more row every 0x14 frames as mRecordRevealTimer counts up. mRecordRevealTimer
   then loops over 0x64..0x8b, and the session best (mRankedScore) blinks on
   that 40-frame loop: in the row of the rank it took (mLastRecordRank, read
   through Mg_GetLastRecordRank), or in a row below the five when it placed
   nowhere. Mg_GetLastRecordRank takes only the scene; the second argument
   passed here is unused. */
void Mg_DrawRecordTable(char* self)
{
    dScMgBase_c *scene = (dScMgBase_c *)self;
    int a2 = 0x28;
    int n;
    int i;
    int j;
    int k;

    scene->mRecordRevealTimer += 1;
    if (scene->mRecordRevealTimer >= 0x8c)
        scene->mRecordRevealTimer = 0x64;

    n = Mg_GetLastRecordRank(self, scene->mRankedScore);
    if (n < 0)
        goto neg;

    i = 0;
    if (n > 0)
    {
        do
        {
            if (scene->mRecordRevealTimer >= i * 0x14)
                Mg_DrawRecordRow(self, 0xa8, a2, func_02013580(Mg_GetSaveTable(self), i));
            a2 += 0x18;
            i++;
        } while (i < n);
    }

    if (scene->mRecordRevealTimer >= n * 0x14)
    {
        if (scene->mRecordRevealTimer < 0x64 || scene->mRecordRevealTimer - 0x64 < 0x14)
            Mg_DrawRecordRow(self, 0xa8, a2, scene->mRankedScore);
        else
            Mg_DrawRecordRow(self, 0xa8, a2, -1);
    }

    a2 += 0x18;
    j = n + 1;
    if (j >= 5)
        return;
    do
    {
        if (scene->mRecordRevealTimer >= j * 0x14)
            Mg_DrawRecordRow(self, 0xa8, a2, func_02013580(Mg_GetSaveTable(self), j));
        a2 += 0x18;
        j++;
    } while (j < 5);
    return;

neg:
    k = 0;
    do
    {
        if (scene->mRecordRevealTimer >= k * 0x14)
            Mg_DrawRecordRow(self, 0xa8, a2, func_02013580(Mg_GetSaveTable(self), k));
        a2 += 0x18;
        k++;
    } while (k < 5);

    if (scene->mRecordRevealTimer < 0x64)
        return;

    if (scene->mRecordRevealTimer - 0x64 < 0x14)
        Mg_DrawRecordRow(self, 0xa8, a2 + 0x10, scene->mRankedScore);
    else
        Mg_DrawRecordRow(self, 0xa8, a2 + 0x10, -1);
}
}
}

// @symbol Mg_DrawRecordRow
namespace s20b1710 {
extern "C" {
extern int GetGameLanguage(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);

extern MgEntry gMgMenuRow;

/* Draws one record row at y. In scoring mode 5 (gMgMenuRow.scoringMode)
   `val` is a time in frames, drawn as seconds, sprite word 7, tenths and
   hundredths ending at x, and a negative val draws nothing. Otherwise
   modes 1, 2 and 4 draw two label sprites at x 0x58 and 0x68 first; mode
   3 then draws val 4 px left through Mg_DrawNumber, every other mode
   through Mg_DrawBigNumber. A negative val draws no number. */
void Mg_DrawRecordRow(int a0, int x, int y, int val)
{
    int idx = GetGameLanguage();
    int mode = gMgMenuRow.scoringMode;
    int *tbl = sMgSpriteTables[idx];
    int flag = 0;

    if (mode == 5) {
        int major;
        int rem;
        int pct;
        if (val < 0)
            return;
        major = val / 60;
        Mg_DrawNumber(x - 0x18, y, major, flag, -1, 1, 0xa);
        Hud_RenderSprite((void *)tbl[7], x - 0x10, y, -1, -1);

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
        Hud_RenderSprite((void *)tbl[9], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    case 2:
        Hud_RenderSprite((void *)tbl[2], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    case 3:
        flag = 1;
        break;
    case 4:
        Hud_RenderSprite((void *)tbl[0x13], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
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
}

// @symbol Mg_DrawTopLeftCounter
namespace s20b19f0 {
extern "C" {
s32 GetGameLanguage(void);
int RenderOamMainScreen(int a, int b, int c, int d, int e);
int Mg_DrawNumber(int a, int b, void* c, int d, int e, int f, int g);
/* Draws sprite word 8 at (0x20, 0xc) and `value`, starting at x
   0x48, through Mg_DrawNumber. */
int Mg_DrawTopLeftCounter(void* value){
  int i = GetGameLanguage();
  RenderOamMainScreen(sMgSpriteTables[i][8], 0x20, 0xc, -1, -1);
  return Mg_DrawNumber(0x48, 0xc, value, 1, -1, 2, 0xa);
}
}
}

// @symbol Mg_DrawTopRightCounter
namespace s20b1a5c {
extern "C" {
s32 GetGameLanguage(void);
int RenderOamMainScreen(int a, int b, int c, int d, int e);
int Mg_DrawNumber(int a, int b, int c, int d, int e, int f, int g);
extern int sMgCounterMaxByDigits[];
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
}

// @symbol Mg_GetLevel
namespace s20b1aec {
extern "C" {
/* The scene's mLevel; 0 with no scene. */
int Mg_GetLevel(void) {
    dScMgBase_c *scene = gMgScene;
    if (scene != 0) return scene->mLevel;
    return 0;
}
}
}

// @symbol Mg_LoseCredits
namespace s20b1b08 {
extern "C" {
extern void Mg_SubCredits(void *c, int val);
/* Subtracts `amount` from the scene's mCredits (Mg_SubCredits). Most
   callers declare the amount as a void *, so it stays one here. */
void Mg_LoseCredits(void *amount) {
    void *v = gMgScene;
    if (v == 0) return;
    Mg_SubCredits(v, (int)amount);
}
}
}

// @symbol Mg_GainCredits
namespace s20b1b40 {
extern "C" {
extern void Mg_AddCredits(void *c, s32 delta);
/* Adds `amount` to the scene's mCredits (Mg_AddCredits). */
void Mg_GainCredits(void *amount) {
    void *v = gMgScene;
    if (v == 0) return;
    Mg_AddCredits(v, (s32)amount);
}
}
}

// @symbol Mg_SubCredits
namespace s20b1b78 {
extern "C" {
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
}

// @symbol Mg_AddCredits
namespace s20b1ba0 {
extern "C" {
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
}

// @symbol Mg_DrawCredits
namespace s20b1bc8 {
extern "C" {
extern void Mg_DrawCreditsIcon(int r0, int r1, int r2, int r3);
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
}

// @symbol Mg_RenderSpriteOnMtx
namespace s20b1c68 {
extern "C" {
struct M { int _00, _01, _10, _11; };
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void Mg_RenderMainSpritePalMtx(void* a0, int a1, int a2, int a3, int a4, void* a5);
/* a3 picks the wrapper: -1 RenderOamBothScreens, 0 Mg_RenderSubSpritePalMtx,
   anything else Mg_RenderMainSpritePalMtx; each is passed -1 in its place. */
void Mg_RenderSpriteOnMtx(void* a0, int a1, int a2, int a3, int a4, struct M* a5){
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
}

// @symbol Mg_RenderSpriteOn
namespace s20b1cf0 {
extern "C" {
void Mg_RenderSpriteBoth(int a, int b, int c, int d, int e);
void Hud_RenderSprite(int a, int b, int c, int d, int e);
void RenderOamMainScreen(int a, int b, int c, int d, int e);
/* `sel` picks the wrapper: -1 Mg_RenderSpriteBoth, 0 Hud_RenderSprite,
   anything else RenderOamMainScreen; each is passed -1 in its place. */
void Mg_RenderSpriteOn(int a, int b, int c, int sel, int e){
  if(sel == -1){ Mg_RenderSpriteBoth(a,b,c,-1,e); return; }
  if(sel == 0){ Hud_RenderSprite(a,b,c,-1,e); return; }
  RenderOamMainScreen(a,b,c,-1,e);
}
}
}

// @symbol Mg_RenderSpriteOnNoMtx
namespace s20b1d60 {
extern "C" {
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void Mg_RenderMainSpritePalMtx(void* a0, int a1, int a2, int a3, int a4, void* a5);
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
}

// @symbol Mg_DrawCreditsIcon
namespace s20b1de8 {
extern "C" {
extern int GetGameLanguage(void);
extern int Mg_RenderSpriteOnNoMtx(int, int, int, int, int);

/* Draws sprite word 2 at (r0, r1) through Mg_RenderSpriteOnNoMtx. */
void Mg_DrawCreditsIcon(int r0, int r1, int r2, int r3) {
    int idx = GetGameLanguage();
    int *t = sMgSpriteTables[idx];
    Mg_RenderSpriteOnNoMtx(t[2], r0, r1, r2, r3);
}
}
}

// @symbol Mg_DrawScoreTally
namespace s20b1e34 {
extern "C" {
extern void Mg_DrawTally(dScMgBase_c *sl, int sb);

/* Draws the mHudScore tally through Mg_DrawTally (a tail call). */
void Mg_DrawScoreTally(void* a, int unused1, int unused2, int unused3) {
    Mg_DrawTally((dScMgBase_c *)a, ((dScMgBase_c *)a)->mHudScore);
}
}
}

// @symbol Mg_DrawTopCenterCounter
namespace s20b1e44 {
extern "C" {
extern int GetGameLanguage(void);
extern void DrawOamSprite(int a, int b, int c, int d);
/* Draws `arg` centred at (0x80, 0xc) and sprite word 3 at (0x60, 0xc). */
void Mg_DrawTopCenterCounter(int arg) {
    int r;
    Mg_DrawBigNumber(0x80, 0xc, arg, 1, -1, 0, 0);
    r = GetGameLanguage();
    DrawOamSprite(sMgSpriteTables[r][3], 0x60, 0xc, 0);
}
}
}

// @symbol Mg_DrawNumber
namespace s20b1ea4 {
extern "C" {
extern int sMgDigitSprites[];
extern void Mg_RenderSpriteOnNoMtx(void *glyph, int x, int a2, int a3, int a4);

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
            Mg_RenderSpriteOnNoMtx((void *)sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
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
            Mg_RenderSpriteOnNoMtx((void *)sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
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
            Mg_RenderSpriteOnNoMtx((void *)sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
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
            Mg_RenderSpriteOnNoMtx((void *)sMgDigitSprites[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[1] != 0) {
        if (mode == 0) x -= 4;
        else if (mode == 1) x -= 8;
        Mg_RenderSpriteOnNoMtx((void *)sMgDigitSprites[fb + d[1]], x, a1, a3, a4);
        x += 8;
    }
    Mg_RenderSpriteOnNoMtx((void *)sMgDigitSprites[d[0] + fb], x, a1, a3, a4);
}
}
}

// @symbol Mg_DrawNumberRotated
#pragma push
#pragma opt_propagation off
namespace s20b2220 {
extern "C" {
/* Draws a score number rotated by an angle: splits the value (clamped to
   9999) into thousands, hundreds, tens and units, builds the rotation-and-
   scale matrix from the sine table, and draws the digits centred on x: four
   digits from x-0x30, three from x-0x20, two from x-0x10, a lone unit at x.
   The fixed-point multiply and the 2x2 rotation are the NitroSDK FX_Mul and
   MTX_Rot22 shapes, written as local inlines. Spelled that way the
   allocator keeps the thousands counter in r0 as the cartridge does; with
   the products written out in the body it lands in ip or r3. */
struct M { int _00, _01, _10, _11; };
extern void Mg_RenderSpriteOnMtx(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
extern s16 data_02082214[];
extern int sMgDigitSprites[];

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

static inline s16 FX_SinIdx(int idx) { return data_02082214[idx << 1]; }
static inline s16 FX_CosIdx(int idx) { return data_02082214[(idx << 1) + 1]; }

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
        Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[th], x - 0x30, y, a3, a4, &m);
        Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[hu], x - 0x10, y, a3, a4, &m);
        Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[te], x + 0x10, y, a3, a4, &m);
        x += 0x30;
    } else if (hu != 0) {
        Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[hu], x - 0x20, y, a3, a4, &m);
        Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[te], x, y, a3, a4, &m);
        x += 0x20;
    } else if (te != 0) {
        Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[te], x - 0x10, y, a3, a4, &m);
        x += 0x10;
    }
    Mg_RenderSpriteOnMtx((void *)sMgDigitSprites[value], x, y, a3, a4, &m);
}
}
}
#pragma pop

// @symbol Mg_DrawBigNumber
namespace s20b2444 {
extern "C" {
extern int sMgDigitSprites[];
extern void Mg_RenderSpriteOnNoMtx(int a0, int a1, int a2, int a3, int a4);

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
}

// @symbol Mg_DrawStockCounter
namespace s20b2574 {
extern "C" {
extern int GetGameLanguage(void);
extern void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);

/* Unless arg1 is 1, draws sprite words 0 and 1 at (12, 12) and (30, 12)
   and arg0 from x 0x30. With arg1 1, draws sprite word 0 arg0 times,
   16 px apart from (12, 12). */
void Mg_DrawStockCounter(int arg0, int arg1)
{
    int *table = sMgSpriteTables[GetGameLanguage()];
    void *sb = (void *)table[0];
    int b = 12;
    if (arg1 != 1) {
        DrawOamSprite(sb, (void *)b, b, 0);
        DrawOamSprite((void *)sMgSpriteTables[GetGameLanguage()][1], (void *)30, b, 0);
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
}

namespace s20b265c {
extern "C" {
// @symbol _ZN11dScMgBase_c11InitEnginesEv
/* dScMgBase_c::InitEngines - slot 33.  The ROM names nothing here: InitEngines
   is the repo's no-name spelling after the +0x84 vtable offset, as fBase_c
   uses for Virtual34 and Virtual38.  See the slot-33 block in
   include/dScMgBase_c.h.

   ENGINE BRING-UP.  Graphics modes for both engines (GX mode 1/0/0, GXS mode
   0), VRAM banks assigned to BG and OBJ on both screens, and BOTH BG-enable
   shadows -- data_0209d45c for the main engine, data_0209d454 for the sub --
   initialised to 0x10, which is the value slots 30 and 31 later save, clear
   bits out of and restore.  A language-indexed character file (indexed by
   GetGameLanguage into sMgStockCounterFiles) is decompressed into BOTH engines'
   BG char VRAM at 0x06404000 and 0x06604000, OBJ palette file 0xc3 is loaded
   into both, and the scene object is published into the global registry --
   sMgGraphCallback[1] = this, then data_0209d4a8 points at that registry.
   The object fields it touches are its own: +0x68 cleared and +0x6c set to -1.

   WHEN IT RUNS.  First out of dScMgBase_c::BeforeInitResources
   (ov004:0x020b0930): the +0x84 dispatch at 0x020b09d0 is near the top and
   slot 31's +0x7c dispatch at 0x020b0a0c is the last thing before the
   function returns 1.  Slot 32 runs out of AfterInitResources.  So the
   sequence is: bring the engines up (33), dress the sub screen (31), then
   dress the main screen (32).

   arity: no explicit parameters, MEASURED.  Scanning arm9 and all 103 overlays
   for the dispatch pair -- `ldr rD,[rN,#0x84]` with Rn != pc, followed within
   three instructions by `blx rD`/`bx rD` -- finds exactly two sites image-wide,
   of which one is in ov004 or ov006: the 0x020b09d0 call above.  It reads
   `mov r0,r4; ldr r1,[r0]; ldr r1,[r1,#0x84]; blx r1`, so r1 is the loaded
   pointer and cannot also be a second argument.  The scanner was validated by
   re-running it at +0x80 and reproducing slot 32's known call site.
   return type: void, which is what all three bodies do -- none assigns a
   result and the one caller ignores whatever falls out. */
extern void Mg_DisableAllBanks(void);
extern void Mg_ResetBgOffsets(void);
extern s32 GetGameLanguage(void);
extern void *Ov004_LoadFile(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void Ov004_Deallocate(void *p);
extern void Mg_ResetOverlays(void);
extern void FreeGfxSlotsById(int arg);
extern int sMgTopScreenYOffset;
extern u8 data_0209d45c;
extern int sMgStockCounterFiles[];
extern u8 data_0209d454;
extern void **data_0209d4a8;
extern int data_0208ee44;
}
}

void dScMgBase_c::InitEngines() {
    using s20b265c::DecompressLZ16;
    using s20b265c::FreeGfxSlotsById;
    using s20b265c::GetGameLanguage;
    using s20b265c::Ov004_Deallocate;
    using s20b265c::data_0208ee44;
    using s20b265c::data_0209d454;
    using s20b265c::data_0209d45c;
    using s20b265c::data_0209d4a8;
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
    *(vu32 *)0x4001000u |= 0x10000u;
    GX::SetGraphicsMode(1, 0, 0);
    GXS::SetGraphicsMode(0);
    *(vu32 *)0x4001000u &= 0xffcfffefu;
    *(vu16 *)0x4000304u |= 0x8000u;
    Mg_DisableAllBanks();
    Mg_ResetBgOffsets();
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    GX::SetBankForBG(3);
    GX::SetBankForOBJ(0x10);
    data_0209d45c = 0x10;
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(8);
    p = Ov004_LoadFile(sMgStockCounterFiles[GetGameLanguage()]);
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    Ov004_Deallocate(p);
    p = Ov004_LoadFile(0xc3);
    CP15::FlushAndInvalidateDataCache((u32)p, 0x100u);
    GX::LoadOBJPltt(p, 0x100u, 0x100u);
    GXS::LoadOBJPltt(p, 0x100u, 0x100u);
    Ov004_Deallocate(p);
    data_0209d454 = 0x10;
    sMgGraphCallback.mScene = this;
    data_0209d4a8 = (void **)&sMgGraphCallback;
    Mg_ResetOverlays();
    FreeGfxSlotsById(0x1d);
    data_0208ee44 = 1;
}

namespace s20b27f4 {
extern "C" {
// @symbol _ZN11dScMgBase_c16InitMainScreenBgEv
// recovered name: dScMgBase_c_AfterClsn  -- WRONG, see below
/* dScMgBase_c::InitMainScreenBg - slot 32.

   The `recovered name:` line above is kept visible because it is wrong.  This
   time the borrowed name is a REAL ROM name -- _ZN16dPathLiftActor_c9AfterClsnEi,
   declared at include/PathLift.h:58 -- which makes it the more misleading of the
   two.  dPathLiftActor_c derives from dBgActor_c, which derives from dActor_c;
   dScMgBase_c is a dScene_c.  The chains share only dBase_c, which adds no
   virtual, so the two slot 32s have fBase_c's first eighteen entries in common
   and nothing else.  That AfterClsn also takes an int.  See the slot-32 block in
   include/dScMgBase_c.h.

   It touches no collision.  It is slot 31 with the other display engine: three
   read-modify-writes leave the MAIN BG1CNT at 0x0400000a holding exactly 0x1000,
   the layer's scroll is reset, BG1's bit is cleared from data_0209d45c -- the
   MAIN BG-enable shadow slot 30 restores the main DISPCNT from, where 31 cleared
   the sub's data_0209d454 -- and a language-indexed character file plus the
   shared screen map (file 0x67 here, 0x5b there) are installed.  It builds this
   minigame's TOP-screen background, from AfterInitResources. */
extern int GetGameLanguage(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char data_0209d45c[];
}
}

void dScMgBase_c::InitMainScreenBg()
{ using s20b27f4::GetGameLanguage; using s20b27f4::LoadCompressedFileAt; using s20b27f4::data_0209d45c;
    int f;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~3;
    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~0x40;
    SetBg1Offset(0, 0);
    data_0209d45c[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(sMgPauseBgNcgByLanguage[f], (void *)func_02054ea8());
    LoadCompressedFileAt(0x67, G2::GetBG1ScrPtr());
}

namespace s20b2880 {
extern "C" {
// @symbol _ZN11dScMgBase_c15InitSubScreenBgEv
// recovered name: dScMgBase_c_Kill  -- WRONG, see below
/* dScMgBase_c::InitSubScreenBg - slot 31.

   The `recovered name:` line above is kept visible because it is wrong.  The
   ROM names nothing here: dScMgBase_c is a SCENE (fBase_c -> dBase_c ->
   dScene_c -> dScMgBase_c), not an actor, and dActor_c -- whose names slots
   18-30 borrowed by index -- has no slot 31 at all.  `Kill` was carried across
   from dBgActor_c, dActor_c's own child, where _ZN10dBgActor_c4KillEv
   (ov002:0x020ee55c) is that class's new slot 31.  Different branch, same
   index, no relationship.  See the slot-31 block in include/dScMgBase_c.h.

   It also does not kill anything.  Three read-modify-writes leave the sub
   engine's BG1CNT holding exactly 0x10 -- priority 0, no mosaic -- then the
   layer's scroll is reset, BG1's bit is cleared from the sub BG-enable shadow
   that slot 30 restores the sub DISPCNT from, and a language-indexed character
   file plus the shared screen map (file 0x5b) are installed.  It builds this
   minigame's touch-screen background, from BeforeInitResources. */
extern int GetGameLanguage(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char data_0209d454[];
}
}

int dScMgBase_c::InitSubScreenBg() {
    using s20b2880::GetGameLanguage;
    using s20b2880::LoadCompressedFileAt;
    using s20b2880::data_0209d454;
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x10;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(sMgOptionNcgByLanguage[f], (void *)G2S::GetBG1CharPtr());
    LoadCompressedFileAt(0x5b, G2S::GetBG1ScrPtr());
}

// @symbol Mg_ResetBgOffsets
namespace s20b290c {
extern "C" {
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
}

// @symbol Mg_DisableAllBanks
namespace s20b2980 {
extern "C" {
void Mg_DisableAllBanks(void) {
    GX::DisableAllBanks();
}
}
}

// @symbol _ZN11dScMgBase_c10OnDealDoneEv
/* dScMgBase_c::OnDealDone - slot 21. */
void dScMgBase_c::OnDealDone()
{
}

// @symbol _ZN11dScMgBase_c11OnNewRecordEv
/* Slot 20: the shared results state calls it once while it shows a new
   record (dMgState_c, state 17). See dScMgBase_c.h. */
/* The base does nothing: the ROM body is a single `bx lr`. */
void dScMgBase_c::OnNewRecord()
{
}

// @symbol _ZN11dScMgBase_c11CanEndStateEi
/* dScMgBase_c::CanEndState - vtable slot 19. Both the name and the signature are evidenced here: this
   body carries its own `recovered name:` above, and dScMgJump_c and
   dScMgBSC_c carry theirs. The parameter is an int, not dActor_c.h:132's
   `Player &` -- see include/dScMgBase_c.h. The base ignores the mode. */
int dScMgBase_c::CanEndState(int /* mode */)
{
    return 1;
}

// @symbol _ZN11dScMgBase_c10StartRoundEi
/* vtable slot 18. The name is NOT a `recovered name:` -- this body carried
   none. It comes from include/dActor_c.h:131 and from ov006/symbols.txt,
   which already named dScMgCoin_c's override of this slot. The signature,
   unlike the name, is measured; see include/dScMgBase_c.h.
   The base ignores both arguments -- every descendant that cares reads
   them. */
void dScMgBase_c::StartRound(int arg)
{
}

// @symbol Mg_CallStartRound
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
// @symbol _ZN11dScMgBase_cC2Ev
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

// @symbol _ZN11dScMgBase_cD2Ev
// @symbol _ZN11dScMgBase_cD0Ev
// @symbol _ZN11dScMgBase_cD1Ev
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

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D2 AND D0 NAMES, AND MSVC NEVER EMITS THEM.
 * MSVC folds the Itanium destructor variants into the one ~dScMgBase_c()
 * above. Descendants tearing down a dScMgBase_c base subobject call the
 * base-object variant by its flat name; the deleting variant is the D1 body
 * followed by the class-specific operator delete. Each is spelled here in
 * terms of the one host destructor, called qualified so it is a direct call.
 * mwccarm never defines _MSC_VER, so nothing here reaches the cartridge
 * object. */
extern "C" dScMgBase_c *_ZN11dScMgBase_cD2Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();
    return thiz;
}

extern "C" dScMgBase_c *_ZN11dScMgBase_cD0Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();
    dScMgBase_c::operator delete(thiz);
    return thiz;
}
#endif

#pragma pop
