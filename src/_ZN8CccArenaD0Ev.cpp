//cpp
// @symbol _ZN8CccArenaD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole
 * body. Declaring `~CccArena()` is enough; mwcc emits D2, D0 and D1
 * together and objisolate keeps the one this file is bound to. The
 * deallocation is an inline operator delete, inherited from dActor_c. */
#include "CccArena.h"

CccArena::~CccArena()
{
}
