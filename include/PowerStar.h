/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PowerStar: 7 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef POWERSTAR_H
#define POWERSTAR_H
#include "types.h"

struct PowerStar {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xa2];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x1c];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x3c];
    u8  mCylinderClsn;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModelAnim1;            /* 0x30c */
    u8  pad_30d[0x63];
    u8  mModelAnim2;            /* 0x370 */
    u8  pad_371[0x63];
    u8  mShadowModel;            /* 0x3d4 */
    u8  pad_3d5[0x67];
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    u8  pad_444[0x54];
    s8  unk_498;            /* 0x498 */
    u8  pad_499[0x1];
    u8  unk_49a;            /* 0x49a */
    u8  pad_49b[0x2];
    u8  unk_49d;            /* 0x49d */
    u8  pad_49e[0x1];
    u8  unk_49f;            /* 0x49f */
    u8  pad_4a0[0x2];
    u8  unk_4a2;            /* 0x4a2 */
    u8  pad_4a3[0x5];
    s32 unk_4a8;            /* 0x4a8 */
    s32 unk_4ac;            /* 0x4ac */
    s32 unk_4b0;            /* 0x4b0 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
    s32 InitResources();
    void AddStarMarker();
#endif
};

#endif
