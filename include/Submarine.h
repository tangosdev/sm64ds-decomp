/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Submarine: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SUBMARINE_H
#define SUBMARINE_H
#include "types.h"

struct Submarine {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x68];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  unk_110;            /* 0x110 */
    u8  pad_111[0x3];
    u8  mModelAnim;            /* 0x114 */
    u8  pad_115[0x7];
    u8  unk_11c;            /* 0x11c */
    u8  pad_11d[0x47];
    u8  mAnimation;            /* 0x164 */
    u8  pad_165[0xb];
    s32 unk_170;            /* 0x170 */
    u8  pad_174[0x4];
    u8  mTextureTransformer;            /* 0x178 */
    u8  pad_179[0xb];
    s32 unk_184;            /* 0x184 */
    u8  pad_188[0x20];
    s32 unk_1a8;            /* 0x1a8 */
    s32 unk_1ac;            /* 0x1ac */
    s32 unk_1b0;            /* 0x1b0 */
    u8  pad_1b4[0x4];
    u8  unk_1b8;            /* 0x1b8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
