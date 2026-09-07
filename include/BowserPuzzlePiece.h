/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserPuzzlePiece: 5 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERPUZZLEPIECE_H
#define BOWSERPUZZLEPIECE_H
#include "dBgActor_c.h"

/* ROM identity versus compatibility spelling:
 *
 * The slot array historically named _ZTV17BowserPuzzlePiece points back to
 * _ZTI16daObjFl_Puzzle_c. Its type-name bytes say daObjFl_Puzzle_c and its
 * sole base is dBgActor_c. BowserPuzzlePiece is the readable compatibility
 * spelling used by the already-matched method symbols, not a claim about the
 * original EAD RTTI name. Per-function isolation therefore discards the
 * compiler-only BowserPuzzlePiece RTTI and rebinds the vptr to the ROM table.
 *
 * The destructor proves that the first 0x320 bytes are one dBgActor_c base:
 * it restores _ZTV10dBgActor_c, destroys the moving-mesh member at 0x124 and
 * Model at 0x0d4, then chains to dActor_c::~dActor_c. */
struct BowserPuzzlePiece : dBgActor_c {
    u32 mOtherPieceId;       /* 0x320 */
    s32 mStateInfo;            /* 0x324 */
    u8  mStateIndex;            /* 0x328 */
    u8  pad_329[0x3];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x4];
    u16 mMoveTimer;            /* 0x334 */
    u8  mState;            /* 0x336 */
    u8  mType;            /* 0x337 */
    u8  mHadClsn;            /* 0x338 */
    u8  mFreezeState;            /* 0x339 */
    u8  mCanSpawnCoin;            /* 0x33a */
    /* Inline is load-bearing: the two small forcing translation units emit the
     * ROM's D1 and D0 while objisolate discards their wrappers and D2. */
    virtual ~BowserPuzzlePiece() {}

    /* Overrides of fBase_c's resource/behavior/render slots. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char BowserPuzzlePiece_size_must_be_0x33c[
    sizeof(struct BowserPuzzlePiece) == 0x33c ? 1 : -1];

#endif
