//cpp
// @symbol _ZN4Heap6IntactEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual bool m(); };

bool Heap::Intact()
{
  bool r = ((Base *)this)->m();
  if (r) return r;
  if (!data_020a0e98) data_020a0e98 = 1;
  return r;
}
