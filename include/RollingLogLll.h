/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FallBlockLll: 5 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FALLBLOCKLLL_H
#define FALLBLOCKLLL_H
#include "types.h"
#include "Model.h"

struct FallBlockLll {
    u8  pad_000[0xd4];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12FallBlockLllD1Ev.c] */
    Model mModel;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
#endif
};

#endif
