//cpp
// @symbol _ZN13ExpandingHeap7VRescueEv
/* recovered: named members + shared header, real C++ method */
#include "ExpandingHeap.h"
/* ExpandingHeap::VRescue() at 0x0203c630 -- Heap vtable slot (VRescue).
 * Rescue does nothing; it only ever translates to a direct return. The name is
 * conjectural (it appears to be called just before/by Crash, perhaps to rescue
 * heap data before a fatal exception). No-op override. */

struct ExpandingHeap;

void ExpandingHeap::VRescue()
{

}
