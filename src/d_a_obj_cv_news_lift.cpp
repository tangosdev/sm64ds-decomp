//cpp
// @symbol daObjCvNewsLift_c_classInit
#include "daObjCvNewsLift_c.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void*);
extern void __cxa_vec_ctor(void*,int,int,void*,void*);
extern int _ZTV17daObjCvNewsLift_c[];
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void _ZN10dBgW_KcMbgD1Ev(void*);
extern void _ZN10dBgW_KcMbgC1Ev(void*);
/* Reconstructed source-style name: SM64DS proves daObjCvNewsLift_c through
 * RTTI, allocation size, vtable identity, and the CV_NEWS_LIFT registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical aliases: WorkElevator (the class) and
 * WorkElevator_Spawn. */
void* daObjCvNewsLift_c_classInit(void){
  char* c = (char*)_ZN7fBase_cnwEj(sizeof(struct daObjCvNewsLift_c));
  if(c){
    _ZN10dBgActor_cC2Ev(c);
    *(int*)c = (int)_ZTV17daObjCvNewsLift_c;
    __cxa_vec_ctor(c+0x320, 4, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    __cxa_vec_ctor(c+0x520, 4, 0x1c8, (void*)_ZN10dBgW_KcMbgC1Ev, (void*)_ZN10dBgW_KcMbgD1Ev);
  }
  return c;
}
}
