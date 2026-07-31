/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Cannon: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CANNON_H
#define CANNON_H
#include "types.h"

struct Cannon {
    u8  pad_000[0xd4];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingCylinderClsn;            /* 0x124 */
    u8  pad_125[0x5b];
    s32 unk_180;            /* 0x180 */
    u8  pad_184[0x1];
    u8  unk_185;            /* 0x185 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
