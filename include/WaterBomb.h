/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WaterBomb: 6 matched functions, 8 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WATERBOMB_H
#define WATERBOMB_H
#include "types.h"

struct WaterBomb {
    u8  pad_000[0x80];
    u8  unk_080;            /* 0x080 */
    u8  pad_081[0x7f];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModel;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mShadowModel;            /* 0x350 */
    u8  pad_351[0x73];
    s32 unk_3c4;            /* 0x3c4 */
    s32 unk_3c8;            /* 0x3c8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
