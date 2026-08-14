/* class cMgSmartball_pushswitch_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ed38,
 * RTTI ov006:0x0213ec1c (_ZTI25cMgSmartball_pushswitch_c),
 * _ZTS25cMgSmartball_pushswitch_c ov006:0x0213eeb4.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x38, from _Znwj(0x38) in src/func_ov006_02115b0c.c. Base ends at
 * 0x34, so this class adds exactly one field: a 4-byte countdown at 0x34.
 *
 * unk_034 is a countdown gating how long the switch stays pushed.
 * SaveSnapshot decrements it while positive, then -- only while the base's
 * unk_031 flag is set -- calls the out-of-scope helper func_ov006_0210e120
 * (an escape/proximity check over the actor list); if it returns true and
 * the countdown has reached zero, unk_031 is cleared (the switch pops back
 * out), otherwise the countdown is reloaded to 0xa. RestoreInitial zeroes
 * it, read exhaustively, and says nothing more.
 *
 * THIS CLASS ALSO TOUCHES THE BASE'S 0x31 BYTE, and does so through the
 * base's own member name in every access: SaveSnapshot/RestoreInitial read
 * and write it as a plain byte, and Update reads it as an ARRAY INDEX
 * (data_ov006_02137a9c[unk_031]) -- still a single-byte load, so the access
 * width matches the base's declared u8 and the name states exactly what the
 * ROM does. No raw cast is needed here (unlike wing_c/ana_c's 0x32).
 *
 * CONSTRUCTED BY func_ov006_0210e480, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. It calls the base constructor, stores this vtable, and
 * sets the base's unk_028 to 0x8000 -- the per-child constant every sibling
 * constructor also writes, each with its own value. It does not touch
 * unk_034; the field carries whatever the allocator left there until the
 * first RestoreInitial call. */
#ifndef CMGSMARTBALL_PUSHSWITCH_C_H
#define CMGSMARTBALL_PUSHSWITCH_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_pushswitch_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034; /* 0x034 -- countdown; see header comment */
};

typedef char cMgSmartball_pushswitch_c_size_must_be_0x38[sizeof(cMgSmartball_pushswitch_c) == 0x38 ? 1 : -1];

#endif
