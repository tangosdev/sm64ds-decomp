//cpp
// @symbol _ZN8CccArena8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CccArena.h"
extern "C" {
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
}

int CccArena::Behavior()
{
  void* o = *(void**)((char*)&unk_320);
  if(*(int*)((char*)o+8)){
    char* base = (char*)o+8;
    int adj = *(int*)(base+4);
    char* self = ((char*)this) + (adj>>1);
    void* fn;
    if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)base); }
    else fn=*(void**)base;
    ((void(*)(char*))fn)(self);
  }
  Matrix4x3_FromRotationXYZExt(((char*)this)+0xf0, unk_08c, unk_08e, unk_090);
  unk_114 = mPosX >> 3;
  unk_118 = mPosY >> 3;
  unk_11c = mPosZ >> 3;
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  return 1;
}
