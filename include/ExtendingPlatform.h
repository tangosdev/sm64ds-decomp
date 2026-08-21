/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ExtendingPlatform: 6 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef EXTENDINGPLATFORM_H
#define EXTENDINGPLATFORM_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbgSclY.h"

struct ExtendingPlatform {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    s8  mGrowing;            /* 0x0d4 */
    u8  pad_0d5[0x3];
    /* Model member, named by _ZN5ModelD1Ev at +0xd8 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad ran 0x30
       bytes PAST the end of the object; that space is not evidenced and stays explicit
       padding rather than being folded into the member. */
    Model mModel;            /* 0x0d8 */
    u8  pad_128[0x30];
    dBgW_KcMbgSclY mCollider; /* 0x158 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char ExtendingPlatform_size_must_be_0x328[
    sizeof(struct ExtendingPlatform) == 0x328 ? 1 : -1];

#endif
