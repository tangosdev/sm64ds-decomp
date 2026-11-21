/* class cMgSmartball_dokan_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ecac, RTTI ov006:0x0213ebf8.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * ADDS NO FIELDS. Its allocation is operator new(0x34) in dScMgSmartball_c::SpawnObjects, and
 * 0x34 is exactly the base's size: this class is three overrides and nothing
 * else. That is not an assumption from an empty field scan -- the literal
 * says so, and no code in the family touches an offset >= 0x34 through a
 * dokan pointer.
 *
 * CONSTRUCTED BY the real cMgSmartball_dokan_c(dScMgSmartball_c *, int, Vector2)
 * constructor; the cartridge keeps its C1, the C2 is deadstripped. It calls
 * the base constructor, stores this vtable, and sets the base's mRadius to
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
    cMgSmartball_dokan_c(dScMgSmartball_c *manager, int index, Vector2 pos);
    ~cMgSmartball_dokan_c() {}

    /* Recovered helper: the ball-vs-pipe-mouth proximity test -- inside the
       x band and the z window the mouth opens on. */
    int  CheckBall(int *pos);
};

typedef char cMgSmartball_dokan_c_size_must_be_0x34[sizeof(cMgSmartball_dokan_c) == 0x34 ? 1 : -1];

#endif
