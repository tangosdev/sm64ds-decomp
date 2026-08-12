// @symbol func_ov003_020ade54
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void __destroy_arr(void*, int, int, void*);
extern void*_ZTV5Scene[];
extern int data_0208e4b8[];
extern void _ZN9ActorBaseD2Ev(void*);
extern void* data_020a0eac;
int* func_ov003_020ade54(int* t){
  t[0]=(int)data_ov003_020b1704;
  __destroy_arr((char*)t+0x64, 2, 0x50, (void*)_ZN5ModelD1Ev);
  t[0]=(int)_ZTV5Scene;
  t[0]=(int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
