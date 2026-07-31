/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Squasher: 6 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SQUASHER_H
#define SQUASHER_H
#include "types.h"

struct Squasher {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x17];
    s16 unk_08c;            /* 0x08c */
    u16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x31];
    s16 unk_31e;            /* 0x31e */
    s16 unk_320;            /* 0x320 */
    u8  unk_322;            /* 0x322 */
    u8  pad_323[0x1];
    u8  mShadowModel;            /* 0x324 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
