/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserTail: 4 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERTAIL_H
#define BOWSERTAIL_H
#include "types.h"

struct BowserTail {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x33];
    u32 unk_108;            /* 0x108 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
