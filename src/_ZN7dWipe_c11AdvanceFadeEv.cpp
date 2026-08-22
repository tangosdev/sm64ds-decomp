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
    if (unk_014 == 1) {
        FaderColor::AdvanceFade();
        return;
    }
    switch (unk_010) {
    case 0:
        return;
    case 1:
        unk_01c += unk_020;
        unk_020 += unk_024;
        if (unk_01c >= 0x200000) {
            unk_01c = 0x200000;
            unk_00f = 0;
            func_0202fb30(this);
            unk_010 = 2;
        }
        break;
    case 2:
        return;
    case 3:
        unk_01c += unk_020;
        unk_020 += unk_024;
        if (unk_01c <= 0) {
            int b;
            unk_01c = 0;
            unk_00f = 0;
            b = (unk_014 == 0) ? 0x10 : -0x10;
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4000050, 0x3f, b);
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4001050, 0x3f, b);
            func_0202fb30(this);
            unk_010 = 4;
        }
        break;
    case 4:
    default:
        return;
    }
    func_0202f290(this);
}
