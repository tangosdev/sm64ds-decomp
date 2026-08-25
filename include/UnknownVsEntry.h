/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class UnknownVsEntry: 6 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef UNKNOWNVSENTRY_H
#define UNKNOWNVSENTRY_H
#include "types.h"
#include "Model.h"
#include "ModelAnim.h"

struct UnknownVsEntry {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x44];
    u8  mParticle;            /* 0x050 */
    u8  pad_051[0x81b];
    /* Model member. The cartridge's own ~UnknownVsEntry calls _ZN5ModelD1Ev at +0x86c
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x86c */
    /* ModelAnim member. The cartridge's own ~UnknownVsEntry calls _ZN9ModelAnimD1Ev at
       +0x8bc (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ModelAnim mModelAnim;            /* 0x8bc */
    u8  pad_920[0x560];
    u8  unk_e80;            /* 0xe80 */
    u8  pad_e81[0xa7];
    s32 mCamPosX;            /* 0xf28 */
    s32 mCamPosY;            /* 0xf2c */
    s32 mCamPosZ;            /* 0xf30 */
    s32 mCamTargetX;            /* 0xf34 */
    s32 mCamTargetY;            /* 0xf38 */
    s32 mCamTargetZ;            /* 0xf3c */
    u8  mAnimActive;            /* 0xf40 */
    u8  mState;            /* 0xf41 */
    u8  mFocusedPlayer;            /* 0xf42 */
    u8  mPlayerCount;            /* 0xf43 */
    u8  mSuspended;            /* 0xf44 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char UnknownVsEntry_size_must_be_0xf48[sizeof(struct UnknownVsEntry) == 0xf48 ? 1 : -1];

#endif
