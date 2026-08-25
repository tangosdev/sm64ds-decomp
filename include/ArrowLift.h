/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ArrowLift: 6 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ARROWLIFT_H
#define ARROWLIFT_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct ArrowLift {
    u8  pad_000[0x98];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x38];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~ArrowLift calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 unk_320;            /* 0x320 */
    /* trailing extent the ROM's `new ArrowLift` literal proves; see tools/opnew_sizes.py */
    u8 pad_324[0x4];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char ArrowLift_size_must_be_0x328[sizeof(struct ArrowLift) == 0x328 ? 1 : -1];

#endif
