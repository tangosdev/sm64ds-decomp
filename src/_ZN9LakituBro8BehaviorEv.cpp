//cpp
// @symbol _ZN9LakituBro8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LakituBro.h"
struct C;
typedef void (C::*PMF)();
struct State { char pad[8]; PMF fn; };
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern void _ZN9Animation7AdvanceEv(void*);
}
extern State data_ov085_021307d0;
extern State data_ov085_021307e0;
struct C { char pad[0x10000]; };

int LakituBro::Behavior()
{
  char* p=(char*)((C*)this);
  DecIfAbove0_Short((unsigned short*)(p+0x100));
  State* st=*(State**)(p+0x1ec);
  if(st->fn) (((C*)this)->*st->fn)();
  _ZN8dActor_c9UpdatePosEP5dCc_c(((C*)this), 0);
  _ZN9Animation7AdvanceEv(p+0x160);
  _ZN9Animation7AdvanceEv(p+0x1d8);
  if(*(State**)(p+0x1ec)==&data_ov085_021307d0){
    *(short*)(p+0x8c)=*(short*)(p+0x92);
    *(short*)(p+0x8e)=*(short*)(p+0x94);
    *(short*)(p+0x90)=*(short*)(p+0x96);
  }
  if(*(State**)(p+0x1ec)==&data_ov085_021307e0) return 1;
  if(*(int*)(p+0x2d0)==0) func_ov085_0212e858(((C*)this));
  else func_ov085_0212e778(((C*)this));
  return 1;
}
