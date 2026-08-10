//cpp
// @symbol _ZN17BowserPuzzlePiece8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BowserPuzzlePiece.h"
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
void func_ov064_0211987c(void* c);
}
struct C;
typedef void (C::*PMF)();
struct Obj { int x; int y; PMF pmf; };
struct C {
  char pad[0x300];
  Obj* obj;
};

s32 BowserPuzzlePiece::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&(*(u8 *)&unk_100)));
  if(((C*)this)->obj->pmf){
    (((C*)this)->*(((C*)this)->obj->pmf))();
  }
  *(short*)((char*)&mAngleX) = *(short*)((char*)&mPrevAngleX);
  *(short*)((char*)&mAngleY) = *(short*)((char*)&mPrevAngleY);
  *(short*)((char*)&mAngleZ) = *(short*)((char*)&mPrevAngleZ);
  func_ov064_0211987c(((C*)this));
  return 1;
}
