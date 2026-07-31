/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BigBrickBlock: 6 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BIGBRICKBLOCK_H
#define BIGBRICKBLOCK_H
#include "types.h"

struct BigBrickBlock {
    u8  pad_000[0xc];
    u16 mActorId;            /* 0x00c */
    u8  pad_00e[0xc6];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
    u8  unk_31f;            /* 0x31f */
    u8  mEventID;            /* 0x320 */
    u8  pad_321[0x3];
    u8  mSwitch;            /* 0x324 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
