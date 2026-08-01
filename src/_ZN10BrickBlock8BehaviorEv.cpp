//cpp
// @symbol _ZN10BrickBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BrickBlock.h"
struct C;
typedef void (C::*PMF)();
extern "C" PMF data_ov002_0210dd30[];
extern "C" {
void* _ZN5Actor4NextEPKS_(const void* prev);
int Vec3_Dist(const void* a, const void* b);
void _ZN9ActorBase18MarkForDestructionEv(void* c);
}
struct C { char pad[0x1000]; };

int BrickBlock::Behavior()
{
  char* o = 0;
  if (unk_0d8 != 0) goto d6;
  o = (char*)_ZN5Actor4NextEPKS_(0);
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
      unk_0d8 = 1;
      return 1;
    }
  next:
    o = (char*)_ZN5Actor4NextEPKS_(o);
  }
  if (o) goto d6;
  _ZN9ActorBase18MarkForDestructionEv(((char*)this));
  return 1;
d6:
  if (unk_0d6 != 0){
    int idx = unk_0d7;
    (((C*)((char*)this))->*data_ov002_0210dd30[idx])();
    _ZN9ActorBase18MarkForDestructionEv(((char*)this));
  }
  return 1;
}
