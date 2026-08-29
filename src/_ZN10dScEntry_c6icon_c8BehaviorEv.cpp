//cpp
// @symbol _ZN10dScEntry_c6icon_c8BehaviorEv
#include "dScEntry_c.h"

extern "C" {
int _Z15ApproachLinear2Rsss(short *, short, short);
extern unsigned char data_0209b2e4[];
}

void dScEntry_c::icon_c::Behavior()
{
    if (unk_01c == 0xd) {
        if (unk_020 == data_0209b2e4[0]) {
            _Z15ApproachLinear2Rsss(&unk_006, 0x14, 8);
        } else {
            if (_Z15ApproachLinear2Rsss(&unk_006, -0x24, 8) != 0)
                unk_020 = data_0209b2e4[0];
        }
    }
    dThIcon_c::Behavior();
}
