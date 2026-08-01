//cpp
// @symbol _ZN13ExpandingHeap22VMaxAllocationUnitSizeEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_ExpandingHeapAllocator.h"
/* recovered: named members + shared header, real C++ method */
#include "ExpandingHeap.h"
extern "C" {
}

unsigned int ExpandingHeap::VMaxAllocationUnitSize()
{
    return _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi(*(void**)((char*)&unk_014), 4);
}
