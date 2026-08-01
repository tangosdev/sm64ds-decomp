//cpp
// @symbol _ZN13KoopaTheQuick8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KoopaTheQuick.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* b);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* a, void* b, unsigned int j);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
}

int KoopaTheQuick::Behavior()
{
  int idx = unk_38c;
  char* ent = (char*)&data_ov062_0211e0a4[idx*2];
  int adj = *(int*)(ent+4);
  char* self = ((char*)this) + (adj>>1);
  void* fn;
  if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)ent); }
  else fn=*(void**)ent;
  ((void(*)(char*))fn)(self);
  _ZN9Animation7AdvanceEv((char*)&unk_350);
  mAngleY = mPrevAngleY;
  _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this)+0x110);
  _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), ((char*)this)+0x144, 0);
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
  func_ov062_0211aac0(((char*)this));
  return 1;
}
