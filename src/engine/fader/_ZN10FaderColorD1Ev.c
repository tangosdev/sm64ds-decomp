// @symbol _ZN10FaderColorD1Ev
/* recovered: named members + shared header */
#include "FaderColor.h"
/* FaderColor::~FaderColor() at 0x020175c4
 * Sets self->vtable to the FaderColor vtable, delegates to the base-subobject
 * destructor (_ZN15FaderBrightnessD2Ev, shared with Color::~Color), and returns self
 * (ARM C++ dtor ABI). The u16 color member at 0xc needs no teardown.
 */

extern int data_0208eb2c[];     /* vtable for FaderColor */
extern int _ZN15FaderBrightnessD2Ev(int *self); /* base-subobject destructor */

int _ZN10FaderColorD1Ev(int *self)
{
    self[0] = (int)data_0208eb2c;  /* +0x00 vptr */
    _ZN15FaderBrightnessD2Ev(self);
    return (int)self;
}
