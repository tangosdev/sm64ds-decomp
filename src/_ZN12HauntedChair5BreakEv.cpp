//cpp
// @symbol _ZN12HauntedChair5BreakEv
#include "HauntedChair.h"

extern "C" {
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned,int,int,int);
extern void _ZN8dActor_c8PoofDustEv(void*);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
}

void HauntedChair::Break()
{
  char* c = (char*)this;
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x25,*(int*)(c+0x5c),*(int*)(c+0x60),*(int*)(c+0x64));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x26,*(int*)(c+0x5c),*(int*)(c+0x60),*(int*)(c+0x64));
  _ZN8dActor_c8PoofDustEv(c);
  _ZN7fBase_c18MarkForDestructionEv(c);
}
