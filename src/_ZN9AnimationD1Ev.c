// @symbol _ZN9AnimationD1Ev
/* recovered: named members + shared header */
#include "Animation.h"
/* Animation::~Animation() (complete-object destructor, D1) at 0x02015ce8
 * Trivial destructor: restores the Animation vtable pointer into self->vtable
 * (slot 0x0) and returns. The three Fix12i members need no teardown.
 */

extern int _ZTV9Animation[];  /* vtable for Animation */

void _ZN9AnimationD1Ev(int *self)
{
    self[0] = (int)_ZTV9Animation;  /* +0x00 vptr */
}
