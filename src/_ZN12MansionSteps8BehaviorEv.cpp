//cpp
// @symbol _ZN12MansionSteps8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MansionSteps.h"
extern "C" {
extern int func_ov063_0211c6f8(char* c);
}
struct C;
typedef void (C::*PMF)();
extern PMF data_ov063_0211ef38[];

int MansionSteps::Behavior()
{
  unsigned char before = unk_150;
  int idx = unk_140;
  C* obj = (C*)((char*)this);
  (obj->*data_ov063_0211ef38[idx])();
  unsigned short* ctr = (unsigned short*)(((int)((char*)this) + 0x14c) & 0xFFFFFFFFFFFFFFFF);
  *ctr = *ctr + 1;
  if (before != unk_150) {
    unsigned short* base = (unsigned short*)(((int)((char*)this) + 0x100) & 0xFFFFFFFFFFFFFFFF);
    base[0x4c/2] = 0;
  }
  func_ov063_0211c684(((char*)this));
  func_ov063_0211c6f8(((char*)this));
  unk_124 = 0;
  return 1;
}
