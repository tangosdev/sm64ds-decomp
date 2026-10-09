//cpp
// @symbol _ZdlPv
/* operator delete(void*) at 0x0203cbf0 -- global deallocator. Loads
 * Memory::defaultHeapPtr and tail-calls Heap::_Deallocate(ptr).
 *
 * Language-mode migration: the compiler mangles `operator delete(void*)` to
 * `_ZdlPv`. The library declaration carries `throw()`, so the definition must
 * match that exception-spec. `#pragma exceptions off` keeps this a five-
 * instruction tail call under both match.py's default C++ flags and the ROM
 * build's `-Cpp_exceptions off` (without it, match flags emit EH glue and the
 * body grows to 0x44).
 *
 * Heap::_Deallocate is a three-word veneer to Heap::Deallocate; see
 * include/Heap.h. */
#pragma exceptions off
#include "Heap.h"

namespace Memory {
extern Heap* defaultHeapPtr;
}

void operator delete(void* ptr) throw()
{
    Memory::defaultHeapPtr->_Deallocate(ptr);
}
