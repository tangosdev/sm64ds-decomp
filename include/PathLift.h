#ifndef PATHLIFT_H
#define PATHLIFT_H

#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"
#include "PathPtr.h"

/* The ROM names this class dPathLiftActor_c in its RTTI record at
 * ov002:0x0210af0c. It is the shared base for the path-lift actors, derives
 * from dBgActor_c, and its own data starts
 * in the base's tail padding at 0x31e (dBgActor_c's last field ends there and its
 * size rounds to 0x320).
 *
 * THE ARRAY IS Model[3]. The ROM destroys it with
 * __destroy_arr(this + 0x320, 3, 0x50, _ZN5ModelD1Ev) -- the element
 * destructor is named in the ROM already, and 0x50 is sizeof(Model), so this
 * one needed no new type at all. 0x320 + 3 * 0x50 = 0x410.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

struct dPathLiftActor_c : dBgActor_c {
    u8  pad_31e[0x2];
    Model mModels[3];                     /* 0x320 */
    Vector3 mInitialPos;                  /* 0x410 */
    Vector3_16 mInitialAngle;             /* 0x41c */
    Vector3_16 mPrevPathAngle;            /* 0x422 */
    u16 mWaitTimer;                       /* 0x428 */
    u8  mAfterClsnRan;                    /* 0x42a -- set by AfterClsn, cleared by the last statement of BaseBehavior */
    u8  mTriggerDelay;                    /* 0x42b -- DecIfAbove0_Byte; AfterClsn only fires the state change at 0 */
    u8  unk_42c;                          /* 0x42c */
    u8  pad_42d[0x3];
    PathPtr mPath;                        /* 0x430 */
    s32 mCurrPathNode;                    /* 0x438 */
    s32 mPathDirection;                   /* 0x43c -- +1 or -1 */
    s32 mPathSpeed;                       /* 0x440 -- 20.12 fixed point */
    s32 mFallStartY;                      /* 0x444 */
    u8  mFallDelay;                       /* 0x448 */
    u8  mFallBounceTimer;                 /* 0x449 */
    s16 mFallAngle;                       /* 0x44a */
    s32 mState;                           /* 0x44c */

    /* DEFINED INLINE, and that is the point. The ROM's daObjPathLift_c
       destructor contains dPathLiftActor_c's cleanup verbatim rather than a
       `bl` to a D2 symbol that this image does not contain -- so the original
       destructor was inline in the class body and every subclass inlined it,
       the same arrangement include/dBase_c.h records. The base's own D0/D1
       still exist as out-of-line symbols because the vtable needs an address;
       their files force the emission and objisolate keeps the bound variant. */
    virtual ~dPathLiftActor_c() {}

    /* Slot 32 of the ROM vtable. Its definition anchors this class's RTTI and
       vtable in the same original translation unit as the state machine. */
    virtual void AfterClsn(int clsnResult);

    void StateWaitInit();
    void StateWait();
    void StatePathInit();
    void StatePath();
    void StateFallInit();
    void StateFall();
    void SetState(int state);
    void BaseInitResources();
    void ResetPath();
    void RenderPathModels();
    void UpdatePathModels();
    int HasNonzeroAngleZ() const;
    int Param10ModeIs1() const;
    int Param12ModeIs1() const;
    int Param08ModeIs2() const;
    int Param08ModeIs1Or2() const;
    int IsUnk42cSet() const;
    /* Non-virtual. Subclasses (e.g. daObjPathLift_c) call it from their own
       Behavior override to run dPathLiftActor_c's shared step first. */
    void BaseBehavior();
};

typedef char dPathLiftActor_c_size_must_be_0x450[
    sizeof(dPathLiftActor_c) == 0x450 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct dPathLiftActor_c {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member. The cartridge's own base destructor calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's base destructor calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x13c];
    u16 mWaitTimer;         /* 0x428 */
    u8  mAfterClsnRan;            /* 0x42a */
    u8  mTriggerDelay;            /* 0x42b */
    u8  unk_42c;            /* 0x42c */
    u8  pad_42d[0x3];
    struct PathPtr mPath;    /* 0x430 */
    s32 mCurrPathNode;      /* 0x438 */
    s32 mPathDirection;     /* 0x43c */
    s32 mPathSpeed;         /* 0x440 */
    s32 mFallStartY;        /* 0x444 */
    u8  mFallDelay;         /* 0x448 */
    u8  mFallBounceTimer;   /* 0x449 */
    s16 mFallAngle;         /* 0x44a */
    s32 mState;             /* 0x44c */
};

#endif /* __cplusplus */

#endif /* PATHLIFT_H */
