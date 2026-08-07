/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ShipUp: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SHIPUP_H
#define SHIPUP_H
#include "types.h"
#include "Model.h"

struct ShipUp {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x7e];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  mModelIndex;            /* 0x31e */
    u8  pad_31f[0x1];
    u16 unk_320;            /* 0x320 */
    u8  pad_322[0x2];
    s32 unk_324;            /* 0x324 */
    u16 unk_328;            /* 0x328 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
