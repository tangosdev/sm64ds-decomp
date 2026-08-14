/* class cMgSmartball_dokan_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ecac, RTTI ov006:0x0213ebf8.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * ADDS NO FIELDS. Its allocation is _Znwj(0x34) in func_ov006_02115b0c, and
 * 0x34 is exactly the base's size: this class is three overrides and nothing
 * else. That is not an assumption from an empty field scan -- the literal
 * says so, and no code in the family touches an offset >= 0x34 through a
 * dokan pointer.
 *
 * CONSTRUCTED BY func_ov006_02110bc0, left a free function for the same
 * reason the base's is (this tree has migrated zero constructors). It calls
 * the base constructor, stores this vtable, and sets the base's unk_028 to
 * 0x20000 -- the per-child constant every sibling's constructor also writes,
 * each with its own value. */
#ifndef CMGSMARTBALL_DOKAN_C_H
#define CMGSMARTBALL_DOKAN_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_dokan_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 -- see the .cpp: a tail call */
};

typedef char cMgSmartball_dokan_c_size_must_be_0x34[sizeof(cMgSmartball_dokan_c) == 0x34 ? 1 : -1];

#endif
