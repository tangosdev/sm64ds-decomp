/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserPuzzlePiece: 4 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERPUZZLEPIECE_H
#define BOWSERPUZZLEPIECE_H
#include "types.h"

struct BowserPuzzlePiece {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0x80];
    u8  unk_08c;            /* 0x08c */
    u8  pad_08d[0x1];
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0x1];
    u8  unk_090;            /* 0x090 */
    u8  pad_091[0x1];
    u8  unk_092;            /* 0x092 */
    u8  pad_093[0x1];
    u8  unk_094;            /* 0x094 */
    u8  pad_095[0x1];
    u8  unk_096;            /* 0x096 */
    u8  pad_097[0x69];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1cf];
    s32 unk_314;            /* 0x314 */
    s32 unk_318;            /* 0x318 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
