//cpp
// @symbol _ZN8Particle14SimpleCallbackC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"
extern void *data_0208f3b4;
/* `self` is a Particle::SimpleCallback, NOT the SysTracker -- eight bytes of
   {Particle::Callback vtable; s16}. It used to be typed `struct Particle *`
   and reach the s16 as `&self->unk_004`, which was only ever an accident of
   the two objects both beginning with a pointer-sized word. */
extern "C" void _ZN8Particle14SimpleCallbackC2Ev(char *self) {
    *(void **)((char *)self) = &data_0208f3b4;
    *(void **)((char *)self) = &data_0208f3c4;
    *(short *)((char *)self + 4) = 0;
}
