//cpp
// @symbol _ZN10BowserTailD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The stand-in structs this file used to carry (`struct dActor_c { char pad[0xd0]; }`,
 * `struct dCcAc_c { char pad[0x4]; }`) were sized to make the offsets
 * come out, not to describe anything. They are gone: the base is the real dActor_c and
 * the member is the real dCcAc_c, whose own size assertion pins the
 * 0x0d4 the ROM destroys.
 */
#include "BowserTail.h"

BowserTail::~BowserTail()
{
}
