/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MansionSteps: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MANSIONSTEPS_H
#define MANSIONSTEPS_H
#include "types.h"

struct MansionSteps {
    u8  pad_000[0xd4];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    s32 unk_124;            /* 0x124 */
    u8  pad_128[0x18];
    s32 unk_140;            /* 0x140 */
    u8  pad_144[0xc];
    u8  unk_150;            /* 0x150 */
    u8  pad_151[0x5];
    u8  unk_156;            /* 0x156 */
    u8  pad_157[0x5];
    u8  mMovingMeshCollider;            /* 0x15c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
