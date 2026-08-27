//cpp
// @symbol _ZN8Particle14SimpleCallbackC2Ev
/* recovered: real C++ constructor
 *
 * `self` is a Particle::SimpleCallback, NOT the SysTracker -- eight bytes of
 * {Particle::Callback vtable; s16}. No Particle::Callback type is modeled
 * (see include/Particle.h), so the base vtable store and the derived
 * override both stay explicit in this one body, same as the ROM.
 */
#include "Particle.h"

extern void *data_0208f3b4;
extern void *data_0208f3c4;

Particle::SimpleCallback::SimpleCallback()
{
    *(void**)this = &data_0208f3b4;
    *(void**)this = &data_0208f3c4;
    unk_004 = 0;
}
