//cpp
// @symbol _ZN10BrickBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BrickBlock.h"
struct C;
typedef void (C::*PMF)();
extern "C" PMF data_ov002_0210dd30[];
extern "C" {
void* _ZN8dActor_c4NextEPKS_(const void* prev);
int Vec3_Dist(const void* a, const void* b);
void _ZN7fBase_c18MarkForDestructionEv(void* c);
}
struct C { char pad[0x1000]; };

int BrickBlock::Behavior()
{
  char* o = 0;
  if (mIsAttached != 0) goto d6;
  o = (char*)_ZN8dActor_c4NextEPKS_(0);
  while (o){
    unsigned short t = *(unsigned short*)(o + 0xc);
    int b;
    b = (t == 0x10);
    if (!b){
      b = (t == 0xf);
      if (!b){
        b = (t == 0x11);
        if (!b) goto next;
      }
    }
    if (Vec3_Dist(((char*)this) + 0x5c, o + 0x5c) < 0x32000){
      *(char**)(o + 0x328) = ((char*)this);
      mIsAttached = 1;
      return 1;
    }
  next:
    o = (char*)_ZN8dActor_c4NextEPKS_(o);
  }
  if (o) goto d6;
  _ZN7fBase_c18MarkForDestructionEv(((char*)this));
  return 1;
d6:
  if (mActionPending != 0){
    int idx = mActionIndex;
    (((C*)((char*)this))->*data_ov002_0210dd30[idx])();
    _ZN7fBase_c18MarkForDestructionEv(((char*)this));
  }
  return 1;
}
