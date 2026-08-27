//cpp
// @symbol _ZN12WorkElevatorD1Ev
/* recovered: real C++ complete destructor
 *
 * The four moving-mesh colliders and four models are destroyed in reverse
 * declaration order, then dBgActor_c's inline destructor tears down its own
 * collider/model pair and chains into dActor_c. The compiler owns every vptr
 * store, array teardown, and destructor-variant call below this empty body.
 */
#include "WorkElevator.h"

WorkElevator::~WorkElevator()
{
}
