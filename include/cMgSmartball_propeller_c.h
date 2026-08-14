/* class cMgSmartball_propeller_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ece8, RTTI
 * ov006:0x0213ec34.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x78, from _Znwj(0x78) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds 0x44 bytes.
 *
 * ONLY TWO OF THOSE BYTES ARE TOUCHED BY THIS CLASS'S OWN FOUR FUNCTIONS
 * (the constructor, SaveSnapshot, Update, RestoreInitial): an s16 "ease
 * toward target" pair at 0x34/0x36. SaveSnapshot steps 0x36 by +-8 toward
 * the target at 0x34 (clamping once it crosses), then adds the stepped
 * value onto the BASE's own contested-width field at 0x32 -- so 0x36 is a
 * rotation speed and 0x32 accumulates it into a rotation angle.
 * RestoreInitial zeroes 0x36, sets 0x34 to 0x40 (the default target speed),
 * and (through the base call) zeroes mCurrent. Update never touches 0x34+
 * at all -- it only reads the base's angle (0x32) and position
 * (mCurrent0/mCurrent1) to look up a rotation matrix and draw two sprites.
 *
 * 0x32 IS NOT DECLARED HERE, same as every sibling that reaches it: it
 * belongs to cMgSmartball_object_c (unk_032), reached with the base's own
 * raw `(char*)this + 0x32` idiom because the base's declared width for that
 * field is contested (see cMgSmartball_object_c.h). This class's own
 * Update is in fact one of the files that resolved the base header's note --
 * it indexes a sine table with `>> 4` (lsr, not asr) on that field, i.e. an
 * UNSIGNED reading, one of the several conflicting accesses the base header
 * already records.
 *
 * THE TAIL FROM 0x38 TO 0x77 IS AN EXPLICIT PAD, and it is UNMODELLED, NOT
 * UNREAD. func_ov006_02110e28 -- a free helper called from both
 * SaveSnapshot and RestoreInitial, and not touched by this migration --
 * reinterprets `this` through its own local struct and read-modify-writes
 * eight (int x, int y) pairs there (its `v[8]`), a ring of propeller-blade
 * tip positions offset by the base's mCurrent0/mCurrent1 and rotated by the
 * base's angle at 0x32. That evidence lives in the helper's own file, which
 * this migration does not touch, so this header does not assert field names
 * for the range -- the gap is in what THIS file models, not in the ROM's
 * use of the bytes.
 *
 * CONSTRUCTED BY func_ov006_02111220, left a free function for the same
 * reason the base's is (this tree has migrated zero constructors). It calls
 * the base constructor, stores this vtable, and sets the base's unk_028 to
 * 0x20000 -- the per-child constant every sibling's constructor also
 * writes, each with its own value. It does not touch 0x34/0x36 itself; they
 * carry whatever the allocator left there until RestoreInitial first sets
 * them.
 *
 * SaveSnapshot's own .cpp does NOT use unk_034/unk_036 by name, even though
 * they are declared below and Update/RestoreInitial use them freely: named
 * member access there measurably changed codegen. Compiled with the named
 * members it was 0x98 bytes against a required 0xac -- mwcc CSEs the
 * repeated `this->unk_036` reads/writes into fewer reloads than the ROM's
 * three separately-cast `*(short*)(c+0x36)` expressions allow. The .cpp
 * keeps the original's raw-offset shape (a local `char* c = (char*)this`
 * and explicit casts) to reproduce that; see the file for the measurement
 * and cMgSmartball_object_c.h's own note on why this hierarchy is a
 * rename-and-reshape job that still has to stop reshaping the moment the
 * bytes move. */
#ifndef CMGSMARTBALL_PROPELLER_C_H
#define CMGSMARTBALL_PROPELLER_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_propeller_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s16 unk_034;       /* 0x034 -- target rotation speed; RestoreInitial
                           sets it to 0x40 */
    s16 unk_036;       /* 0x036 -- current rotation speed, eased toward
                           unk_034 by +-8 per SaveSnapshot call and added
                           onto the base's angle (unk_032) each time;
                           zeroed by RestoreInitial */
    u8  pad_038[0x40]; /* 0x038..0x077 -- unmodelled, not unread; see
                           above */
};

typedef char cMgSmartball_propeller_c_size_must_be_0x78[sizeof(cMgSmartball_propeller_c) == 0x78 ? 1 : -1];

#endif
