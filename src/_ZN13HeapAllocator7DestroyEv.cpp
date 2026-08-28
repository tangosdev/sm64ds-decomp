//cpp
// @symbol _ZN13HeapAllocator7DestroyEv
/* HeapAllocator::Destroy() -- detach this allocator from its containing heap.
 * CodeWarrior emits the ROM's 12-byte tail veneer to Remove().
 */
#include "HeapAllocator.h"

void HeapAllocator::Destroy()
{
    Remove();
}
