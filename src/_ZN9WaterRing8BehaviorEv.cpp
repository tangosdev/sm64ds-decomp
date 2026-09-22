//cpp
// @symbol _ZN9WaterRing8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterRing.h"
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* self, char* cc);
extern void _ZN5dCc_c5ClearEv(char* c);
extern void _ZN5dCc_c6UpdateEv(char* c);
extern void _ZN9Animation7AdvanceEv(char* c);
}
struct C;
typedef void (C::*PMF)();

int WaterRing::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&mStateTimer));
  char* obj = *(char**)((char*)&unk_370);
  if (*(int*)(obj + 8) != 0) {
    PMF* p = (PMF*)(obj + 8);
    C* c = (C*)((char*)this);
    (c->**p)();
  }
  _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), ((char*)this) + 0x110);
  mAngleX = mPrevAngleX;
  mAngleY = mPrevAngleY;
  mAngleZ = mPrevAngleZ;
  func_ov064_02119f1c(((char*)this));
  _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
  _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
  _ZN9Animation7AdvanceEv((char*)&mTextureTransformer);
  return 1;
}
