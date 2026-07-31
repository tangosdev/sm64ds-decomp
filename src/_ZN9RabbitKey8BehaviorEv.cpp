//cpp
// @symbol _ZN9RabbitKey8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "RabbitKey.h"
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* a, void* b);
}

int RabbitKey::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
  void* o = *(void**)((char*)&unk_188);
  if(*(int*)((char*)o+8)){
    char* base = (char*)o+8;
    int adj = *(int*)(base+4);
    char* self = ((char*)this) + (adj>>1);
    void* fn;
    if(adj&1){ void* vt=*(void**)self; fn=*(void**)((char*)vt + *(int*)base); }
    else fn=*(void**)base;
    ((void(*)(char*))fn)(self);
  }
  int s = unk_0a8 + unk_09c;
  int lim = unk_0a0;
  if(s >= lim) lim = s;
  int t = unk_0ac;
  unk_0a8 = lim;
  unk_0ac = t;
  _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(((char*)this), 0);
  return 1;
}
