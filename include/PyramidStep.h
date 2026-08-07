/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PyramidStep: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PYRAMIDSTEP_H
#define PYRAMIDSTEP_H
#include "types.h"
#include "Model.h"

struct PyramidStep {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x82];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x18];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel2;            /* 0x320 */
    s16 unk_370;            /* 0x370 */
    u8  unk_372;            /* 0x372 */
    u8  pad_373[0x1];
    u8  unk_374;            /* 0x374 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
