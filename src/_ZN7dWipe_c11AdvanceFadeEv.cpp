//cpp
// @symbol _ZN7dWipe_c11AdvanceFadeEv
/* recovered: named members + real C++ method */
/* dWipe_c::AdvanceFade() -- vtable slot 2. Type 1 is the plain colour fade and
 * the base owns it. Otherwise this steps the wipe's own 20.12 ramp: state 1
 * runs it up to the halfway mark and hands over to state 2, state 3 runs it
 * back down to zero and pins the blend brightness on both engines before
 * settling in state 4. States 0/2/4 are resting and return early. */
#include "dWipe_c.h"
#include "decl_FaderColor.h"
#include "decl_common.h"

extern "C" void _ZN3G2x18SetBlendBrightnessEPVtts(unsigned short *p, unsigned short a, int b);

void dWipe_c::AdvanceFade()
{
    if (type == 1) {
        FaderColor::AdvanceFade();
        return;
    }
    switch (state) {
    case 0:
        return;
    case 1:
        wipeInterp += wipeSpeed;
        wipeSpeed += wipeAccel;
        if (wipeInterp >= 0x200000) {
            wipeInterp = 0x200000;
            needsCleanup = 0;
            func_0202fb30(this);
            state = 2;
        }
        break;
    case 2:
        return;
    case 3:
        wipeInterp += wipeSpeed;
        wipeSpeed += wipeAccel;
        if (wipeInterp <= 0) {
            int b;
            wipeInterp = 0;
            needsCleanup = 0;
            b = (type == 0) ? 0x10 : -0x10;
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4000050, 0x3f, b);
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4001050, 0x3f, b);
            func_0202fb30(this);
            state = 4;
        }
        break;
    case 4:
    default:
        return;
    }
    func_0202f290(this);
}
