/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SeesawBob: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SEESAWBOB_H
#define SEESAWBOB_H
#include "types.h"
#include "Model.h"

struct SeesawBob {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x7e];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x20];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    s16 unk_324;            /* 0x324 */
    u8  unk_326;            /* 0x326 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
