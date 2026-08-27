//cpp
// @symbol _ZN12HauntedChair8BehaviorEv
#include "decl_common.h"
#include "HauntedChair.h"
extern "C" {
extern void func_0200f760(char* a, char* b);
extern void _ZN5dCc_c5ClearEv(char* c);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(char* c, char* v);
extern void _ZN5dCc_c6UpdateEv(char* c);
}

int HauntedChair::Behavior()
{
  func_0200f760(((char*)this), ((char*)this) + 0x17c);
  switch (mState) {
  case 0: State0(); break;
  case 1: State1(); break;
  case 2: State2(); break;
  case 3: State3(); break;
  }
  UpdateModel();
  _ZN5dCc_c5ClearEv((char*)&mCylinder);
  _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x17c, ((char*)this) + 0x38c);
  _ZN5dCc_c6UpdateEv((char*)&mCylinder);
  return 1;
}
