/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MontyMoleRock: 5 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MONTYMOLEROCK_H
#define MONTYMOLEROCK_H
#include "types.h"
#include "Model.h"

struct MontyMoleRock {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x6c];
    /* Model member, named by _ZN5ModelD1Ev at +0x110 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x110 */
    u8  mMovingCylinderClsn;            /* 0x160 */
    u8  pad_161[0x33];
    u8  mWithMeshClsn;            /* 0x194 */
    u8  pad_195[0x1bb];
    u8  unk_350;            /* 0x350 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
