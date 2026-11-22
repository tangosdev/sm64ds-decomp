/* class cMgSmartball_pushswitch_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ed38,
 * RTTI ov006:0x0213ec1c (_ZTI25cMgSmartball_pushswitch_c),
 * _ZTS25cMgSmartball_pushswitch_c ov006:0x0213eeb4.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x38, from operator new(0x38) in dScMgSmartball_c::SpawnObjects. The
 * base's data ends at 0x31, so this class adds a pressed flag in the base's
 * tail padding (0x31) and a 4-byte countdown at 0x34.
 *
 * mReleaseDelay is how long the switch stays down once nothing is holding it.
 * CheckBall -- the ball-vs-switch proximity test -- is what presses
 * the switch: it sets mTriggered, arms this field with 0xa, starts the
 * slot machine (StartSpin on mgr+0x4778) and plays sound 0x1a4.
 * SaveSnapshot then decrements it while positive and, only while mTriggered is
 * set, calls BallsClear, which returns 1 exactly when NO active ball
 * is within 0x12000 of the switch. Clear and drained to zero -> mTriggered is
 * cleared and the switch pops back out; a ball still near -> the field is
 * reloaded to 0xa, so the ten frames are counted from the last frame a ball
 * was on it, not from the press. RestoreInitial zeroes it, read exhaustively,
 * and says nothing more.
 *
 * CONSTRUCTED BY the real cMgSmartball_pushswitch_c(dScMgSmartball_c *, int, Vector2)
 * constructor; the cartridge keeps its C1, the C2 is deadstripped. It calls
 * the base constructor, stores this vtable, and sets the base's mRadius to
 * 0x8000 -- the per-child constant every sibling
 * constructor also writes, each with its own value. It does not touch
 * mReleaseDelay; the field carries whatever the allocator left there until the
 * first RestoreInitial call. */
#ifndef CMGSMARTBALL_PUSHSWITCH_C_H
#define CMGSMARTBALL_PUSHSWITCH_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_pushswitch_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */
    cMgSmartball_pushswitch_c(dScMgSmartball_c *manager, int index, Vector2 pos);
    ~cMgSmartball_pushswitch_c() {}

    /* Recovered helpers: BallsClear is the release check SaveSnapshot runs
       (1 when no active ball is within 0x12000); CheckBall is the
       ball-vs-switch proximity test that presses the switch and starts the
       slot machine. */
    int  BallsClear();
    int  CheckBall(int *pos);

    u8 mTriggered;     /* 0x031 -- the switch is pressed; Update draws the
                          pressed sprite while it is set */
    s32 mReleaseDelay; /* 0x034 -- how long the switch stays down after the
                           last ball leaves it; see the header comment */
};

typedef char cMgSmartball_pushswitch_c_size_must_be_0x38[sizeof(cMgSmartball_pushswitch_c) == 0x38 ? 1 : -1];

#endif
