//cpp
// @symbol _ZN7BooCageD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is D1's work plus the deallocation. Both come from the same source form:
 * one `BooCage::~BooCage() {}` makes mwcc emit D2, D0 and D1 together and
 * objisolate keeps the variant this file is bound to, which is why the identical
 * body stands in _ZN7BooCageD1Ev.cpp. See include/BooCage.h for the layout
 * witnesses -- the four typed members are what produce the destroy chain.
 *
 * The deallocation is dEnemyBase_c's inline `operator delete`
 * (include/dEnemyBase_c.h): without it mwcc would call the GLOBAL _ZdlPv where
 * the ROM calls Memory::Deallocate, one relocated word that every isolated byte
 * gate wildcards and only the relink sees.
 */
#include "BooCage.h"

BooCage::~BooCage()
{
}
