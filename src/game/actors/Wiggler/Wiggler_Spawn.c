#include "Wiggler.h"
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern void _ZN12dEnemyBase_cC2Ev(void *p);
extern int func_020733a8(void *p, int a, int b, void *ctor, void *dtor);
extern int _ZN10dBgCh_ActrC1Ev(void *p);
extern int _ZN10dCcAcPos_cC1Ev(void *p);
extern int _ZN10dCcAcPos_cD1Ev(void *p);
extern int func_0203d73c(void *p);
extern int _ZN8Vector3sD1Ev(void *p);
extern int func_0203d384(void *p);
extern int _ZN7Vector3D1Ev(void *p);
extern int _ZN15TextureSequenceC1Ev(void *p);
extern int _ZN15TextureSequenceD1Ev(void *p);
extern int _ZN15MaterialChangerC1Ev(void *p);
extern int _ZN15MaterialChangerD1Ev(void *p);
extern int _ZN9ModelAnimC1Ev(void *p);
extern int _ZN9ModelAnimD1Ev(void *p);
extern int _ZTV7Wiggler[];

void *Wiggler_Spawn(void){
  char *c = (char *)_ZN7fBase_cnwEj(sizeof(struct Wiggler));
  if(c){
    _ZN12dEnemyBase_cC2Ev(c);
    *(int**)(c) = _ZTV7Wiggler;
    func_020733a8(c+0x110, 5, 0x64, _ZN9ModelAnimC1Ev, _ZN9ModelAnimD1Ev);
    func_020733a8(c+0x304, 5, 0x14, _ZN15MaterialChangerC1Ev, _ZN15MaterialChangerD1Ev);
    func_020733a8(c+0x368, 5, 0x14, _ZN15TextureSequenceC1Ev, _ZN15TextureSequenceD1Ev);
    func_020733a8(c+0x3cc, 5, 0xc, func_0203d384, _ZN7Vector3D1Ev);
    func_020733a8(c+0x408, 5, 0xc, func_0203d384, _ZN7Vector3D1Ev);
    func_020733a8(c+0x444, 5, 6, func_0203d73c, _ZN8Vector3sD1Ev);
    func_020733a8(c+0x478, 5, 0x40, _ZN10dCcAcPos_cC1Ev, _ZN10dCcAcPos_cD1Ev);
    func_020733a8(c+0x5b8, 5, 0x40, _ZN10dCcAcPos_cC1Ev, _ZN10dCcAcPos_cD1Ev);
    _ZN10dBgCh_ActrC1Ev(c+0x708);
  }
  return c;
}
