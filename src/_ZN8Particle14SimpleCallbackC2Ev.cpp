//cpp
// @symbol _ZN8Particle14SimpleCallbackC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"
extern void *data_0208f3b4;
extern "C" void _ZN8Particle14SimpleCallbackC2Ev(struct Particle *self) {
    *(void **)((char *)self) = &data_0208f3b4;
    *(void **)((char *)self) = &data_0208f3c4;
    *(short *)((char *)&self->unk_004) = 0;
}
