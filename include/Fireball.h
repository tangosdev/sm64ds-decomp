/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Fireball: 4 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FIREBALL_H
#define FIREBALL_H
#include "types.h"

struct Fireball {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0xf4];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x1b];
    u8  unk_12c;            /* 0x12c */
    u8  pad_12d[0x17];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mShadowModel;            /* 0x300 */
    u8  pad_301[0x5f];
    s32 unk_360;            /* 0x360 */
    s32 unk_364;            /* 0x364 */
    u8  pad_368[0x2];
    u16 unk_36a;            /* 0x36a */
    u8  pad_36c[0x1];
    u8  unk_36d;            /* 0x36d */
    u8  pad_36e[0x2];
    s32 unk_370;            /* 0x370 */
    s32 unk_374;            /* 0x374 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
