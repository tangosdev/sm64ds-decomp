//cpp
// @symbol _ZN4Heap23SetupSolidHeapAsDefaultEjPS_i
/* Heap::SetupSolidHeapAsDefault(u32, Heap*, int) at 0x0203c2e4 -- push a solid
 * scratch heap in front of the current default, remembering the old one so
 * Heap::RestoreFromTemporary can put it back. One level deep only: a second
 * call overwrites the saved pointer and the first default is lost.
 *
 * Note the order. The outgoing default is read straight out of the global
 * rather than from SetDefault's return value, even though SetDefault returns
 * exactly that -- so the save happens before the swap, not after it.
 *
 * The file used to open with `typedef void Heap;'. */
#include "Heap.h"

namespace Memory { extern Heap* defaultHeapPtr; }   /* 0x020a0ea0 */
extern "C" Heap* data_020a0ea8;                     /* 0x020a0ea8, unnamed */

void* Heap::SetupSolidHeapAsDefault(u32 size, Heap* root, int align)
{
    Heap* heap = CreateSolidHeap(size, root, align);
    if (!heap)
        return 0;

    data_020a0ea8 = Memory::defaultHeapPtr;
    heap->SetDefault();
    return heap;
}
