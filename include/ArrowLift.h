/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ArrowLift: 6 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ARROWLIFT_H
#define ARROWLIFT_H
#include "types.h"
#include "Model.h"

struct ArrowLift {
    u8  pad_000[0x98];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x38];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
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
