//cpp
// @symbol _ZN8CccArenaD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Own vptr, then dBgActor_c's -- inlined, because dBgActor_c's destructor
 * is defined in its class body -- then dBgActor_c's own dBgW_Kc and
 * Model, then dActor_c. This class adds no member with a destructor of
 * its own (see include/CccArena.h).
 */
#include "CccArena.h"

CccArena::~CccArena()
{
}
