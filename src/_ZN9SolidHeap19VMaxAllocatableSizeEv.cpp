//cpp
// @symbol _ZN9SolidHeap19VMaxAllocatableSizeEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SolidHeapAllocator.h"
/* recovered: named members + shared header, real C++ method */
#include "SolidHeap.h"
extern "C" {
}

unsigned int SolidHeap::VMaxAllocatableSize()
{
    return _ZN18SolidHeapAllocator10MemoryLeftEi(*(void**)((char*)&unk_014), 4);
}
