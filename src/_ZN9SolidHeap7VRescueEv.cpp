//cpp
// @symbol _ZN9SolidHeap7VRescueEv
/* recovered: named members + shared header, real C++ method */
#include "SolidHeap.h"
/* SolidHeap::VRescue() at 0x0203c62c -- Heap vtable slot (VRescue).
 * Rescue does nothing; it only ever translates to a direct return. The name is
 * conjectural (it appears to be called just before/by Crash, perhaps to rescue
 * heap data before a fatal exception). No-op override. */

struct SolidHeap;

void SolidHeap::VRescue()
{

}
