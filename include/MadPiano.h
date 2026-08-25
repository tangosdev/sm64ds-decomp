/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MadPiano: 6 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MADPIANO_H
#define MADPIANO_H
#include "types.h"
#include "ModelAnim.h"
#include "Model.h"
#include "dBgW_KcMbg.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct MadPiano {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    s32 mPrevPosX;            /* 0x068 */
    s32 mPrevPosY;            /* 0x06c */
    s32 mPrevPosZ;            /* 0x070 */
    u8  pad_074[0x1a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    /* Model member. The cartridge's own ~MadPiano calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~MadPiano calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x320 */
    /* ShadowModel member. The cartridge's own ~MadPiano calls _ZN11ShadowModelD1Ev at
       +0x384 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel1;            /* 0x384 */
    /* ShadowModel member. The cartridge's own ~MadPiano calls _ZN11ShadowModelD1Ev at
       +0x3ac (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel2;            /* 0x3ac */
    /* ShadowModel member. The cartridge's own ~MadPiano calls _ZN11ShadowModelD1Ev at
       +0x3d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel3;            /* 0x3d4 */
    u8  pad_3fc[0x110];
    /* dBgCh_Actr member. The cartridge's own ~MadPiano calls _ZN10dBgCh_ActrD1Ev at
       +0x50c (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgCh_Actr mWithMeshClsn;            /* 0x50c */
    u8  pad_6c8[0x4];
    s32 mMinPosY;            /* 0x6cc */
    u8  pad_6d0[0x4];
    s32 mHomePosX;            /* 0x6d4 */
    s32 mHomePosY;            /* 0x6d8 */
    s32 mHomePosZ;            /* 0x6dc */
    /* trailing extent the ROM's `new MadPiano` literal proves; see tools/opnew_sizes.py */
    u8 pad_6e0[0x4];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char MadPiano_size_must_be_0x6e4[sizeof(struct MadPiano) == 0x6e4 ? 1 : -1];

#endif
