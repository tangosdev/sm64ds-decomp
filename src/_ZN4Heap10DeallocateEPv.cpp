//cpp
// @symbol _ZN4Heap10DeallocateEPv
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void m(void*); };

void Heap::Deallocate(void * a)
{
 ((Base *)this)->m(a);
}
