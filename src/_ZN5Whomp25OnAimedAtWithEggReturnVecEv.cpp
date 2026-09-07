//cpp
// @symbol _ZN5Whomp25OnAimedAtWithEggReturnVecEv
// recovered name: Whomp::OnAimedAtWithEggReturnVec
/* Whomp::OnAimedAtWithEggReturnVec - vtable slot 30, overriding
 * dActor_c::OnAimedAtWithEggReturnVec(). The ROM's function at 0x2123b54 is
 * a 0xc-byte tail-call veneer to func_ov079_02123d4c (0x2123d4c): `ldr ip,
 * [pc]; bx ip; .word 0x2123d4c`. func_ov079_02123d4c's ABI -- (int* out,
 * char* a) -- lines up exactly with the hidden-return-pointer + this shape
 * a Vector3-by-value virtual gets (see include/dActor_c.h's note on this
 * slot), so a plain forwarding return lets mwcc's sibling-call optimizer
 * regenerate the same veneer. */
#include "Whomp.h"

extern "C" Vector3 func_ov079_02123d4c(void *thiz);

Vector3 Whomp::OnAimedAtWithEggReturnVec()
{
    return func_ov079_02123d4c(this);
}
