//cpp
#include "Flamethrower.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN8dCcPos_cD1Ev(void*);
extern void _ZN8dCcPos_cC1Ev(void*);
extern void _ZN7Vector3D1Ev(void*);
extern void func_0203d384(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV12Flamethrower[];

/* Reconstructed source-style name: SM64DS proves daObjFlamethrower_c through RTTI,
 * allocation size, vtable identity, and the OBJ_FLAMETHROWER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Flamethrower_Spawn. */
int* daObjFlamethrower_c_classInit(void){
  int* p = (int*)_ZN7fBase_cnwEj(sizeof(struct Flamethrower));
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV12Flamethrower;
    func_020733a8((char*)p+0xd4, 0xc, 0x3c, _ZN8dCcPos_cC1Ev, _ZN8dCcPos_cD1Ev);
    func_020733a8((char*)p+0x3a4, 0xc, 0xc, func_0203d384, _ZN7Vector3D1Ev);
  }
  return p;
}
}
