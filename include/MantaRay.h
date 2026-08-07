/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MantaRay: 5 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MANTARAY_H
#define MANTARAY_H
#include "types.h"
#include "ModelAnim.h"

struct MantaRay {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
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
    u8  pad_098[0x4];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0x3];
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x30c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x30c */
    u8  unk_370;            /* 0x370 */
    u8  pad_371[0xb];
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 mPathNode;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int Render();
#endif
};

#endif
