// @symbol func_ov075_02115b28
// @emits dScEntry_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScEntry_c::OnYoshiTryEat - recovered from vtable slot identity */
extern int _ZTV5Stage[];
extern int data_0208e4b8[];
extern void __destroy_arr(void*, int, int, void*);
extern void _ZN9ActorBaseD2Ev(void*);
extern void* data_020a0eac;
int* dScEntry_c_OnYoshiTryEat(int* t){
  t[0]=(int)data_ov075_0211d304;
  __destroy_arr((char*)t+0x1b4, 4, 0x2c, (void*)func_ov075_02115bc8);
  __destroy_arr((char*)t+0x70, 9, 0x24, (void*)func_ov075_02115bac);
  t[0]=(int)_ZTV5Stage;
  t[0]=(int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
