//cpp
// @symbol _ZN6BowserD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The stand-in structs this file used to carry -- `struct Actor { char pad[0xd0]; }`
 * and five members sized 0x64, 0x14, 0x1bc, 0x58, 0x4 -- are gone. The base is the
 * real Actor and every member is its real class, each pinned by its own size
 * assertion. One of those stand-ins was also wrong: ShadowModel is 0x28, not the
 * 0x58 this file claimed, and it only worked because nothing downstream of it was
 * being computed.
 *
 * The ROM destroys 0x360, 0x308, 0x14c, 0x138, 0x0d4 -- exactly reverse declaration
 * order -- then chains to Actor. That order is the evidence the declaration order
 * is right.
 */
#include "Bowser.h"

Bowser::~Bowser()
{
}
