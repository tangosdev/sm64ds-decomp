// @symbol _ZN15FaderBrightnessD1Ev
/* recovered: named members + shared header */
#include "FaderBrightness.h"
/* FaderBrightness::~FaderBrightness() at 0x02017814
 * Sets self->vtable to the FaderBrightness vtable, delegates to its base
 * (Fader) subobject destructor (func_02017838), and returns self
 * (ARM C++ dtor ABI). FaderBrightness adds no members of its own.
 */

extern int data_0208eacc[]; /* vtable for FaderBrightness */
extern int func_02017838(int *self);  /* base (Fader) subobject destructor */

int _ZN15FaderBrightnessD1Ev(int *self)
{
    self[0] = (int)data_0208eacc;  /* +0x00 vptr */
    func_02017838(self);
    return (int)self;
}
