//cpp
// @symbol daObjCvNewsLift_c_classInit
#include "WorkElevator.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void*);
extern void func_020733a8(void*,int,int,void*,void*);
extern int _ZTV12WorkElevator[];
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void _ZN10dBgW_KcMbgD1Ev(void*);
extern void _ZN10dBgW_KcMbgC1Ev(void*);
/* Reconstructed source-style name: SM64DS proves daObjCvNewsLift_c through
 * RTTI, allocation size, vtable identity, and the CV_NEWS_LIFT registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's WorkElevator implementation aliases remain
 * unchanged. Historical alias: WorkElevator_Spawn. */
void* daObjCvNewsLift_c_classInit(void){
  char* c = (char*)_ZN7fBase_cnwEj(sizeof(struct WorkElevator));
  if(c){
    _ZN10dBgActor_cC2Ev(c);
    *(int*)c = (int)_ZTV12WorkElevator;
    func_020733a8(c+0x320, 4, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x520, 4, 0x1c8, (void*)_ZN10dBgW_KcMbgC1Ev, (void*)_ZN10dBgW_KcMbgD1Ev);
  }
  return c;
}
}
