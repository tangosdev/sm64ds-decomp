/* class cMgSmartball_ana_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed4c, RTTI ov006:0x0213ec4c.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x38, from operator new(0x38) in dScMgSmartball_c::SpawnObjects. The
 * base's data ends at 0x31, so this class adds two flag bytes in the base's
 * tail padding (0x31, 0x32) and a 4-byte countdown at 0x34.
 *
 * WHAT THE ONE FIELD IS. mRespawnTimer is armed with 0x3c -- one second --
 * by Swallow, which cMgSmartball_ball_c::UpdatePhysics (the ball physics)
 * calls on this hole the frame a ball falls into it; the same call raises
 * mHasBall, awards the drop and plays the effect. SaveSnapshot opens
 * with the base's own SaveSnapshot body written out inline (same pattern as
 * every sibling), then ages the timer, and on the frame it reaches zero calls
 * dScMgSmartball_c::ServeNextBall on the manager -- which walks the ball table and puts
 * the first active ball that is not yet in play INTO play -- and sets
 * mServed. So the countdown is the delay between losing a ball down this
 * hole and being served the next one. Update reads it as exactly that: while
 * mHasBall is set it picks one of three warning sprites at the 0x28 and 0x14
 * thresholds, i.e. the countdown is shown to the player. RestoreInitial
 * zeroes it after calling the base's RestoreInitial.
 *
 * CONSTRUCTED BY cMgSmartball_ana_c_Construct, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. */
#ifndef CMGSMARTBALL_ANA_C_H
#define CMGSMARTBALL_ANA_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_ana_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */
    ~cMgSmartball_ana_c() {}

    /* Recovered helpers: HasBall reads mHasBall (a ball has fallen in);
       Swallow is the drop itself -- arms mRespawnTimer, raises mHasBall,
       and awards the 2000-point burst on the manager. */
    int  HasBall();
    void Swallow();

    u8 mHasBall;       /* 0x031 -- a ball has fallen into this hole */
    u8 mServed;        /* 0x032 -- the countdown ran out and the next ball was
                          served */
    s32 mRespawnTimer; /* 0x034 -- frames from a ball dropping into this hole
                           to the next waiting ball being put into play; see
                           the header comment */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char cMgSmartball_ana_c_size_must_be_0x38[sizeof(cMgSmartball_ana_c) == 0x38 ? 1 : -1];
#endif

#endif
