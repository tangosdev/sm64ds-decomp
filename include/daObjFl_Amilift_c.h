#ifndef DAOBJFL_AMILIFT_C_H
#define DAOBJFL_AMILIFT_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "PathPtr.h"

/* A moving platform ("Amilift") that rides a path: the base model and mesh
 * collider come from dBgActor_c, and this class adds the path walk, a sink
 * when the player stands on it, and a small bob. The low four bits of param1
 * pick the path (PathPtr::FromID) and bit 8 selects auto-run (see mAutoRun).
 *
 * TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_Amilift_c_classInit  fBase_c::operator new(872 = 0x368), dBgActor_c::dBgActor_c(), stores _ZTV17daObjFl_Amilift_c,
 *                 then the member below in this order.
 *   ~daObjFl_Amilift_c   the same member destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x368 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjFl_Amilift_c : dBgActor_c {
    s32 mSinkOffset;      /* 0x320 -- eased toward -0x28000 while the player is on it */
    s32 mBobOffset;       /* 0x324 -- sin(mBobPhase) * 10, added to the height */
    s16 mBobPhase;        /* 0x328 */
    u8  pad_32a[2];
    s32 mHomeX;           /* 0x32c -- position at spawn; only mHomeY is read again */
    s32 mHomeY;           /* 0x330 */
    s32 mHomeZ;           /* 0x334 */
    u16 mStateFrames;     /* 0x338 -- frames in the current state */
    u8  mRiderOn;         /* 0x33a -- set by func_ov064_02117fb4 when the player touches it; cleared each frame */
    u8  mState;           /* 0x33b */
    u8  mAutoRun;         /* 0x33c -- param1 bit 8: 1 keeps walking, 0 waits for the player */
    u8  pad_33d[3];
    s32 mNodeCount;       /* 0x340 */
    s32 mNodeIndex;       /* 0x344 -- the node it is walking toward */
    s32 mFromX;           /* 0x348 -- the node it last reached */
    s32 mFromY;           /* 0x34c */
    s32 mFromZ;           /* 0x350 */
    s32 mToX;             /* 0x354 -- position of node mNodeIndex */
    s32 mToY;             /* 0x358 */
    s32 mToZ;             /* 0x35c */
    PathPtr                mPathPtr;     /* 0x360 */

    virtual ~daObjFl_Amilift_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    void func_ov064_02117a14(Vector3* a, Vector3* b);
    int  func_ov064_02117a44();
    void func_ov064_02117b8c();
    int  func_ov064_02117bdc();
    void func_ov064_02117c24();

    /* mState values. The state handlers live in a three-entry table in ov064's
       .data (see d_a_obj_fl_amilift.cpp), indexed by mState. */
    enum {
        STATE_WAIT = 0,       /* waits at the start for the player to stand on it */
        STATE_FORWARD = 1,    /* walking toward higher node indices */
        STATE_BACKWARD = 2    /* walking back toward node 0 */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Amilift_c_size_must_be_0x368[sizeof(daObjFl_Amilift_c) == 0x368 ? 1 : -1];
#endif

#endif /* DAOBJFL_AMILIFT_C_H */
