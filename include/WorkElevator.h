/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WorkElevator: 6 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WORKELEVATOR_H
#define WORKELEVATOR_H
#include "types.h"

struct WorkElevator {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x233];
    u8  unk_520;            /* 0x520 */
    u8  pad_521[0x1c7];
    u8  unk_6e8;            /* 0x6e8 */
    u8  pad_6e9[0x1c7];
    u8  unk_8b0;            /* 0x8b0 */
    u8  pad_8b1[0x1c7];
    u8  unk_a78;            /* 0xa78 */
    u8  pad_a79[0x1c7];
    s32 unk_c40;            /* 0xc40 */
    s32 unk_c44;            /* 0xc44 */
    s32 unk_c48;            /* 0xc48 */
    u8  pad_c4c[0x20];
    s32 unk_c6c;            /* 0xc6c */
    u8  pad_c70[0xa];
    s8  unk_c7a;            /* 0xc7a */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
#endif
};

#endif
