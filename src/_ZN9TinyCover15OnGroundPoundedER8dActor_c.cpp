//cpp
// @symbol _ZN9TinyCover15OnGroundPoundedER8dActor_c
#include "TinyCover.h"

/* TinyCover::OnGroundPounded -- vtable slot 21, ov033 0x0211123c.
 *
 * Real member function: fBase_c::MarkForDestruction is a real method
 * (include/fBase_c.h), and &mCamSpacePosX (a real named field,
 * include/dActor_c.h) replaces `this + 0x74` as the Vector3 argument.
 * `other`'s own position (mPosX/mPosY/mPosZ) stays a raw pointer read
 * (`(char *)&other + 0x5c`, then `v[0..2]`) rather than three named-field
 * reads: the ROM computes one base address and offsets from it (`add r3,
 * r1, #0x5c` then three `ldr [r3, #N]`), not three independent field
 * loads -- member-access syntax compiled to the latter and cost 4 words. */
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void *v);
extern void _ZN5Event6SetBitEj(unsigned int b);
}

void TinyCover::OnGroundPounded(dActor_c &other)
{
    int *v = (int *)(((char *)&other) + 0x5c);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x28, v[0], v[1], v[2]);
    _ZN5Sound9PlayBank3EjRK7Vector3(0xf, &mCamSpacePosX);
    _ZN5Event6SetBitEj(0xe);
    MarkForDestruction();
}
