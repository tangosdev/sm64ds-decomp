//cpp
// @symbol func_ov060_02115a84
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void* p);
extern void _ZN8dActor_c13LandingDustAtER7Vector3b(void* a, struct Vector3* v, int b);
extern void func_02012694(int id, void* p);
void func_ov060_02115a84(char* c, char* arg){
  if(_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x14c)==0) return;
  *(unsigned short*)arg = *(unsigned short*)arg + 1;
  if(*(unsigned short*)arg >= 4) return;
  struct Vector3 v;
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  _ZN8dActor_c13LandingDustAtER7Vector3b(c, &v, 0);
  func_02012694(0xbd, c+0x74);
}
}
