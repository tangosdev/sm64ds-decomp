/* class cMgSmartball_wing_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed24, RTTI ov006:0x0213ec40.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x88, from operator new(0x88) in dScMgSmartball_c::SpawnObjects. Base ends at 0x34, so
 * this class adds 0x54 bytes.
 *
 * FIVE CONTROL FIELDS. All five are zeroed by RestoreInitial, but
 * only two of these five control fields are read:
 *
 *   - 0x40 mTriggerCount is incremented by Trigger, which
 *     cMgSmartball_pakkun_c::SaveSnapshot calls on this object (reached as
 *     mgr+0x4780) each time the piranha takes a ball. Three readers agree it
 *     is a count and not a flag: this class's SaveSnapshot eases the angle
 *     only while it is > 0, this class's Update draws the two extra base
 *     sprites only while it is <= 1, and HitTest -- the wing's
 *     collision routine -- runs its body test only while it is <= 1.
 *     Trigger itself branches on the value reaching exactly 1,
 *     playing sound 0x1a1 the first time and 0x1a2 after.
 *   - 0x44 mAngleSettled is raised by IsTableAtRest, the manager's
 *     "is the table at rest" check, once IsSettled reports this
 *     wing done -- which means either it was never triggered or its angle has
 *     reached its 0x3000 limit. Its one reader is this class's SaveSnapshot,
 *     which stops easing the angle while it is set.
 *
 * The other three (0x34, 0x38, 0x3c) are WRITTEN by Trigger on
 * the same call that bumps mTriggerCount -- 0x34 to 0x14 the first time and 0
 * after, 0x3c to 1, 0x38 to 0 -- and zeroed by RestoreInitial. No reader has
 * been identified for these fields. mTriggerTime, mTriggerPhase and
 * mTriggerFlag are provisional names; their runtime roles remain uncertain.
 *
 * ONE FIELD DOES NOT APPEAR BELOW: offset 0x32, a 16-bit angle eased toward
 * 0x3000 by SaveSnapshot and zeroed by RestoreInitial and the constructor,
 * belongs to the BASE -- it sits below 0x34. Migrating this class is what
 * turned it up: the base header used to call bytes 0x31-0x33 padding, and
 * they are not padding (11 files touch 0x31, 43 touch 0x32). The base header
 * now declares all three as u8, because migrating cMgSmartball_board_c
 * showed the region has no single type -- three children read the same three
 * bytes three incompatible ways, and this class's signed 16-bit angle at
 * 0x32-0x33 is one of them. A signed cast from the base's unk_032 byte
 * states this reader's view without asserting it is the region's type.
 *
 * RebuildVerts stores eight collision vertices at 0x48..0x87. The first
 * four form one wing and the last four its mirror; HitTest splits each
 * four-vertex rectangle into two triangles. The three bytes before the
 * vertices are alignment padding. */
#ifndef CMGSMARTBALL_WING_C_H
#define CMGSMARTBALL_WING_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_wing_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    /* Recovered helpers: IsSettled reports the wing done (never triggered or
       angle at its 0x3000 limit); Trigger is the piranha's feed bump;
       RebuildVerts rewrites the 0x48..0x87 fan from the 0x32 angle;
       HitTest is the ball-vs-wing collision routine. */
    /* This reader interprets the existing two bytes as a signed wing angle.
       The base keeps byte storage because other children use different views. */
    s16 GetAngle() const {
        return *(const s16 *)&unk_032;
    }

    int IsSettled();
    void Trigger();
    void RebuildVerts();
    int HitTest(int *pos);

    s32 mTriggerTime;  /* 0x034 -- set to 0x14 on the first trigger and 0 on
                           every later one by Trigger; no reader */
    s32 mTriggerPhase; /* 0x038 -- zeroed by the same call; no reader */
    u8 mTriggerFlag;   /* 0x03c -- set to 1 by the same call; no reader */
    u8 pad_03d[0x3];   /* explicit: the compiler inserts this either way, but
                          an implicit gap is a gap nobody has looked at */
    s32 mTriggerCount; /* 0x040 -- how many times the piranha has fed this
                           wing; see the header comment for its three
                           readers */
    u8 mAngleSettled;  /* 0x044 -- the angle has finished easing. Raised by
                           IsTableAtRest; while set, SaveSnapshot leaves
                           the angle alone */
    u8 pad_045[3];     /* align the collision vertices */
    struct CollisionVertex {
        s32 x, y;
    };
    CollisionVertex mVertices[8]; /* 0x048..0x087, rebuilt from the wing angle */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char cMgSmartball_wing_CollisionVertex_size_must_be_0x8
    [sizeof(cMgSmartball_wing_c::CollisionVertex) == 0x8 ? 1 : -1];
typedef char cMgSmartball_wing_c_size_must_be_0x88[sizeof(cMgSmartball_wing_c) == 0x88 ? 1 : -1];
#endif

#endif
