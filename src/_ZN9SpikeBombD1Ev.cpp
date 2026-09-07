//cpp
// @symbol _ZN9SpikeBombD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The stand-in structs this file used to carry (`struct dActor_c { char pad[0xd0]; }`,
 * `struct Model { char pad[0x50]; }`, and a 4-byte `dCcAcPos_c`
 * sized to nothing in particular) are gone. The base is the real dActor_c and both
 * members are their real classes, each pinned by its own size assertion -- which
 * is what turned the 0x4b bytes of padding behind the old marker at 0x124 into
 * the dCcAcPos_c it always was.
 *
 * The ROM destroys 0x124 then 0x0d4 -- reverse declaration order -- then chains to
 * dActor_c. That order is the evidence the declaration order is right.
 */
#include "SpikeBomb.h"

SpikeBomb::~SpikeBomb()
{
}
