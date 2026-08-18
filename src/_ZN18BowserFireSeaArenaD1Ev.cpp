//cpp
// @symbol _ZN18BowserFireSeaArenaD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version wrote the vptr TWICE and called four member destructors
 * by hand. Both vptr stores are what a derived destructor does: set your own, tear
 * down your own members, then run the base's destructor, which sets ITS vptr and
 * tears down ITS members. Declaring `BowserFireSeaArena : dBgActor_c` lets the
 * compiler emit all of it, and reproducing the second store is the check that the
 * base is right -- an unrelated class with four members at those offsets would emit
 * one vptr store, not two.
 *
 * The ROM destroys 0x374, 0x324, then (as dBgActor_c) 0x124, 0x0d4, then chains to
 * dActor_c -- exactly reverse declaration order through both classes.
 */
#include "BowserFireSeaArena.h"

BowserFireSeaArena::~BowserFireSeaArena()
{
}
