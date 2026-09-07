//cpp
// @symbol _ZN16BowserShockwavesD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version listed all eight member destructors and the vtable
 * store by hand. Declaring the layout in BowserShockwaves.h instead lets the
 * compiler emit every one of them, and the ROM's order is the evidence that the
 * declaration order is right: 0x200, 0x1ec, 0x1d8, 0x174, 0x160, 0x14c, 0x138,
 * 0x0d4 -- exactly reverse declaration order -- then dActor_c's base-object
 * destructor.
 *
 * Those eight separate calls are also why the two identical four-member groups are
 * eight members and not a two-element array: an array member is torn down through
 * the runtime's array-cleanup helper, not by eight literal offsets.
 */
#include "BowserShockwaves.h"

BowserShockwaves::~BowserShockwaves()
{
}
