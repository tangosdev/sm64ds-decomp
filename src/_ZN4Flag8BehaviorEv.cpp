//cpp
// @symbol _ZN4Flag8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Flag.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* c);
extern void Matrix4x3_FromRotationY(void* m, short angle);
}

int Flag::Behavior()
{
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
  Matrix4x3_FromRotationY(((char*)this)+0xf0, mAngleY);
  mModelAnim.mat4x3.t.x=mPosX>>3;
  mModelAnim.mat4x3.t.y=mPosY>>3;
  mModelAnim.mat4x3.t.z=mPosZ>>3;
  return 1;
}
