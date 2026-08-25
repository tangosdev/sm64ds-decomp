/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserPuzzlePiece: 5 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERPUZZLEPIECE_H
#define BOWSERPUZZLEPIECE_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct BowserPuzzlePiece {
    u8  pad_000[0x8];
    s32 param1;            /* 0x008 */
    u8  pad_00c[0x82];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member, named by the class's own destructor calling
       dBgW_KcMbg's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN17BowserPuzzlePieceD1Ev.c] */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x37];
    s32 mStateInfo;            /* 0x324 */
    u8  mStateIndex;            /* 0x328 */
    u8  pad_329[0x3];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x4];
    s16 mMoveTimer;            /* 0x334 */
    u8  mState;            /* 0x336 */
    u8  mType;            /* 0x337 */
    u8  mHadClsn;            /* 0x338 */
    u8  mFreezeState;            /* 0x339 */
    u8  mCanSpawnCoin;            /* 0x33a */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
    void InitResources();
#endif
};

typedef char BowserPuzzlePiece_size_must_be_0x33c[
    sizeof(struct BowserPuzzlePiece) == 0x33c ? 1 : -1];

#endif
