//cpp
// @symbol _ZN4Heap6RescueEv
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual int m(); };

int Heap::Rescue()
{
 return ((Base *)this)->m();
}
