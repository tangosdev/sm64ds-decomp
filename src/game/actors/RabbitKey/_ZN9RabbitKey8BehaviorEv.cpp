//cpp
// @symbol _ZN9RabbitKey8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "RabbitKey.h"
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void* a, void* b);
}

int RabbitKey::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&mStateTimer));
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
  int s = mVertSpeed + mVertAccel;
  int lim = mTerminalVelocity;
  if(s >= lim) lim = s;
  int t = unk_0ac;
  mVertSpeed = lim;
  unk_0ac = t;
  _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(((char*)this), 0);
  return 1;
}
