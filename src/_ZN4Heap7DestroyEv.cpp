//cpp
// @symbol _ZN4Heap7DestroyEv
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
struct Base { virtual void v0(); virtual void v1(); virtual void m(); };
extern "C" void _ZN4Heap10DeallocateEPv(void*, void*);

void Heap::Destroy()
{
  ((Base *)this)->m();
  *(int*)((char*)&unk_004)=0;
  *(int*)((char*)&unk_008)=0;
  void* p = *(void**)((char*)&unk_00c);
  if (p==0) return;
  _ZN4Heap10DeallocateEPv(p, ((Base *)this));
  *(int*)((char*)&unk_00c)=0;
}
