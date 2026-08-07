/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class QuestionSwitch: 6 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef QUESTIONSWITCH_H
#define QUESTIONSWITCH_H
#include "types.h"
#include "ModelAnim.h"

struct QuestionSwitch {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  unk_124;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 mActiveMeshCollider;            /* 0x320 */
    u8  unk_324;            /* 0x324 */
    u8  pad_325[0x1c7];
    u8  mMovingMeshCollider;            /* 0x4ec */
    u8  pad_4ed[0x1c7];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x6b4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnim (+0x50 = the
       Animation base), unk_70c (+0x58 = currFrame), unk_710 (+0x5c = speed), which the
       header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x6b4 */
    s8  unk_718;            /* 0x718 */
    u8  pad_719[0x1];
    u8  unk_71a;            /* 0x71a */
    u8  unk_71b;            /* 0x71b */
    u16 unk_71c;            /* 0x71c */
    u8  pad_71e[0x2];
    s32 mTalkingPlayer;            /* 0x720 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
