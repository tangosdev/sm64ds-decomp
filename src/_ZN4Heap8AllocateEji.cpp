//cpp
// @symbol _ZN4Heap8AllocateEji
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual int m(unsigned int,int); };

int Heap::Allocate(unsigned int a, int b)
{
  int r = ((Base *)this)->m(a,b);
  if (r == 0 && (*(int*)((char*)&unk_010) & 0x4000)) Crash();
  return r;
}
