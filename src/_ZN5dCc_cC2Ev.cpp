//cpp
// @symbol _ZN12CylinderClsnC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "CylinderClsn.h"
extern "C" {
/* Sets the vptr, then zeroes both intrusive-list links: an unlinked node. */
void _ZN12CylinderClsnC2Ev(struct CylinderClsn *self) {
  *(int*)self=(int)data_0208e6ec;
  *(int*)&self->prev=0;
  *(int*)&self->next=0;
}
}
