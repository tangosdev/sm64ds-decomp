/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FlameChompFire: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLAMECHOMPFIRE_H
#define FLAMECHOMPFIRE_H
#include "types.h"

struct FlameChompFire {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mShadowModel;            /* 0x0d4 */
    u8  pad_0d5[0x27];
    u8  mMovingCylinderClsn;            /* 0x0fc */
    u8  pad_0fd[0x33];
    u8  mWithMeshClsn;            /* 0x130 */
    u8  pad_131[0x1bb];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x37];
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
