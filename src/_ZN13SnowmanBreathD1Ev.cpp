//cpp
// @symbol _ZN13SnowmanBreathD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "SnowmanBreath.h"
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void func_ov027_02112158(void*);
int _ZN13SnowmanBreathD1Ev(char* c){
  *(int**)c = RotatingPlatformRr_SpawnInfo;
  __destroy_arr(c+0xd4, 0x32, 0x60, (void*)func_ov027_02112158);
  _ZN5ActorD2Ev(c);
  return (int)c;
}
}
