//cpp
// @symbol _ZN15RecRoomCupboardD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body.
 *
 * D0 is the DELETING destructor: run the destructor -- vptr store, the
 * five-element dCcAcPos_c array destroy at +0xd4, then the dActor_c base step
 * -- and hand the object back to its heap. The deallocation is dActor_c's
 * inline `operator delete`, which is why nothing below mentions a heap.
 */
#include "RecRoomCupboard.h"

RecRoomCupboard::~RecRoomCupboard()
{
}
