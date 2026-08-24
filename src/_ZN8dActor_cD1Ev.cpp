//cpp
// @symbol _ZN8dActor_cD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
extern int _ZTV8dActor_c[];
extern int data_0209b468[];         /* the global actor list */
extern int _ZTV7dBase_c[];
extern void _ZN7fBase_cD2Ev(int c);
/* Unlinks the actor from the global list, then chains the bases. dBase_c's
   destructor is inlined, which is why its vtable is stored on the way out. */
int _ZN8dActor_cD1Ev(struct dActor_c *self) {
  *(int*)((int)self) = (int)_ZTV8dActor_c;
  /* Spelt as an offset, not as `&self->mListPrev`: the named form costs bytes
     here even though it is free in the constructors. Measured, do not "clean". */
  func_0203b27c((int)data_0209b468, ((int)self)+0x50);
  func_02044104((int)&self->mListPrev);
  *(int*)((int)self) = (int)_ZTV7dBase_c;
  _ZN7fBase_cD2Ev(((int)self));
  return ((int)self);
}
}
