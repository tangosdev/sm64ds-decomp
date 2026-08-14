/* class cMgSmartball_wing_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed24, RTTI ov006:0x0213ec40.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x88, from _Znwj(0x88) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds 0x54 bytes.
 *
 * offsets 0x34, 0x38, 0x3c, 0x40 and 0x44 are all zeroed by RestoreInitial
 * and read back by SaveSnapshot/Update to gate the wing's flap/angle-ease
 * behaviour; nothing beyond "state used for that gating" is evidenced, so
 * they keep unk_ names.
 *
 * ONE FIELD DOES NOT APPEAR BELOW: offset 0x32, a 16-bit angle eased toward
 * 0x3000 by SaveSnapshot and zeroed by RestoreInitial and the constructor,
 * belongs to the BASE -- it sits below 0x34. Migrating this class is what
 * turned it up: the base header used to call bytes 0x31-0x33 padding, and
 * they are not padding (11 files touch 0x31, 43 touch 0x32). That is fixed
 * in cMgSmartball_object_c.h now, where the contested signedness is
 * recorded. This class still reaches the byte with an explicit
 * `(char*)this + 0x32` cast rather than the base's name, because the name's
 * type is not yet settled and the cast states exactly what the ROM does.
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

    s32 unk_034;
    s32 unk_038;
    u8  unk_03c;
    u8  pad_03d[0x3];  /* explicit: the compiler inserts this either way, but
                          an implicit gap is a gap nobody has looked at */
    s32 unk_040;
    u8  unk_044;
    u8  pad_045[0x43]; /* 0x45..0x87 -- unmodelled, not unread; see above */
};

typedef char cMgSmartball_wing_c_size_must_be_0x88[sizeof(cMgSmartball_wing_c) == 0x88 ? 1 : -1];

#endif
