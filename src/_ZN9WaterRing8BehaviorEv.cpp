//cpp
// @symbol _ZN9WaterRing8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterRing.h"
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* self, char* cc);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN12CylinderClsn6UpdateEv(char* c);
extern void _ZN9Animation7AdvanceEv(char* c);
}
struct C;
typedef void (C::*PMF)();

int WaterRing::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
  char* obj = *(char**)((char*)&unk_370);
  if (*(int*)(obj + 8) != 0) {
    PMF* p = (PMF*)(obj + 8);
    C* c = (C*)((char*)this);
    (c->**p)();
  }
  _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this) + 0x110);
  unk_08c = unk_092;
  unk_08e = unk_094;
  unk_090 = unk_096;
  func_ov064_02119f1c(((char*)this));
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
  _ZN9Animation7AdvanceEv((char*)&mTextureTransformer);
  return 1;
}
