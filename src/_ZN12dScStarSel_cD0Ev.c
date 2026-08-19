// @symbol _ZN12dScStarSel_cD0Ev
/* dScStarSel_c::~dScStarSel_c() (deleting destructor / D0) -- vtable slot
 * 17. Same teardown as D1 (see src/_ZN12dScStarSel_cD1Ev.cpp) plus
 * Memory::Deallocate. Plain C carries the literal mangled name with no
 * mangling needed -- see include/dScStarSel_c.h. */
#include "decl_Model.h"
#include "decl_common.h"
extern void __destroy_arr(void*, int, int, void*);
extern int data_0208e4b8[];
extern int _ZTV12dScStarSel_c[];
extern void _ZN7fBase_cD2Ev(void*);
extern void* data_020a0eac;
int* _ZN12dScStarSel_cD0Ev(int* t){
  t[0]=(int)_ZTV12dScStarSel_c;
  __destroy_arr((char*)t+0x64, 2, 0x50, (void*)_ZN5ModelD1Ev);
  t[0]=(int)_ZTV8dScene_c;
  t[0]=(int)data_0208e4b8;
  _ZN7fBase_cD2Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
