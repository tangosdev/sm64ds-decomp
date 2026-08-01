/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class YoshiEgg: 6 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef YOSHIEGG_H
#define YOSHIEGG_H
#include "types.h"

struct YoshiEgg {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x48];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mAnimation;            /* 0x350 */
    u8  pad_351[0x13];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x27];
    u8  mPlayer;            /* 0x38c */
    u8  pad_38d[0x63];
    s32 unk_3f0;            /* 0x3f0 */
    u8  pad_3f4[0x2c];
    u8  unk_420;            /* 0x420 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
