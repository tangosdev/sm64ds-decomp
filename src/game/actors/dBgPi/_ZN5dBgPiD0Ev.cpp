//cpp
// @symbol _ZN5dBgPiD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: store this class's vptr, destroy the dBgPc
 * base sub-object, then return the object to its heap. None of that is
 * written here; declaring `~dBgPi()` is enough, because mwcc emits D2, D0 and
 * D1 together and objisolate keeps the one this file is bound to.
 *
 * The heap call is dBgPi's inline `operator delete` in include/dBgPi.h, which
 * routes to Memory::operator_delete2 (0x0203cbcc) -- the destination the ROM
 * body actually branches to. Without that member mwcc would call the global
 * `_ZdlPv` (0x0203cbf0) instead: same bytes, wrong relocation.
 */
#include "dBgPi.h"

dBgPi::~dBgPi()
{
}
