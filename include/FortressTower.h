/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FortressTower: 6 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FORTRESSTOWER_H
#define FORTRESSTOWER_H
#include "types.h"
#include "Model.h"

struct FortressTower {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
