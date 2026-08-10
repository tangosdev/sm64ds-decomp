#ifndef KINGBOBOMB_H
#define KINGBOBOMB_H

#include "types.h"

/* Derives from Enemy: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "BlendModelAnim.h"
#include "CommonModel.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct KingBobOmb : Enemy {
    WithMeshClsn mWithMeshClsn;       /* 0x110 */
    BlendModelAnim mBlendModelAnim;   /* 0x2cc */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;/* 0x33c */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos_37c;/* 0x37c */
    CommonModel mCommonModel;         /* 0x3bc */
    ShadowModel mShadowModel;         /* 0x3f8 */
    u8  pad_420[0x74];
    s32 unk_494;                      /* 0x494 */
    u8 unk_498;                       /* 0x498 */
    u8  pad_499[0x7];
    s32 unk_4a0;                      /* 0x4a0 */
    u8  pad_4a4[0x30];
    s32 unk_4d4;                      /* 0x4d4 */
    s32 unk_4d8;                      /* 0x4d8 */
    s32 unk_4dc;                      /* 0x4dc */
    s32 unk_4e0;                      /* 0x4e0 */
    s32 unk_4e4;                      /* 0x4e4 */
    s32 unk_4e8;                      /* 0x4e8 */
    u8  pad_4ec[0x10];
    s32 unk_4fc;                      /* 0x4fc */
    s32 unk_500;                      /* 0x500 */
    u8  pad_504[0x3];
    u8 unk_507;                       /* 0x507 */
    u8  pad_508[0x1];
    u8 unk_509;                       /* 0x509 */

    /* --- vtable --- */
    virtual ~KingBobOmb();

    int Behavior();
    int InitResources();
    int Render();
};

typedef char KingBobOmb_size_must_be_0x50c[sizeof(KingBobOmb) == 0x50c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct KingBobOmb {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    /* 0x0b0..0x110 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 unk_0b0;                 /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;                 /* 0x0ce */
    u8  pad_0d0[0x40];
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10KingBobOmbD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x110 */
    /* BlendModelAnim member, named by _ZN14BlendModelAnimD1Ev at +0x2cc -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    BlendModelAnim mBlendModelAnim;            /* 0x2cc */
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x33c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10KingBobOmbD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos1;            /* 0x33c */
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x37c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10KingBobOmbD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos2;            /* 0x37c */
    /* CommonModel member, named by the class's own destructor calling
       CommonModel's D1 at +0x3bc -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10KingBobOmbD1Ev.c] */
    CommonModel mCommonModel;            /* 0x3bc */
    u8  mShadowModel;            /* 0x3f8 */
    u8  pad_3f9[0x9b];
    s32 unk_494;            /* 0x494 */
    u8  unk_498;            /* 0x498 */
    u8  pad_499[0x7];
    s32 unk_4a0;            /* 0x4a0 */
    u8  pad_4a4[0x30];
    s32 unk_4d4;            /* 0x4d4 */
    s32 unk_4d8;            /* 0x4d8 */
    s32 unk_4dc;            /* 0x4dc */
    s32 unk_4e0;            /* 0x4e0 */
    s32 unk_4e4;            /* 0x4e4 */
    s32 unk_4e8;            /* 0x4e8 */
    u8  pad_4ec[0x10];
    s32 unk_4fc;            /* 0x4fc */
    s32 unk_500;            /* 0x500 */
    u8  pad_504[0x3];
    u8  unk_507;            /* 0x507 */
    u8  pad_508[0x1];
    u8  unk_509;            /* 0x509 */
};

#endif /* __cplusplus */

#endif /* KINGBOBOMB_H */
