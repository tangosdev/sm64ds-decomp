//cpp
// @symbol _ZN5ActorD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Actor.h"
extern "C" {
extern int data_0208e3a4[];
extern int data_0209b468[];
extern int data_0208e4b8[];
extern void _ZN9ActorBaseD2Ev(int c);
int _ZN5ActorD1Ev(struct Actor *self) {
  *(int*)((int)self) = (int)data_0208e3a4;
  func_0203b27c((int)data_0209b468, ((int)self)+0x50);
  func_02044104((int)&self->unk_050);
  *(int*)((int)self) = (int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(((int)self));
  return ((int)self);
}
}
