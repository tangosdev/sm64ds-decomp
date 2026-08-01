// @symbol _ZN7ClipperD1Ev
/* recovered: named members + shared header */
#include "Clipper.h"
/* Clipper::~Clipper() at 0x02015720
 * Trivial destructor: stores the Clipper vtable pointer into self->vtable
 * (slot 0x0) and returns. No members need teardown.
 */

extern int _ZTV7Clipper[];  /* vtable for Clipper */

void _ZN7ClipperD1Ev(int *self)
{
    self[0] = (int)_ZTV7Clipper;  /* +0x00 vptr */
}
