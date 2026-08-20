//cpp
// @symbol _ZN12FlamethrowerD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Flamethrower.h"
extern "C" {
extern int _ZN7Vector3D1Ev[];
extern int data_020a0eac[];
extern int _ZN8dCcPos_cD1Ev[];
void __destroy_arr(void*, int, int, void*);
void _ZN8dActor_cD2Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* _ZN12FlamethrowerD0Ev(char* c){
  *(int*)c = (int)_ZTV12Flamethrower;
  __destroy_arr(c+0x3a4, 0xc, 0xc, (void*)_ZN7Vector3D1Ev);
  __destroy_arr(c+0xd4, 0xc, 0x3c, (void*)_ZN8dCcPos_cD1Ev);
  _ZN8dActor_cD2Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac[0]);
  return c;
}
}
