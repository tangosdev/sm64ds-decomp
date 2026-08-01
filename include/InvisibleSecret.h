/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class InvisibleSecret: 6 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef INVISIBLESECRET_H
#define INVISIBLESECRET_H
#include "types.h"

struct InvisibleSecret {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x33];
    u8  mTextureSequence;            /* 0x124 */
    u8  pad_125[0x7];
    s32 unk_12c;            /* 0x12c */
    u8  pad_130[0x8];
    s32 unk_138;            /* 0x138 */
    s32 unk_13c;            /* 0x13c */
    s32 mStartPosY;            /* 0x140 */
    s32 unk_144;            /* 0x144 */
    s32 unk_148;            /* 0x148 */
    u16 unk_14c;            /* 0x14c */
    u8  unk_14e;            /* 0x14e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
