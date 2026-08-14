/* class cMgSmartball_kinoko_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed88, RTTI ov006:0x0213ec10
 * (_ZTI21cMgSmartball_kinoko_c), _ZTS21cMgSmartball_kinoko_c ov006:0x0213ee80.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x40, from _Znwj(0x40) in src/func_ov006_02115b0c.c. Base ends at
 * 0x34, so this class adds 0x0c bytes -- one of the smallest children.
 *
 * FIELD EVIDENCE. SaveSnapshot (func_ov006_0211192c) and Update
 * (func_ov006_021117bc) are the only two functions that touch anything past
 * 0x34; RestoreInitial (func_ov006_02111b20) zeroes unk_038 and unk_03c but,
 * read exhaustively, says nothing at all about unk_034 -- so this header
 * does not invent a reset for it.
 *
 * unk_034 is a 3-way state selector: Update switches on it (0 and 1 each
 * draw a different OAM pair; there is no case for 2, so state 2 renders
 * nothing). The constructor (func_ov006_02111b40) sets it to 2 as the
 * initial dormant state; SaveSnapshot only ever reads it.
 *
 * unk_038 is a 0/1 flag: when SaveSnapshot finds it set, it runs a one-shot
 * transition (an effect/sound trigger, gated on unk_03c still being 0),
 * clears the flag, and reloads unk_03c from unk_034 (0x100 if unk_034 == 1,
 * else 4). RestoreInitial zeroes it; the constructor does not touch it.
 *
 * unk_03c is a countdown SaveSnapshot decrements while positive; once it
 * drops under 4 it scans a fixed actor list for a hit within a fixed-point
 * distance of 0x11000 and, on a hit, resets itself to 4. It also gates a
 * base unk_028 ease toward 0x7000 or 0. RestoreInitial zeroes it; the
 * constructor does not touch it -- both unk_038 and unk_03c carry whatever
 * the allocator left there until the first RestoreInitial call.
 *
 * pad_039 is a pure alignment gap between the u8 at 0x38 and the s32 at
 * 0x3c -- neither SaveSnapshot, Update, nor RestoreInitial's zero pass ever
 * touches it (house style: explicit pad over an implicit compiler-inserted
 * one).
 *
 * CONSTRUCTED BY func_ov006_02111b40, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. It sets the base's unk_028 to 0x7000 and unk_034 to 2;
 * it does not touch unk_038 or unk_03c. */
#ifndef CMGSMARTBALL_KINOKO_C_H
#define CMGSMARTBALL_KINOKO_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_kinoko_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034;      /* 0x034 -- state selector; see header comment */
    u8  unk_038;      /* 0x038 -- one-shot transition flag */
    u8  pad_039[0x3]; /* 0x039-0x03b -- alignment; see header comment */
    s32 unk_03c;      /* 0x03c -- countdown / proximity-scan gate */
};

typedef char cMgSmartball_kinoko_c_size_must_be_0x40[sizeof(cMgSmartball_kinoko_c) == 0x40 ? 1 : -1];

#endif
