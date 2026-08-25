/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PyramidLift: 5 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PYRAMIDLIFT_H
#define PYRAMIDLIFT_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct PyramidLift {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x18];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel1;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~PyramidLift calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_33c (+0x1c = mat4x3), which
       the header declared separately inside it. */
    Model mModel2;            /* 0x320 */
    s32 unk_370;            /* 0x370 */
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    u8  pad_37c[0x78];
    u16 mShakeTimer;            /* 0x3f4 */
    u8  mState;            /* 0x3f6 */
    u8  mHadClsn;            /* 0x3f7 */
    u8  mNextBullet;            /* 0x3f8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    int Render();
#endif
};

typedef char PyramidLift_size_must_be_0x3fc[sizeof(struct PyramidLift) == 0x3fc ? 1 : -1];

#endif
