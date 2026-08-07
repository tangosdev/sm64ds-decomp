/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TtcConveyorBeltLarge: 6 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TTCCONVEYORBELTLARGE_H
#define TTCCONVEYORBELTLARGE_H
#include "types.h"
#include "Model.h"

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
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0dc (+0x8 = data), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
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
    s16 unk_39c;            /* 0x39c */
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
