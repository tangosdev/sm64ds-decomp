/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PoleLift: 6 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef POLELIFT_H
#define POLELIFT_H
#include "types.h"

struct PoleLift {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    s8  unk_0d4;            /* 0x0d4 */
    u8  pad_0d5[0x3];
    u8  mModel;            /* 0x0d8 */
    u8  pad_0d9[0x7f];
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
