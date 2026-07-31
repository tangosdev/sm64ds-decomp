//cpp
// @symbol _ZN13ExpandingHeap12VResizeToFitEv
/* recovered: named members + shared header, real C++ method */
#include "ExpandingHeap.h"
/* ExpandingHeap::VResizeToFit() at 0x0203c388 -- Heap vtable slot (VResizeToFit).
 * Resizing an ExpandingHeap to fit is a forbidden operation, so this override
 * always fails by returning 0 (the new size, 0 == failure). */

typedef unsigned int u32;

struct ExpandingHeap;

u32 ExpandingHeap::VResizeToFit()
{
    return 0;
}
