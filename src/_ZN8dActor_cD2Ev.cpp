//cpp
// @symbol _ZN8dActor_cD2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
extern int _ZTV8dActor_c[];
extern int data_0209b468[];
extern int _ZTV7dBase_c[];
extern void _ZN7fBase_cD2Ev(int c);
int _ZN8dActor_cD2Ev(struct dActor_c *self) {
  *(int*)((int)self) = (int)_ZTV8dActor_c;
  func_0203b27c((int)data_0209b468, ((int)self)+0x50);
  func_02044104((int)&self->mListPrev);
  *(int*)((int)self) = (int)_ZTV7dBase_c;
  _ZN7fBase_cD2Ev(((int)self));
  return ((int)self);
}
}
