#ifndef PYRAMIDTOP_H
#define PYRAMIDTOP_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

struct PyramidTop : dBgActor_c {
    u8  pad_31e[0x2];
    Model mModel;                     /* 0x320 */
    /* The second collision matrix: InitResources passes `this + 0x370' as the
       `const Matrix4x3 &' argument of dBgW_KcMbg::SetFile, and 0x370 + 0x30
       lands exactly on mHomePosX. Left a u8 marker, the idiom this family's
       C twins already use. */
    u8  mClsnMat2[0x30];              /* 0x370 */
    s32 mHomePosX;                    /* 0x3a0 -- InitResources copies mPosX/Y/Z here */
    s32 mHomePosY;                    /* 0x3a4 */
    s32 mHomePosZ;                    /* 0x3a8 */
    s32 mSpinParticleID;                      /* 0x3ac */
    s16 mAngVelY;                      /* 0x3b0 */
    u16 mStateTimer;                      /* 0x3b2 */
    s16 mSoundTimer;                      /* 0x3b4 */
    u8 mNumTagsTriggered;                       /* 0x3b6 */
    u8 mState;                       /* 0x3b7 */

    /* --- vtable --- */
    virtual ~PyramidTop();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char PyramidTop_size_must_be_0x3b8[sizeof(PyramidTop) == 0x3b8 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct PyramidTop {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* 0x074..0x08e is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10PyramidTopD1Ev.c] */
    Model mModel1;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~PyramidTop calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad ran 0x30
       bytes PAST the end of the object; that space is not evidenced and stays explicit
       padding rather than being folded into the member. */
    Model mModel2;            /* 0x320 */
    u8  mClsnMat2[0x30];    /* 0x370 */
    s32 mHomePosX;            /* 0x3a0 */
    s32 mHomePosY;            /* 0x3a4 */
    s32 mHomePosZ;            /* 0x3a8 */
    s32 mSpinParticleID;            /* 0x3ac */
    s16 mAngVelY;            /* 0x3b0 */
    u16 mStateTimer;            /* 0x3b2 */
    s16 mSoundTimer;            /* 0x3b4 */
    u8  mNumTagsTriggered;            /* 0x3b6 */
    u8  mState;            /* 0x3b7 */
};

#endif /* __cplusplus */

#endif /* PYRAMIDTOP_H */
