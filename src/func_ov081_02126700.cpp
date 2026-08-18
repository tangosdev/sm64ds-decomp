//cpp
// @symbol func_ov081_02126700
/* recovered: shared common types */
#include "common.h"

extern "C"{
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void*,Vector3 const&,unsigned int,int,short);
void _ZN8dActor_c8PoofDustEv(void*);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
void func_ov081_02126700(char* c){
  Vector3 t; t.x=*(int*)(c+0x5c); t.y=*(int*)(c+0x60); t.z=*(int*)(c+0x64);
  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c,t,5,0xf000,0);
  _ZN8dActor_c8PoofDustEv(c);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}}
