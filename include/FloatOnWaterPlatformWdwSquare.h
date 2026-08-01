/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FloatOnWaterPlatformWdwSquare: 6 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLOATONWATERPLATFORMWDWSQUARE_H
#define FLOATONWATERPLATFORMWDWSQUARE_H
#include "types.h"

struct FloatOnWaterPlatformWdwSquare {
    u8  pad_000[0x98];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x38];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
