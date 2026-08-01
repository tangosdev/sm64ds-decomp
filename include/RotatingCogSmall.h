/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RotatingCogSmall: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROTATINGCOGSMALL_H
#define ROTATINGCOGSMALL_H
#include "types.h"

struct RotatingCogSmall {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x80];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    s16 unk_31e;            /* 0x31e */
    s16 unk_320;            /* 0x320 */
    s16 unk_322;            /* 0x322 */
    s16 unk_324;            /* 0x324 */
    u8  pad_326[0x6];
    s32 mRotationState;            /* 0x32c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
