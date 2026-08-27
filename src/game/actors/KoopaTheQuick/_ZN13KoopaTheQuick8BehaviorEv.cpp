//cpp
// @symbol _ZN13KoopaTheQuick8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KoopaTheQuick.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, void* b);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* a, void* b, unsigned int j);
extern void _ZN5dCc_c5ClearEv(void* p);
extern void _ZN5dCc_c6UpdateEv(void* p);
}

int KoopaTheQuick::Behavior()
{
  int idx = mState;
  char* ent = (char*)&data_ov062_0211e0a4[idx*2];
  int adj = *(int*)(ent+4);
  char* self = ((char*)this) + (adj>>1);
  void* fn;
  if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)ent); }
  else fn=*(void**)ent;
  ((void(*)(char*))fn)(self);
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
  mAngleY = mPrevAngleY;
  _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), ((char*)this)+0x110);
  _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), ((char*)this)+0x144, 0);
  _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
  _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
  func_ov062_0211aac0(((char*)this));
  return 1;
}
