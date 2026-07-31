// @symbol _ZN5FaderD1Ev
/* recovered: named members + shared header */
#include "Fader.h"
/* Fader::~Fader() at 0x0201786c
 * Trivial destructor: restores the Fader vtable pointer into self->vtable
 * (slot 0x0) and returns. The currInterp/speed Fix12i members need no teardown.
 */

extern int _ZTV5Fader[];  /* vtable for Fader */

void _ZN5FaderD1Ev(int *self)
{
    self[0] = (int)_ZTV5Fader;  /* +0x00 vptr */
}
