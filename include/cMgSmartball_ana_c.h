/* class cMgSmartball_ana_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed4c, RTTI ov006:0x0213ec4c.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x38, from _Znwj(0x38) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds exactly one field: a 4-byte countdown at 0x34.
 *
 * WHAT THE ONE FIELD IS. mRespawnTimer is armed with 0x3c -- one second --
 * by func_ov006_021115cc, which src/unnamed/ov006/0211/func_ov006_02112ad8.c (the ball physics)
 * calls on this hole the frame a ball falls into it; the same call raises the
 * base's unk_031, awards the drop and plays the effect. SaveSnapshot opens
 * with the base's own SaveSnapshot body written out inline (same pattern as
 * every sibling), then ages the timer, and on the frame it reaches zero calls
 * func_ov006_02114ec0 on the manager -- which walks the ball table and puts
 * the first active ball that is not yet in play INTO play -- and sets the
 * base's unk_032. So the field is the delay between losing a ball down this
 * hole and being served the next one. Update reads it as exactly that: while
 * unk_031 is set it picks one of three warning sprites at the 0x28 and 0x14
 * thresholds, i.e. the countdown is shown to the player. RestoreInitial
 * zeroes it after calling the base's RestoreInitial.
 *
 * OFFSETS 0x31 AND 0x32 ARE NOT DECLARED HERE even though this class's own
 * functions touch them -- they belong to cMgSmartball_object_c (unk_031,
 * unk_032), not to this class's 0x34+ region. Both are reached through the
 * base's member names, and every access here is a byte.
 *
 * That is a change from how this class first landed. The base then declared
 * 0x32 as a 16-bit field, so this class's byte-wide 0/1 write had to go
 * through a raw `(char*)this + 0x32` cast to avoid misstating the emitted
 * instruction. Migrating cMgSmartball_board_c showed the region is three
 * independent bytes read three incompatible ways, the base was corrected to
 * three u8s, and this class's view now agrees with the declaration. Siblings
 * that read a 16-bit angle across 0x32-0x33 still need their casts.
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

    s32 mRespawnTimer; /* 0x034 -- frames from a ball dropping into this hole
                           to the next waiting ball being put into play; see
                           the header comment */
};

typedef char cMgSmartball_ana_c_size_must_be_0x38[sizeof(cMgSmartball_ana_c) == 0x38 ? 1 : -1];

#endif
