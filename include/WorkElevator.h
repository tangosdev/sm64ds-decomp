/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WorkElevator: 6 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WORKELEVATOR_H
#define WORKELEVATOR_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct WorkElevator {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member. The cartridge's own ~WorkElevator calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~WorkElevator calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  mClsnMat;            /* 0x2ec */
    u8  pad_2ed[0x233];
    u8  mPlatformClsn0;            /* 0x520 */
    u8  pad_521[0x1c7];
    u8  mPlatformClsn1;            /* 0x6e8 */
    u8  pad_6e9[0x1c7];
    u8  mPlatformClsn2;            /* 0x8b0 */
    u8  pad_8b1[0x1c7];
    u8  mPlatformClsn3;            /* 0xa78 */
    u8  pad_a79[0x1c7];
    s32 unk_c40;            /* 0xc40 */
    s32 unk_c44;            /* 0xc44 */
    s32 unk_c48;            /* 0xc48 */
    u8  pad_c4c[0x20];
    s32 unk_c6c;            /* 0xc6c */
    u8  pad_c70[0xa];
    s8  mLoweredPlatform;            /* 0xc7a */
    /* trailing extent the ROM's `new WorkElevator` literal proves; see tools/opnew_sizes.py */
    u8 pad_c7c[0x4];
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
#endif
};

typedef char WorkElevator_size_must_be_0xc80[sizeof(struct WorkElevator) == 0xc80 ? 1 : -1];

#endif
