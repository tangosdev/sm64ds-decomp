/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TreasureChest: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TREASURECHEST_H
#define TREASURECHEST_H
#include "types.h"
#include "ModelAnim.h"

struct TreasureChest {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0xc8];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    u8  mMovingCylinderClsn;            /* 0x138 */
    u8  pad_139[0x39];
    u8  unk_172;            /* 0x172 */
    u8  pad_173[0x1];
    u8  unk_174;            /* 0x174 */
    u8  unk_175;            /* 0x175 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
