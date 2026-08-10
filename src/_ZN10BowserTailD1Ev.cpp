//cpp
// @symbol _ZN10BowserTailD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The stand-in structs this file used to carry (`struct Actor { char pad[0xd0]; }`,
 * `struct MovingCylinderClsn { char pad[0x4]; }`) were sized to make the offsets
 * come out, not to describe anything. They are gone: the base is the real Actor and
 * the member is the real MovingCylinderClsn, whose own size assertion pins the
 * 0x0d4 the ROM destroys.
 */
#include "BowserTail.h"

BowserTail::~BowserTail()
{
}
