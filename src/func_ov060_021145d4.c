// @symbol func_ov060_021145d4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
extern void func_0200fa04(void *c, void *v, int flag);
extern void func_02012694(int a, void *b);
extern int _ZN6Player7IsInAirEv(void *p);



int func_ov060_021145d4(char *c){
  struct Vector3 v;
  int b;
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x14c)){
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0xa8) = 0;
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    func_0200fa04(c, &v, 0);
    func_ov060_02111cc0(c, 0xc, 0x40000000);
    func_02012694(0xb6, c + 0x74);
    if(*(unsigned char*)(c + 0x414) == 0){
      b = (*(int*)(c + 0x3ec) >= 0x352000);
      if(!_ZN6Player7IsInAirEv(*(void**)(c + 0x3a0)))
        func_ov002_020c56f0(*(unsigned char**)(c + 0x3a0), b);
    }
    return 1;
  }
  return 0;
}
