//cpp
// @symbol _ZN17BowserSkyPlatformD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The stand-in structs this file used to carry (`struct Actor { char pad[0xd0]; }`,
 * `struct Model { char pad[0x50]; }`, and a 4-byte `MovingCylinderClsnWithPos`
 * sized to nothing in particular) are gone. The base is the real Actor and both
 * members are their real classes, each pinned by its own size assertion -- which
 * is what turned the 0x4b bytes of padding behind the old marker at 0x124 into
 * the MovingCylinderClsnWithPos it always was.
 *
 * The ROM destroys 0x124 then 0x0d4 -- reverse declaration order -- then chains to
 * Actor. That order is the evidence the declaration order is right.
 */
#include "BowserSkyPlatform.h"

BowserSkyPlatform::~BowserSkyPlatform()
{
}
