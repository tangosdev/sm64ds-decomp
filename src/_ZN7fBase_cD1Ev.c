// @symbol _ZN9ActorBaseD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ActorBase.h"
extern void *_ZTV9ActorBase[];   /* 0x02099edc */
int *_ZN9ActorBaseD1Ev(struct ActorBase *self) {
    ((int *)self)[0] = (int)_ZTV9ActorBase;
    func_020440e8((char *)&self->renderNode);
    func_020440e8((char *)&self->behavNode);
    return ((int *)self);
}
