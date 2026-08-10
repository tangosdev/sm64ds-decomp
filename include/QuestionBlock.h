#ifndef QUESTIONBLOCK_H
#define QUESTIONBLOCK_H

#include "types.h"
#include "Platform.h"
#include "ModelAnim.h"
#include "ShadowModel.h"

/* Derives from Platform: the destructor stores this class's vtable, then
 * Platform's -- inlined -- then destroys the MovingMeshCollider at 0x124 and
 * the Model at 0xd4 before chaining to Actor. All three belong to Platform.
 * Everything this header used to restate below 0x31e was Actor's and
 * Platform's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

struct QuestionBlock : Platform {
    u8  pad_31e[0x2];
    ModelAnim mModelAnim;             /* 0x320 */
    ShadowModel mShadowModel;         /* 0x384 */
    u8 unk_3ac;                       /* 0x3ac */
    u8  pad_3ad[0x33];
    s32 unk_3e0;                      /* 0x3e0 */
    u8  pad_3e4[0x4];
    s32 unk_3e8;                      /* 0x3e8 */
    u8  pad_3ec[0x4];
    u8 unk_3f0;                       /* 0x3f0 */
    u8 unk_3f1;                       /* 0x3f1 */
    u8  pad_3f2[0x1];
    u8 unk_3f3;                       /* 0x3f3 */

    /* --- vtable --- */
    virtual ~QuestionBlock();

    int Behavior();
    int CleanupResources();
    int Render();
};

typedef char QuestionBlock_size_must_be_0x3f4[sizeof(QuestionBlock) == 0x3f4 ? 1 : -1];

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
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x33];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x320 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x320 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x384 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13QuestionBlockD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x384 */
    u8  unk_3ac;            /* 0x3ac */
    u8  pad_3ad[0x33];
    s32 unk_3e0;            /* 0x3e0 */
    u8  pad_3e4[0x4];
    s32 unk_3e8;            /* 0x3e8 */
    u8  pad_3ec[0x4];
    u8  unk_3f0;            /* 0x3f0 */
    u8  unk_3f1;            /* 0x3f1 */
    u8  pad_3f2[0x1];
    u8  unk_3f3;            /* 0x3f3 */
};

#endif /* __cplusplus */

#endif /* QUESTIONBLOCK_H */
