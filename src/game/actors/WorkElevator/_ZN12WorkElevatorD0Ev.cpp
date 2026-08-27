//cpp
// @symbol _ZN12WorkElevatorD0Ev
/* recovered: real C++ deleting destructor
 *
 * mwccarm generates D0 from the same source destructor as D1, then resolves
 * the inherited actor delete path. objisolate retains only this file's D0
 * contribution and rebinds its vtable references to the ROM-owned table.
 */
#include "WorkElevator.h"

WorkElevator::~WorkElevator()
{
}
