/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PathLift: 4 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PATHLIFT_H
#define PATHLIFT_H
#include "types.h"

struct PathLift {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xc6];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x305];
    u8  unk_42a;            /* 0x42a */
    u8  unk_42b;            /* 0x42b */
    u8  pad_42c[0x20];
    s32 unk_44c;            /* 0x44c */
#ifdef __cplusplus
    /* methods */
    void AfterClsn();
#endif
};

#endif
