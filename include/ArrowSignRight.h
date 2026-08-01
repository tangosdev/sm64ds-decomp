/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ArrowSignRight: 6 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ARROWSIGNRIGHT_H
#define ARROWSIGNRIGHT_H
#include "types.h"

struct ArrowSignRight {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x80];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mShadowModel;            /* 0x320 */
    u8  pad_321[0x27];
    u8  unk_348;            /* 0x348 */
    u8  pad_349[0x33];
    u8  unk_37c;            /* 0x37c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
