//cpp
// @symbol _ZN8CccArena8BehaviorEv
/* recovered: named members + shared header, real C++ method */
/* Model.h comes FIRST on purpose, now that CccArena derives from
   dBgActor_c (which reaches common.h's FLAT Matrix4x3 first for its own
   reasons -- see include/dBgActor_c.h). Model.h reaches math/Matrix.h,
   whose Matrix4x3 is the structured one, and it wins the include guard
   race so common.h's flat spelling stands down for this TU. mat4x3.t is
   only nameable this way round; the two spellings are the same 0x30
   bytes. */
#include "Model.h"
#include "CccArena.h"
/* recovered: declarations from a shared header */
#include "decl_common.h"
extern "C" {
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* c);
}

int CccArena::Behavior()
{
  void* o = *(void**)&mState;
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
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  return 1;
}
