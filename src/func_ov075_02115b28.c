// @symbol func_ov075_02115b28
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
extern void*_ZTV5Scene[];
extern int data_0208e4b8[];
extern void __destroy_arr(void*, int, int, void*);
extern void _ZN9ActorBaseD2Ev(void*);
extern void* data_020a0eac;
int* func_ov075_02115b28(int* t){
  t[0]=(int)data_ov075_0211d304;
  __destroy_arr((char*)t+0x1b4, 4, 0x2c, (void*)func_ov075_02115bc8);
  __destroy_arr((char*)t+0x70, 9, 0x24, (void*)func_ov075_02115bac);
  t[0]=(int)_ZTV5Scene;
  t[0]=(int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
