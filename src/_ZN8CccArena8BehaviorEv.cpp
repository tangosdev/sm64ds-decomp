//cpp
// @symbol _ZN8CccArena8BehaviorEv
/* recovered: named members + shared header, real C++ method */
/* The class header comes FIRST on purpose: it reaches math/Matrix.h, whose
   Matrix4x3 is the structured one, and include/common.h's flat spelling stands
   down behind the guard. mat4x3.t is only nameable this way round, and the two
   spellings are the same 0x30 bytes. */
#include "CccArena.h"
/* recovered: declarations from a shared header */
#include "decl_common.h"
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
  Matrix4x3_FromRotationXYZExt(((char*)this)+0xf0, mAngleX, mAngleY, mAngleZ);
  mModel.mat4x3.t.x = mPosX >> 3;
  mModel.mat4x3.t.y = mPosY >> 3;
  mModel.mat4x3.t.z = mPosZ >> 3;
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  return 1;
}
