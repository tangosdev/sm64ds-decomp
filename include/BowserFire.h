/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserFire: 4 matched functions, 22 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERFIRE_H
#define BOWSERFIRE_H
#include "types.h"

struct BowserFire {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x64];
    u8  mWithMeshClsn;            /* 0x110 */
    u8  pad_111[0x1bb];
    s32 unk_2cc;            /* 0x2cc */
    u8  mMovingCylinderClsn;            /* 0x2d0 */
    u8  pad_2d1[0x17];
    s32 unk_2e8;            /* 0x2e8 */
    u8  pad_2ec[0x18];
    u8  mShadowModel;            /* 0x304 */
    u8  pad_305[0x57];
    s32 unk_35c;            /* 0x35c */
    s32 unk_360;            /* 0x360 */
    s32 unk_364;            /* 0x364 */
    u8  pad_368[0x4];
    s32 unk_36c;            /* 0x36c */
    u8  pad_370[0x4];
    s16 unk_374;            /* 0x374 */
    u8  pad_376[0x2];
    u8  unk_378;            /* 0x378 */
    u8  unk_379;            /* 0x379 */
    u8  pad_37a[0x2];
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int CleanupResources();
    int Render();
    int Behavior();
#endif
};

#endif
