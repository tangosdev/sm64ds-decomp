// @symbol func_ov003_020ade54
// @emits dScStarSel_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScStarSel_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void func_0207328c(void*, int, int, void*);
extern int _ZTV5Stage[];
extern int data_0208e4b8[];
extern void _ZN9ActorBaseD2Ev(void*);
extern void* data_020a0eac;
int* dScStarSel_c_OnYoshiTryEat(int* t){
  t[0]=(int)data_ov003_020b1704;
  func_0207328c((char*)t+0x64, 2, 0x50, (void*)_ZN5ModelD1Ev);
  t[0]=(int)_ZTV5Stage;
  t[0]=(int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
