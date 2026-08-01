// @symbol _ZN9AnimationD2Ev
/* recovered: named members + shared header */
#include "Animation.h"
/* Animation::~Animation() (base-object destructor, D2) at 0x02015cb4
 * Trivial destructor: restores the Animation vtable pointer into self->vtable
 * (slot 0x0) and returns. The three Fix12i members need no teardown.
 */

extern int data_0208e7e4[];  /* vtable for Animation */

void _ZN9AnimationD2Ev(int *self)
{
    self[0] = (int)data_0208e7e4;  /* +0x00 vptr */
}
