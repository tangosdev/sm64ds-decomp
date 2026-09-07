//cpp
// @symbol _ZN5dCc_c4InitE5Fix12IiES1_jj
/* recovered: named members + shared header */
#include "dCc_c.h"
/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dCc_c.h is the real one and callers may use it. */
extern "C" void _ZN5dCc_c4InitE5Fix12IiES1_jj(struct dCc_c *self, int a, int b, unsigned d, unsigned e) {
  *(int*)&self->radius=a;
  *(int*)&self->height=b;
  *(int*)&self->flags=d;
  *(int*)&self->vulnFlags=e;
}
