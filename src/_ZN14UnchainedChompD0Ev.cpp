//cpp
// @symbol _ZN14UnchainedChompD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Nothing below spells the teardown. The three `__destroy_arr` calls, the six
 * Model and six ShadowModel array elements, the sub-object destructors and the
 * dEnemyBase_c chain are all consequences of include/UnchainedChomp.h's typed
 * members; the deallocation is dEnemyBase_c's inline `operator delete`.
 *
 * The identical body stands in _ZN14UnchainedChompD1Ev.cpp: one
 * `~UnchainedChomp() {}` emits D2, D0 and D1 together and objisolate keeps the
 * variant each file is bound to.
 */
#include "UnchainedChomp.h"

UnchainedChomp::~UnchainedChomp()
{
}
