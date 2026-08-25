/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SnowmanBody: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SNOWMANBODY_H
#define SNOWMANBODY_H
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct SnowmanBody {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    /* 0x080..0x08c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x42];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* ShadowModel member. The cartridge's own ~SnowmanBody calls _ZN11ShadowModelD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x124 */
    /* dCcAc_c member. The cartridge's own ~SnowmanBody calls _ZN7dCcAc_cD1Ev at +0x14c
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    dCcAc_c mdCcAc_c;            /* 0x14c */
    /* dBgCh_Actr member. The cartridge's own ~SnowmanBody calls _ZN10dBgCh_ActrD1Ev at
       +0x180 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgCh_Actr mWithMeshClsn;            /* 0x180 */
    s32 mHomePosX;            /* 0x33c */
    s32 mHomePosY;            /* 0x340 */
    s32 mHomePosZ;            /* 0x344 */
    s16 mHomeAngleX;            /* 0x348 */
    s16 mHomeAngleY;            /* 0x34a */
    s16 mHomeAngleZ;            /* 0x34c */
    u8  pad_34e[0x2];
    u8  mShadowMat;            /* 0x350 */
    u8  pad_351[0x3f];
    s32 mTalkPlayer;            /* 0x390 */
    u8  pad_394[0x10];
    u8  unk_3a4;            /* 0x3a4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char SnowmanBody_size_must_be_0x3a8[sizeof(struct SnowmanBody) == 0x3a8 ? 1 : -1];

#endif
