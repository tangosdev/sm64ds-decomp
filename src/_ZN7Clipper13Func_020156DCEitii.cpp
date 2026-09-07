//cpp
#include "types.h"
// @symbol _ZN7Clipper13Func_020156DCEitii
#include "Clipper.h"

/* The former `Ev` spelling claimed this method took no arguments, contradicting
   every call site and the register/stack reads in the ROM. Scalar parameter
   types reproduce both the observed ABI and the complete function bytes. */
void Clipper::Func_020156DC(Fix12i aspectRatio, u16 fovAngle,
                            Fix12i nearZ, Fix12i farZ)
{
    mAspectRatio = aspectRatio;
    mFovAngle = fovAngle;
    mNearZ = nearZ;
    mFarZ = farZ;
    Func_0201559C();
}
