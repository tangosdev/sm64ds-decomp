/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RollingLogTtm: 5 matched functions, 27 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROLLINGLOGTTM_H
#define ROLLINGLOGTTM_H
#include "types.h"

struct RollingLogTtm {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x5b];
    s32 unk_130;            /* 0x130 */
    u8  pad_134[0x4];
    u8  mShadowModel;            /* 0x138 */
    u8  pad_139[0x27];
    u8  mMovingCylinderClsn;            /* 0x160 */
    u8  pad_161[0x33];
    u8  mWithMeshClsn;            /* 0x194 */
    u8  pad_195[0x1eb];
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    s32 unk_38c;            /* 0x38c */
    s32 unk_390;            /* 0x390 */
    s32 unk_394;            /* 0x394 */
    u8  pad_398[0x10];
    s32 unk_3a8;            /* 0x3a8 */
    s32 unk_3ac;            /* 0x3ac */
    u32 unk_3b0;            /* 0x3b0 */
    u8  pad_3b4[0x14];
    u8  unk_3c8;            /* 0x3c8 */
    u8  pad_3c9[0x2];
    u8  unk_3cb;            /* 0x3cb */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
