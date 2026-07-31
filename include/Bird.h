/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Bird: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BIRD_H
#define BIRD_H
#include "types.h"

struct Bird {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    u8  pad_008[0x54];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x2];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x33];
    u8  mAnimation;            /* 0x124 */
    u8  pad_125[0x13];
    u8  mShadowModel;            /* 0x138 */
    u8  pad_139[0x27];
    s32 unk_160;            /* 0x160 */
    s32 unk_164;            /* 0x164 */
    s32 unk_168;            /* 0x168 */
    u8  pad_16c[0xc];
    s32 unk_178;            /* 0x178 */
    s32 unk_17c;            /* 0x17c */
    u8  unk_180;            /* 0x180 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
