/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TtcConveyorBeltLarge: 6 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TTCCONVEYORBELTLARGE_H
#define TTCCONVEYORBELTLARGE_H
#include "types.h"

struct TtcConveyorBeltLarge {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x20];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x7];
    u8  unk_0dc;            /* 0x0dc */
    u8  pad_0dd[0x47];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    u8  mTextureTransformer;            /* 0x320 */
    u8  pad_321[0xb];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x4];
    u8  mShadowModel;            /* 0x334 */
    u8  pad_335[0x57];
    s32 mBeltSpeed;            /* 0x38c */
    s32 mTargetBeltSpeed;            /* 0x390 */
    s32 unk_394;            /* 0x394 */
    s32 unk_398;            /* 0x398 */
    u8  unk_39c;            /* 0x39c */
    u8  pad_39d[0x1];
    u8  mVariant;            /* 0x39e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
