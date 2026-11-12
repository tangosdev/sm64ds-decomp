/* class cMgSmartball_object_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ed10.
 *
 * A ROOT. It has no RTTI base at all -- this is a separate hierarchy from
 * the fBase_c/dScene_c tree that the rest of this directory models, and the
 * first class in it anyone has touched. Eleven direct children:
 * ball, pakkun, dokan, kinoko, pushswitch, propeller, wing, ana, board,
 * spring, slot.
 *
 * NO VIRTUAL DESTRUCTOR. Its vtable is exactly THREE slots and none of them
 * is a structor -- all three are ordinary virtual methods. That makes this
 * family a different shape from every other class in this tree, where the
 * destructor pair is what gets migrated first. There is nothing to migrate
 * here but real methods, and no operator delete is involved.
 *
 * The inline constructor writes the manager, the slot, and the incoming pair
 * into both position slots, then zeroes radius and velocity and sets
 * mIsActive. cMgSmartball_object_c_Construct writes those same stores into
 * the object it is given.
 *
 * SIZE 0x34, DATA SIZE 0x31. The constructor's last write is the u8 at 0x30
 * and nothing in the class lies past it. The class is not a POD (it has a
 * vtable), so mwccarm lays a child's first fields into its tail padding:
 * each child owns its own bytes from 0x31 on (pushswitch's mTriggered, the
 * board's nine-cell array, the wing's and propeller's angle at 0x32).
 *
 * THE THREE 2-WORD PAIRS ARE THE INTERESTING PART, and their RELATIONSHIP
 * is what is evidenced, not their meaning. The constructor writes the same
 * incoming pair to BOTH 0x08 and 0x18. Slot 0 copies 0x08/0x0c -> 0x10/0x14.
 * Slot 2 copies 0x18/0x1c -> 0x08/0x0c. So 0x18 holds a value that 0x08 can
 * be restored from, and 0x10 holds a snapshot of 0x08. The field names below
 * say that much and no more.
 *
 * METHOD NAMES ARE COINED FROM THE BODIES, not recovered from the ROM.
 * Nothing in this hierarchy has a named slot map to inherit from -- it is a
 * root, so there is no base whose names could settle them. Each name below
 * describes exactly what its body does; none of them is a claim about what
 * the original called it. Renaming later costs one commit. */
#ifndef CMGSMARTBALL_OBJECT_C_H
#define CMGSMARTBALL_OBJECT_C_H
#include "types.h"

struct dScMgSmartball_c;
struct S;
struct V2xz;

struct cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 -- mSnapshot = mCurrent */
    virtual void Update();         /* slot 1 -- empty in this class */
    virtual void RestoreInitial(); /* slot 2 -- mCurrent = mInitial */

    /* Recovered helpers. PointInTriangle is the fan-collision predicate every
       child's HitTest calls on `this` (the receiver is never read). SetVel is
       the writer half of the mVelocity pair accessors -- the reader halves
       take (out, obj) order, so they stay free functions in the TU. */
    int  PointInTriangle(int *p, int *q0, int *q1, int *q2);
    /* Callers that keep a corner in a struct pass its address; the layout
       is the same pair of ints, so forward to the array body. */
    int PointInTriangle(S *p, S *q0, S *q1, S *q2) {
        return PointInTriangle((int *)p, (int *)q0, (int *)q1, (int *)q2);
    }
    int PointInTriangle(V2xz *p, V2xz *q0, V2xz *q1, V2xz *q2) {
        return PointInTriangle((int *)p, (int *)q0, (int *)q1, (int *)q2);
    }
    void SetVel(int *v);           /* mVel0/mVel1 = v[0]/v[1] */

    /* The C spelling of this class gets no explicit vtable member because
       no .c file declares it; every reader reaches these fields by raw
       offset today. */
    dScMgSmartball_c *mpManager; /* 0x004 -- the manager that owns this object.
                                    Every child dereferences it as that
                                    dScMgSmartball_c (mgr+0x4664 the index of
                                    the ball in play, mgr+0x4668/0x4688 the
                                    tracked-ball count and table,
                                    mgr+0x4670/0x46bc the kinoko count and
                                    table, mgr+0x595c/0x595d two pause
                                    flags). */
    s32 mCurrent0;    /* 0x008 -- restored from mInitial by RestoreInitial */
    s32 mCurrent1;    /* 0x00c */
    s32 mSnapshot0;   /* 0x010 -- written from mCurrent by SaveSnapshot */
    s32 mSnapshot1;   /* 0x014 */
    s32 mInitial0;    /* 0x018 -- constructor writes the same pair here and
                          at mCurrent */
    s32 mInitial1;    /* 0x01c */
    s32 mVel0;        /* 0x020 -- constructor zeroes it. Velocity, x half:
                          cMgSmartball_ball_c::SaveSnapshot does
                          `mCurrent0 += mVel0; mCurrent1 += mVel1;` and passes
                          &mVel0 to Vec2_Len as a 2-vector. */
    s32 mVel1;        /* 0x024 -- constructor zeroes it; the z half of that
                          pair, and the value cMgSmartball_spring_c eases
                          mCurrent1 back to rest with. */
    s32 mRadius;      /* 0x028 -- constructor zeroes it and each child's own
                          constructor then writes its own constant (0x8000
                          ball, 0x14000 pakkun, 0x18000 board, 0x20000 dokan/
                          propeller/spring, 0x7000 kinoko). Both readers use it
                          as a distance: cMgSmartball_ball_c::SaveSnapshot
                          compares a signed plane distance against -mRadius,
                          and cMgSmartball_kinoko_c eases it to 0 and back to
                          0x7000 as the mushroom squashes, with that class's
                          Update turning it into a render scale. Ten other
                          files in this family read or write it, the
                          most-touched field of the class. */
    s32 mIndex;       /* 0x02c -- the constructor's second argument: this
                          object's slot in the manager's tables.
                          QueueBehind compares it against mgr+0x4664
                          and indexes mgr+0x4688 with it minus one. */
    u8  mIsActive;    /* 0x030 -- constructor sets it to 1. While it is 0
                          cMgSmartball_ball_c's SaveSnapshot and Update return
                          immediately, ArmExpire refuses to arm an
                          expiry, and both cMgSmartball_board_c::SaveSnapshot
                          and cMgSmartball_kinoko_c::SaveSnapshot skip a
                          tracked ball whose byte at 0x30 is 0. */

    cMgSmartball_object_c(dScMgSmartball_c *a1, int a2, int *a3) {
        int lo, hi;
        mpManager = a1;
        mIndex = a2;
        hi = a3[1];
        lo = a3[0];
        mInitial0 = lo;
        mInitial1 = hi;
        hi = a3[1];
        lo = a3[0];
        mCurrent0 = lo;
        mCurrent1 = hi;
        mRadius = 0;
        mVel0 = 0;
        mVel1 = 0;
        mIsActive = 1;
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char cMgSmartball_object_c_size_must_be_0x34[sizeof(cMgSmartball_object_c) == 0x34 ? 1 : -1];
#endif

#endif
