//cpp
// @symbol _ZN12FlamethrowerD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Flamethrower.h"
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern int _ZN7Vector3D1Ev[];
extern int _ZN8dCcPos_cD1Ev();
int _ZN12FlamethrowerD1Ev(char* c){
  *(int**)c = _ZTV12Flamethrower;
  __destroy_arr((char*)c+0x3a4, 0xc, 0xc, (void*)_ZN7Vector3D1Ev);
  __destroy_arr((char*)c+0xd4, 0xc, 0x3c, (void*)&_ZN8dCcPos_cD1Ev);
  _ZN8dActor_cD2Ev(c);
  return (int)c;
}
}
