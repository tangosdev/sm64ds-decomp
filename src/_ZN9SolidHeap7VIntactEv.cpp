//cpp
// @symbol _ZN9SolidHeap7VIntactEv
/* recovered: named members + shared header, real C++ method */
#include "SolidHeap.h"
/* SolidHeap::VIntact() at 0x0203c654 -- Heap vtable slot (VIntact).
 * Returns true to report the heap as a valid object. (VIntact is not a reliable
 * way to detect a broken heap; this override unconditionally returns 1.) */

struct SolidHeap;

bool SolidHeap::VIntact()
{
    return 1;
}
