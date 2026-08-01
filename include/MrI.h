/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MrI: 5 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MRI_H
#define MRI_H
#include "types.h"

struct MrI {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x3];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0xc];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x5b];
    s32 unk_130;            /* 0x130 */
    u8  pad_134[0x4];
    u8  mTextureSequence;            /* 0x138 */
    u8  pad_139[0xb];
    s32 unk_144;            /* 0x144 */
    u8  pad_148[0x4];
    u8  mShadowModel;            /* 0x14c */
    u8  pad_14d[0x27];
    u8  mMovingCylinderClsnWithPos;            /* 0x174 */
    u8  pad_175[0x3f];
    u8  unk_1b4;            /* 0x1b4 */
    u8  pad_1b5[0x37];
    s32 unk_1ec;            /* 0x1ec */
    s32 unk_1f0;            /* 0x1f0 */
    u8  pad_1f4[0xc];
    s32 unk_200;            /* 0x200 */
    u8  pad_204[0x8];
    s16 unk_20c;            /* 0x20c */
    u8  pad_20e[0x8];
    u8  unk_216;            /* 0x216 */
    u8  unk_217;            /* 0x217 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int Render();
#endif
};

#endif
