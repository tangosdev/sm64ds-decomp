/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PoleLift: 6 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef POLELIFT_H
#define POLELIFT_H
#include "types.h"
#include "Model.h"

struct PoleLift {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    s8  unk_0d4;            /* 0x0d4 */
    u8  pad_0d5[0x3];
    /* Model member, named by _ZN5ModelD1Ev at +0xd8 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad ran 0x30
       bytes PAST the end of the object; that space is not evidenced and stays explicit
       padding rather than being folded into the member. */
    Model mModel;            /* 0x0d8 */
    u8  pad_128[0x30];
    u8  mCollider;            /* 0x158 */
    u8  pad_159[0x4c];
    u8  unk_1a5;            /* 0x1a5 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
