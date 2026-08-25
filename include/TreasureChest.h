/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TreasureChest: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TREASURECHEST_H
#define TREASURECHEST_H
#include "types.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct TreasureChest {
    u8  pad_000[0x8];
    s32 param1;            /* 0x008 */
    u8  pad_00c[0xc8];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x138 */
    u8  pad_16c[0x6];
    u8  mOrder;            /* 0x172 */
    u8  pad_173[0x1];
    u8  mStarID;            /* 0x174 */
    u8  mTrackedStarID;            /* 0x175 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char TreasureChest_size_must_be_0x178[sizeof(struct TreasureChest) == 0x178 ? 1 : -1];

#endif
