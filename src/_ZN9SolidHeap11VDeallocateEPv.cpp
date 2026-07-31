//cpp
// @symbol _ZN9SolidHeap11VDeallocateEPv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SolidHeap.h"

void SolidHeap::VDeallocate(void * a)
{
  if (a == 0) return;
  Crash();
}
