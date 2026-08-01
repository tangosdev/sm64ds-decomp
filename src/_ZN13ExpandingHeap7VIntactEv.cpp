//cpp
// @symbol _ZN13ExpandingHeap7VIntactEv
/* recovered: named members + shared header, real C++ method */
#include "ExpandingHeap.h"
/* ExpandingHeap::VIntact() at 0x0203c65c -- Heap vtable slot (VIntact).
 * Returns true to report the heap as a valid object. (VIntact is not a reliable
 * way to detect a broken heap; this override unconditionally returns 1.) */

struct ExpandingHeap;

bool ExpandingHeap::VIntact()
{
    return 1;
}
