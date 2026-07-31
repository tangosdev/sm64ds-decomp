/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CheepCheep: 5 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CHEEPCHEEP_H
#define CHEEPCHEEP_H
#include "types.h"

struct CheepCheep {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x1a];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModelAnim;            /* 0x30c */
    u8  pad_30d[0x67];
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
