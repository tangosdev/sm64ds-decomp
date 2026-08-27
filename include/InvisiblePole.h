#ifndef INVISIBLEPOLE_H
#define INVISIBLEPOLE_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* daBar_c in the ROM's RTTI -- the invisible climbing pole. The decomp's own name is
 * kept, as it is for every other actor whose factory names it: InvisiblePole_Spawn
 * builds this class, and that is what makes the name this class's rather than the
 * next one's (see the naming series, #1418 onwards).
 *
 * TWO WITNESSES, and they close on each other exactly:
 *
 *   InvisiblePole_Spawn  fBase_c::operator new(264 = 0x108), dActor_c::dActor_c(),
 *                        stores _ZTV13InvisiblePole, then dCcAc_c at 0xd4.
 *   ~InvisiblePole       the same vtable store, dCcAc_c::~dCcAc_c
 *                        at 0xd4, then dActor_c::~dActor_c.
 *
 * SIZE 0x108, the factory's own literal. dActor_c ends at 0xd0, the one member starts at
 * 0xd4 and dCcAc_c is 0x34, so 0xd4 + 0x34 = 0x108 closes the object with
 * nothing left over. The four bytes at 0xd0 are the only padding this class has, and
 * they are padding because both witnesses skip them.
 *
 * THE VTABLE, at ov002 0x02108480, diffed slot by slot against _ZTV8dActor_c. Five slots
 * differ, and they are the five declared below; every other slot holds dActor_c's own
 * word and is inherited, so it is deliberately NOT redeclared here.
 *
 * No `operator delete` member is needed for the deleting destructor: mwcc inlines it
 * from the class or its IMMEDIATE base, and dActor_c -- this class's immediate base --
 * carries one. See the long note in include/dActor_c.h.
 */
struct InvisiblePole : dActor_c {
    u8                  pad_0d0[0x4];
    dCcAc_c  mClsn;           /* 0x0d4 */

    virtual ~InvisiblePole() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();        /* slot  0 */
    virtual s32  CleanupResources();     /* slot  3 */
    virtual s32  Behavior();             /* slot  6 */
    virtual s32  Render();               /* slot  9 */
    virtual void OnPendingDestroy();     /* slot 12 */
};

typedef char InvisiblePole_size_must_be_0x108[sizeof(InvisiblePole) == 0x108 ? 1 : -1];

#endif /* INVISIBLEPOLE_H */
