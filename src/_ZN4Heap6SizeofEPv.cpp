//cpp
// @symbol _ZN4Heap6SizeofEPv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual int m(void*); };

int Heap::Sizeof(void * a)
{
  int r = ((Base *)this)->m(a);
  if (r == -1 && (*(int*)((char*)&unk_010) & 0x4000)) Crash();
  return r;
}
