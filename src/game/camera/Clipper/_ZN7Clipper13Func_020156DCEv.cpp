//cpp
#include "types.h"
// @symbol _ZN7Clipper13Func_020156DCEv
/* recovered: named members + shared header */
#include "Clipper.h"
extern "C" {

void _ZN7Clipper13Func_020156DCEv(struct Clipper *self, u32 a, u16 b, Fix12i c, Fix12i d) {
    self->mAspectRatio = a;
    self->mFovAngle = b;
    *(Fix12i*)((char*)&self->mNearZ) = c;
    *(Fix12i*)((char*)&self->mFarZ) = d;
    self->Func_0201559C();
}

}
