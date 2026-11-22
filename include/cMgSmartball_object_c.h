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
 * destructor pair is what gets migrated first. The destructor is an empty
 * non-virtual inline: SpawnObjects and AfterCleanupResources delete through
 * the child pointer types, so each delete inlines the child's destructor,
 * which rewrites the child's vtable and then this one before operator delete.
 *
 * The constructor takes the incoming pair by value and writes it into both
 * position slots, then zeroes radius and velocity and sets mIsActive. The
 * cartridge keeps one copy (0x0211474c), the C2 every child constructor
 * calls; C1 is deadstripped since nothing builds a bare cMgSmartball_object_c.
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
#include "Vector2.h"

struct dScMgSmartball_c;
struct Vector2;

struct cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 -- mSnapshot = mCurrent */
    virtual void Update();         /* slot 1 -- empty in this class */
    virtual void RestoreInitial(); /* slot 2 -- mCurrent = mInitial */

    /* Recovered helpers. PointInTriangle is the fan-collision predicate every
       child's HitTest calls on `this` (the receiver is never read). SetVel is
       the writer half of the mVelocity pair accessors -- the reader halves
       take (out, obj) order, so they stay free functions in the TU. */
    int PointInTriangle(Vector2 *p, Vector2 *q0, Vector2 *q1, Vector2 *q2);
    void SetVel(Vector2 *v); /* mVel = *v */

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
    Vector2 mCurrent;            /* 0x008 -- restored from mInitial by RestoreInitial */
    Vector2 mSnapshot;           /* 0x010 -- written from mCurrent by SaveSnapshot */
    Vector2 mInitial;            /* 0x018 -- constructor writes the same pair here and
                                    at mCurrent */
    Vector2 mVel;                /* 0x020 -- constructor zeroes it. Velocity:
                                    cMgSmartball_ball_c::SaveSnapshot does
                                    `mCurrent.x += mVel.x; mCurrent.y += mVel.y;`, and
                                    cMgSmartball_spring_c eases mCurrent.y back to rest
                                    with mVel.y. */
    s32 mRadius;                 /* 0x028 -- constructor zeroes it and each child's own
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

    cMgSmartball_object_c(dScMgSmartball_c *manager, int index, Vector2 pos);
    ~cMgSmartball_object_c() {}
};

typedef char cMgSmartball_object_c_size_must_be_0x34[sizeof(cMgSmartball_object_c) == 0x34 ? 1 : -1];

#endif
