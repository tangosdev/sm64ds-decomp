//cpp
// @symbol _ZN12HauntedChair8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HauntedChair.h"
extern "C" {
extern void func_0200f760(char* a, char* b);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char* c, char* v);
extern void _ZN12CylinderClsn6UpdateEv(char* c);
}

int HauntedChair::Behavior()
{
  func_0200f760(((char*)this), ((char*)this) + 0x17c);
  switch (unk_378) {
  case 0: func_ov020_021130c8(((char*)this)); break;
  case 1: func_ov020_02112e94(((char*)this)); break;
  case 2: func_ov020_02112b00(((char*)this)); break;
  case 3: func_ov020_021129dc(((char*)this)); break;
  }
  func_ov020_02113240(((char*)this));
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
  _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x17c, ((char*)this) + 0x38c);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
  return 1;
}
