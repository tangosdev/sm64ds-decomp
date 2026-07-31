/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class KoopaFlag: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KOOPAFLAG_H
#define KOOPAFLAG_H
#include "types.h"

struct KoopaFlag {
    u8  pad_000[0xd4];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x23];
    u32 unk_0f8;            /* 0x0f8 */
    u8  pad_0fc[0xc];
    u8  mModelAnim;            /* 0x108 */
    u8  pad_109[0x4f];
    u8  mAnimation;            /* 0x158 */
    u8  pad_159[0x13];
    u16 mVictoryTimer;            /* 0x16c */
    u8  unk_16e;            /* 0x16e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
