/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MansionSteps: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MANSIONSTEPS_H
#define MANSIONSTEPS_H
#include "types.h"
#include "Model.h"

struct MansionSteps {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
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
