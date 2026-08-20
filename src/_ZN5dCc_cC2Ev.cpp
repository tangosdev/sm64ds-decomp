//cpp
// @symbol _ZN5dCc_cC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dCc_c.h"
extern "C" {
/* Sets the vptr, then zeroes both intrusive-list links: an unlinked node. */
void _ZN5dCc_cC2Ev(struct dCc_c *self) {
  *(int*)self=(int)data_0208e6ec;
  *(int*)&self->prev=0;
  *(int*)&self->next=0;
}
}
