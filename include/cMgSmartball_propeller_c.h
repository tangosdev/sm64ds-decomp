/* class cMgSmartball_propeller_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ece8, RTTI
 * ov006:0x0213ec34.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x78, from operator new(0x78) in dScMgSmartball_c::SpawnObjects. Base ends at 0x34, so
 * this class adds 0x44 bytes.
 *
 * ONLY TWO OF THOSE BYTES ARE TOUCHED BY THIS CLASS'S OWN FOUR FUNCTIONS
 * (the constructor, SaveSnapshot, Update, RestoreInitial): an s16 "ease
 * toward target" pair at 0x34/0x36. SaveSnapshot steps 0x36 by +-8 toward
 * the target at 0x34 (clamping once it crosses), then adds the stepped
 * value onto the BASE's field at 0x32 -- so 0x36 is a rotation speed and
 * 0x32 accumulates it into a rotation angle.
 * RestoreInitial zeroes 0x36, sets 0x34 to 0x40 (the default target speed),
 * and (through the base call) zeroes mCurrent. Update never touches 0x34+
 * at all -- it only reads the base's angle (0x32) and position
 * (mCurrent0/mCurrent1) to look up a rotation matrix and draw two sprites.
 *
 * 0x32 IS NOT DECLARED HERE, same as every sibling that reaches it: it
 * belongs to cMgSmartball_object_c (unk_032), reached by a per-reader cast
 * because 0x31-0x33 is a three-byte region with no single type, read three
 * incompatible ways (see cMgSmartball_object_c.h). This class reads it both
 * ways itself: RebuildVerts and SaveSnapshot signed, Update unsigned -- it
 * indexes a sine table with `>> 4` (lsr, not asr) on that field.
 *
 * THE TAIL FROM 0x38 TO 0x77 IS mVertices[8], eight (x, y) pairs. Both
 * readers are in d_s_mg_smartball.cpp: RebuildVerts (called from SaveSnapshot
 * and RestoreInitial) seeds the eight blade-tip offsets, rotates each by the
 * base's angle at 0x32 and adds mCurrent0/mCurrent1; HitTest reads them back
 * as the corners of four triangles -- {0,1,2}, {3,1,2}, {4,5,6}, {7,5,6} --
 * for its PointInTriangle tests. The wing models its own vertex ring the
 * same way (cMgSmartball_wing_c::mVertices).
 *
 * CONSTRUCTED BY cMgSmartball_propeller_c_Construct, left a free function for the same
 * reason the base's is (this tree has migrated zero constructors). It calls
 * the base constructor, stores this vtable, and sets the base's mRadius to
 * 0x20000 -- the per-child constant every sibling's constructor also
 * writes, each with its own value. It does not touch 0x34/0x36 itself; they
 * carry whatever the allocator left there until RestoreInitial first sets
 * them.
 *
 * SaveSnapshot reads mSpinSpeedTarget/mSpinSpeed by name but steps the
 * speed through a local `short *p` taken from &mSpinSpeed; written as a
 * plain member assignment it is 0x9c bytes against the ROM's 0xac. See the
 * function for the measurement. */
#ifndef CMGSMARTBALL_PROPELLER_C_H
#define CMGSMARTBALL_PROPELLER_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_propeller_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    /* Recovered helpers: HitTest runs the ball-vs-blade fan collision (the
       0x38..0x77 tip ring); RebuildVerts rewrites that ring from the base's
       0x32 angle and mCurrent position. */
    int  HitTest(int *pos);
    void RebuildVerts();
    s16 GetAngle() const; /* the base's 0x32 angle, signed; inline in the .cpp */

    s16 mSpinSpeedTarget; /* 0x034 -- the rotation speed mSpinSpeed is eased
                              toward. RestoreInitial sets it to 0x40 and the
                              scene's KickGate negates it, reversing the
                              spin; "target" is what the ease relationship
                              proves. */
    s16 mSpinSpeed;       /* 0x036 -- the live rotation speed. SaveSnapshot
                              steps it by +-8 toward mSpinSpeedTarget, clamping
                              on overshoot, and then ADDS it onto the base's
                              angle at 0x32 every frame -- which is what makes
                              it an angular speed rather than an angle.
                              RestoreInitial zeroes it. */
    struct CollisionVertex {
        s32 x, y;
    };
    CollisionVertex mVertices[8]; /* 0x038..0x077, rebuilt from the angle */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char cMgSmartball_propeller_CollisionVertex_size_must_be_0x8
    [sizeof(cMgSmartball_propeller_c::CollisionVertex) == 0x8 ? 1 : -1];
typedef char cMgSmartball_propeller_c_size_must_be_0x78[sizeof(cMgSmartball_propeller_c) == 0x78 ? 1 : -1];
#endif

#endif
