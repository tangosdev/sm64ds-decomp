//cpp
// @symbol _ZN4Heap10SetDefaultEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"

int Heap::SetDefault()
{
 int old = G; G = ((int)this); return old;
}
