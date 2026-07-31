// @symbol _ZN10FaderColorD1Ev
/* recovered: named members + shared header */
#include "FaderColor.h"
/* FaderColor::~FaderColor() at 0x020175c4
 * Sets self->vtable to the FaderColor vtable, delegates to the base-subobject
 * destructor (func_020177c4, shared with Color::~Color), and returns self
 * (ARM C++ dtor ABI). The u16 color member at 0xc needs no teardown.
 */

extern int _ZTV10FaderColor[];     /* vtable for FaderColor */
extern int func_020177c4(int *self); /* base-subobject destructor */

int _ZN10FaderColorD1Ev(int *self)
{
    self[0] = (int)_ZTV10FaderColor;  /* +0x00 vptr */
    func_020177c4(self);
    return (int)self;
}
