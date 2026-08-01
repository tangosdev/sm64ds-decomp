//cpp
// @symbol _ZN10FaderColor11AdvanceFadeEv
/* recovered: named members + shared header, real C++ method */
#include "FaderColor.h"
extern "C" {

void _ZN5Fader13AdvanceInterpEv(void* thiz);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short* p, unsigned short a, int b);
}

void FaderColor::AdvanceFade()
{
    int old = unk_004;
    _ZN5Fader13AdvanceInterpEv(((char*)this));
    if (unk_004 == old) return;
    {
        unsigned short color = unk_00c;
        int m = color ? 0x10 : -0x10;
        int r = (unk_004 * m) >> 12;
        if (r != 0) {
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4000050, 0x3f, r);
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4001050, 0x3f, r);
        } else {
            *(unsigned short*)0x4000050 = 0;
            *(unsigned short*)0x4001050 = 0;
        }
    }
}
