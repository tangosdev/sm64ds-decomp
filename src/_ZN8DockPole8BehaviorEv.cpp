//cpp
// @symbol _ZN8DockPole8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "DockPole.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* c);
extern void Matrix4x3_FromRotationY(void* m, short angle);
}

int DockPole::Behavior()
{
  _ZN9Animation7AdvanceEv((char*)&mAnimation);
  Matrix4x3_FromRotationY(((char*)this)+0xf0, mAngleY);
  unk_114=mPosX>>3;
  unk_118=mPosY>>3;
  unk_11c=mPosZ>>3;
  return 1;
}
