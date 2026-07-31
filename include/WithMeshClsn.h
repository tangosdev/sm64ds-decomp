/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WithMeshClsn: 22 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WITHMESHCLSN_H
#define WITHMESHCLSN_H
#include "types.h"

struct WithMeshClsn {
    u8  pad_000[0x10];
    u8  mFlags;            /* 0x010 */
    u8  pad_011[0x3];
    u8  mActor;            /* 0x014 */
    u8  pad_015[0x3];
    s32 unk_018;            /* 0x018 */
    s32 unk_01c;            /* 0x01c */
    u8  mSphereClsn;            /* 0x020 */
    u8  pad_021[0x4b];
    u8  unk_06c;            /* 0x06c */
    u8  pad_06d[0x23];
    u8  mClsnFlags;            /* 0x090 */
    u8  pad_091[0x97];
    s32 unk_128;            /* 0x128 */
    s32 unk_12c;            /* 0x12c */
    s32 unk_130;            /* 0x130 */
    u8  mRaycastLine;            /* 0x134 */
    u8  pad_135[0x83];
    s32 unk_1b8;            /* 0x1b8 */
#ifdef __cplusplus
    /* methods */
    void ClearAllGroundFlags();
    void ClearGroundFlag();
    void ClearJustHitGroundFlag();
    void ClearLimMovFlag();
    void SetGroundFlag();
    void SetLimMovFlag();
    void UpdateDiscreteNoLava();
    void UpdateDiscreteNoLava_2();
#endif
};

#endif
