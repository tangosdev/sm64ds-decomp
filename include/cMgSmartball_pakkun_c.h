/* class cMgSmartball_pakkun_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed9c, RTTI ov006:0x0213ebe0.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * ADDS A TOGGLE BYTE AND TWO s32 FIELDS. Its allocation is operator new(0x3c)
 * in dScMgSmartball_c::SpawnObjects. The toggle sits at 0x31, in the base's
 * tail padding; the two ints follow at 0x34. All three are read and written
 * only by this child's own SaveSnapshot/Update/RestoreInitial.
 *
 * CONSTRUCTED BY cMgSmartball_pakkun_c_Construct, left a free function for the same
 * reason the base's is (this tree has migrated zero constructors). It calls
 * the base constructor, stores this vtable, and sets the base's mRadius to
 * 0x14000 -- the per-child constant every sibling's constructor also writes,
 * each with its own value. It does not touch mIdleTimer/mActionTimer; they
 * carry whatever the allocator left there until RestoreInitial first zeroes
 * them.
 *
 * SLOT 2 IS A REAL OVERRIDE, NOT A VENEER: it calls the base's
 * RestoreInitial() (qualified -- see the .cpp) and then also zeroes its own
 * fields.
 *
 * mIdleTimer counts scans that find no target and wraps at 0x3c, flipping
 * mTriggered; mActionTimer is set to 0x61 when a target IS found and then
 * counts back down, both spent by SaveSnapshot and read by Update to drive
 * a rotate/scale render while it is nonzero. Neither name is a claim about
 * what the original called them -- see cMgSmartball_object_c.h's own note on
 * why this hierarchy has no names to recover. */
#ifndef CMGSMARTBALL_PAKKUN_C_H
#define CMGSMARTBALL_PAKKUN_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_pakkun_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */
    ~cMgSmartball_pakkun_c() {}

    /* Recovered helper: the ball-vs-mouth region test -- checks the two head
       triangles and the swallow circle while the piranha's state allows. */
    int  CheckBall(int *pos);

    u8 mTriggered;    /* 0x031 -- the mouth toggle: SaveSnapshot flips it,
                          Update picks the idle frame from it */
    s32 mIdleTimer;   /* 0x034 -- SaveSnapshot increments it once per scan
                          that finds no target; at 0x3c it wraps to 0 and
                          flips the mTriggered toggle. RestoreInitial
                          zeroes it. */
    s32 mActionTimer; /* 0x038 -- SaveSnapshot sets it to 0x61 when a target
                          is found; while it is counting down (>0), each
                          later SaveSnapshot call just decrements it instead
                          of scanning, clearing the base's mIsActive active
                          flag when it reaches 0. Update reads it as the
                          countdown driving the rotate/scale render.
                          RestoreInitial zeroes it. */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char cMgSmartball_pakkun_c_size_must_be_0x3c[sizeof(cMgSmartball_pakkun_c) == 0x3c ? 1 : -1];
#endif

#endif
