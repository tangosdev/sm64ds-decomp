/* class cMgSmartball_wing_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed24, RTTI ov006:0x0213ec40.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x88, from _Znwj(0x88) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds 0x54 bytes.
 *
 * FIVE FIELDS, TWO OF THEM NAMED. All five are zeroed by RestoreInitial, but
 * only two are ever READ, and neither reader is in this class:
 *
 *   - 0x40 mTriggerCount is incremented by func_ov006_0210d8bc, which
 *     cMgSmartball_pakkun_c::SaveSnapshot calls on this object (reached as
 *     mgr+0x4780) each time the piranha takes a ball. Three readers agree it
 *     is a count and not a flag: this class's SaveSnapshot eases the angle
 *     only while it is > 0, this class's Update draws the two extra base
 *     sprites only while it is <= 1, and func_ov006_0210dbb0 -- the wing's
 *     collision routine -- runs its body test only while it is <= 1.
 *     func_ov006_0210d8bc itself branches on the value reaching exactly 1,
 *     playing sound 0x1a1 the first time and 0x1a2 after.
 *   - 0x44 mAngleSettled is raised by func_ov006_021156f8, the manager's
 *     "is the table at rest" check, once func_ov006_0210d898 reports this
 *     wing done -- which means either it was never triggered or its angle has
 *     reached its 0x3000 limit. Its one reader is this class's SaveSnapshot,
 *     which stops easing the angle while it is set.
 *
 * The other three (0x34, 0x38, 0x3c) are WRITTEN by func_ov006_0210d8bc on
 * the same call that bumps mTriggerCount -- 0x34 to 0x14 the first time and 0
 * after, 0x3c to 1, 0x38 to 0 -- and zeroed by RestoreInitial. NOTHING IN THE
 * TREE READS THEM BACK, so they keep unk_ names: the shape suggests a timer,
 * a flag and a counter, but a suggestion is not evidence.
 *
 * ONE FIELD DOES NOT APPEAR BELOW: offset 0x32, a 16-bit angle eased toward
 * 0x3000 by SaveSnapshot and zeroed by RestoreInitial and the constructor,
 * belongs to the BASE -- it sits below 0x34. Migrating this class is what
 * turned it up: the base header used to call bytes 0x31-0x33 padding, and
 * they are not padding (11 files touch 0x31, 43 touch 0x32). The base header
 * now declares all three as u8, because migrating cMgSmartball_board_c
 * showed the region has no single type -- three children read the same three
 * bytes three incompatible ways, and this class's signed 16-bit angle at
 * 0x32-0x33 is one of them. The explicit `(char*)this + 0x32` cast stays for
 * exactly that reason: it states this reader's view without asserting it is
 * the region's type.
 *
 * THE TAIL FROM 0x45 TO 0x87 IS AN EXPLICIT PAD, and it is UNMODELLED, NOT
 * UNREAD. func_ov006_0210d93c -- a free helper called from both
 * SaveSnapshot and RestoreInitial -- reinterprets `this` through its own
 * local struct and read-modify-writes eight (int x, int y) pairs at
 * 0x48..0x87 (its `v[8]`), plus reads mCurrent0/mCurrent1 and the 0x32
 * angle. That evidence lives in the helper's own file, not in this class's
 * methods, so this header does not assert field names for it -- the range
 * is a real gap in what THIS file models, not a gap in the ROM's use of the
 * bytes. */
#ifndef CMGSMARTBALL_WING_C_H
#define CMGSMARTBALL_WING_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_wing_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034;       /* 0x034 -- set to 0x14 on the first trigger and 0 on
                           every later one by func_ov006_0210d8bc; no reader */
    s32 unk_038;       /* 0x038 -- zeroed by the same call; no reader */
    u8  unk_03c;       /* 0x03c -- set to 1 by the same call; no reader */
    u8  pad_03d[0x3];  /* explicit: the compiler inserts this either way, but
                          an implicit gap is a gap nobody has looked at */
    s32 mTriggerCount; /* 0x040 -- how many times the piranha has fed this
                           wing; see the header comment for its three
                           readers */
    u8  mAngleSettled; /* 0x044 -- the angle has finished easing. Raised by
                           func_ov006_021156f8; while set, SaveSnapshot leaves
                           the angle alone */
    u8  pad_045[0x43]; /* 0x45..0x87 -- unmodelled, not unread; see above */
};

typedef char cMgSmartball_wing_c_size_must_be_0x88[sizeof(cMgSmartball_wing_c) == 0x88 ? 1 : -1];

#endif
