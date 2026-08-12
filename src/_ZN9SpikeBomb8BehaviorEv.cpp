//cpp
// @symbol _ZN9SpikeBomb8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SpikeBomb.h"
extern "C" {
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* p, void* v);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
struct V3 { int x,y,z; };
}

int SpikeBomb::Behavior()
{
  int idx = unk_170;
  char* ent = (char*)&data_ov060_0211b1d8[idx*2];
  int adj = *(int*)(ent+4);
  char* self = ((char*)this) + (adj>>1);
  void* fn;
  if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)ent); }
  else fn=*(void**)ent;
  ((void(*)(char*))fn)(self);
  func_ov060_02118690(((char*)this));
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
  struct V3 v;
  v.x = 0;
  v.y = -0x96000;
  v.z = 0;
  _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this)+0x124, &v);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
  return 1;
}
