//cpp
#include "daOts_c.h"

extern "C" int _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(void*,int,int,int,int);
extern "C" int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,int);
extern "C" int func_ov064_02116560(char* c){
  ((daOts_c*)c)->UpdateRunState();
  int r=_ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(c,*(int*)(c+0x3a8),*(int*)(c+0x60),*(int*)(c+0x3b0),0x3e8);
  if(r) return r;
  *(int*)(c+0x398)=0;
  return _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x110, (void*)*(int*)(*(int*)(*(int*)(c+0x330)+0x10)+4), 0, 0x1000, 0);
}
