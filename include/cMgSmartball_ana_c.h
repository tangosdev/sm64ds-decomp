/* class cMgSmartball_ana_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed4c, RTTI ov006:0x0213ec4c.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x38, from _Znwj(0x38) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds exactly one field: a 4-byte countdown at 0x34.
 *
 * SaveSnapshot opens with the base's own SaveSnapshot body written out
 * inline (same pattern as every sibling), then decrements the countdown;
 * when it reaches zero it calls the shared helper func_ov006_02114ec0 and
 * sets a flag. Update reads the countdown to pick a warning sprite once a
 * base flag is set. RestoreInitial zeroes the countdown after calling the
 * base's RestoreInitial.
 *
 * OFFSETS 0x31 AND 0x32 ARE NOT DECLARED HERE even though this class's own
 * functions touch them -- they belong to cMgSmartball_object_c (unk_031,
 * unk_032), not to this class's 0x34+ region. The two are reached
 * DIFFERENTLY, and the difference is deliberate. 0x31 goes through the
 * base's member name: the base declares it u8 and every access here is a
 * byte, so the name states exactly the right thing. 0x32 keeps a raw
 * `(char*)this + 0x32` cast, because the base declares it 16 bits wide --
 * the more common reading among its 43 touching files -- while this class
 * only ever writes a u8 0/1 into its low byte. The name would misstate the
 * instruction there; the cast says what the ROM does. See
 * cMgSmartball_object_c.h on that field's contested width.
 *
 * CONSTRUCTED BY func_ov006_02111774, left a free function per the recipe
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

    s32 unk_034; /* 0x034 -- countdown; SaveSnapshot decrements it, RestoreInitial zeroes it */
};

typedef char cMgSmartball_ana_c_size_must_be_0x38[sizeof(cMgSmartball_ana_c) == 0x38 ? 1 : -1];

#endif
