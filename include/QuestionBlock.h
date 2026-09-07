#ifndef QUESTIONBLOCK_H
#define QUESTIONBLOCK_H

#include "types.h"
#include "dBgW_KcMbg.h"
#include "Model.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daObjHatenaBlock_c. The reconstructed
 * factory daObjHatenaBlock_c_classInit_HATENA_BLOCK (historical alias
 * QuestionBlock_Spawn) constructs it for the HATENA_BLOCK
 * registry profile.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"

struct QuestionBlock : dBgActor_c {
    u8  pad_31e[0x2];
    ModelAnim mModelAnim;             /* 0x320 */
    ShadowModel mShadowModel;         /* 0x384 */
    /* InitResources copies the 0x30 bytes at mModel + 0x1c (Model's own
       mat4x3) into this offset, which is exactly where mShadowModel ends --
       the same ShadowModel/Matrix4x3 pairing dActor_c::DropShadowScaleXYZ
       takes. 0x3ac + 0x30 = 0x3dc. */
    Matrix4x3 mShadowMat;             /* 0x3ac */
    u8  pad_3dc[0x4];
    s32 mHomePosY;                      /* 0x3e0 */
    u8  pad_3e4[0x4];
    s32 mState;                      /* 0x3e8 */
    u8  pad_3ec[0x4];
    u8 mStarTracked;                       /* 0x3f0 */
    u8 mStarId;                       /* 0x3f1 */
    /* Written (truncated from the caller's u32 param1/actorID-ish word) by
       every one of this class's five combat-callback overrides below, always
       immediately before func_ov102_02149da8(this, 1) -- a state-machine
       dispatch through data_ov102_0214e890 keyed on mState. Was a u8
       marker. */
    u8 mHitterParam;                       /* 0x3f2 */
    u8 mContentType;                       /* 0x3f3 */

    /* --- vtable --- */
    virtual ~QuestionBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Slots 21/22/24/27/28, all dActor_c combat-callback overrides (see
       include/dActor_c.h). Attributed by the vtable, not by the
       pre-migration `recovered name:` comments. None is the key function:
       ~QuestionBlock() above stays the first out-of-line virtual, so none of
       these five TUs newly emits _ZTV13QuestionBlock -- checked with
       objisolate, not assumed.

       OnKicked, OnGroundPounded and OnHitByMegaChar return `void`, matching
       include/dActor_c.h's slots 24, 21 and 27 (corrected from `int` by
       BigBrickBlock::OnKicked and Stump::OnGroundPounded/OnHitByMegaChar
       respectively -- see include/BigBrickBlock.h, include/Stump.h and
       src/_ZN8dActor_c8OnKickedERS_.cpp for the falsifying evidence; this
       class's own overrides happened to byte-match under the old `int` too,
       re-verified rather than assumed). The other two stay `int`, dActor_c's
       own declared type for their slots. */
    void OnGroundPounded(dActor_c &other);     /* slot 21 */
    int OnAttacked1(dActor_c &other);          /* slot 22 */
    void OnKicked(dActor_c &other);            /* slot 24 */
    void OnHitByMegaChar(Player &player);      /* slot 27 */
    int OnHitFromUnderneath(dActor_c &other);  /* slot 28 */

    /* Tail padding. The field span stops short of the real size: daObjHatenaBlock_c_classInit_CAP_BLOCK_L and daObjHatenaBlock_c_classInit_CAP_BLOCK_M
       call fBase_c::operator new(0x3f8), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_3f4[0x4];      /* 0x3f4, to the ROM's 0x3f8 */
};

typedef char QuestionBlock_size_must_be_0x3f8[sizeof(QuestionBlock) == 0x3f8 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct QuestionBlock {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorId;            /* 0x00c */
    u8  pad_00e[0x52];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x10];
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    /* Model member. The cartridge's own ~QuestionBlock calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_0f0
       (+0x1c = mat4x3), which the header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~QuestionBlock calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x320 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x320 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x384 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13QuestionBlockD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x384 */
    struct Matrix4x3 mShadowMat;    /* 0x3ac */
    u8  pad_3dc[0x4];
    s32 mHomePosY;            /* 0x3e0 */
    u8  pad_3e4[0x4];
    s32 mState;            /* 0x3e8 */
    u8  pad_3ec[0x4];
    u8  mStarTracked;            /* 0x3f0 */
    u8  mStarId;            /* 0x3f1 */
    u8  mHitterParam;            /* 0x3f2 */
    u8  mContentType;            /* 0x3f3 */
};

#endif /* __cplusplus */

#endif /* QUESTIONBLOCK_H */
