//cpp
// @symbol _ZN9JetStream8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The state pointer at 0x300 used to be reached through a local
 * `struct C { char pad[0x300]; Obj* obj; }` stand-in; JetStream.h declares it as
 * JetStream::State now, so the cast and the dummy structs are gone. Same shape
 * and same treatment as Bullet::Behavior.
 */
#include "JetStream.h"
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
void func_ov064_0211987c(void* c);
}

s32 JetStream::Behavior()
{
  DecIfAbove0_Short((unsigned short*)&mStateTimer);
  State* h = mState;
  /* Reads the handler's pointer word directly rather than as `&h->mMain`:
     taking the ADDRESS of a pointer-to-member makes mwcc materialise the whole
     8-byte pmf. Reading one to CALL it is free. */
  if (*(int*)((char*)h + 8) != 0) {
    (this->*(h->mMain))();
  }
  mAngleX = mPrevAngleX;
  mAngleY = mPrevAngleY;
  mAngleZ = mPrevAngleZ;
  func_ov064_0211987c(this);
  return 1;
}
