#ifndef PATHLIFT_H
#define PATHLIFT_H

#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

/* A lift that runs along a path. Derives from dBgActor_c, and its own data starts
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

struct PathLift : dBgActor_c {
    u8  pad_31e[0x2];
    Model mModels[3];                     /* 0x320 */
    u8  pad_410[0x1a];
    u8  mAfterClsnRan;                    /* 0x42a -- set by AfterClsn, cleared by the last statement of BaseBehavior */
    u8  mTriggerDelay;                    /* 0x42b -- DecIfAbove0_Byte; AfterClsn only fires the state change at 0 */
    u8  pad_42c[0x20];
    s32 mState;                          /* 0x44c */

    virtual ~PathLift();

    void AfterClsn();
    /* Non-virtual. Subclasses (e.g. daObjPathLift_c) call it from their own
       Behavior override to run PathLift's shared step first. */
    void BaseBehavior();
};

typedef char PathLift_size_must_be_0x450[sizeof(PathLift) == 0x450 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct PathLift {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member. The cartridge's own ~PathLift calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~PathLift calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x13e];
    u8  mAfterClsnRan;            /* 0x42a */
    u8  mTriggerDelay;            /* 0x42b */
    u8  pad_42c[0x20];
    s32 mState;            /* 0x44c */
};

#endif /* __cplusplus */

#endif /* PATHLIFT_H */
