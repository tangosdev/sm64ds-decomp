//cpp
#include "types.h"
extern "C" {
void* _ZN7fBase_cnwEj(u32 sz);
void _ZN11dCapEnemy_cC2Ev(void* t);
void _ZN10dCcAcPos_cC1Ev(void* t);
void _ZN10dBgCh_ActrC1Ev(void* t);
void _ZN9ModelAnimC1Ev(void* t);
void _ZN5ModelC1Ev(void* t);
void _ZN11ShadowModelC1Ev(void* t);
extern void* _ZTV7daTrs_c;
/* TERESA and BOSS_TERESA both construct the ROM-proven daTrs_c class. A
 * class-anchored classInit rename would collide, so this factory keeps its
 * historical project spelling until the original distinction is recovered. */
void* BigBoo_Spawn(void){
  char* t=(char*)_ZN7fBase_cnwEj(0x5e0);
  if(t){
    _ZN11dCapEnemy_cC2Ev(t);
    *(void**)t = &_ZTV7daTrs_c;
    _ZN10dCcAcPos_cC1Ev(t+0x184);
    _ZN10dBgCh_ActrC1Ev(t+0x1c4);
    _ZN9ModelAnimC1Ev(t+0x380);
    _ZN5ModelC1Ev(t+0x3e4);
    _ZN11ShadowModelC1Ev(t+0x434);
    _ZN11ShadowModelC1Ev(t+0x45c);
  }
  return t;
}
}
