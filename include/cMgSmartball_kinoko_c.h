/* class cMgSmartball_kinoko_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed88, RTTI ov006:0x0213ec10
 * (_ZTI21cMgSmartball_kinoko_c), _ZTS21cMgSmartball_kinoko_c ov006:0x0213ee80.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x40, from _Znwj(0x40) in src/unnamed/ov006/0211/func_ov006_02115b0c.c. Base ends at
 * 0x34, so this class adds 0x0c bytes -- one of the smallest children.
 *
 * FIELD EVIDENCE. SaveSnapshot (func_ov006_0211192c) and Update
 * (func_ov006_021117bc) are the only two functions that touch anything past
 * 0x34; RestoreInitial (func_ov006_02111b20) zeroes mWasHit and mHitTimer but,
 * read exhaustively, says nothing at all about mVariant -- so this header
 * does not invent a reset for it.
 *
 * mVariant says WHICH KIND of mushroom this is, not what it is doing. It is
 * written exactly twice in the whole tree, both by the spawner
 * func_ov006_02115b0c, which walks two different tilemap layers looking for
 * the same tile id 0x328: the objects it builds from the first layer get 0,
 * the ones from the second get 1. The constructor func_ov006_02111b40 leaves
 * 2 behind, which is the value Update has no case for -- a kinoko the spawner
 * never claimed renders nothing. Update switches on it and SaveSnapshot only
 * reads it; nothing ever assigns it at runtime.
 *
 * mWasHit is raised by func_ov006_02112190 -- the ball-vs-kinoko proximity
 * test -- on the object it found a hit against, reached through the manager's
 * kinoko table at mgr+0x46bc (count at mgr+0x4670). SaveSnapshot consumes it:
 * on the frame it sees the flag it fires the effect and sound (only if
 * mHitTimer had drained), clears the flag, and re-arms mHitTimer -- 0x100
 * frames for a variant-1 mushroom, 4 for any other. RestoreInitial zeroes it;
 * the constructor does not touch it.
 *
 * mHitTimer is how long the mushroom stays down. SaveSnapshot ages it while
 * positive and, in its last four frames, sweeps the manager's ball table at
 * mgr+0x4688 for a live ball within 0x11000 of this mushroom -- if one is
 * still resting on it, the timer is re-armed to 4 and it stays down. Its
 * value is also what drives the base's mRadius for a variant-1 mushroom:
 * eased down by 0x1000 a frame to 0 while the timer runs, back up to 0x7000
 * once it drains, which is the squash Update turns into a render scale.
 * RestoreInitial zeroes it; the constructor does not touch it -- both mWasHit
 * and mHitTimer carry whatever the allocator left there until the first
 * RestoreInitial call.
 *
 * FIELD NAMES and their evidence are tabulated in
 * notes/smartball-provenance.md.
 *
 * pad_039 is a pure alignment gap between the u8 at 0x38 and the s32 at
 * 0x3c -- neither SaveSnapshot, Update, nor RestoreInitial's zero pass ever
 * touches it (house style: explicit pad over an implicit compiler-inserted
 * one).
 *
 * CONSTRUCTED BY func_ov006_02111b40, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. It sets the base's mRadius to 0x7000 and mVariant to 2;
 * it does not touch mWasHit or mHitTimer. */
#ifndef CMGSMARTBALL_KINOKO_C_H
#define CMGSMARTBALL_KINOKO_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_kinoko_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 mVariant;     /* 0x034 -- which tilemap layer spawned this mushroom
                          (0 or 1), or 2 for one the spawner never claimed;
                          see the header comment */
    u8  mWasHit;      /* 0x038 -- a ball touched this mushroom; raised by
                          func_ov006_02112190, consumed by SaveSnapshot */
    u8  pad_039[0x3]; /* 0x039-0x03b -- alignment; see header comment */
    s32 mHitTimer;    /* 0x03c -- frames the mushroom stays down, re-armed
                          while a ball is still resting on it */
};

typedef char cMgSmartball_kinoko_c_size_must_be_0x40[sizeof(cMgSmartball_kinoko_c) == 0x40 ? 1 : -1];

#endif
