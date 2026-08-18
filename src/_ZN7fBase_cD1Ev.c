// @symbol _ZN7fBase_cD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "fBase_c.h"
extern void *_ZTV7fBase_c[];   /* 0x02099edc */
int *_ZN7fBase_cD1Ev(struct fBase_c *self) {
    ((int *)self)[0] = (int)_ZTV7fBase_c;
    func_020440e8((char *)&self->renderNode);
    func_020440e8((char *)&self->behavNode);
    return ((int *)self);
}
