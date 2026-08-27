//cpp
/* dScene_c::SetAndStopColorFader() at 0x0202e1b4, 0x2c bytes.
 *
 * Static, and this function is the evidence that SetFaders is static too: it loads
 * &data_0209f5e8 into r0 and `bl`s SetFaders without writing r1. If SetFaders
 * took a `this`, r0 would be it, and this call would be handing a fader to a
 * dScene_c*.
 *
 * The `speed = 0` afterwards stops the fade where SetFaders left it. Offset 0x8 is
 * Fader::speed, pinned by Fader::AdvanceInterp picking its target from that word's
 * sign. */
#include "dScene_c.h"
#include "FaderBrightness.h"

extern "C" {
/* Really a FaderColor -- see src/_ZN8dScene_c19ResetFadersAndSoundEv.cpp. */
extern FaderBrightness data_0209f5e8;
}

void dScene_c::SetAndStopColorFader()
{
    SetFaders(&data_0209f5e8);
    data_0209f5e8.speed = 0;
}
