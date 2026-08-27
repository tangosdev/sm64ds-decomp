//cpp
// @symbol _ZN8Particle14SimpleCallbackC2Ev
/* recovered: real C++ base-object constructor
 *
 * The ROM's steps are: store base vtable, overwrite with derived vtable,
 * and initialize the short member at offset 4 to zero -- exactly what
 * `Particle::SimpleCallback : Particle::Callback` with a member initializer
 * for the short would mean.
 */
#include "Particle.h"

extern void *data_0208f3c4;

namespace Particle {
    struct SimpleCallback {
        void* vtable;  // Inherited from base
        short unk_004;  // Member at offset 4

        SimpleCallback();
    };
}

Particle::SimpleCallback::SimpleCallback()
{
    *(void**)this = &data_0208f3c4;  // Set derived vtable directly
    unk_004 = 0;                    // Initialize member
}
