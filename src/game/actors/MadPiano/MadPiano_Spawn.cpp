//cpp
#include "MadPiano.h"
extern "C" {
void* _ZN7fBase_cnwEj(unsigned int);
void _ZN10dBgActor_cC2Ev(void*);
void _ZN9ModelAnimC1Ev(void*);
void _ZN11ShadowModelC1Ev(void*);
void func_020733a8(void*, int, int, void*, void*);
void _ZN10dCcAcPos_cC1Ev(void*);
void _ZN10dCcAcPos_cD1Ev(void*);
void _ZN10dBgCh_ActrC1Ev(void*);
extern int _ZTV8MadPiano[];
void* MadPiano_Spawn(void) {
  char* r4 = (char*)_ZN7fBase_cnwEj(sizeof(struct MadPiano));
  if (r4) {
    _ZN10dBgActor_cC2Ev(r4);
    *(int*)r4 = (int)_ZTV8MadPiano;
    _ZN9ModelAnimC1Ev(r4 + 0x320);
    _ZN11ShadowModelC1Ev(r4 + 0x384);
    _ZN11ShadowModelC1Ev(r4 + 0x3ac);
    _ZN11ShadowModelC1Ev(r4 + 0x3d4);
    func_020733a8(r4 + 0x48c, 2, 0x40, (void*)_ZN10dCcAcPos_cC1Ev, (void*)_ZN10dCcAcPos_cD1Ev);
    _ZN10dBgCh_ActrC1Ev(r4 + 0x50c);
  }
  return r4;
}
}
