// @symbol _ZN10dScEntry_cD0Ev
/* dScEntry_c::~dScEntry_c() (deleting destructor / D0) -- vtable slot 17.
 * Same teardown as D1 (see src/_ZN10dScEntry_cD1Ev.cpp) plus
 * Memory::Deallocate. Plain C function carrying the literal mangled name --
 * see include/dScEntry_c.h. */
#include "decl_common.h"
extern int data_0208e4b8[];
extern void __destroy_arr(void*, int, int, void*);
extern void _ZN7fBase_cD2Ev(void*);
extern void* data_020a0eac;
int* _ZN10dScEntry_cD0Ev(int* t){
  t[0]=(int)data_ov075_0211d304;
  __destroy_arr((char*)t+0x1b4, 4, 0x2c, (void*)func_ov075_02115bc8);
  __destroy_arr((char*)t+0x70, 9, 0x24, (void*)func_ov075_02115bac);
  t[0]=(int)_ZTV8dScene_c;
  t[0]=(int)data_0208e4b8;
  _ZN7fBase_cD2Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
