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

int BowserPuzzlePiece::Behavior()
{
  DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
  if(((C*)this)->obj->pmf){
    (((C*)this)->*(((C*)this)->obj->pmf))();
  }
  *(short*)((char*)&unk_08c) = *(short*)((char*)&unk_092);
  *(short*)((char*)&unk_08e) = *(short*)((char*)&unk_094);
  *(short*)((char*)&unk_090) = *(short*)((char*)&unk_096);
  func_ov064_0211987c(((C*)this));
  return 1;
}
